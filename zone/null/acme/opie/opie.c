/*
** OPIE -- Object Parsing Interface Engine
**
** by Zamboni@EOTL  950626
**
*/

#include <acme.h>
#include OpieInc

#define Default_Engine opieAcmeSpec

/*
** handles[eng_file] = ({ eng_object, closure })
**   eng_file   - the filename of a parsing engine
**   eng_object - the object pointer used to make the closure
**   closure    - a closure bound to eng_object->OpieFunc()
**
** The whole point is to only have to resolve the object and function
** references once for each object, unless the object is destroyed, in
** which case we need to remake the closure.
** 
** This is only used for the main parsing function.  The other
** routines aren't used often enough to warrant caching.
** 
** Is it really worth doing all this?  Is the overhead more than the
** savings?  I should do some timing tests to see.  If Opie is used
** often enough, the functions may be in the driver's cache anyway, in
** which case this is all worthless.  
** 
*/
mapping handles;

create()
{
  seteuid(getuid());
  handles = ([ ]);
}

closure
engine_handle( string name )
{
  closure func;
  
  // Do we have it cached?  Is the object pointer still good?
  if( member( handles, name )
      && objectp(handles[name][0]) )
    return handles[name][1];  // return the closure

  // This will load the object too
  func = symbol_function( opieFunc, name );

  // If we got it, toss it in the cache
  if( closurep( func ) )
    handles[name] = ({ find_object( name ), func });

  return func;
}


/*
** Return filename of parse engine to use.
*/
varargs string 
choose_engine(object client)
{
  string engine;
  
  /* Check the user's preference */
  if(THISP->_IsVariableSet(opieVar)
     && (engine = THISP->_ExpandVariable(opieVar)))
  {
    if(engine == "0")
      printf(
  "The %s environment variable needs a parameter.\n"
  "Try: \n"
  "  setv %s <filename>\n"
  "where <filename> is the name of the parsing engine you want to use.\n",
	     opieVar, opieVar);
    else
      return engine;
  }

  if(!client)
    client = previous_object();

  /* else, Check the tool's default */
  if(engine = client->query(opieVar))
    return engine;

  /* Maybe it doesn't use query() */
  if(engine = client->query_opie())
    return engine;

  /* else, Use the global default */
  return Default_Engine;
}


/*
** This is the main function that everything calls.
*/
varargs object *
opie(string arg, string priorities)
{
  closure handle;

  handle = engine_handle( choose_engine() );
  if(!closurep( handle ))
    return 0;

  return funcall(handle, arg, previous_object(), priorities);
}

/*
** Use this if you want just a single object
*/
varargs object
opie1(string arg, string priorities)
{
  object *obs;
  closure handle;

  handle = engine_handle( choose_engine() );
  if(!closurep( handle ))
    return 0;

  obs = funcall(handle, arg, previous_object(), priorities);
  return (pointerp(obs) && sizeof(obs)) ? obs[0] : 0;
}

/*
** Return values can be either a single object
** or an array of objects.
*/
string
help()
{
  string engine, help;

  engine = choose_engine();
  if(!engine)
    return 0;

  help = call_other(engine, opieHelpFunc);
  if(!help)
    return "No help available.\n";

  if((searcha(help, ' ') > -1) || (searcha(help, '\n') > -1))
    return help;
  
  if(file_size(help) > 0)
    return read_file(help);

  return help;
}

/*
** For setting variables
*/
status
set(mixed key, mixed value)
{
  string engine;

  engine = choose_engine();
  if(!engine)
    return 0;

  call_other(engine, opieSetFunc, key, value);
  return 1;
}

status
query_prevent_shadow()
{
  return 1;
}
