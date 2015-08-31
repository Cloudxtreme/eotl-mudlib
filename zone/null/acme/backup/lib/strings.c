/*
    NAME
        strings.c - Handy string manipulation routines
    DESCRIPTION
        munge_filename()   - Mangle filename into a "standard" c-style format
        find_nonws()       - Gives index of first non-space
        trim_right()       - Removes some trailing character
        trim_left()        - Removes some leading character
        is_escaped()       - True if the char at <i> is escaped by a backslash
        searcha_any_str()  - Like searcha() but matches any of several chars
        searcha_str_unescaped() - Like searcha() but ignores escaped chars
        unescape()         - Unescapes everything in a string
        match_quote()      - Finds endquote
        unquote()          - Removes surrounding quotes
        untabify()         - removes tabs in a string.
        untabify_list()    - untabifies a list or a string with newlines.
        expand_unescape()  - escapes and interprets escaped characters.
        expand_variables() - expands variables in a string
        remove_ansi()      - returns str without any ansi escape sequences.
        indent()           - indents a str or array of strs nspaces
        center()           - centers a str or array of strs
    NOTES
        None.
    LAST MODIFIED
        Zamboni 960814
 
*/
 
#include <acme.h>
 
private inherit AcmeError;
private inherit AcmeArray;
 
#include AcmeErrorInc
#include AcmeArrayInc
#include AcmeStringsInc
 
/*----------------------------- munge_filename ---------------------------*/
/*
    Description:
      Munges a filename into a standard format.
    Parameters:
      name - An absolute filename that refers to a .c file
    Returns:
      A filename of the form /foo/bar.c
    Notes:
      This is to work around grossness in the driver.  inherit_list()
      gives filenames in the form "foo/bar.c" whereas file_name()
      gives them as "/foo/bar".  This function converts them both to
      the form "/foo/bar.c".  Grossness begats grossness.
*/
string
munge_filename(string name)
{
  int len;
  
  if(!name || !stringp(name))
    return 0;
 
  len = strlen(name);
  
  if(!len)
    return "";
 
  if(name[0] != '/')
  {
    name = "/" + name;
    len++;
  }
  
  if((index(name, '#') == -1)
     && ((len < 2) || (name[(len-2) .. (len-1)] != ".c")))
    name += ".c";
 
  return name;
}
 
 
/*----------------------------- find_nonws ---------------------------*/
/*
    Description:
      Find a non-whitespace char
    Parameters:
      str   - String to search
      start - optional starting position
    Returns:
      The index of the first non-whitespace char
    Notes:
      Faster than using AcmeArray->anti_searcha( str, ' ', start ),
      but yields the same results.
*/
varargs int
find_nonws(string str, int start)
{
  int size;
  for(size = strlen(str); (start < size) && (str[start] == ' '); start++);  
  return start;
}
 
/*----------------------------- trim_right ---------------------------*/
/*
    Description:
      Removes copies of a specified character from the end of a string
    Parameters:
      str - The string that needs trimming
      chr - The character to trim off the end
    Returns:
      The new string.
    Notes:
      Useful for getting rid of trailing spaces.
      See trim_left().
*/
string
trim_right(string str, int chr)
{
  int pos;
  for(pos = strlen(str)-1; (pos >= 0) && (str[pos] == chr); pos--);
  return (pos == -1) ? "" : str[0..pos];
}
 
 
/*----------------------------- trim_left ---------------------------*/
/*
    Description:
      Removes copies of a specified character from the beginning of a string
    Parameters:
      str - The string that needs trimming
      chr - The character to trim off the beginning
    Returns:
      The new string.
    Notes:
      Useful for getting rid of leading spaces.
      See trim_right().
*/
string
trim_left(string str, int chr)
{
  return str[index( str, chr ) + 1 ..];
}
 
