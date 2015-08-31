
#include <acme.h>
#include AcmeCmdInc

void
create() {}

string
cmd_help(string func) {}

string *
cmd_usage(string func) {}

status
do_cmd(string name, string cmd_name) {}


varargs string
UsageString(string name, string func)
{
  string *usage;
  string output;
  int i, size;

  if(!name)
    name = query_verb() || "";

  usage = cmd_usage(func || "do_cmd");
  
  output = "Usage:\n";
  
  for(i=0, size=sizeof(usage); i < size; i++)
    output += sprintf("%s%s %s\n", "    " , name, usage[i]);

  return output;
}

void
Output(string message)
{
  THISP->more_string(strformat(message));
}

