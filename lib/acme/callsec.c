/*
    NAME

        callsec.c - A basic extensible security system 

    DESCRIPTION

      This package is for controlling access to functions in a
      flexible, but secure way.  It maintains lists of approved things
      for various operations.  

                        ----------------------
                                 API

      These functions are publicly callable, but are access controlled.
      They can only be called when everything in the call stack, plus
      this_interactive() are all approved for the SEC_MANAGE
      operation, and efun::remote_command() returns 0.

        sec_add    - Approve an object, file, or uid
                     for a particular operation

        sec_remove - Remove approval of an object, file, or uid
                     for a particular operation

        sec_clear  - Remove approval of all objects, files, and uids 
                     for a particular operation

        sec_query  - Return list of all objects, files, or uids
                     approved for a particular operation

        sec_lock   - Lock the system, turning on security.  It starts
                     out unlocked.  The object has no security at all
                     until this is done.

        sec_oplist - Return a list of all operations.

      These functions are static, and therefore can not be called by
      other objects.  They perform no security checks.  

        sec_check_command - Verify that all relevant objects leading
                            up to this function call are approved for
                            a particular operation, or any of a list
                            of operations.  

        sec_is_member     - Check if an object is approved for some
                            operation.

        sec_caller_stack  - Returns the list of objects in the caller 
                            stack + THISI.  Useful for debugging.

    NOTES

        See man/pkg/AcmeCallSecurity
     
    LAST MODIFIED
        Zamboni 970813


*/

#include <acme.h>

#include AcmeVarSpaceInc
#include AcmeCallSecurityInc

private inherit AcmeVarSpace;

#define DEBUG 0

private status SEC_locked;

/*----------------------------- sec_is_member ---------------------------*/
/*
    Description:
      Check whether a particular object is approved for an operation.
      This is not useful for checking overall security, but can be useful
      in debugging security policies.  
    Parameters:
      ob        - The object to check
      operation - The operation to check it on
    Returns:
      1 if it <ob> is approved for <operation>, 0 if not.
    Notes:
      No side effects.
*/
static status
sec_is_member(object ob, mixed operation)
{
  if( !ob )
    return 0;
  
  // Approved filename
  if(efun::member(vquery(SEC_FILENAME, operation) || ({ }), 
            load_name(ob)) > -1)
    return 1;
  
  // Approved object
  if(efun::member(vquery(SEC_OBJECT, operation) || ({ }), ob) > -1)
    return 1;  
    
  // In the approved uid list
  if(efun::member(vquery(SEC_UID, operation) || ({ }), efun::getuid(ob)) > -1)
    return 1;
  
#if DEBUG
  printf( "Security check failed for %O for %s\n", ob, operation );
#endif
  return 0;
}

/*----------------------------- sec_caller_stack ---------------------------*/
/*
    Description:
      Return a list of the objects involved in the current invocation.
      These are the objects which would be checked in sec_check_command().
      This function is meant for debugging your security policies.
    Parameters:
      None.
    Returns:
      See Description.
    Notes:
      No side effects.
*/
static object *
sec_caller_stack()
{
  int i, size;
  object ob;
  object *list;


  size = efun::caller_stack_depth();
  list = allocate( size + 1 );

  list[size] = THISI;
  
  for(i = 0; i < size; i++)
  {
    list[i] = efun::previous_object(i);

    /* We could remove duplicates from the array, but they probably
       won't happen very often, so removing them is probably more of a
       speed hit than letting those rare ones through. */
  }
  return list;
}

/*----------------------------- sec_check_command ---------------------------*/
/*
    Description:
      Determine whether the current action is being done securely.
      (i.e. whether all objects involved in the current invocation are 
      trusted to perform at least one of the specified operations.)
    Parameters:
      operations - A single operation or a list of operation.
                   If a list is supplied, each object will be accepted 
                   if it is approved for at least one of the specified 
                   operations.  
    Returns:
      1 if the current invocation is thought to be secure, 0 if not.
    Notes:
      No side effects.
      Not access controlled.
      To check whether the current invocation is secure for more than
      one operation (rather than whether each object is approved for
      one of many operation, make sure it's approved for each of
      them), just call this function once for each operation you want
      to check.
*/
static status
sec_check_command( mixed operations )
{
  object *list;
  int i, j;
  status result;
  
/*
  if( efun::remote_command() )
    return 0;
*/
  
  if ( !pointerp( operations ) )
    operations = ({ operations });

  // We could do all this as map_array()s, but that would often mean
  // lots of extra calls to sec_is_member().  Which is faster?  Dunno.
  list = sec_caller_stack();
  for(i = 0; i < sizeof( list ); i++ )
  {
    for( j = 0; j < sizeof( operations ); j++ )
    {
      result = sec_is_member( list[i], operations[j] );
      if( result ) // no need to continue
        break;
    }
    if ( !result )
      break;
  }
  return result;
}

// Turn "foo/bar.c", "foo/bar", or "/foo/bar.c" into "/foo/bar"
// That way they match what program_name() returns.
private string
garble_programname( string name )
{
  if( !efun::stringp( name ) || !efun::strlen( name ))
    return name;
  
  if( name[0] != '/' )
    name = "/" + name;
  if( name[<2..] == ".c" )
    name = name[0..<3];
  return name;
}



/********************************************************************
**
** Public access functions, protected only by the security system
**
*/

