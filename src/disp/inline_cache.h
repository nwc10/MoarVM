/* Inline cache storage is used for instructions that want to cache data as
 * they are interpreted. This is primarily used by the dispatch instructions
 * in order to store guards, but getlexstatic_o also emits a super-simple
 * guard tree too which just has an instruction to return the value.
 *
 * Each initialized static frame may have inline cache storage. This is not
 * really inline (since we mmap bytecode), but stored as an array off to the
 * side. Lookups are based on the instruction offset into the instruction.
 * It would be very memory intensive to have an entry per byte, so instead we
 * look at the minimum distance between instructions that might use the cache
 * during frame setup, then pick the power of 2 below that as the interval.
 * We can thus do a lookup in the inline cache by doing a bit shift on the
 * current instruction address. */

/* This is the top level cache struct, living in a static frame. */
struct MVMDispInlineCache {
    /* Cache entries. Atomically updated, released via. safepoint. These are
     * always initialized for instructions that would use them to the initial
     * entry for that kind of instruction (in PIC parlance, "unlinked"). */
    MVMDispInlineCacheEntry **entries;

    /* The number of entries, used when we need to GC-walk them. */
    MVMuint32 num_entries;

    /* The bit shift we should do on the instruction address in order to
     * find an entry for a instruciton. */
    MVMuint32 bit_shift;
};

/* We always invoke an action using the cache by calling a function pointer.
 * These are the kinds of pointer we have: one for getlexstatic, another for
 * dispatch. */
typedef MVMObject * MVMDispInlineCacheRunGetLexStatic(MVMThreadContext *tc,
        MVMDispInlineCacheEntry **entry_ptr, MVMString *name);
typedef void MVMDispInlineCacheRunDispatch(MVMThreadContext *tc,
        MVMDispInlineCacheEntry **entry_ptr, MVMDispInlineCacheEntry *seen,
        MVMString *id, MVMCallsite *cs, MVMuint16 *arg_indices,
        MVMRegister *source, MVMStaticFrame *sf, MVMuint32 bytecode_offset);

#define MVM_INLINE_CACHE_KIND_INITIAL 0
#define MVM_INLINE_CACHE_KIND_INITIAL_FLATTENING 1
/* The baseline inline cache entry. These always start with a pointer to
 * invoke to reach the handler. */
struct MVMDispInlineCacheEntry {
    /* The callback to run when we reach this location. */
    union {
        MVMDispInlineCacheRunGetLexStatic *run_getlexstatic;
        MVMDispInlineCacheRunDispatch *run_dispatch;
    };
};

#define MVM_INLINE_CACHE_KIND_RESOLVED_GET_LEX_STATIC 2
/* A resolved entry for getlexstatic. */
struct MVMDispInlineCacheEntryResolvedGetLexStatic {
    MVMDispInlineCacheEntry base;
    MVMObject *result;
};

#define MVM_INLINE_CACHE_KIND_MONOMORPHIC_DISPATCH 3
/* A resolved monomorphic entry for dispatch. */
struct MVMDispInlineCacheEntryMonomorphicDispatch {
    MVMDispInlineCacheEntry base;
    MVMDispProgram *dp;
};

#define MVM_INLINE_CACHE_KIND_MONOMORPHIC_DISPATCH_FLATTENING 4
/* A resolved monomorphic entry for dispatch with flattening. */
struct MVMDispInlineCacheEntryMonomorphicDispatchFlattening {
    MVMDispInlineCacheEntry base;
    MVMCallsite *flattened_cs;
    MVMDispProgram *dp;
};

#define MVM_INLINE_CACHE_KIND_POLYMORPHIC_DISPATCH 5
/* A resolved polymorphic entry for dispatch. */
struct MVMDispInlineCacheEntryPolymorphicDispatch {
    MVMDispInlineCacheEntry base;
    MVMDispProgram **dps;
    MVMuint32 num_dps;
    MVMuint32 max_temporaries;
};

#define MVM_INLINE_CACHE_KIND_POLYMORPHIC_DISPATCH_FLATTENING 6
/* A resolved polymorphic entry for dispatch with flattening. */
struct MVMDispInlineCacheEntryPolymorphicDispatchFlattening {
    MVMDispInlineCacheEntry base;
    MVMCallsite **flattened_css;
    MVMDispProgram **dps;
    MVMuint32 num_dps; // Also the number of flattend callsites
    MVMuint32 max_temporaries;
};

/* The maximum degree of polymorphism allowed at a given inline cache site. */
#define MVM_INLINE_CACHE_MAX_POLY 64

void MVM_disp_inline_cache_setup(MVMThreadContext *tc, MVMStaticFrame *sf);
void MVM_disp_inline_cache_mark(MVMThreadContext *tc, MVMDispInlineCache *cache,
        MVMGCWorklist *worklist);
MVMObject * MVM_disp_inline_cache_get_lex_resolution(MVMThreadContext *tc, MVMStaticFrame *sf,
        MVMuint32 bytecode_offset);
MVMuint32 MVM_disp_inline_cache_get_slot(MVMThreadContext *tc, MVMStaticFrame *sf,
        MVMuint32 bytecode_offset);
MVMuint32 MVM_disp_inline_cache_transition(MVMThreadContext *tc,
        MVMDispInlineCacheEntry **entry_ptr, MVMDispInlineCacheEntry *entry,
        MVMStaticFrame *root, MVMDispDefinition *initial_disp,
        MVMCallsite *initial_cs, MVMDispProgram *dp);
void MVM_disp_inline_cache_destroy(MVMThreadContext *tc, MVMDispInlineCache *cache);
MVMuint32 MVM_disp_inline_cache_get_kind(MVMThreadContext *tc, MVMDispInlineCacheEntry *entry);
MVMint32 MVM_disp_inline_cache_try_get_kind(MVMThreadContext *tc, MVMDispInlineCacheEntry *entry);
