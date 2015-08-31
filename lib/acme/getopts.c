/*
    NAME
        getopts.c - Routines for handling command line options
 
    DESCRIPTION
       
        getopts()          - Parse options from an array of arguments
        getopts_long()     - getopts + allow long options like --foo
 
    NOTES
        None.
 
    LAST MODIFIED
        Morgaine 971205
*/
 
#include <acme.h>
#include AcmeGetOptsInc
 
 
 
/*------------------------------ getopts ---------------------------*/
/*
    Description:
      Extract options from a set of command line args
    Parameters:
      args      - An array of arguments (strings)
      validopts - A list of valid options, or 0 to accept any option.
                  For example, "ab-c:d" means that -a, -b, -c, and -d
                  are valid; -b and -c take a single parameter, -b may
                  be used more than once and the parameter(s) will be 
                  stored in an array, and the others options (-a and -d)
                  take no parameters.  The characters ':', ' ', and '-' 
                  may not be used as options.
 
    Returns:
      An array:   
        ({ string *other_args, mapping options, string badopts })
 
      other_args - A subset of args with all options and their
                   parameters removed.
 
      options    - A mapping of options used.  The option characters
                   (e.g. 'a', not "a") are the indices.  
                   options['a'] == the parameter of -a if "a:" in 
                   validopts, an array containing all parameters if
                   "a-" in validopts, or 0 if -a doesn't take a parameter.
 
      badopts    - A string containing all the options which were 
                   invalid (not in validopts) or which were supposed
                   to take a parameter but couldn't because no args
                   were left to consume.
 
    Notes:
      This function has no side effects.
      An arg of "--" can be used to explicitly mark the end of
      options.  The "--" arg will be consumed, and any args after this
      will be left alone.  Args of "-" will be silently consumed.
 
*/
mixed *
getopts(string *args, string validopts)
{
  string badopts;
  mapping options;
  int i, size, opt, pos, j, sz;
  status anyargs;
 
  badopts = "";
  options = ([ ]);
 
  if( stringp( validopts ) )
    validopts += " ";        // pad it to make checking for ':' easier
  else
    anyargs = 1;             /* They can choose not to specify
                              * validargs, in which case all are ok. */
 
  for( size = sizeof(args); i < size ; i++ )
  {
    if( args[i][0] != '-' )  // Reached a non-option
      break;
    
    if( args[i] == "--" )    // Explicit end of options
    {
      i++;                   // increment past this so we don't keep it
      break;
    }
 
    if( args[i] == "-" )     // Stray '-'.  Skip it
      continue;
    
    for( j = 1, sz = strlen( args[ i ] ) - 1; j < sz; j++ )
    {
      opt = args[ i ][ j ];
      if( anyargs || (
        (pos = member( validopts, opt )) > -1  && validopts[pos+1] != '-' && validopts[pos+1] != ':' ) )
        options[ opt ] = 0;
      else
        badopts += args[ i ][ j .. j ];
    }

 
    opt = args[i][<1];        // Extract the option char  ('s' from "-s")
    
    if( anyargs )
    {
      options[opt] = 0;
      continue;
    }
    
    pos=member( validopts, opt );
    if( pos > -1 )  // Valid option
    {
      if( validopts[pos+1] == ':' )  // This option takes an argument
      {
        if( ++i < size )             // If we're not at the end
        {
          options[opt] = args[i];
          continue;
        }
      } else
      if( validopts[pos+1] == '-' )  // This option takes an argument
      {                              // and can be repeated.
        if( ++i < size )             // If we're not at the end
        {
          if(!pointerp(options[opt]))
            options[opt] = ({ });
          options[opt] += ({ args[i] });
          continue;
        }
      } else
      {
        options[opt] = 0;
        continue;
      }
    }
 
    /* If we made it this far, the option is either invalid or
     * requires and arg and wasn't given one.  */
 
    badopts = sprintf( "%s%c", badopts, opt );
    
  }
 
  return ({ args[i..], options, badopts });
}
 
