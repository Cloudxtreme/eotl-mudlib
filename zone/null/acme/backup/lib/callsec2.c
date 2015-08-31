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

        sec_lock   - Lock or unlock the system.  It starts out unlocked.
                     The object has no security at all except when the
                     system is locked.

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
        Zamboni 960718


*/

#include <acme.h>

private inherit AcmeVarSpace;
#include AcmeVarSpaceInc
//#include AcmeCallSecurityInc
#include AcmeIncDir "callsec2.h"

private status SEC_locked;

static status
sec_is_member(object ob, mixed operation)
{
  if( !ob )
    return 0;
  
  // Approved filename
  if(efun::member(vquery(SEC_FILENAME, operation) || ({ }), 
            program_name(ob)) > -1)
    return 1;
  
  // Approved object
  if(efun::member(vquery(SEC_OBJECT, operation) || ({ }), ob) > -1)
    return 1;  
    
  // In the approved uid list
  if(efun::member(vquery(SEC_UID, operation) || ({ }), efun::getuid(ob)) > -1)
    return 1;
  
  return 0;
}

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

static status
sec_check_command( mixed operations )
{
  object *list;
  int i, j;
  status result;
  
  if( efun::remote_command() )
    return 0;
  
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

public status
sec_add( string field, mixed item, mixed operation )
{
  mixed oldval;
  if( SEC_locked && !sec_check_command( SEC_MANAGE ) )
    return 0;

  // This is a hack, but without it people will waste tons of time
  // wondering why things don't work.  (Gee, why do I know this?)
  if( field == SEC_FILENAME )
    item = garble_programname( to_string( item ) );

  oldval = vquery( field, operation );
  vset( field, ( oldval || ({ }) ) + ({ item }), operation );

  return 1;
}

public status
sec_remove( string field, mixed item, mixed operation )
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

public mixed
sec_query( string field, mixed operation )
{
  if( SEC_locked && !sec_check_command( SEC_MANAGE ) )
    return 0;
  return vquery( field, operation ) || ({ });
}

public status
sec_lock( status newval )
{
  // Perhaps we should always check access for this.  It would suck to
  // lock yourself out when you couldn't get back in.
  if( SEC_locked && !sec_check_command( SEC_MANAGE ) )
    return 0;
  SEC_locked = newval;
  return 1;
}

