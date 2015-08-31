
#include <acme.h>

inherit ObjectCode;
static inherit AcmeTadji;
static inherit AcmeCallSecurity;

#include AcmeTadjiInc
#include AcmeCallSecurityInc

#define TadjiCmd    AcmeTadjiDir "cmds/tadji"
#define HelpCmd     AcmeTadjiDir "cmds/help"
#define TadjiSecCmd AcmeTadjiDir "cmds/tsec"
#define TCmdsCmd    AcmeTadjiDir "cmds/tcmds"

//static status initialized;

create()
{
  mixed t1, t2, t3, err;
 
  seteuid(getuid());
#if 0
  t1 = read_file( "/UPSINCE" );
  if( stringp( t1 ) )
  {
    t2 = explode( t1, "\n" );
    if( sizeof( t2 ) >= 2 )
    {
      t3 = t2[1];
    }
  }
  if( !stringp( t3 ) )
  {
    t1 = read_file( "/log/misc/upsince" );
    if( t1 )
    {
      t1 = to_int( t1 );
      if( t1 )
      {
        t3 = ctime( t1 );
      }
    }
  }
  if( !stringp( t3 ) )
  {
    t3 = "[unknown]";
  }
  err = catch( write_file( "/zone/null/acme/log/tadji.log", 
              sprintf( "%s | %s at %s | P_O: %O | THISP: %O | THISI: %O"
              " | VERB: %O | ARG: %O\n", 
              t3, 
              ( clonep() ? "CLONED" : "LOADED" ), 
              ctime(),
              previous_object(), 
              ( this_player() ? getuid( this_player() ) : 0 ),
              ( this_interactive() ? getuid( this_interactive() ) : 0 ),
              query_verb(), 
              query_arg() ) 
          ) );
  if(err)
     write("tadji write_file failed: " + err);
#endif


  ::create();
  initialize();
  tadji::create();
  
  set("id", "tadji");
  set("long", 
    "This is a Tadji -- a simple Acme Command Router.  With it, you can "
    "manage your command modules.  It probably has at least the commands "
    "\"tadji\" and \"help\".\n");
}

static void
initialize()
{
  object owner, orig;  
  owner = THISP;

  sec_clear( SEC_MANAGE );
  sec_clear( "config" );
  sec_clear( "command" );
  sec_clear( "query" );
  
  // THISI is /secure/bin/logon  during .logon, so we need to add 
  // this in case they want to use the tadji in their .logon
  orig = efun::caller_stack()[<1];
  if ( orig && (load_name(orig) == "/secure/bin/login")
       && geteuid(orig) == getuid(THISI) )
  {
    sec_add( SEC_OBJECT,   SEC_MANAGE, orig );
    sec_add( SEC_OBJECT, "command", orig );
    sec_add( SEC_OBJECT, "config",  orig );
    sec_add( SEC_OBJECT, "query",   orig );
  } else {
    // In the old driver & lib, THISI is what we want.
    sec_add( SEC_OBJECT, SEC_MANAGE, THISI );
    sec_add( SEC_OBJECT, "command", THISI );
    sec_add( SEC_OBJECT, "config",  THISI );
    sec_add( SEC_OBJECT, "query",   THISI );
  }

  sec_add( SEC_OBJECT,   SEC_MANAGE, owner );
  sec_add( SEC_OBJECT,   SEC_MANAGE, THISO );
  sec_add( SEC_FILENAME, SEC_MANAGE, TadjiSecCmd );

  sec_add( SEC_OBJECT, "command", owner );
  sec_add( SEC_OBJECT, "config",  owner );
  sec_add( SEC_OBJECT, "query",   owner );

  sec_add( SEC_OBJECT, "command", THISO );
  sec_add( SEC_OBJECT, "config",  THISO );
  sec_add( SEC_OBJECT, "query",   THISO );

  sec_add( SEC_FILENAME, "config", TadjiCmd );
  sec_add( SEC_FILENAME, "query",  TadjiCmd );

  sec_add( SEC_FILENAME, "query",  HelpCmd );  
  sec_add( SEC_FILENAME, "query",  TCmdsCmd );

  sec_lock();
}

init()
{
  if(THISP != ENV(THISO))  // this is for cleanliness, not security.
    return;

  add_action("safety_net", "tadji"); /* hardwire this one, just in case    */
  add_action("route_action", "", 2); /* but of course they can override it */
  add_action("panic", "UNTADJI");    /* hardwire this one, just in case    */
}

static status
panic(string arg)
{
  write( "Destroying tadji!\n" );
  destruct(THISO);
  return 1;
}

/* Called as an action, routes the command to the right place */
status
route_action(string arg)
{
  string name, arg1;

  if(!arg || sscanf(arg, "%s %s", name, arg1) != 2)
    name = arg;    
  
  return do_command(name, arg1);
}

/* Always give them the `tadji` command, just in case. */
status
safety_net(string arg)
{
  if( !sec_check_command( "command" ) )
    return 0;

  return TadjiCmd->do_cmd("tadji", arg);
}



/****************************************/

status
query_prevent_shadow()
{
  return 1;
}

void 
clear_command_list ()
{
  if(!sec_check_command( "config" ))
    return;
  tadji::clear_command_list();
}

status 
add_config_file ( string filename )
{
  if(!sec_check_command( "config" ))
    return 0;
  return tadji::add_config_file( filename );
}

status 
add_config_lines ( string *lines )
{
  if(!sec_check_command( "config" ))
    return 0;
  return tadji::add_config_lines( lines );
}

status 
do_command ( string name, string arg )
{
  if( !sec_check_command(  "command"  ) )
    return 0;
  return tadji::do_command( name, arg );
}


string 
default_config_file ()
{
  if( !sec_check_command(  "query"  ) )
    return 0;
  return tadji::default_config_file();
}

status 
has_command ( string name )
{
  if( !sec_check_command(  "query"  ) )
    return 0;
  return tadji::has_command( name );
}

mixed 
query_command ( string name )
{
  if( !sec_check_command(  "query"  ) )
    return 0;
  return tadji::query_command( name ) + ({ });
}

string *
query_commands ()
{
  if( !sec_check_command(  "query"  ) )
    return 0;
  return tadji::query_commands() + ({ });
}
