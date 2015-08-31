/*
    NAME

        args.c - Some routines useful for dealing with argument parsing

    DESCRIPTION        

        explode_args    - Separate space-deliniated arguments, but keep
                          quoted strings together.
        eval_args       - Replace arguments of a special format with what 
                          they reference.
        find_close_char - Find closing brace/paren/bracket/quote, etc.
        explode_unescaped - Same as efun::explode() but ignores escaped chars
        explode_nested    - Keeps parenthetical expressions together

    NOTES

        
     
    LAST MODIFIED
        Zamboni 960319

*/
#include <acme.h>

#include OpieClient

private inherit AcmeStrings;
private inherit AcmeError;
#include AcmeStringsInc
#include AcmeErrorInc



/*------------------------------- explode_args ------------------------------*/
/*
    Description:
      Separate space-deliniated arguments, but keep quoted strings together.
    Parameters:
      arg       - the whole argument string
    Returns:
      an array of separated arguments
    Notes:
      Keeps quoted strings together.  Example:
      explode_args("set name \"purple yam\"") would return
      ({"set", "name", "purple yam"})
*/
string *
explode_args(string arg)
{
  int pos, newpos, len;
  string *args;
  
  args = ({ });
  
  if(!arg)
    return args;
  
  len = strlen(arg);
  while(pos < len)
  {
    pos = find_nonws(arg, pos);
    newpos = match_quote(arg, pos);
    args += ({ unescape(unquote(arg[pos..newpos])) });
    pos = newpos + 1;
  }
  return args;
}


/*------------------------------- eval_args ------------------------------*/
/*
    Description:
      Replace arguments of a special format with what they reference
    Parameters:
      args      - an array of arguments
    Returns:
      an array of evaluated arguments
    Notes:
      [ospec] becomes a single object named by the ospec
      {ospec} becomes an array of objects named by the ospec
*/
mixed *
eval_args(string *args)
{
  int i, size, len, val;
  mixed result;
  mixed *new_array;

  new_array = ({ });
  
  for(i=0, size = sizeof(args); i < size; i++)
  {
    if((sscanf(args[i], "%d", val))
       && ( to_string( val ) == args[i] ))
      result = val;
    else if((len = strlen(args[i])) < 2)
      result = args[i];
    else
    {
      switch(args[i][0..0] + args[i][len-1..len-1])
      {
        case "[]":
          result = opie1(args[i][1..len-2]);
          break;
        case "{}":
          result = opie(args[i][1..len-2]);
          break;
        default:
          result = args[i];
      } /* switch */
    } /* if(strlen...) */

    new_array += ({ result });
    
  } /* for */

  return new_array;
  
}


/*-------------------------- find_close_char_int ---------------------------*/
/*
    Description:
      Private internal function used by find_close_char()
    Parameters:
      str   - The string to search through
      start - The position of the open brace/paren/bracket/quote
      open  - A string of OPEN characters.
      close - A string of CLOSE characters.
      len   - strlen( str )
      style - position of str[0] in open.  ( searcha( open, str[0] ) )
      both  - open + close
    Returns:
      ret > 0    - The position of the matching char in str
      ret <= 0   - Error: Failed to match open char at str[-ret]
    Notes:
      See find_close_char().  If len, style, or both are not what they 
      are supposed to be, bad things will happen.  They are passed in
      to save time and their validity is not checked.
*/
private int
find_close_char_int(string str, int start, string open, string close, 
                    int len, int style, string both)
{
  int first, cursor, tmp;
  int *stack;

  cursor = start + 1;
  
  while(cursor < len)
  {
    first = searcha_any_str(str, both, cursor);

    if(first == -1)  // Close not found.
      return -start;

    if( str[first] == close[style] )  // It's what we've been looking for
      return first;
    
    if(searcha(open, str[first]) == -1)  // It's not an OPEN char
    {
      // Trying to close something that isn't open
      return -start;
    }
    // Must be an OPEN char.  Recurse.  
    tmp = find_close_char_int(str, first, open, close, 
                              len, searcha( open, str[first] ), both);
    if( tmp < 0 )
      return tmp;
    
    cursor = tmp + 1; // Move past the closing char of the subexpression
    
  } /* while */

  // Got to the end of the string and didn't find a match.
  // This would happen for "[..(..)"
  return -start;
}


