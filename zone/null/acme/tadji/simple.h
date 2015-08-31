/*
   The structure of the command mapping elements is:
    ({ name, file, func, style })
   name  -- The name of the command
   file  -- The object file to call a function in
   func  -- The name of the function to call
   style -- a number for the calling style
*/

#define STYLES ({ "acme", "bw", "concept", "none" })
#define StyleAcme    1
#define StyleBW      2
#define StyleConcept 3
#define StyleNone    4

#define CmdName(ar)  (ar[0])
#define CmdFile(ar)  (ar[1])
#define CmdFunc(ar)  (ar[2])
#define CmdStyle(ar) (ar[3])


#define V_Success 1
#define V_Line    2
#define V_Cmd     4
#define V_Full    8
#define V_Total  15   /* Just for convenience */

static status  has_command_m    ( mapping map, string name );
static mixed   query_command_m  ( mapping map, string name );
static string *query_commands_m ( mapping map );
static status  do_command_m     ( mapping map, string name, string arg );
static status  parse_config     ( string *config, mapping map );
