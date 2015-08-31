/*
    NAME

        sentinel.c - Acme Event Sentinel

    DESCRIPTION

        This object collects and operates upon events.  Given an event (see
        below for a listing), the sentinel can check if it is interested in
        acting on the event based on player that triggered it.  The action it
        then calls can be in either AcmeCmd spec, or a bastard version of it.
        (See Aleron if you want to use the bastard version...it's a bit much
        to discuss here...or figure it out from the do_cmd() function.)

        Currently, the action files must be in the same directory as the
        sentinel, but this may change in the future if the sentinel is moved
        onto a tadji-style command mangement system.

        Also currently, there is no interface.  If you want to do something
        with the sentinel, you'll have to patch or Call it.  The interface
        functions are:

          load_db()         - load the sentinel's action database
          save_db()         - save the sentinel's action database
          view_db()         - view the sentinel's action database
          add_target()      - add a target to the action database
          remove_target()   - remove a target from the action database
          add_targ_i()      - add a target to the action database with a
                              customized data-passing format

        add_target() and remove_target() add the data in a format that
        an AcmeCommand would know how to deal with.

        See sentinel.h for more details.

        Lastly, only users listed in a file named "allowed" in the same
        local directory as the sentinel will be able to access the interface
        commands.

    NOTES

        Valid events are:
          "create"      - this event is triggered when the sentinel is
                          created.
                        - the target that it matches for can be anything
                        - this can be useful for loading any servers or
                          objects that we need loaded at sentinel creation
                          time.

          "destruct"    - this even is triggered when the sentinel is
                          destroyed.
                        - the target that it matches for can be anything

          "site"        - this event is triggered when someone logs in
                        - the target that it matches for is the site name
                          (or a subset of the site name) of the player that
                          triggered it

          "logon"       - this event is triggered when someone logs in.
                        - the target it matches for is the name of the player
                          logging in

          "reconnect"   - this event is triggered when a player reconnects
                        - the target it matches for is the name of the player
                          reconnecting

          "logout"      - this event is triggered when a player logs out
                        - the target it matches for is the name of the player
                          logging out

          "disconnect"  - this event is triggered when a player disconnects
                        - the target it matches for is the name of the player
                          disconnecting

          "death"       - this event is triggered when  a player dies
                        - the target it matches for is the name of the player
                          that got killed

          "pkill"       - this event is triggered when a player kills another
                          player
                        - the target it matches for is the name of the player-
                          killer

        For example, we could be doing something like this:                 

            On event "logon",
            check if the person logged on is "zamboni" and,
            if so "dest" him.

          If you're using the tadji Call, the line you would type would
          look like this:

            Call sentinel add_target logon zamboni dest

        or, something like:

             On event "site",
             check if the site matches with "princeton.edu" and,
             if so, "message" that player with a "Go Tigers!"

          If you're using the tadji Call, the line you would type would
          look like this:

             Call sentinel add_target site princeton.edu message 'Go Tigers!'


        CAVEAT: The target may not always exist by the time execution
                trickles down to the action call.  This is ok with most
                AcmeCmds since they usually attempt to find the target
                themselves (usually based on an ospec description) but
                if not, the action command should be written to deal with
                that gracefully.

        SECURITY CAVEAT: The sentinel tries to be as secure as possible
                         by using AcmeCallSecurity, but it makes no
                         assurances about the modules.  All it does currently
                         is refuse to call a module if it detects that that
                         module is being shadowed.

    LAST MODIFIED

        Aleron 960925

*/

#include <acme.h>

inherit AcmeError;
inherit AcmeFile;
inherit AcmeString;
inherit AcmeForceLoad;
inherit AcmeShadow;
static  inherit AcmeCallSecurity;

#include AcmeErrorInc
#include AcmeFileInc
#include AcmeStringInc
#include AcmeShadowInc
#include AcmeForceLoadInc
#include AcmeCallSecurityInc

#include AcmeSentinelInc

#define RN(o)  (o?o->query_real_name():"")

varargs private void event_handler( string event, object targ, mixed extra );

/*  ActionDB records are in the following format:
 |  ([
 |     event:
 |     ([
 |        target:
 |        ({
 |           ({ action_ob, ({ params }) }),  <-- this is a command
 |           ({ action_ob, ({ params }) }),
 |        })
 |     ]),
 |  ]);
 |
 */
mapping ActionDB;

