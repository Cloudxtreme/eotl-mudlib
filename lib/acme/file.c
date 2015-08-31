/*
    NAME
        file.c - Things related to files in general
    DESCRIPTION

        The functions in this package have to do with files and
	filenames, but not with the loading or managing of any code
	that might be in those files.

        expand_path()  - expands relative paths into absolute ones
        expand_pattern() - finds files according to a pattern
        file_exists()  - Tries to see if a file exists
        basename()     - extracts the base filename from a full path
        dirname()      - extracts the path from a full path
        find_file()    - finds a file in a specified path
        get_dirs()     - get all the subdirs of a dir
        get_files()    - get all the files in a dir and its subdirs
        make_absolute()- make a pathname absolute based on a variable home&pwd 
    NOTES
        None.
    LAST MODIFIED
        Zamboni 970813


*/

#include <acme.h>
inherit AcmeStrings;
#include AcmeStringsInc
#include AcmeFileInc

/*------------------------------ expand_filename ---------------------------*/
/*------------------------------ expand_path -------------------------------*/
/*
    Description:
      Returns the absolute filename of the possibly relative filename.
    Parameters:
      pattern  - the name of the file to resolve
      who      - the person or thing doing the resolving (this is used to
                 resolve relative paths and home directories).  Defaults
                 to THISO if not given.
    Returns:
      The expanded filename
    Notes:
      An older version of this function was just a supplement to the 
      ExpandPath() function in the wizard body, and in fact depended on the
      existance of that function.  This is no longer the case.
*/
varargs string 
expand_path( string pattern, object who )
{
  string cwd, base, *split, *new;
  int i, j;

  if( !objectp( who ) )
    who = THISO;

  cwd = who->query_pwd() || 
        THISO->query_pwd() || 
        dirname( file_name( THISO ) );

  if( !stringp( pattern ) || !strlen( pattern ) )
    pattern = cwd;

  // first make pattern absolute
  switch( pattern[0] ) 
  {
    case '/'  : /* already absolute */
      break;
    case '~'  : /* home dir */
      if( pattern == "~" )
        pattern = PlayerRoot + (who->query_real_name()||"");
      else if( pattern[1] == '/' )
        pattern = PlayerRoot + (who->query_real_name()||"") + pattern[1..];
      else
        pattern = PlayerRoot + pattern[1..];
      break;
    default   :
      pattern = cwd + pattern;
      break;
  }

  // expand . and ..
  split = explode( pattern, "/" );
  new = ({ });
  for( i=0 , j=sizeof(split) ; i<j ; i++ )
  {
    switch( split[i] )
    {
      case ".."  : /* walk back one dir */
        new = new[0..<2];
        break;
      case "."   : /* skip */
        break;
      default    :
        new += ({ split[i] });
        break;
    }
  }

  if( !sizeof( new ) )
    pattern =  "/";
  else
/* -- zippo for ldmud */
/*    pattern = "/" + implode( new, "/" ) + ( pattern[<1]=='/' ? "/" : "" ); */
    pattern = implode( new, "/" );

  return pattern;
}


