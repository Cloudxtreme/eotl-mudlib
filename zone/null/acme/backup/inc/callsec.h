#ifndef ACME_CALLSECURITY_INC
#define ACME_CALLSECURITY_INC

#define SEC_UID      "uid"
#define SEC_FILENAME "filename"
#define SEC_OBJECT   "object"

// Use these to check someone's security
static status  sec_check_command( mixed space );
static status  sec_is_member( object ob, mixed context );

#define SEC_MANAGE   "sec_manage"

// These functions are access controlled under the 
// SEC_MANAGE operation/context.
public status sec_add   ( string field, mixed item, mixed context );
public status sec_remove( string field, mixed item, mixed context );
public status sec_clear ( mixed context );
public mixed  sec_query ( string field, mixed context );

// These functions are not access controlled, but can't be called
// externally by default.
static void    sec_add_   ( string field, mixed item, mixed context );
static void    sec_remove_( string field, mixed item, mixed context );
static void    sec_clear_ ( mixed context );
static mixed   sec_query_ ( string field, mixed context );

// This is primarily for debugging
static object *sec_caller_stack();

#endif
