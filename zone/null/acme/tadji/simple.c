
#include <acme.h>

inherit AcmeError;
inherit AcmeFile;
#include "simple.h"
#include AcmeErrorInc
#include AcmeFileInc

private status parse_config_i(string *config, mapping map, string *stack);

/* regularizes filenames based on location of current config file, or
   on CWD if there is none 
*/
private string
munge_path(string file, string *stack)
{
  int size;
  
  if(file[0] == '/')
    return file;

  if(file[0] == '~')
    return THISP->ExpandPath(file);

  if(size = sizeof(stack))
    return THISP->ExpandPath(dirname(stack[size-1]) + "/" + file);

  return expand_path(file, THISP);
}


/*********************** Look up a command *********************/

static status
has_command_m(mapping map, string name)
{
  return member(map, name);
}

static mixed
query_command_m(mapping map, string name)
{
  return map[name];
}

static string *
query_commands_m(mapping map)
{
  return m_indices(map);
}

static status
do_command_m(mapping map, string name, string arg)
{
  mixed cmd, args;
  int i;

  if(!member(map, name))
    return 0;

  cmd = map[name];
  
  switch (CmdStyle(cmd))
  {
   case StyleBW:
    return call_other(CmdFile(cmd), CmdFunc(cmd), THISP, arg);
   case StyleAcme:
    return call_other(CmdFile(cmd), CmdFunc(cmd), name, arg);
   case StyleConcept:
   case StyleNone:
    return call_other(CmdFile(cmd), CmdFunc(cmd), arg);
   default:
    error(ERR_ERROR, "unknown calling style: "+CmdStyle(cmd));
  }
  return 1;
}

/*****************************************************************/

private status
add_to_mapping(mapping map, string *args)
{
  int size, i, style;
  string *filez;
   status name_is_pattern;
  string name, name_pattern, func_pattern;

  size = sizeof(args);
  if(!size)
    return error(ERR_ERROR, "Bad args to add_to_mapping (simple.c)", 0);

  filez = THISP->ExpandPattern(args[0], 1);

  if(!pointerp(filez) || !sizeof(filez))
    return error(ERR_WARNING, sprintf("No files match spec: %s", args[0]), 0);

  style = StyleAcme;
  func_pattern = "do_cmd";
  name_pattern = "%s.c";
  if(size > 1)
  {
    name_pattern = args[1];
    if(size > 2)
    {
      func_pattern = args[2];
      if(size > 3)
	style = searcha(STYLES, lower_case(args[3])) + 1;
    }
  }

  if(searcha(name_pattern, '%') == -1)
    name = name_pattern;   // For those single commands that give a name
  else
    name_is_pattern = 1;    


  size = sizeof(filez);  
  for(i = 0; i < size; i++)
  {
    if(name_is_pattern)
      sscanf(THISP->FileName(filez[i]), name_pattern, name);
    
    if(!member(map, name))
      map[name] = ({ name, filez[i], sprintf(func_pattern, name), style });
  }
  return 1;
}

private status
remove_from_mapping(mapping map, string *args)
{
  m_delete(map, args[0]);
  return 1;
}

private status
rename_in_mapping(mapping map, string *args)
{
  if(!member(map, args[0]) || member(map, args[1]))
    return 0;
  map[args[1]] = map[args[0]];  /* Copy it over */
  m_delete(map, args[0]);        /* Delete the old one */
  map[args[1]][0] = args[1];    /* Change the name inside the structure */
  return 1;
}


private status
add_bw_to_mapping(mapping map, string *args)
{
  return add_to_mapping(map, ({ args[0], "_%s.c", "cmd_%s", "bw" }));
}

private status
add_concept_to_mapping(mapping map, string *args)
{
  return add_to_mapping(map, ({ args[0], "%s.c", "do_%s", "concept" }));
}


private status
parse_config_line(string passedline, mapping map, int lineno, string *pstack)
{
  string line;      // A line in the config
  string *elements; // The elements of the line
  
  // Allow comments of the form   # blah  or   // blah
  if(!sscanf(passedline, "%s#", line) && !sscanf(passedline, "%s//", line))
    line = passedline;
    
  elements = explode(line, " ");
  elements = filter_array(elements, #'strlen);  // Remove blank entries ');

  if(!sizeof(elements))   // Nothing on this line
    return 1;

  if(sizeof(elements) == 1)
  {
    return error(ERR_WARNING, 
	  sprintf("Malformed directive on line %d: %s", lineno, elements[0]),
		 0);
  }
    
    switch (elements[0])
    {
     case "ADD":
     case "ACME":
      return add_to_mapping(map, elements[1..]);
      
     case "RM":
      return remove_from_mapping(map, elements[1..]);
      
     case "RENAME":
       if(sizeof(elements) < 3)
         return error(ERR_ERROR, 
                      sprintf("Too few arguments to RENAME on line %d: %s", lineno,
                              line), 0);

      return rename_in_mapping(map, elements[1..]);

     case "BW":
      return add_bw_to_mapping(map, elements[1..]);

     case "CONCEPT":
     case "COMMAND":
      return add_concept_to_mapping(map, elements[1..]);

     case "PACKAGE":
     case "INCLUDE":
       elements[1] = munge_path(elements[1], pstack);
       if(member(pstack, elements[1]) != -1)
         return error(ERR_ERROR,
                      sprintf("Recursive package specification on line %d: %s",
                              lineno, line), 0);
       return parse_config_i(grab_file(elements[1]), map, 
                             pstack + ({ elements[1] }));

     default:
      return error(ERR_WARNING, 
		   sprintf("Unrecognized directive on line %d: %s", lineno,
			   line), 0);

    }
}    


/*
   Given a configuration and a command mapping, updates the mapping.  
*/
private status
parse_config_i(string *config, mapping map, string *stack)
{
  int i, lines, success;
  
  success = 1;
  lines = sizeof(config);
  for(i=0; i < lines; i++)
  {
    success &= parse_config_line(config[i], map, i, stack);
  }
  return success;
}

/*
   Given a configuration and a command mapping, updates the mapping.  
*/
static status
parse_config(string *config, mapping map)
{
  return parse_config_i(config, map, ({ THISP->query_pwd()+"*current_dir*" }));
}

static status
parse_config_file(string file, mapping map)
{
  return parse_config_i(grab_file(file), map, ({ file }));
}

