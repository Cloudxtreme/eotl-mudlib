#ifndef ACME_CALLSECURITY_INC
#define ACME_CALLSECURITY_INC

#define SEC_UID      "uid"
#define SEC_FILENAME "filename"
#define SEC_OBJECT   "object"

// Use this to check someone's security
static status  sec_check_command( mixed operation );

#define SEC_MANAGE   "sec_manage"

// These functions are access controlled under the SEC_MANAGE operation.
public status sec_add   ( string field, mixed operation, mixed item );
public status sec_remove( string field, mixed operation, mixed item );
public status sec_clear ( mixed operation );
public mixed  sec_query ( string field, mixed operation );
public mixed *sec_oplist();

// This activates the security
static void   sec_lock();

// These are primarily for debugging
static object *sec_caller_stack();
static status  sec_is_member( object ob, mixed operation );

#endif