/*----------------------------- is_escaped ---------------------------*/
/*
    Description:
      See if a certain character in a string is "escaped"
    Parameters:
      str   - String to check in
      index - position of the character to check
    Returns:
      Nonzero if str[index] is escaped.
    Notes:
      is_escaped("\a", 1) == 1.
      is_escaped("\\a", 2) == 0.
*/
status
is_escaped(string str, int index)
{
  int last_non_bs;
  /* It's complicated because backslashes can be escaped too. */
  last_non_bs = anti_searcha(str, '\\', index-1, -1);
  return (index - last_non_bs + 1) % 2;
}
 
 
/*----------------------------- searcha_any_str ---------------------------*/
/*
    Description:
      Search string for any of several characters.  Ignores escaped chars.
    Parameters:
      str      - String to search through
      elements - String of characters to look for
      pos      - Optional starting position
      step     - Optional step size
    Returns:
      The position of the first character in <str> at or after <pos>
      which is in <elements> and is some multiple of <step> characters
      from <pos>.
    Notes: 
      See searcha(), is_escaped(), searcha_str_unescaped()
*/
varargs int
searcha_any_str(string str, string elements, int pos, int step)
{
  int size;
 
  if(!step)
    step = 1;
 
  for(size = strlen(str); (pos < size) && (pos >=0); pos += step)
    if((searcha(elements, str[pos]) != -1) && !is_escaped(str, pos))
      return pos;
 
  return -1;
}
 
/*------------------------- searcha_str_unescaped ---------------------------*/
/*
    Description:
      Search for a certain unescaped character in a string.
    Parameters:
      str      - String to search through
      el       - Character to search for
      pos      - Optional starting position
      step     - Optional step size
    Returns:
      The position of the first occurrance of <el> in <str> at or
      after <pos>, examining only every <step> characters.
    Notes:
      See searcha(), is_escaped, searcha_any_str()
*/
varargs int
searcha_str_unescaped(string str, int el, int pos, int step)
{
  int size;
 
  if(!step)
    step = 1;
 
  for(size = strlen(str); (pos < size) && (pos >= 0); pos += step)
  {
    if(( str[pos] == el ) && !is_escaped(str, pos))
      return pos;
  }
  return -1;
}
 
/*----------------------------- unescape ---------------------------*/
/*
    Description:
      Unescapes the characters in a string.
    Parameters:
      str - String to be unescaped.
    Returns:
      Returns a new copy of the string with everything unescaped by
      one level.  For instance, "moo\ cow" becomes "moo cow", while
      "moo\\\ cow" becomes "moo\ cow" (as would "moo\\ cow")
    Notes:
      This deals with literal backslashes only.  In the examples, and in
      the strings this function is meant to deal with "\a" is meant to 
      represent a string containing the two characters '\' and 'a',
      not a string containing a single '\a' character.)
*/
string
unescape(string str)
{
  int next, i, len;
  string copy;
 
  copy = "";
  for(len = strlen(str); i < len; i = next+2)
  {
    next = searcha(str, '\\', i);
    if(next == -1)
      return copy + str[i..len-1];
    if(next == len-1)
    {
      if(i == next)
        return copy;
      else
        return copy + str[i..len-2];
    }
    copy += str[i..next-1] + str[next+1..next+1];
  }
  return copy;
}
 
/* 
** Returns the index of the last character of the possibly quoted
** string
*/
varargs int
match_quote(string str, int start, string quote_chars)
{
  int end;
  
  if(searcha(quote_chars || QUOTE_CHARS, str[start]) != -1)
    end = searcha_str_unescaped(str, str[start], start+1, 1);
  else
    end = searcha_str_unescaped(str, ' ', start+1, 1) - 1;
  return (end < 0) ? strlen(str)-1 : end;
}
 
/* Removes outer layer of quotes, if there is one */
varargs string
unquote(string str, string quote_chars)
{
  int len;
  len = strlen(str);
 
  if(len < 2)      /* no room for quotes */
    return str;
  
  if(!quote_chars)
    quote_chars = UNQUOTE_CHARS;
 
  if(((str[0] == '\"') || (str[0] == '\''))
     && (str[0] == str[len-1]))
    return (len == 2) ? "" : str[1..len-2];
  else 
    return str;
}
 
/*
** removes tabs by a specified tabwidth or the UNIX default of 8.
** Note:  this function is not supposed to take a string with newlines.
**        use untabify_list to do that.  A string with newlines WILL NOT
**        work, simply because it would be gross for this function to
**        support the short comings of its user.
*/
 
