//--------------------------------- defines ---------------------------------//
// Events
#define eCreate       "create"      // the sentinel is created
#define eDestruct     "destruct"    // the sentinel is destroyed
#define eSite         "site"        // a site logs on
#define eLogon        "logon"       // a player logs on
#define eReconnect    "reconnect"   // a player reconnects
#define eLogout       "logout"      // a player logs out
#define eDisconnect   "disconnect"  // a player disconnects
#define eDeath        "death"       // a player dies
#define ePKill        "pkill"       // a player kills another player

#define EventTypes    ({ eCreate,\
                         eDestruct,\
                         eSite,\
                         eLogon,\
                         eReconnect,\
                         eLogout,\
                         eDisconnect,\
                         eDeath,\
                         ePKill })

#define nEvents       (9)

// defined operation for AcmeCallSecurity
#define SENTINEL      "sentinel"

#define LocalDir      (dirname(munge_filename(file_name())))
#define DefDBFile     (sprintf("%s%s",LocalDir,"sentinel"))
#define ValidEventStr (sprintf("%s and %s",\
                         implode(EventTypes[0..(nEvents-2)],", "),\
                         EventTypes[(nEvents-1)]))

#define AllowedFile   (LocalDir+"allowed")

#define DebugFile     (LocalDir+"debug")       // for debug purposes


//---------------------------------- macros ---------------------------------//

// This will go away if the sentinel ever gets any tadji-like command
// management.
#define ModuleName(x) (munge_filename(sprintf("%s%s",LocalDir,x)))

#define NotAllowedStr        "You are not allowed to do that."
#define AllowedTest()        if (!sec_check_command(SENTINEL))\
                                return error( ERR_ERROR, NotAllowedStr )
#define AllowedTestRet(ret)  if (!sec_check_command(SENTINEL))\
                                return error( ERR_ERROR, NotAllowedStr, ret )

#define NullStr(s)     (!(s) || (""==s))
#define ValidEvent(e)  (!NullStr(e) && (member(EventTypes,(e))!=(-1)))
#define ValidTarg(t)   (!NullStr(t))
#define ValidModule(m) (!NullStr(m) && (file_exists(ModuleName(m))))

#define TargInDB(e,t)      (member(ActionDB[e],t))
#define GetModules(e,t,m)  (filter_array(ActionDB[e][t],modeq_cl,m))
#define ModuleInDB(e,t,m)  (sizeof(GetModules(e,t,m)))


//----------------------------- public functions ----------------------------//

// load the sentinel's action database
// loads from DefDBFile if file isn't specified
varargs void load_db( string file );

// save the sentinel's save database
// saves to DefDBFile if file isn't specified
varargs void save_db( string file );

// views the sentinel's action database
// drills down in the database depending on the passed parameters
// ie:  view_db( eCreate, foo );  would view all actions for target foo in
//                                the create event
varargs void view_db( string event, string targ, string module );

// add an action for a target in whatever data-passing format you want
// don't use this unless you know what you're doing
status add_targ_i( string event, string targ, string module, mixed data );

// add an AcmeCmd spec action for a target
varargs status add_target( string event, string targ, string module,
                           string cmd_line );

// remove a items from the database
// drills down on the database depending on the passed parameters
// ie:  view_db( eDestruct );  would remove all targets and actions for
//                             the destruct event
varargs status remove_target( mixed event, mixed targ, mixed module );
