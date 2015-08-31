
#include <acme.h>
private inherit AcmeArray;
private inherit AcmeShadow;
private inherit AcmeStrings;
#include AcmeArrayInc
#include AcmeShadowInc
#include AcmeStringsInc

/*
** This is the same as function_exists, only with the arguments
** reversed so it can be used in a closure.  (And it accept filename too)
*/
string
exists_function(mixed ob, string func)
{
  if(stringp(ob))
    ob = find_object(ob);

  if(!objectp(ob))
    return 0;
  
  return function_exists(func, ob);
}


/*
** The efun function_exists looks through the inheritance tree.
** This counteracts that and tells you if <func> is declared
** explicitly in <ob_file>.
*/
status
function_really_exists(string ob_file, string func)
{
  object ob;
  string parent;
  return ((ob = find_object(ob_file))
	  && (parent = function_exists(func, ob))
	  && (munge_filename(parent) == munge_filename(file_name(ob))));
}


/*
** The efun function_exists is very useful, but isn't always
** sufficient.  It doesn't account for shadows, nor can it list
** all definitions of a function in an inheritance tree.
** The following two functions provide those services.
*/

/*
** Returns all files which declare the function <func> which might get
** invoked when trying to call <func> in <ob>.  This takes shadows and
** inheritance into account.
*/
string *
all_function_defs(object ob, string func)
{
  string *shadows;
  string *parents;
  
  if(!ob || !objectp(ob) || !func || !stringp(func))
    return 0;

  shadows = map_array(all_shadows(ob), #'file_name);
  parents = inherit_list(ob);

  shadows = filter_array(shadows, #'exists_function, func);
  parents = filter_array(parents, #'function_really_exists, func);

  return map_array(reverse_array(shadows) + parents, #'munge_filename);
}

/*
** Returns the file which declares the function <func> which would get
** invoked first when trying to call <func> in <ob>.  This takes
** shadows and inheritance into account.
** This is faster than all_defs, so use this if you really only want 
** the first declaration.
*/
string
top_function_def(object ob, string func)
{
  string *shadows;

  if(!ob || !objectp(ob) || !func || !stringp(func))
    return 0;

  shadows = map_array(all_shadows(ob), #'file_name);
  shadows = filter_array(shadows, #'exists_function, func);

  if(sizeof(shadows))
    return munge_filename(shadows[sizeof(shadows)-1]);
  return munge_filename(function_exists(func, ob));
}