varargs string 
untabify( string str, int tabwidth )
{
    int offset;
 
    if( !stringp( str ) || !strlen( str ) || ( member( str, '\t' ) < 0 ) )
        return( str );
    tabwidth = tabwidth || 8;
 
    while( ( offset = searcha( str, '\t', offset ) ) > -1 )
        str = sprintf( "%s%*s%s", offset ? str[ 0 .. offset - 1 ] : "",
          ( tabwidth - ( offset % tabwidth )), "", str[ offset + 1 .. ] );
    return( str );
}
 
/*
** removes tabs in a list, that being a pointer type list or a string
** with newlines in it.
** Note: no checks are made to make sure that ptr is made up of only strings.
**     You'll need to do that yourself.
*/
 
varargs mixed 
untabify_list( mixed ptr, int tabwidth )
{
    int s;
 
    if( s = stringp( ptr ) )   // i really wish we had new_explode... sigh
        ptr = regexplode( ptr, endl ) - ({ endl });
    if( !pointerp( ptr ) )
        return( 0 );
    ptr = map_array( ptr, "untabify", tabwidth || 8 );
    if( s )
        ptr = implode( ptr, endl );
    return( ptr );
}
 
/*
** escapes a string, but interprets the characters after the backslashes.
** oh, if you set any escape char to 'x', you will forfit hex expansion,
** and if you set any escape char to 0123456 or 7, you forfit octal
** expansion.  Additionally, putting the escape char in the base_string will
** disallow the escape char to escape itself.
*/
 
#define base_string "a\ab\bt\tn\n"
 
varargs string 
expand_unescape( string str, string base, int bits )
{
    string s;
    int    offset, char, tmp;
 
    base = base || base_string;
 
    if( !stringp( str ) || !strlen( str ) )
        return( str );
    if( strlen( base ) & 0x01 )        // this is faster than % 2 ...
        return( 0 );
 
    while( ( offset = searcha( str, '\\', offset ) ) > -1 )
    {
        if( ( char = member( base, str[ offset + 1 ] ) ) > -1 )
        {
            if( base[ char + 1 ] == '\b' && !( bits & O_EXPAND_BS_LITERAL ) )
                str = offset > 2 ? str[ 0 .. offset - 2 ] : "" +
                  str[ offset + 2 .. ];
            else
                str = sprintf( "%s%c%s", offset ? str[ 0 .. offset - 1 ] : "",
                  base[ char + 1 ], str[ offset + 2 .. ] );
        }
        else if( !( bits & O_NO_EXPAND_HEX ) && str[ offset + 1 ] == 'x' )
        {
            if( ( tmp = member( "0123456789abcdef", str[ offset + 2 ] ) ) < 0 )
                str = ( offset ? str[ 0 .. offset - 1 ] : "" ) + 
                  str[ offset + 1 ];
            else
            {
                tmp <<= 4;
                if( ( char = member( "0123456789abcdef", str[ offset + 3 ] ) ) <                  0 )
                    str = ( offset ? str[ 0 .. offset - 1 ] : "" ) + 
                      str[ offset + 1 ];
                else
                    str = sprintf( "%s%c%s", offset ? str[ 0 .. offset - 1 ] :
                      "", tmp | char, str[ offset + 4 .. ] );
            }
        }
        else if( !( bits & O_NO_EXPAND_OCTAL ) && ( tmp = member( "01234567",
          str[ offset + 1 ] ) ) > -1 )
        {
            tmp <<= 3;
            if( ( char = member( "01234567", str[ offset + 2 ] ) ) < 0 )
                str = ( offset ? str[ 0 .. offset - 1 ] : "" ) + 
                  str[ offset + 1 ];
            else
            {
                tmp |= char;
                tmp <<= 3;
                if( ( char = member( "01234567", str[ offset + 3 ] ) ) <
                  0 )
                    str = ( offset ? str[ 0 .. offset - 1 ] : "" ) + 
                      str[ offset + 1 ];
                else
                    str = sprintf( "%s%c%s", offset ? str[ 0 .. offset - 1 ] :
                      "", tmp | char, str[ offset + 4 .. ] );
            }
        }    
        else
        {
            str = ( offset ? str[ 0 .. offset - 1 ] : "" ) + 
               str[ offset + 1 ];
            offset++; // make sure the next char isnt evaluated
        }
    }
    return( str );
}
 
