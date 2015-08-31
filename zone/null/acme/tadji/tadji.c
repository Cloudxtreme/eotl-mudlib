/*
    NAME
        tadji.c - A command routing system

    DESCRIPTION

	clear_command_list()    - clears the command list
        default_config_file()   - returns the default config file
	add_config_file(file)   - processes a config file
	add_config_lines(lines) - processes an array of config lines

	do_command(name, arg)   - runs a command

	has_command(name)       - returns true if there is a 'name' command
	query_command(name)     - returns some info about the 'name' command
	query_commands()        - returns a list of all commands

    NOTES

    	"tadji" is a Lojban word.  Lojban words typically don't
	translate into English very well since the grammar is so
	different.  The closest I can come is that it has to do with
	something being a process/method/technique/approach/means for
	doing something else.  I thought it was an appropriate name
	for this package.

        This package currently provides a system of creating and
        maintaining an internal list of commands, and a way of calling
        the associated functions.  It does not provide any tools for
        maintaining a configuration, though such a layer could easily
        be added.  Even though configuration management is really a
        separate task, this package does have some simple routines for
        loading configuration files.  Eventually, those routines may
        be moved to an external command.

    LAST MODIFIED
        Zamboni 971124

*/

#include "acme.h"

static inherit AcmeTadjiDir "simple.c";
static inherit AcmeFile;

#include AcmeTadjiInc
#include AcmeFileInc

mapping commands;
static status initialized;

status
initialize_ok()
{
  return ( clonep(THISO) && !initialized );
}

create()
{
  if( !initialize_ok() )
    return;

  seteuid(getuid());
  commands = ([ ]);
  clear_command_list();
  add_config_file();
  initialized = 1;
}


/*--------------------------- default_config_file ---------------------------*/
/*
    Description:
      Select a default config file
    Parameters:
      None.
    Returns:
      the name of the default Tadji config file
    Notes:
      if $tadji is set, use whatever it is set to, otherwise 
      defaults to ~/.tadji
*/
string
default_config_file()
{
  return expand_path((THISP->_IsVariableSet(TADJI_CONFIG_FILE_VAR))
		     ? (THISP->_ExpandVariable(TADJI_CONFIG_FILE_VAR))
		     : (TADJI_DEFAULT_CONFIG_FILE), THISP );
}

/*--------------------------- clear_command_list ---------------------------*/
/*
    Description:
      Clears the internal command list
    Parameters:
      None.
    Returns:
      None.
    Notes:
      After clearing the commands, the standard set is added back in.
*/
void
clear_command_list()
{
  commands = ([ ]);
  add_config_lines(({ "ADD /zone/null/acme/tadji/cmds/*.c" }));
}

/*--------------------------- add_config_file ---------------------------*/
/*
    Description:
      Adds in the configuration contained in the file
    Parameters:
      filename        - the name of the file, or 0 for the default file
    Returns:
      True if there were no errors or warnings
    Notes:
      None.
*/
status
add_config_file(string filename)
{
  return parse_config_file(filename || default_config_file(), commands);
}

/*--------------------------- add_config_lines ---------------------------*/
/*
    Description:
      Adds in the configuration lines passed
    Parameters:
      lines           - an array of configuration lines
    Returns:
      True if there were no errors or warnings
    Notes:
      None.
*/
status
add_config_lines(string *lines)
{
  return parse_config(lines, commands);
}

/*--------------------------- do_command ---------------------------*/
/*
    Description:
      Adds in the configuration contained in the file
    Parameters:
      name    - the name of the command
      arg     - the argument string for the command
    Returns:
      true if a command claims the action.  See the manpage for
      add_action() for more details.
    Notes:
      None.  */
status
do_command(string name, string arg)
{
  return do_command_m(commands, name, arg);
}

/*--------------------------- has_command ---------------------------*/
/*
    Description:
      Query the command list to see if a command exists
    Parameters:
      name       - The name of the command
    Returns:
      true if the command exists
    Notes:
      None.
*/
status
has_command(string name)
{
  return has_command_m(commands, name);
}

/*--------------------------- query_command ---------------------------*/
/*
    Description:
      Get information about the command
    Parameters:
      name       - The name of the command
    Returns:
      An array containing:
        ({ 
	   (string) name,      - The name of the command
           (string) file,      - The file of the command object to call
	   (string) function,  - The function in the command object to call
	   (int)    style      - The calling style of the command
                                 (numbers defined in AcmeTadjiInc)
	})
      
    Notes:
      None.
*/
mixed
query_command(string name)
{
  return query_command_m(commands, name);
}

/*--------------------------- query_commands ---------------------------*/
/*
    Description:
      Get a list of all commands
    Parameters:
      None.
    Returns:
      The names of all loaded commands, in no particular order.
    Notes:
      None.
*/
string *
query_commands()
{
  return query_commands_m(commands);
}

