#include <acme.h>
#include AcmeErrorInc
#include AcmeTadjiInc
#include OpieClient

#define INDENT "    "
#define NO_HELP "Sorry.  No help is available for that command.\n"

/*
** Usage for this command
*/
string *
cmd_usage(string func)
{
  return ({ "<commandname>", "ospec" });
}

/*
** Short description of this command
*/
string cmd_desc(string func)
{
  return( "get help on a tadji command" );
}


/*
** Help on this command
*/
string
cmd_help(string func)
{
  return 
    "Shows the help for a modular command which conforms to  "
    "Acme, BW, or Concept help protocols.  Also provides help on your "
    "Opie selected object specification syntax.";
}


create()
{
  seteuid(getuid());
}


static void
show_help(string message)
{
  if(stringp(message))
    THISP->more_string(strformat(message));
  else
    write(NO_HELP);
}

static string
format_acme_help(string name, string *usage, string help, string path, string
  func)
{
  int i, size;
  string output, desc;

  catch(desc = path->cmd_desc(func));
  output = sprintf("NAME\n%s%s%s\n", INDENT, name, desc ? " - " + desc : "");
  
  if(pointerp(usage) && (size = sizeof(usage)))
  {
    output += "SYNOPSIS\n";
    for(i=0; i < size; i++)
      output += sprintf("%s%s %s\n", INDENT, name, usage[i]);
    
  } else
    if(!stringp(help))
      return NO_HELP;
  
  if(stringp(help))
    output += sprintf("DESCRIPTION\n%s", strformat(help, INDENT));

  output += sprintf("PATH\n%s%s", INDENT, path || "Unknown");
  
  return output;
}

static string
format_concept_help(string name, string usage, string help, string path)
{
  return format_acme_help(name, (stringp(usage) ? ({ usage }) : 0), help,
    path, 0);
}

static void
show_bw_help(mixed cmd_info)
{
  call_other(CmdFile(cmd_info), "help", 0);
  printf("PATH\n%s%s\n", INDENT, CmdFile(cmd_info) || "Unknown");
}


status
do_cmd(string name, string cmd_name)
{
  object device;
  mixed cmd_info;

  if(cmd_name == "ospec")
  {
    THISP->more_string(strformat(opie_help()));
    return 1;
  }
  
  device = previous_object();
  
  if(!device->has_command(cmd_name))
    return 0;
  
  cmd_info = device->query_command(cmd_name);
  if(!pointerp(cmd_info) || (sizeof(cmd_info) < 4))
    return AcmeError->error(ERR_ERROR, "error in command information", 1);
  
  switch(CmdStyle(cmd_info))
  {
   case StyleBW:
    show_bw_help(cmd_info);
    return 1;

   case StyleAcme:
    show_help(format_acme_help(cmd_name,
			       call_other(CmdFile(cmd_info), 
					  "cmd_usage", 
					  CmdFunc(cmd_info)),
			       call_other(CmdFile(cmd_info), 
					  "cmd_help", 
					  CmdFunc(cmd_info)),
                               CmdFile(cmd_info), CmdFunc(cmd_info)));
    return 1;

   case StyleConcept:
    show_help(format_concept_help(cmd_name,
				  call_other(CmdFile(cmd_info), 
					     "_cmd_usage", 
					     CmdName(cmd_info)),
				  call_other(CmdFile(cmd_info), 
					     "_cmd_help", 
					     CmdName(cmd_info)),
                                  CmdFile(cmd_info)));
    return 1;
    
   case StyleNone:
    write(NO_HELP);
    return 1;
  }
  return 1;
}
