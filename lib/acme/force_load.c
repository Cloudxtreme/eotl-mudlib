/*
    NAME
        force_load.c - Force objects to be loaded
    DESCRIPTION
        force_load()        - forces an object to load
        force_load_array()  - forces each object in an array to load
    NOTES
        None.
    LAST MODIFIED
        Zamboni 960111

*/


#include <acme.h>
private inherit AcmeError;
#include AcmeErrorInc


/*------------------------------- force_load -------------------------------*/
/*
    Description:
      Forces an object (named by filename) to load
    Parameters:
      file      - the name of the file to load
    Returns:
      The object (0 if it can't find it, or if there was an error)
    Notes:
      None.
*/
object
force_load(string file)
{
  if (catch(call_other(file, "?_Nonexistent_Function_?")) != 0) 
  {
    error(ERR_ERROR, sprintf("Couldn't load %s", to_string(file)));
    return 0;
  }
  return find_object(file);
}

/*------------------------------- force_load -------------------------------*/
/*
    Description:
      Forces lots of objects (named by filenames) to load
    Parameters:
      filenames      - an array of the names of the file to load
    Returns:
      An array of the objects
    Notes:
      None.
*/
object *
force_load_array(string *filenames)
{
  int i,size;
  object *obs;
  
  obs = ({ });
  
  if(!pointerp(filenames)
     || !sizeof(filenames))
    return ({ });
  
  for(i=0, size=sizeof(filenames); i < size; i++)
    if(!catch(call_other(filenames[i], "?_Nonexistent_Function_?"))) 
      obs += ({ find_object(filenames[i]) });
    else
      error(ERR_ERROR, sprintf("Couldn't load %s", to_string(filenames[i])));
  
  return obs;
}

