
#include <acme.h>
private inherit AcmeStrings;
#include AcmeStringsInc

/*
** Equivalent to (program_name(ob) == progname)
** but can be used in closures
*/
status
from_template(object ob, string progname)
{
  return munge_filename(program_name(ob)) == progname;
}


