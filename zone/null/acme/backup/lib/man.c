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
        Zamboni 960202
*/

#include <acme.h>

private inherit AcmeFile;

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
      name      - the name of the manpage
    Returns:
      The filename, or 0 if not found.
    Notes:
      None.  
*/
string
find_manpage( string name )
{
  return find_file( name, explode(THISP->_ExpandVariable( MANPATHvar ), ":") );
}

