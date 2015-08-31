#include <acme.h>

static inherit AcmeCmd;
static inherit AcmeError;
#include AcmeErrorInc
#include AcmeCmdInc

string *
cmd_usage(string func)
{
  return ({ "list" , "reset [file]" , "config <config line>" });
}

string
cmd_desc(string func)
{
  return( "basic interface to tadji config stuff" );
}

string
cmd_help(string func)
{
  return 
  "Handle some really basic things for the tadji\n"
  "  list   - lists the commands\n"
  "  reset  - clear the commands and reload your config file\n"
  "           (or some other config file instead)\n"
  "  config - manually add a configuration line\n"
  "           (See /zone/null/acme/tadji/config.example)";
}


status
do_cmd(string name, string arg)
{
  object tadji;
  string command, args;

  tadji = previous_object();

  if(arg && sscanf(arg, "%s %s", command, args) != 2)
    command = arg;

  switch(command)
  {
  case "config":
    if(!args || (args == ""))
      return error(ERR_ERROR, "You need to specify a configuration line", 1);

    if(tadji->add_config_lines(({ args })))
      write("Success.\n");
    return 1;

  case "list":
    THISP->more_string(sprintf("%-78#s\n", 
      implode(sort_array(tadji->query_commands(),
        #'>) 
       , "\n")));
    return 1;

  case "reset":
    tadji->clear_command_list();
    if(tadji->add_config_file(args))
      write(args 
       ? sprintf("Reset from %s\n", args) 
       : "Reset.\n");
    return 1;
  }
  FailUsage(name);
}