// closure to check of a module is in an action...used in GetModules macro
private static closure modeq_cl;

private static status initialized;


//--------------------------------- security --------------------------------//


static object *
sec_caller_stack()
{
  int i, size;
  object ob;
  object *list;
  list = ( initialized || THISI ) ? ({ THISI }) : ({ });
  
  for(i = 0, size = efun::caller_stack_depth(); i < size; i++)
  {
    ob = efun::previous_object(i);
    if(efun::member(list, ob) == -1)
      list += ({ ob });
  }
  return list;
}

static void
setup_security()
{
   closure  load_sec_cl;
   string  *allowed;
   int      ret;

   if ( ret = file_exists( AllowedFile ) )
      allowed = grab_file( AllowedFile );  // file exists, grab it
   else if ( ret = -1 )
      error( ERR_ERROR,                    // no permission, error
         sprintf( "Insufficient permissions to read %s", AllowedFile ) );
   else
      allowed = ({});                      // no file, initalize to no one

   load_sec_cl = lambda( ({ 'uid, }),
      ({ #'sec_add, SEC_UID, SENTINEL, 'uid }) );

   sec_clear( SENTINEL );
   sec_add( SEC_FILENAME, SENTINEL, "/secure/master"         );
   sec_add( SEC_FILENAME, SENTINEL, AcmeTadjiDir "tool"      );
   sec_add( SEC_FILENAME, SENTINEL, "/usr/zamboni/cmds/Call" );   
   sec_add( SEC_FILENAME, SENTINEL, AcmeSentinel             );
   sec_add( SEC_FILENAME, SENTINEL, AcmeSentinelDir "load"   );
   sec_add( SEC_OBJECT,   SENTINEL, THISO                    );
   map_array( allowed, load_sec_cl );

   sec_clear( SEC_MANAGE );
   sec_add( SEC_UID,      SEC_MANAGE, "aleron"                 );
   sec_add( SEC_FILENAME, SEC_MANAGE, AcmeTadjiDir "tool"      );
   sec_add( SEC_FILENAME, SEC_MANAGE, "/usr/zamboni/cmds/Call" );  
   sec_add( SEC_FILENAME, SEC_MANAGE, AcmeSentinel             );
   sec_add( SEC_FILENAME, SEC_MANAGE, AcmeSentinelDir "load"   );
   sec_add( SEC_OBJECT,   SEC_MANAGE, THISO                    );
   sec_lock();
}


//--------------------------------- database --------------------------------//

varargs void
load_db( string file )
{
   int i;
   status dirty;  // do we need to save out a new sentinel DB file?

   AllowedTest();

   dirty = 0;
   if ( !restore_object( file || DefDBFile ) )
      ActionDB = ([]);
   for ( i = 0; i < nEvents; ++i )
      if ( !ActionDB[ EventTypes[ i ] ] )
      {
         dirty = 1;
         ActionDB[ EventTypes[ i ] ] = ([]);
      }

   if ( dirty )
      save_db( file );
}

varargs void
save_db( string file )
{
   AllowedTest();

   save_object( file || DefDBFile);
}

varargs void
view_db( string event, string targ, string module )
{
   if ( !NullStr( event ) )
      if ( ValidEvent( event ) )
         if ( !NullStr( targ ) )
            if ( TargInDB( event, targ ) )
               if ( !NullStr( module ) )
                  printf( "ActionDB[ %s ][ %s ][ %s ] =\n%O\n",
                         event, targ, module,
                         GetModules( event, targ, module ) );
               else
                  printf( "ActionDB[ %s ][ %s ] =\n%O\n", event, targ,
                         ActionDB[ event ][ targ ] );
            else
               printf( "ActionDB[ %s ][ %s ] =\n%O\n", event, targ,
                      ActionDB[ event ][ targ ] );
         else
            printf( "ActionDB[ %s ] =\n%O\n", event, ActionDB[ event ] );
      else
         error( ERR_ERROR, sprintf( "Invalid event type %O.\n"
            "Valid types are: %s", event, ValidEventStr ) );
   else
      printf( "ActionDB =\n%O\n", ActionDB );
}

// Only use this if you know what the heck you're doing
status
add_targ_i( string event, string targ, string module, mixed data )
{
   AllowedTestRet( 0 );

   if ( !ValidEvent( event ) )
      return error( ERR_ERROR, sprintf( "Invalid event type %O.\n"
         "Valid types are:\n%s.", event, ValidEventStr ), 0 );
   if ( !ValidTarg( targ ) )
      return error( ERR_ERROR, "Null target.\n", 0 );
   if ( !ValidModule( module ) )
      return error( ERR_ERROR,
         sprintf( "Cannot find module %O.\n", ModuleName( module ) ), 0 );

   if ( !member( ActionDB[ event ], targ ) )
      ActionDB[ event ][ targ ] = ({});

   if ( data )
      ActionDB[ event ][ targ ] += ({ ({ module, data }) });

   return 1;
   
}

varargs status
add_target( string event, string targ, string module, string cmd_line )
{
   AllowedTestRet( 0 );

   if ( !ValidEvent( event ) )
      return error( ERR_ERROR, sprintf( "Invalid event type %O.\n"
         "Valid types are:\n%s.", event, ValidEventStr ), 0 );
   if ( !ValidTarg( targ ) )
      return error( ERR_ERROR, "Null target.\n", 0 );
   if ( !ValidModule( module ) )
      return error( ERR_ERROR,
         sprintf( "Cannot find module %O.\n", ModuleName( module ) ), 0 );

   if ( !member( ActionDB[ event ], targ ) )
      ActionDB[ event ][ targ ] = ({});

   if ( cmd_line )
      ActionDB[ event ][ targ ] += ({ ({ module, ({ cmd_line }) }) });
   else
      ActionDB[ event ][ targ ] += ({ ({ module }) });

   return 1;
}

varargs status
remove_target( mixed event, mixed targ, mixed module )
{
   int i, max;

   AllowedTestRet( 0 );

   if ( ValidEvent( event ) )
      if ( ValidTarg( targ ) )
         if ( TargInDB( event, targ ) )
            if ( !NullStr( module ) )
            {
               max = sizeof ( ActionDB[ event ][ targ ] );
               for ( i = 0; i < max; i++ )
                  if ( ActionDB[ event ][ targ ][ i ][ 0 ] == module )
                  {
                     // valid event, targ and module
                     ActionDB[ event ][ targ ] = deletea(
                        ActionDB[ event ][ targ ], i, i );

                     // if targ has no actions anymore, delete targ too
                     if ( sizeof( ActionDB[ event ][ targ ]) == 0 )
                        m_delete( ActionDB[ event ], targ );
                     break;
                  }

               if ( i == max )   // didn't find a match
               {
                  // valid event, targ, invalid module
                  return error( ERR_ERROR, sprintf(
                     "There is no %O module for target %s of event %s",
                     module, targ, event ), 0 );
               }
            }
            else
            {
               // valid event, targ
               m_delete( ActionDB[ event ], targ );
            }
         else
         {
            // valid event, invalid targ
            return error( ERR_ERROR, sprintf(
               "There is no %O target for event %s", targ, event ), 0 );
         }
      else
      {
         // valid event
         ActionDB[ event ] = ([]);
      }
   else
   {
      // invalid event
      return error( ERR_ERROR, sprintf( "Invalid event type %O.\n"
         "Valid types are: %s", event, ValidEventStr ), 0 );
   }

   return 1;
}


//---------------------------------- events ---------------------------------//

status site_match ( string s1, string s2 )
{
   string *s1a, *s2a;
   int i, j;

   s1a = explode ( s1, "." );
   s2a = explode ( s2, "." );
   i = sizeof ( s1a ) - 1;
   j = sizeof ( s2a ) - 1;
   while ( i >= 0 && j >= 0 ){
      if ( lower_case ( s1a[i] ) != lower_case ( s2a[j] ) )
         return 0;
      i--;  j--;
   }
   return 1;
}

private status
add_notification()
{
   if ( !NOTIFIER->add_notify( THISO ) )
      return error( ERR_ERROR, "Failed to add to notifier as client.", 0 );
   CHARON->add_object( 1 );  // CHARON doesn't give sucess status
   return 1;
}

private status
remove_notification()
{
   if ( !NOTIFIER->remove_notify() )
      return error( ERR_ERROR, "Failed to remove from notifier as client.",
                   0 );
   CHARON->remove_object( 1 );  // CHARON doesn't give sucess status
   return 1;
}

void
notify_logon( object who, int recon )
{
   event_handler( eSite, who, query_ip_name( who ) );
   event_handler( ( recon ? eReconnect : eLogon ), who, RN( who ) );
}

void
notify_logout( object who, int discon )
{
   event_handler( ( discon ? eDisconnect : eLogout ), who, RN( who ) );
}

void
notify_death( mixed data )
{
   object  who, killer;
   string  cause;

   who = data[0];
   killer = data[1];
   cause = data[2];
   event_handler( eDeath, who, ({ killer, RN( who ), RN( killer ) }) );
   if ( interactive( killer ) )  // if the killer is a player
      event_handler( ePKill, killer, ({ who, RN( killer ), RN( who ) }) );
}

void
do_cmd( mixed cmd, object targ, mixed extra )
{
   object ob;

   if ( ob = force_load ( ModuleName( cmd[ 0 ] ) ) )
   {
      if ( sizeof( all_shadows( ob ) ) )
      {
         error( ERR_ERROR, sprintf( "%O is being shadowed.  Aborting call.\n",
                                   ob ) );
         return;
      }
      apply( #'call_other, ob, "do_cmd",
         ({ cmd[ 0 ] }) +
         ( sizeof( cmd ) == 2 ? cmd[ 1 ] : ({}) ) +
         ({ targ }) +
         ( extra ? ( pointerp( extra ) ? extra : ({ extra }) ) : ({}) )
      );
   }
}

void
do_cmds( string key, mixed cmds, object targ, mixed extra )
{
   map_array( cmds[ key ], "do_cmd", THISO, targ, extra );
}

varargs private void
event_handler( string event, object targ, mixed extra )
{
   mixed actions;

   if ( !ActionDB )   // this is for debugging..ActionDB SHOULD exist by now
   {
      remove_notification();

      // debugging
/*
      write_file( DebugFile, sprintf( "----------\nActionDB does not exist.\n"
         "Call Stack: %O\n----------\n", sec_caller_stack() ) );
*/

      return error( ERR_ERROR, "ActionDB does not exist." );
   }

   actions = ActionDB[ event ];
   switch( event )
   {
   case eCreate:      // the sentinel is created
      // no actions needed
      break;

   case eDestruct:    // the sentinel is destroyed
      // no actions needed
      break;

   case eSite:        // a site logs on -- extra is the ip
      actions = filter_mapping( actions, "site_match", THISO, extra );
      break;

   case eLogon:       // a player logs on -- extra is the player name
      // FALL THROUGH 

   case eReconnect:   // a player reconnects -- extra is the player name
      // FALL THROUGH 

   case eLogout:      // a player logs out -- extra is the player name
      // FALL THROUGH 

   case eDisconnect:  // a player disconnects -- extra is the player name
      actions = filter_mapping( actions, #'==, extra );
      break;

   case eDeath:       // a player dies -- extra is ({ killer, player name,
                      //                              killer name })
      // FALL THROUGH 

   case ePKill:       // a player kills another player
                      //     -- extra is ({ victim, killer name, victim name })
      actions = filter_mapping( actions, #'==, extra[ 1 ] );
      break;

   default:
      return error( ERR_ERROR, sprintf( "Invalid event type %O.\n"
         "Valid types are:\n%s.", event, ValidEventStr ) );
   }

   map_mapping( actions, "do_cmds", THISO, actions, targ, extra );
}


//----------------------------------- misc ----------------------------------//

// disallow shadows
status
query_prevent_shadow()
{
   return 1;
}

void
create()
{
   seteuid( getuid() );

   add_notification();
   setup_security();
   load_db();

   event_handler( eCreate );

   modeq_cl = lambda( ({ 'm1, 'm2 }), ({ #'==, ({ #'[, 'm1, 0 }), 'm2 }) );

   // we are now initialized, turn on full security
   initialized = 1;
}

status
destruct_signal()
{
   event_handler( eDestruct );
   return 0;   // ok, let me be destructed
}

#ifdef SENT_NEVER  //-------------------- cut out -----------------------------
// for debug purposes
#define ESTRING ({ "usage", "note", "warning", "error" })
varargs mixed
error( int severity, string msg, mixed ret )
{
   // log to a file
   write_file( DebugFile, sprintf( "%s: %s\n", ESTRING[ severity ], msg ) );
   return ::error( severity, msg, ret );
}
#endif  // SENT_NEVER  //---------------- cut out -----------------------------

no_clean_up(int ref)
{
return 1;
}
