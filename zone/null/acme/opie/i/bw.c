
#define HELP_FILE "/usr/bw/moejoe/bw.obj"

create()
{
  seteuid(getuid());
}

object *
evaluate_object_spec(string arg, object tool)
{
  return "/usr/bw/moejoe/cmd.c"->eval_ospec(arg);
}

string
object_spec_help()
{
  return read_file(HELP_FILE);
}
