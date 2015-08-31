/*
    NAME
        file.c - Things related to files in general
    DESCRIPTION

        The functions in this package have to do with files and
	filenames, but not with the loading or managing of any code
	that might be in those files.

        expand_path()  - expands relative paths into absolute ones
        file_exists()  - Tries to see if a file exists
        basename()     - extracts the base filename from a full path
        dirname()      - extracts the path from a full path
        find_file()    - finds a file in a specified path
    NOTES
        None.
    LAST MODIFIED
        Zamboni 960202


*/

#include <acme.h>
#include AcmeFileInc

/*------------------------------ expand_filename ---------------------------*/
/*
    Description:
      Returns the absolute filename of the possibly relative filename.
      This is just to make up for a deficiency in the almost-sufficient
      function ExpandPath in the wizard body.
    Parameters:
      file      - the name of the file
    Returns:
      The expanded filename
    Notes:
      None.  
*/
string
expand_path(string file)
{
  if(( file != "" )
     && ( file[0] != '.' )
     && ( file[0] != '~' )
     && ( file[0] != '/' ))
    file = "./" + file;
  
  return THISP->ExpandPath( file );
}


/*------------------------------ file_exists -------------------------------*/
/*
    Description:
      Tries to determine whether a file really exists
    Parameters:
      file      - the name of the file to load
    Returns:
     -1   - Maybe.   (No access to that directory)
      0   - Definately not.
      1   - Definately yes.
    Notes:
      If no euid is set for this object it will return -1.
      This function is best inherited, not call_other'd.
*/
int
file_exists(string file)
{
  if(!stringp(file))
    return 0;

  /* If we can't read the dir, we don't really know if it's there. */
  if(!ACCESSOBJECT->CheckAccess(dirname(file), 
				getuid(), GetLevel(THISP), 
				"file_exists", READ ))
    return -1;
  
  return (file_size(file) != -1);
}


/*------------------------------ basename ---------------------------*/
/*
    Description:
      Returns the presumed base filename from file.  file should be a
      pathname delimited by "/".  Any null or non-string path will
      cause an error.  This function was lifted directly from
      /secure/wizard/support/fileExpand.c
    Parameters:
      file      - the name of the file
    Returns:
      The base path of the file
    Notes:
      None.  
*/
string
basename(string file)
{
   string *Split;

   Split = explode(file, "/");
   return Split[sizeof(Split) - 1];
}

/*------------------------------ basename ---------------------------*/
/*
    Description:
      Returns the presumed parent directory of file.  file should be a
      filename string delimited by "/".  Any null or non-string file
      will cause an error.  This function was lifted directly from
      /secure/wizard/support/fileExpand.c
    Parameters:
      file      - the name of the file
    Returns:
      The path of the file
    Notes:
      None.
*/
string
dirname(string file)
{
  int len;
  string *Split;

  Split = explode(file, "/");
  if (!Split)
    return "/.";

  Split = deletea(Split, sizeof(Split) - 1);
  if (!sizeof(Split))
    return "";
  if (strlen(file) && file[0]!='/')
    return implode(Split, "/") + "/";
  return "/" + implode(Split, "/") + "/";
}


/*------------------------------ find_file ---------------------------*/
/*
    Description:
      Searches the path for the particular file
    Parameters:
      file - name of the file to find.  ( can contain '/' )
      path - array of directory names
    Returns:
      The full path of the first file found
    Notes:
      None.
*/
string
find_file( string file, string *path )
{
  int i, size;
  string fullpath;
  
  for( size=sizeof( path ); i < size; i++ )
  {    
    fullpath = sprintf( "%s/%s", path[i], file );
    
    if( file_size(fullpath) >= 0 )
      return fullpath;
  }
  
  return 0;
}

