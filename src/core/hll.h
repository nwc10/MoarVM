/* The configuration for a particular High Level Language. Of note, this
 * indicates the types to use in various situations. */
struct MVMHLLConfig {
    /* HLL name. This is needed for serialize_stable(), and needs to be first
     * for MVMFixKeyHash. */
    MVMString *name;
    /* The types the language wishes to get things boxed as. */
    MVMObject *int_box_type;
    MVMObject *num_box_type;
    MVMObject *str_box_type;

    /* The type to use for slurpy arrays. */
    MVMObject *slurpy_array_type;

    /* The type to use for slurpy hashes. */
    MVMObject *slurpy_hash_type;

    /* The type to use for array iteration (should have VMIter REPR). */
    MVMObject *array_iterator_type;

    /* The type to use for hash iteration (should have VMIter REPR). */
    MVMObject *hash_iterator_type;

    /* The value to substitute for null. */
    MVMObject *null_value;

    /* Language's handler to run at a block's exit time, if needed. */
    MVMCode *exit_handler;

    /* Language's object finalize handler, which can take a list of objects
     * which need to have a finalizer run. */
    MVMCode *finalize_handler;

    /* Language's handler for various errors, if needed. */
    MVMCode *bind_error;
    MVMObject *method_not_found_error;
    MVMCode *lexical_handler_not_found_error;

    /* Native reference types. */
    MVMObject *int_lex_ref;
    MVMObject *num_lex_ref;
    MVMObject *str_lex_ref;
    MVMObject *int_attr_ref;
    MVMObject *num_attr_ref;
    MVMObject *str_attr_ref;
    MVMObject *int_pos_ref;
    MVMObject *num_pos_ref;
    MVMObject *str_pos_ref;
    MVMObject *int_multidim_ref;
    MVMObject *num_multidim_ref;
    MVMObject *str_multidim_ref;

    /* Booleans. */
    MVMObject *true_value;
    MVMObject *false_value;

    /* The language's call dispatcher, method call dispatcher, and method
     * lookup names. Used by the lang-call and lang-method-call built-in
     * dispatchers to resolve dispatches. */
    MVMString *call_dispatcher;         /* callee, args... */
    MVMString *method_call_dispatcher;  /* decont'd inv, name, inv, args... */
    MVMString *find_method_dispatcher;  /* decont'd inv, name, throw_if_not_found */
    MVMString *resume_error_dispatcher; /* whatever args were given for resumption */
    MVMString *hllize_dispatcher;       /* object to hllize, optional HLL name */
    MVMString *istype_dispatcher;       /* object, type */
    MVMString *isinvokable_dispatcher;  /* object */

    /* The maximum code size that we'll inline. */
    MVMuint32 max_inline_size;
};

MVMHLLConfig * MVM_hll_get_config_for(MVMThreadContext *tc, MVMString *name);
MVMObject * MVM_hll_set_config(MVMThreadContext *tc, MVMString *name, MVMObject *config_hash);
MVM_PUBLIC MVMHLLConfig * MVM_hll_current(MVMThreadContext *tc);
void MVM_hll_enter_compilee_mode(MVMThreadContext *tc);
void MVM_hll_leave_compilee_mode(MVMThreadContext *tc);
MVM_PUBLIC MVMObject * MVM_hll_sym_get(MVMThreadContext *tc, MVMString *hll, MVMString *sym);