/*------------------------------- find_close_char ---------------------------*/
/*
    Description:
      Find closing brace/paren/bracket/quote that matches specified open.
    Parameters:
      str   - The string to search through
      start - The position of the open brace/paren/bracket/quote
      quiet - nonzero means omit error messages
      open  - A string of OPEN characters.  Defaults to "\"([{"
      close - A string of CLOSE characters.  Defaults to "\")]}"
    Returns:
      ret > 0   - The position of the matching char in str
      ret ==0   - Error: str[start] was not an OPEN char.
      ret < 0   - Error: Failed to match open char at str[-ret - 1]
    Notes:
      open and close must be the same length, and be in the correct
      order so that open[i] matches close[i].
*/
varargs int
find_close_char(string str, int start, status quiet,
                string open, string close)
{
  int retval, style;

  // If you explicitly specify 0 for open, you will get errors.
  if( !stringp(close) )
  {
    open  = "\"([{";
    close = "\")]}";
  }

  // Must be an OPEN char.
  style = searcha( open, str[start] );
  if( style == -1 )
  {
    if( !quiet )
      error(ERR_ERROR, sprintf("Not an OPEN char:\n%s\n%*s\n", 
                               str, start, "^"));
    return 0;
  }
  
  retval = find_close_char_int( str, start, open, close, 
                                strlen( str ), style, open+close );

  if( retval <= 0 )
  {
    if( !quiet )
      error(ERR_ERROR, sprintf("Unmatched %c\n%s\n%*s\n", 
                               str[-retval], str, -retval + 1, "^"));
    return retval-1;
  }

  return retval;
}


/*----------------------------- explode_unescaped ---------------------------*/
/*
    Description:
      Separate string into substrings by some unescaped delimeter
    Parameters:
      str   - The string to search through
      delim - string that separates elements in str.
    Returns:
      The array of strings which had been separated by the delimeter.
    Notes:
      Similar to efun::explode().
      explode_unescaped( "a:b\:c:d", ":" ) would yield
      ({ "a", "b\:c", "d" }).
*/
string *
explode_unescaped(string str, string delim)
{
  string *result;
  int delim_len, cursor, max, start;
  
  delim_len = strlen( delim );
  if( delim_len == 0 )
    return explode( str, delim );
  
  result = ({ });
  max = strlen( str ) - delim_len;

  while( cursor <= max )
  {
    cursor = strstr( str, delim, cursor );
    if( cursor == -1 )
      break;
    if( is_escaped( str, cursor ) )
    {
      cursor++;
    } else 
    {
      result += ({ str[start..cursor-1] });
      cursor = cursor + delim_len;
      start = cursor;
    }
  }

  result += ({ str[start..] });
  
  return result;
}

/*----------------------------- explode_unescaped ---------------------------*/
/*
    Description:
      Separate string into substrings by some unescaped, unnested delimeter
    Parameters:
      str   - The string to search through
      delim - string that separates elements in str.
      quiet - nonzero means omit error messages
      open  - A string of OPEN characters.
      close - A string of CLOSE characters.
    Returns:
      The array of strings which had been separated by the delimeter.
    Notes:
      Similar to explode_unescaped(), but also skips over any
      delimiters that are nested inside things.  See find_close_char()
      for an explanation of nesting.

      explode_nested( "a:b(c:d):e", ":" ) would yield
      ({ "a", "b(c:d)", "e" }).
*/
string *
explode_nested( string str, string delim, status quiet,
                string open, string close )
{
  string *result;
  int delim_len, cursor, next_delim, max, start, next_open;
  
  delim_len = strlen( delim );
  if( delim_len == 0 )
    return explode( str, delim );
  
  // If you explicitly specify 0 for open, you will get errors.
  if( !stringp(close) )
  {
    open  = "\"([{";
    close = "\")]}";
  }

  result = ({ });
  max = strlen( str ) - delim_len;

  while( cursor <= max )
  {
    next_delim = strstr( str, delim, cursor );
    if( next_delim == -1 )
      break;
    if( is_escaped( str, next_delim ) )
    {
      cursor = next_delim + 1;
    } else if( (( next_open > cursor ) // The last one we found is still ahead
                || ((next_open = searcha_any_str( str, open, cursor )) != -1) )
               && ( next_open < next_delim ))
    {
      // Skip over the matching close char
      cursor = 1 + find_close_char( str, next_open, quiet, open, close );
      if( cursor <= 1 )  // Something went wrong
        return 0;
    } else
    {
      result += ({ str[start..next_delim-1] });
      cursor = next_delim + delim_len;
      start = cursor;
    }
  }

  result += ({ str[start..] });
  
  return result;  
}
