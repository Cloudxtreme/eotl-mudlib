
#include <acme.h>
#define HELP_FILE OpieDir "i/tracer.help"

create()
{
  seteuid(getuid());
}

object *
evaluate_object_spec(string arg, object tool)
{
  mixed val;
  val = "/obj/tool/my_trace.c"->parse_list(arg);
  if (pointerp(val))
    return filter_array(val, #'objectp);
  else
    return ({ val });
}

string
object_spec_help()
{
  return read_file(HELP_FILE);
}

