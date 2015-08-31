/*
    NAME
        man.c - Manpage handling stuff
    DESCRIPTION

        Eventually there will be more interesting things, like 
        an apropos that works on whatis databases.

        find_manpage()    - returns the file of a manpage in env(MANPATH)
    NOTES
        None.
    LAST MODIFIED
        Zamboni 980105
*/

#include <acme.h>

inherit AcmeFile;
#include AcmeFileInc
#include AcmeManInc


create()
{
  seteuid( getuid() );
}


/*------------------------------ find_manpage ---------------------------*/
/*
    Description:
      Find the absolute filename of a manpage
    Parameters:
      name        - the name of the manpage
      defaultPath - the search path to use if MANPATHvar is not set
    Returns:
      The filename, or 0 if not found.
    Notes:
      None.  
*/
varargs string
find_manpage( string name, string defaultPath )
{
  string path;
  if ( THISP->_IsVariableSet( MANPATHvar ) )
    path = THISP->_ExpandVariable( MANPATHvar );
  else
    path = defaultPath;
  return find_file( name, explode(path, ":") );
}