/*
** expands variables in a string, given a mapping and a variable delimiter,
** which defaults to the UNIX '$'.  If the value of a key is a closure, 
** it will be funcall'd before being entered (allowing for auto-expanding
** values).  Optional arguments:
**     varchar - an alternate variable delimiter (see above)
**     func    - a closure or function to handle undefined variables
**     args    - argument(s) that will be apply'd along with func...
**     kill_if_no_match - if we come to a variable we don't know, return with
**               an error code (-1) unless there was a) a func to call and
**               b) the return value was false.
**
*/
 
private closure lensort, varfilter;
 
varargs mixed 
expand_variables( string str, mapping vars, int varchar,
                  mixed func, mixed args, int kill_if_no_match )
{
    mixed  variables, tmp, t;
    string current_var;
    int    offset, i;
 
    if( !stringp( str ) || !mappingp( vars ) || !sizeof( vars ) )
        return( str );
    varchar = varchar || '$';
    lensort = lensort || lambda( ({ 'a, 'b }),
      ({ #'<, ({ #'strlen, 'a }), ({ #'strlen, 'b }) }) );
    varfilter = varfilter || lambda( ({ 'x, 'o }),
      ({ #'==, ({ #'[..], 'o, 0, ({ #'-, ({ #'strlen, 'x, }), 1 }) }), 'x }) );
    variables = sort_array( m_indices( vars ), lensort );
 
    while( ( offset = searcha( str, varchar, offset ) ) > -1 && 
      !is_escaped( str, offset ) )
    {
        current_var = ( i = searcha_any( str, " \t", offset ) ) < 0 ?
            str[ offset + 1 .. ] : str[ offset + 1 .. i - 1 ];
        if( sizeof( tmp = filter_array( variables, varfilter, current_var ) ) )
        {
            if( closurep( vars[ tmp[ 0 ] ] ) )
                vars[ tmp[ 0 ] ] = to_string( funcall( vars[ tmp[ 0 ] ] ) );
            else vars[ tmp[ 0 ] ] = to_string( vars[ tmp[ 0 ] ] );
 
            str = sprintf( "%s%s%s", offset ? str[ 0 .. offset - 1 ] : "",
                vars[ tmp[ 0 ] ], str[ offset + strlen( tmp[ 0 ] ) + 1 .. ] );
        }
        else
        {
            if( stringp( func ) )
                i = apply( #'call_other, ({ THISO, func, current_var }) + 
                  args );
            else if( closurep( func ) )
                i = apply( func, ({ current_var }) + args );
            if( kill_if_no_match && !t )
                return( -1 );
            else
            {
                str = sprintf( "%s%s%s", offset ? str[ 0 .. offset - 1 ] : "",
                    t ? to_string( t ) : "", str[ offset + strlen( 
                    current_var ) + 1 .. ] );
            }
        }
    }
    return( str );
}
 
/*
   name: remove_ansi
   parameters: string str - string to scan
   description: returns str without any ansi escape sequences.
   notes: the algorithm used doesnt accound for some strange MSDOS ansi
       signals, such as 'p', which can handle strings (in quotes).
   last modified:
       960314 Morgaine
*/

string 
remove_ansi( string str )
{
  int i, j, sz;
 
  if( ( i = strstr( str, ESC "[" ) ) < 0 )
    return( str );
  sz = strlen( str );
  do
  {                            
    for( j = i + 2; j < sz && !IsAlpha( str[ j ] ); j++ )
      /* do nothing */;
    str = str[ 0 .. i - 1 ] + str[ j + 1 .. ];
  }
  while( ( i = strstr( str, ESC "[", i ) ) > -1 );
  return( str );
}

//---------------------------------- indent ---------------------------------//
/*
    description: indents a string or array of strings nspaces
    parameters: source    - the string or array of strings to indent
                nspaces   - the number of spaces to indent the string(s)
    returns: the indented string or an array of indented strings
    notes: This function indents all lines by nspaces.  It considers an
           array element at least one line of itself.
      ie:
          indent( ({ "hi", "there", "you\ndork" }), 5 );
      would return,
          ({ "     hi", "     there", "     you\n     dork" })

      and similarly,
          indent( "hi there!\nyou dork\n", 5 );
      would return,
          "     hi there!\n     you dork\n"
      
*/
private closure indent_cl;

mixed
indent( mixed src, int nspace )
{
   status  firstnewline;
   status  lastnewline;
   string *srca;
   int     srca_sz;
   string  indent_str;
   string  pre;

   if ( !stringp( src ) && !pointerp( src ) )
      return error( ERR_ERROR, "Type mismatch: "
         "parameter 1 should be either a string or an array of strings.", 0 );

   if ( pointerp( src ) )
      return ( map_array( src, #'indent, nspace ) );

   indent_cl = indent_cl ||
      lambda( ({ 's, 'istr }), ({ #'sprintf, "%s%s\n", 'istr, 's  }) );
   indent_str = pad( "", nspace );

   if ( "" == src )
      return indent_str;

   firstnewline = ( src[ 0 ] == '\n' );
   lastnewline = ( src[ ( strlen( src ) - 1) ] == '\n' );
   srca = explode( src, "\n" );
   srca_sz = sizeof( srca );

   if ( srca_sz == 1 )
      pre = "";
   else
      pre = implode( map_array( srca[ 0..( srca_sz - 2 ) ],
         indent_cl, indent_str ), "" );

   return sprintf( "%s%s%s%s%s",
      ( firstnewline ? "\n" : "" ),
      pre, indent_str, srca[ srca_sz - 1 ],
      ( lastnewline ? "\n" : "" ) );
}


//---------------------------------- center ---------------------------------//
/*
    description: centers a string or array of strings nspaces
    parameters: source    - the string or array of strings to center
                width     - the width of the line to center into
    returns: the centered string or an array of centered strings
    notes: This function centers all lines by nspaces.  It considers an
           array element at least one line of itself.
      ie:
          center( ({ "hi", "there", "you\ndork" }), 10 );
      would return,
          ({ "    hi", "  there", "   you\n   dork" })
      and similarly,
          center( "hi there!\nyou dork\n", 10 );
      would return,
         "hi there!\n you dork\n"

      The default line width is DEF_LINEWIDTH (probably 78).
*/
private closure center_cl;

varargs mixed
center( mixed src, int width )
{
   status  firstnewline;
   status  lastnewline;
   string *srca;
   int     srca_sz;
   int     linelen;
   string  pre;

   if ( !stringp( src ) && !pointerp( src ) )
      return error( ERR_ERROR, "Type mismatch: "
         "parameter 1 should be either a string or an array of strings.", 0 );

   if ( pointerp( src ) )
      return ( map_array( src, #'center, width ) );

   if ( !width )
      width = DEF_LINEWIDTH;

   center_cl = center_cl ||
      lambda( ({ 's, 'wd }),
         ({ #'?,
            ({ #'>=, ({ #'strlen, 's }), DEF_LINEWIDTH }),
            's,
            ({ #'indent, 's,
               ({ #'/, ({ #'-, 'wd, ({ #'strlen, 's }) }), 2 })
            })
         })
       );
   
   firstnewline = ( src[ 0 ] == '\n' );
   lastnewline = ( src[ ( strlen( src ) - 1) ] == '\n' );
   srca = explode( src, "\n" );
   srca_sz = sizeof( srca );

   if ( srca_sz == 1 )
      pre = "";
   else
      pre = sprintf( "%s\n", implode( map_array( srca[ 0..( srca_sz - 2 ) ],
         center_cl, width ), "\n" ) );
   
   return sprintf( "%s%s%s%s",
      ( firstnewline ? "\n" : "" ),
      pre,
      indent( srca[ srca_sz - 1 ],
         ( width - strlen( srca[ srca_sz -1 ] ) ) / 2 ),
      ( lastnewline ? "\n" : "" ) );
}
