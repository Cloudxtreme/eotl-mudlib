
#include <acme.h>

inherit AcmeStrings;
inherit AcmeAncestry;

#include AcmeStringsInc
#include AcmeAncestryInc

/*
** Returns a list of all objects shadowing <ob>.
** The first item in the list is the object closest to <ob> in the
** shadow chain.  (i.e.  was the first one shadowing, has the lowest
** precedence)
*/
object *
all_shadows(object ob)
{
  object *list;

  if(!ob)
    return 0;
  
  list = ({ });
  ob = shadow(ob, 0);
  while(ob)
  {
    list += ({ ob });
    ob = shadow(ob, 0);
  }
  return list;
}

/*
** If you expect someone to have a shadow from a particular template,
** use this to find all instances of it which are shadowing them.
*/
object *
find_shadows(object host, string progname)
{
  object *list;
  list = all_shadows(host);
  list = filter_array(list, #'from_template, munge_filename(progname));
  return list;
}
