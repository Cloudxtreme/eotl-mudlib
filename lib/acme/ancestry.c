
#include <acme.h>
inherit AcmeStrings;
#include AcmeStringsInc

/*
** Equivalent to (load_name(ob) == progname)
** but can be used in closures
*/
status
from_template(object ob, string progname)
{
  return munge_filename(load_name(ob)) == progname;
}


