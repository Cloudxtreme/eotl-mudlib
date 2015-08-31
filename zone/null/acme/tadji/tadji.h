#ifndef ACME_TADJI_INC
#define ACME_TADJI_INC

#include AcmeFileInc
#include "simple.h"

#define TADJI_CONFIG_FILE_VAR     "tadji"
#define HOMEDIR                   (PLAYERROOT + PRNAME + "/")
#define TADJI_DEFAULT_CONFIG_FILE (HOMEDIR + ".tadji")


string  default_config_file ();
void    clear_command_list ();
status  add_config_file  ( string  filename );
status  add_config_lines ( string *lines );
status  do_command       ( string  name, string arg );
status  has_command      ( string  name );
mixed   query_command    ( string  name );
string *query_commands ();


#endif