/*------------------------------ expand_pattern ----------------------------*/
/*
    Description:
      Returns a list of files found on disk that match a shell pattern.
    Parameters:
      pattern  - the pattern to match files against
      who      - the person or thing doing the resolving (this is used to
                 resolve relative paths and home directories).  Defaults
                 to THISO if not given.
    Returns:
      An array of strings, all filenames found that match pattern.
      The calling object must have read access to anything searched.
    Notes:
      Uses the _expander() function below to implement a recursive
      directory search.
*/
varargs string *
expand_pattern( string pattern, object who )
{
  closure add_slash;

  add_slash = lambda( ({ 'file, 'dir }),
    ({ #'+, ({ #'+, 'dir, "/" }), 'file }) );

  pattern = expand_path( pattern, who );
  if( pattern[<1] == '/' )
    pattern += "*";

  return _expander( explode( pattern, "/" ), ({ "" }), add_slash );
}

private string *
_expander( string *pats, string *dirs, closure add_slash )
{
  int i, j, size;
  string *out, pat;
  mixed *tmp;

  out = ({ });
  pat = "/" + pats[0];

  if( sizeof( pats ) > 1 )
  {
    for( i=0, j=sizeof(dirs) ; i<j ; i++ )
    {
      tmp = order_alist( get_dir( dirs[i]+pat, 0x20|0x02 ), 
                         get_dir( dirs[i]+pat, 0x20|0x01 ) );
      if( !( size = sizeof( tmp[0] ) ) )
        continue;
      tmp = tmp[1][0..member( tmp[0], -2, size-1, -1 )];
      tmp -= ({ ".", ".." });
      out += map_array( tmp, add_slash, dirs[i] );
    }
    return _expander( pats[1..], out, add_slash );
  }
  else
  {
    for( i=0, j=sizeof(dirs) ; i<j ; i++ )
    {
      tmp = get_dir( dirs[i]+pat, 0x20|0x01 ),
      out += map_array( tmp, add_slash, dirs[i] );
    }
    return out;
  }
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
  if(!MASTEROBJECT->valid_read( dirname(file), geteuid(), "file_size", THISO ))
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
   string *split;

   split = explode(file, "/");
   return file[<1] == '/' ? split[<2] : split[<1];
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
  string *split;

  split = explode(file, "/");
  if (!split)
    return "/.";

  split = deletea(split, sizeof(split) - 1);
  if (!sizeof(split))
    return "";
/* -- zippo for ldmud (2 lines commented out) */
/*  if (strlen(file) && file[0]!='/') */
    return implode(split, "/") + "/";
/*  return "/" + implode(split, "/") + "/"; */
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

/*----------------------------- get_dirs ---------------------------------*/
/*
    Description:
      Get the all the subdirs of a dir.
    Parameters:
      path - An absolute filename that refers to a directory
    Returns:
      An array (breadth first) of all the subdirectories (absolute 
      paths) of path, including path.
    Notes:
      To get a depth first sort, just alphabetize the list.
*/
 
static private closure get_dirs_cl;
 
string *
get_dirs( string path )
{
  string *out, new;
  int current, last;
 
  seteuid(getuid());
  path = munge_dirname(path);
  if( !IsDir(path) )
    return ({ });
 
  if( !closurep(get_dirs_cl) )
  {
    closure inner_cl;
 
    // this will make file_names into absolute paths
    // we'll use it to build get_dirs_cl, then toss it out
    inner_cl = lambda( ({ 'x, 'y }),
      ({ #'sprintf, "%s%s/", 'y, 'x }) );
 
    // every subdir passes through this closure
    get_dirs_cl = lambda( ({ 'x, 'y }), 
      ({ #',,
         ({ #'=,                   // 'tmp is an ordered alist of file sizes
            'tmp,                  // to file names contained in 'x
            ({ #'order_alist,
               ({ #'||,
                  ({ #'get_dir, 'x, 2 }),
                  quote( ({ }) )
               }),
               ({ #'||,
                  ({ #'get_dir, 'x, 0x21 }),
                  quote( ({ }) )
               })
            })
         }),
         ({ #'=,                   // 'j is the index of the last directory
            'j,                    // in our sorted alist 'tmp
            ({ #'member,
               ({ #'[, 'tmp, 0 }),
               -2,
               ({ #'-, 
                  ({ #'||,
                     ({ #'sizeof, ({ #'[, 'tmp, 0 }) }),
                     1
                  }), 
                  1 
               }),
               -1
            })
         }),
         ({ #'?,                   // we found one or more subdirs,
            ({ #'!=, 'j, -1 }),    // lets add them to 'y (&out)
            ({ #'+=, 
               'y,
               ({ #'map_array,     // convert to absolute paths using inner_cl
                  ({ #'[..],
                     ({ #'[, 'tmp, 1 }),
                     0,
                    'j
                  }),
                  inner_cl,
                  'x
               })
            })
         })
      }) 
    );
  }
 
  out = ({ path });
  last = -1;
  current = sizeof(out)-1;
  do
  {
    filter_array( out[(last+1)..], get_dirs_cl, &out );
    last = current;
    current = sizeof(out)-1;
  }
  while(last != current);
  return out;
}
 
/*----------------------------- get_files --------------------------------*/
/*
    Description:
      Get the all the files in a dir and its subdirs.
    Parameters:
      path - An absolute filename that refers to a directory
    Returns:
      An array (breadth first) of all the files (absolute paths) 
      contained in path and path's subdirectories, not including the
      directories themselves.
    Notes:
      To get a depth first sort, just alphabetize the list.
*/
 
static private closure get_files_cl;
 
string *
get_files( string path )
{
  string *out, *dirs;
  int current, last;
 
  seteuid(getuid());
  path = munge_dirname(path);
  if( !IsDir(path) )
    return ({ });
 
  if( !closurep(get_files_cl) )
  {
    closure inner_dir_cl, inner_file_cl;
 
    // this will make directory names into absolute paths
    // we'll use it to build get_files_cl, then toss it out
    inner_dir_cl = lambda( ({ 'x, 'y }),
      ({ #'sprintf, "%s%s/", 'y, 'x }) );
 
    // this will make file names into absolute paths
    // we'll use it to build get_files_cl, then toss it out
    inner_file_cl = lambda( ({ 'x, 'y }),
      ({ #'+, 'y, 'x }) );
 
    // every subdir passes through this closure
    get_files_cl = lambda( ({ 'x, 'y, 'z }), 
      ({ #',,
         ({ #'=,                   // 'tmp is an ordered alist of file sizes
            'tmp,                  // to file names contained in 'x
            ({ #'order_alist,
               ({ #'||,
                  ({ #'get_dir, 'x, 2 }),
                  quote( ({ }) )
               }),
               ({ #'||,
                  ({ #'get_dir, 'x, 0x21 }),
                  quote( ({ }) )
               })
            })
         }),
         ({ #'=, 'size, ({ #'sizeof, ({ #'[, 'tmp, 0 }) }) }),
         ({ #'=,                   // 'j is the index of the last directory
            'j,                    // in our sorted alist 'tmp
            ({ #'?,
               'size,
               ({ #'member,
                  ({ #'[, 'tmp, 0 }),
                  -2,
                  ({ #'-, 'size, 1 }),
                  -1
               }),
               -1
            }),
         }),
         ({ #'?,                   // we found one or more subdirs,
            ({ #'!=, 'j, -1 }),    // lets add them to 'y (&dirs)
            ({ #'+=, 
               'y,
               ({ #'map_array,     // convert to absolute paths
                  ({ #'[..],       // using inner_dir_cl
                     ({ #'[, 'tmp, 1 }),
                     0,
                    'j
                  }),
                  inner_dir_cl,
                  'x
               })
            })
         }),
         ({ #'?,                   // found one more more files in dir
            ({ #'<,                // let's add them to 'z (&out)
               ({ #'+=, 'j, 1 }), 
               'size 
            }),
            ({ #'+=,
               'z,
               ({ #'map_array,     // convert to absolute paths 
                  ({ #'[..,        // using inner_file_cl
                     ({ #'[, 'tmp, 1 }),
                     'j
                  }),
                  inner_file_cl,
                  'x
               })
            })
         }),
      }) 
    );
  }
 
  dirs = ({ path });
  out = ({ });
  last = -1;
  current = sizeof(dirs)-1;
  do
  {
    filter_array( dirs[(last+1)..], get_files_cl, &dirs, &out );
    last = current;
    current = sizeof(dirs)-1;
  }
  while(last != current);
  return out;
}

/*------------------------------ make_absolute -----------------------------*/
/*
    Description:
      Makes a path absolute based on a passed present working directory and
      home directory.
    Parameters:
      filename - file to be expanded
      pwd      - present working directory
      home     - home directory
    Returns:
      The full path.
*/
string make_absolute( string filename, string pwd, string home )
{
    mixed path;
    int   i, sz;
 
    /*** make sure the home and pwd's are the way we want them ***/
    if( stringp( home ) && home[ <1 ] == '/' )
        home = home[ 0 .. <2 ];
    if( pwd[ <1 ] != '/' )
        pwd += "/";

    /*** make sure the filename is absolute ***/
    if( filename[ 0 .. 1 ] == "~/" )
        filename = home + filename[ 1 .. ];
    else if( filename[ 0 ] == '~' )
        filename = "/usr/" + filename[ 1 .. ];
    else if( filename[ 0 ] != '/' )
        filename = pwd + filename;
 
    /*** probably root ***/
    if( !sz = sizeof( path = explode( filename, "/" ) ) )
        return( filename );
 
    /*** walk through the path ***/
    for( i = 0; i < sz; i++ )
    {
        /*** '.' is silently consumed. ***/
        if( path[ i ] == "." )
        {
            if( !i )
                path = path[ i-- .. ];
            else
            {
                sz--;
                path = path[ 0 .. i - 1 ] + path[ i-- + 1 .. ];
            }
        }
 
        /*** '..' is in reference to one directory up the ladder ***/
        else if( path[ i ] == ".." )
        {
            /*** this is bad and shouldn't happen ***/
            if( !i )
                return( "/" );
            else
            {
                sz -= 2;
                path = path[ 0 .. i -= 2 ] + path[ i + 3 .. ];
            }
        }
    }
 
/* -- zippo for ldmud */
/*return( "/" + implode( path, "/" ) + ( filename[ <1 ] == '/' ? "/" : "" ) );*/
    return( implode( path, "/" ));
 
}
