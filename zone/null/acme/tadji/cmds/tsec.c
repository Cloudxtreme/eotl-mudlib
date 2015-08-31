#include <acme.h>

/*
    LAST MODIFIED
        Zamboni 970813
 */

static inherit AcmeCmd;
static inherit AcmeArgs;
static inherit AcmeError;

#include AcmeCmdInc
#include AcmeArgsInc
#include AcmeErrorInc
#include OpieClient
#include AcmeCallSecurityInc

static string
field_name( string field )
{
  switch( field )
  {
    case "object":
      return "object";
    case "uid":
      return "uid";
    case "file":
    case "filename":
      return "filename";
  }
  return 0;  
}


static mixed
parse_value( string field, string arg )
{
  object *val;
  
  switch( field )
  {
    case "object":
      val = opie( arg );
      if( !val || !sizeof( val ) )
      {
        error( ERR_ERROR, sprintf( "No objects found: %s", arg ) );
        return 0;
      }
      if( sizeof( val ) < 1 )
      {
        error( ERR_ERROR, sprintf( "Spec matched multiple objects.  "
                                   "You may only name one object.", arg ) );
        return 0;
      }
      return val[0];
    case "uid":
      return arg;
    case "file":
    case "filename":
      val = opie( arg );
      if( !val || !sizeof( val ) )
      {
        error( ERR_ERROR, sprintf( "No objects found: %s", arg ) );
        return 0;
      }
      if( sizeof( val ) < 1 )
      {
        error( ERR_ERROR, sprintf( "Spec matched multiple objects.  "
                                   "You may only name one object.", arg ) );
        return 0;
      }
      return load_name( val[0] );
    default:
      error( ERR_ERROR, sprintf( "Invalid field \"%s\"", field ) );
      return 0;
  }
      
}


status
do_cmd(string name, string arg)
{
  object PO;
  string *args;
  mixed result;
  mixed value;
  string field;
  int i;  
  
  if( !arg )
  {
    FailUsage( name );
  }
  args = explode_args( arg );
  if( sizeof( args ) < 1 )
  {
    FailUsage( name );
  }
  
  PO = previous_object();

  switch( args[0] )
  {
    case "clear":
      if( sizeof( args ) != 2 )
      {
        FailUsage( name );
      }
      result = PO->sec_clear(args[1]);
      break;
      
    case "add":
    case "rm":
    case "rem":
    case "remove":
      if( sizeof( args ) != 4 )
      {
        FailUsage( name );
      }
      field = field_name( args[1] );      
      if( !field )
      {
        error( ERR_ERROR, sprintf( "Invalid field: %s", args[1] ) );
        return 1;
      }
      value = parse_value( field, args[2] );
      if( !value )
        return 1;  // already gave them an error message.
      
      if( args[0] == "add" )
        result = PO->sec_add( field, args[3], value );
      else
        result = PO->sec_remove( field, args[3], value );
      break;
    case "operations":
      value = PO->sec_oplist();
      printf( "There are %d operations.\n", sizeof( value ) );
      for ( i=sizeof( value ) - 1; i >= 0; i-- )
      {
        printf( "  %s\n", value[i] );
      }
      return 1;
    case "query":
    case "list":
       if ( sizeof( args ) == 1 )
         args += ({ "command" });
      result = PO->sec_query( SEC_FILENAME, args[1] );
      if ( result == 0 )  // permission denied.
        break;
      printf( "Authorization lists for %O:\n"
              "  Files/Programs:\n", args[1] );
      for ( i=sizeof( result )-1; i >= 0; i-- )
        printf( "    %s\n", result[i] );

      printf( "  Objects:\n" );
      result = PO->sec_query( SEC_OBJECT, args[1] );
      for ( i=sizeof( result )-1; i >= 0; i-- )
        printf( "    %O\n", result[i] );

      printf( "  Uids:\n" );
      result = PO->sec_query( SEC_UID, args[1] );
      for ( i=sizeof( result )-1; i >= 0; i-- )
        printf( "    %s\n", result[i] );
      
      result = 1;
      break;
    default:
      error( ERR_ERROR, sprintf( "Invalid command: %s", args[0] ) );
      return 1;
  }
  
  if( result == 0 )
  {
    write( "Operation failed.\n" );
    if( searcha( inherit_list( PO ), AcmeCallSecurity[1..] ) == -1)
      printf( "%s does not use AcmeCallSecurity.\n", to_string( PO ) );
    else
      write("Access was denied for some reason.\n");
  } else
  {
    write( "Operation succeeded.\n" );
    if( result != 1 )
      printf( "Returned: %O\n", result );
  }
  return 1;
}

string
cmd_desc(string func)
{
  return( "tadji security interface" );
}

string *
cmd_usage(string func)
{
  return ({ "add   <field> <value> <operation>",
            "rm    <field> <value> <operation>",
            "list <operation>",
            "clear <operation>",
            "operations",
              });
}

string
cmd_help(string func)
{
  return 
    "Configures the security of your tadji (or whatever the calling "
    "object is).  "
    "<field> must be \"uid\", \"object\", or \"filename\"|\"file\".  "
    "<value> is the uid, object spec, or filename.  <operation> is the "
    "name of the group/operation list you want to affect/list.  "
    "This help is deliberately vague.  See AcmeCallSecurity for "
    "complete documentation on managing security policies.  "
    AcmeTadjiDir "cmds/tsec.README may also prove enlightening.  ";
}