/*---------------------------- getopts_long ---------------------------*/
/*
    Description:
      Extract options from a set of command line args
    Parameters:
      args      - An array of arguments (strings)
      validopts - A list of valid options, or 0 to accept any option.
                  For example, "abc:d" means that -a, -b, -c, and -d
                  are valid; -c takes a single parameter, and the
                  others options take no parameters.  The characters
                  ':', ' ', and '-' may not be used as options.
      longopts  - A mapping of valid long options strings to flags of 
                  whether each takes an parameter, or 0 to accept any
                  long options.  Example:
                  ([ "foo" : 0, "bar" : 1, "bog" : -1 ])
                  would mean that --foo, --bar and --bog were valid 
                  options, that --bar and --bog took a single parameter, 
                  --bog may be used more than once and each parameter
                  will be stored in an array, and --foo takes no 
                  parameters.
    Returns:
      An array:   
        ({ string *other_args, mapping options, mixed *badopts })
 
      other_args - A subset of args with all options and their
                   parameters removed.
 
      options    - A mapping of options used.  The option characters
                   (e.g. 'a', not "a") and the long options are the
                   indices.  
                   options['a'] == the parameter of -a if "a:" in 
                   validopts, an array containing all parameters if
                   "a-" in validopts, or 0 if -a doesn't take a parameter.
                   Similarly, options["foo"] == the parameter of --foo.
 
      badopts    - An array containing all the options which were 
                   invalid (not in validopts) or which were supposed
                   to take a parameter but couldn't because no args
                   were left to consume.  Note that this is different
                   than in getopts().
 
    Notes:
      This function has no side effects.
      An arg of "--" can be used to explicitly mark the end of
      options.  The "--" arg will be consumed, and any args after this
      will be left alone.  Args of "-" will be silently consumed.
 
*/
mixed *
getopts_long(string *args, string validopts, mapping longopts)
{
  mixed badopts;
  mapping options;
  int i, size, opt, pos, j, sz;
  status anyargs, anylongargs;
  string longopt;
 
  badopts = ({ });
  options = ([ ]);
 
  if( stringp( validopts ) )
    validopts += " ";        // pad it to make checking for ':' easier
  else
    anyargs = 1;             /* They can choose not to specify
                              * validargs, in which case all are ok. */
 
  if( !mappingp( longopts ) )
    anylongargs = 1;
 
  for( size = sizeof(args); i < size ; i++ )
  {
    if( args[i][0] != '-' )  // Reached a non-option
      break;
    
    if( args[i] == "--" )    // Explicit end of options
    {
      i++;                   // increment past this so we don't keep it
      break;
    }
 
    if( args[i] == "-" )     // Stray '-'.  Skip it
      continue;
    
    if( args[ i ][ 1 ] != '-' )
      for( j = 1, sz = strlen( args[ i ] ) - 1; j < sz; j++ )
      {
        opt = args[ i ][ j ];
        if( anyargs || ( member( validopts, opt ) > -1 ) )
          options[ opt ] = 0;
        else
          badopts += ({ opt });
      }
 
    opt = args[i][<1];        // Extract the option char  ('s' from "-s")
 
    if( args[ i ][ 1 ] == '-' )          // It's a long option
    {
      longopt = args[i][2..];     // We already checked for "--"
      if( anylongargs )
      {
        options[longopt] = 0;
        continue;
      }
      
      if( !member( longopts, longopt ) )
      {
        badopts += ({ longopt });
        continue;
      }
 
      if( longopts[longopt] > 0 )        // This option takes an argument
      {
        if( ++i < size )             // If we're not at the end
        {
          options[longopt] = args[i];
        } else                       // Ran out of args
        {
          badopts += ({ longopt });
        }        
      } else 
      if( longopts[longopt] < 0 )    // This option takes an argument
      {                              // that can be used more than once.
        if( ++i < size )             // If we're not at the end
        {
          if(!pointerp(options[longopt]))
            options[longopt] = ({ });
          options[longopt] += ({ args[i] });
        } else                       // Ran out of args
        {
          badopts += ({ longopt });
        }        
      } else                         // Doesn't take an argument
      {
        options[longopt] = 0;
      }
      continue;      
    } // End of longopts
    
    if( anyargs )
    {
      options[opt] = 0;
      continue;
    }
    
    pos=member( validopts, opt );
    if( pos > -1 )  // Valid option
    {
      if( validopts[pos+1] == ':' )  // This option takes an argument
      {
        if( ++i < size )             // If we're not at the end
        {
          options[opt] = args[i];
          continue;
        }
      } else
      if( validopts[pos+1] == '-' )  // This option takes an argument
      {                              // and can be repeated.
        if( ++i < size )             // If we're not at the end
        {
          if(!pointerp(options[opt]))
            options[opt] = ({ });
          options[opt] += ({ args[i] });
          continue;
        }
      } else
      {
        options[opt] = 0;
        continue;
      }
    }
 
    /* If we made it this far, the option is either invalid or
     * requires and arg and wasn't given one.  */
 
    badopts += ({ opt });
  }
 
  return ({ args[i..], options, badopts });
}
 