/*----------------------------- sec_add ---------------------------*/
/*
    Description:
      Add approval of an item for an operation.
    Parameters:
      field     - SEC_UID, SEC_FILENAME, or SEC_OBJECT
      operation - which operation it should be removed from
      item      - the uid, filename, or object to remove
    Returns:
      1 on success, 0 on failure ( due to lack of access ).
    Notes:
      All objects on the stack must be approved for SEC_MANAGE to call this.
*/
public status
sec_add( string field, mixed operation, mixed item )
{
  mixed oldval;
#if DEBUG
  printf( "Checking %O to %s field for %s group\n", item, field, operation );
#endif

  if( SEC_locked && !sec_check_command( SEC_MANAGE ) )
    return 0;

#if DEBUG
  printf( "Check passed.\n" );
#endif

  // This is a hack, but without it people will waste tons of time
  // wondering why things don't work.  (Gee, why do I know this?)
  if( field == SEC_FILENAME )
    item = garble_programname( to_string( item ) );

#if DEBUG
  printf( "Adding %O to %s field for %s group\n", item, field, operation );
#endif

  oldval = vquery( field, operation );

#if DEBUG
  printf( "Group was: %O\n", oldval );
#endif

  vset( field, ( oldval || ({ }) ) + ({ item }), operation );

#if DEBUG
  printf( "Group is : %O\n", vquery( field, operation ) );
#endif

  return 1;
}

/*----------------------------- sec_remove ---------------------------*/
/*
    Description:
      Remove approval of an item for an operation.
    Parameters:
      field     - SEC_UID, SEC_FILENAME, or SEC_OBJECT
      operation - which operation it should be removed from
      item      - the uid, filename, or object to remove
    Returns:
      1 on success, 0 on failure ( due to lack of access ).
    Notes:
      All objects on the stack must be approved for SEC_MANAGE to call this.
*/
public status
sec_remove( string field, mixed operation, mixed item )
{
  if( SEC_locked && !sec_check_command( SEC_MANAGE ) )
    return 0;

  // This is a hack, but without it people will waste tons of time
  // wondering why things don't work.  (Gee, why do I know this?)
  if( field == SEC_FILENAME )
    item = garble_programname( item );

  vset( field, ( ( vquery( field, operation ) || ({ }) )
                 - ({ item }) ), operation );

  return 1;
}

/*----------------------------- sec_clear ---------------------------*/
/*
    Description:
      Clears and initializes the lists for an operation
    Parameters:
      operation - the operation whose lists should be cleared
    Returns:
      1 on success, 0 on failure ( due to lack of access ).
    Notes:
      All objects on the stack must be approved for SEC_MANAGE to call this.
*/
public status
sec_clear( mixed operation )
{
  int i;
  string *fields;
  if( SEC_locked && !sec_check_command( SEC_MANAGE ) )
    return 0;

  fields = ({ SEC_UID, SEC_FILENAME, SEC_OBJECT });
  for( i=sizeof( fields )-1; i >= 0; i-- )
  {
    vset( fields[i], ({ }), operation );
  }
  
  return 1;
}

/*----------------------------- sec_query ---------------------------*/
/*
    Description:
      Returns a list of things approved for the specified operation and type.
    Parameters:
      field     - SEC_UID, SEC_FILENAME, or SEC_OBJECT
      operation - the operation about which you want information
    Returns:
      A list of all objects, files, or uids approved for <operation>.
      Which list is returned depends on <field>.
    Notes:
      No side effects, but the list is not a copy.
      All objects on the stack must be approved for SEC_MANAGE to call this.
*/
public mixed
sec_query( string field, mixed operation )
{
  if( SEC_locked && !sec_check_command( SEC_MANAGE ) )
    return 0;
  return vquery( field, operation ) || ({ });
}

/*----------------------------- sec_opslist ---------------------------*/
/*
    Description:
      Returns a list of defined operations.
    Parameters:
      None.
    Returns:
      An array of operations (of unspecified type).
    Notes:
      None.
*/
public mixed *
sec_oplist()
{
  if( SEC_locked && !sec_check_command( SEC_MANAGE ) )
    return 0;
  return vspaces() || ({ });
}


/*----------------------------- sec_lock ---------------------------*/
/*
    Description:
      Activates the security system.
    Parameters:
      None.
    Returns:
      Nothing.
    Notes:
      Until this is called, the security system doesn't do anything.
      Not access controlled.
*/
public void
sec_lock()
{
  SEC_locked = 1;
  return;
  /*
   * As I see it, there are three options for the access control on 
   * this command: 
   *
   *       1) No access control.  Anyone can lock the system.  Objects
   *          could lock themselves out.  Objects that don't finish
   *          their security setup before create() is finished could
   *          be locked out by malicious objects that call lock().
   *          Objects really ought to finish their security setup by
   *          the end of create() though.
   *
   *       2) Check for SEC_MANAGE.  Objects can't get locked out by
   *          calling sec_lock(), because if they're allowed to lock
   *          it, they're allowed to do anything else too.  Of course,
   *          they can still get locked out by removing themselves
   *          from the list later, but that's less likely to happen by
   *          accident.  I don't think that locking yourself out at
   *          setup is that big a deal, because you just need to fix
   *          the code, dest the object, and try again.  It also might
   *          be intentional and desired.  A bigger problem with this
   *          approach is that if the object gets loaded by a chain of
   *          untrusted objects, the locking will fail and the object
   *          will be active but not secure.  This is Bad, and the
   *          only decent way around it is to not use this solution.
   *
   *       3) Make the function static.  Only this object can activate
   *          its security.  That's a minor loss of openness, but I
   *          don't think the object is really ready to behave in an
   *          open way until the security is turned on.  On the other 
   *          hand, it's better to have too much security than too
   *          little.  If the object activates security during create(), 
   *          then there's no difference between this and (1).  If they
   *          don't, then they're asking for trouble.  Might as well make
   *          it public.
   *
   */
}

