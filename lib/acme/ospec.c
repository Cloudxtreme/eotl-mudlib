/*
    NAME
        ospec.c - Object SPECification parsing system
    DESCRIPTION

     Opie interface:
      object_spec_variable_set() - sets a variable for use in specs
      object_spec_help()         - returns name of help file
      evaluate_object_spec()     - parses spec and return list of objects

      parse_ospec()              - Same thing, but not Opie conformant

    NOTES
        The only side effects are in the VarSpace code.
        Other than that, this object is stateless.
    LAST MODIFIED
        Devo 971218

*/

#include <acme.h>


#include AcmeOSpecInc
#include AcmeErrorInc
#include AcmeArrayInc
#include AcmeForceLoadInc
#include AcmeRoomInc
#include AcmeShadowInc
#include AcmeStringsInc
#include AcmeUserInc
#include AcmeVarSpaceInc
#include AcmeArgsInc
#include AcmeFindTargetInc
#include AcmeClosuresInc
#include AcmeFileInc

inherit AcmeError;
inherit AcmeArray;
inherit AcmeForceLoad;
inherit AcmeRoom;
inherit AcmeShadow;
inherit AcmeStrings;
inherit AcmeUser;
inherit AcmeVarSpace;
inherit AcmeArgs;
inherit AcmeFile;
inherit AcmeFindTarget;

#ifndef DEBUG
#define write(s) /* do nothing */
#endif

#define SafeBind(c)    ( AcmeClosureServer->secure_closure(c) )

#define HELP_FILE AcmeDocDir "ospec.help"

string spec_delimiter, subspec_delimiter, internal_delimiter;
string default_priorities;

mapping cl;  // For storing lots of often-used closures

// Mappings which store closures which get evaluated when a match is found
mapping exact_matches, // when the entire item matches a literal string
        number_prefix, // "#" of "#0347"
        string_prefix, // "*" of "*foo" or "!=" of "!=blah"
        normal_op;     // "x" of "x.blah" or "f" of "f.!.me"

// I do this grossness because my editor doesn't fully grok closure
// syntax and wants to match the brace in #'[, and if I don't appease
// it, it won't indent my code right.  I could complain about my
// syntax table limitations, but it's just as tempting to whine about
// Amylaar's stupid closure syntax.
#define CL_OPEN_BRACKET #'[
#if 0
]
#endif

#define MAP_AND_FLATTEN(x) \
  ({ #'flatten_array1, ({ #'map_array, 'prev, x }) })

#define MAP_AND_FLATTEN_cl(key) \
  ({ #'flatten_array1, ({ #'map_array, 'prev, \
                         ({ CL_OPEN_BRACKET, ({ #'cl }), key }), 'arg }) })


// Make sure these are set right or your longer prefixes won't get checked.
int max_len_string_prefix;


#define DATASPACE ( getuid(THISP||THISO) )
#define DATASPACE_CL ({ #'getuid, ({ #'this_player }) })

// If anyone wants to use something else, I'll make this a global
// variable like all the delimiters so you can customize it through
// inheritance.  Until then, I'll just use this.
#define C_OPEN "("
#define C_CLOSE ")"


// Some prototypes
static closure make_filter_closure( string *args );
static string *explode_argspec( string argspec );
static mixed value_closure( string arg );



/*************************************
**
** The following functions are for use in the parsing closures.
** Despite being publicly callable, they are not part of the official
** interface and may change without notice.
**
*/

closure
SafeBindSym( string funcname )
{
  return AcmeClosureServer->secure_fsym( funcname );
}

object *
spec_rooms( object *prev, string *args, status first )
{
  if(first)
    prev = THISP ? ({ ENV(THISP) }) : ({ });
  if(args[1] == "!")
    return force_load_array(flatten_array1(map_array(prev, #'all_exits)));
  return force_load_array(map_array(prev, #'get_exit, args[1]));
}

object *
spec_mapfile( object *prev, string *args, status first )
{
  mixed mapped;
  string file, *lines;
  file = expand_path( args[1], THISP );
  if ( file_exists( file ) != 1 )
  {
    error( ERR_ERROR, "map file " + file + " not found." );
    return ({ });
  }

  lines = grab_file( file );
  switch ( args[2] )
  {
    case "FILTER":  // special case so people can insert filters
      if ( !first )
      {
        mapped = map_array( lines, #'parse_ospec, prev );
        break;
      } // else, act like OSPEC
    case "OSPEC":   // special case so people can give ospecs
      mapped = map_array( lines, #'parse_ospec );
      break;
    default:
      mapped = map_array( lines, symbol_function( args[2] ) );
  }

  mapped = flatten_array1( mapped );   // make sure it's not an array of arrays

  if ( first )
    return mapped;
  return intersect_array( prev, mapped );
}

object *
spec_filter_closure( object *prev, string *args )
{
  closure tmpcl;
  tmpcl = make_filter_closure(args[1..]);
  return closurep(tmpcl) ? filter_array(prev, tmpcl, prev) : ({ });
}

static closure
sort_cl( status high_to_low, closure val )
{
  return lambda( ({ 'a, 'b }),
                 ({ high_to_low ? #'< : #'> ,
                      ({ #'funcall, val, 'a }),
                      ({ #'funcall, val, 'b }) }));
}

object *
spec_sort_closure( object *prev, string *args )
{
  closure tmpcl;
  tmpcl = make_filter_closure(args[1..]);
  return closurep(tmpcl) ? sort_array(prev, sort_cl(1, tmpcl)) : ({ });
}

object *
spec_revsort_closure( object *prev, string *args )
{
  closure tmpcl;
  tmpcl = make_filter_closure(args[1..]);
  return closurep(tmpcl) ? sort_array(prev, sort_cl(0, tmpcl)) : ({ });
}

object *
spec_ip( object *prev, string *args, status first )
{
  if(first)
    prev = users();
  return filter_by_hostname(prev, implode(args[1..], ".") );
}

object *
spec_level( object *prev, string *args, status first )
{
  int itmp, itmp2;

  if( first )
    prev = users();

  itmp = to_ordlevel(args[1]);
  if( sizeof(args) > 2 )
    itmp2 = to_ordlevel(args[2]);
  else
    itmp2 = itmp;
  return level_range(prev, itmp, itmp2);
}

mixed
spec_funcall_cl( string arg )
{
  string argspec, funcname, *args;
  int first_open;

  if( arg[0..1] == "->" )
    arg = arg[2..];

  first_open = member( arg, '\(' );
  if( -1 == first_open )
    return ({ #'call_other, 'thiso, arg });

  argspec = arg[ searcha_str_unescaped( arg, '\(') + 1
               ..searcha_str_unescaped( arg, '\)', strlen(arg)-1, -1) - 1];
  funcname = arg[0..member( arg, '\(' )-1 ];

  if( argspec == "" )
    return ({ #'call_other, 'thiso, funcname });

  args = map_array( explode_argspec( argspec ), #'value_closure );
  return args
    ? ( ({ #'apply, #'call_other, 'thiso, funcname }) + args )
    : 0; // there was some kind of error.
}


object *
spec_funcall( object *prev, string arg )
{
  closure cl;
  mixed the_call;
  the_call = spec_funcall_cl( arg );
  if( !pointerp( the_call ) )
    return ({ });

  cl = unbound_lambda( ({ 'thiso, 'list }), the_call );
  cl = SafeBind( cl );
  return map_array( prev, cl, prev );
}



/*----------------------------- make_closures ---------------------------*/
/*
    Description:
      Binds some closures for convenient use in ospec transforms.
    Parameters:
      None.
    Returns:
      Nothing.
    Notes:
      None.
*/
static void
make_closures()
{
  cl = ([ ]);
  exact_matches = ([ ]);
  string_prefix = ([ ]);
  number_prefix = ([ ]);
  normal_op = ([ ]);

  cl["shad_1"] = lambda( ({ 'o, 'i }),
                        ({ #'checked_item,
			   ({ #'all_shadows, 'o }), 'i }));
  cl["shad_1_rev"] = lambda( ({ 'o, 'i }),
                        ({ #'checked_item_reverse,
                           ({ #'all_shadows, 'o }), 'i }));
  cl["invnum"] = lambda( ({ 'o, 'i }),
                         ({ #'checked_item,
		           ({ #'all_inventory, 'o }), 'i }));

  cl["progname"] = lambda( ({ 'o, 'pn }),
                           ({ #'== , ({ #'load_name, 'o }), 'pn }));

  cl["present"] = lambda( ({ 'o, 'arg }),
                          ({ #'present, 'arg, 'o }) );

  exact_matches["me"] = lambda(0, ({ #'this_player }) );
  exact_matches["here"] = lambda( 0, ({ #'environment, ({ #'this_player }) }));
  exact_matches["it"] = exact_matches["him"] =
    exact_matches["her"] = exact_matches["them"] =
    lambda( ({ 'prev, 'arg, 'first }),
            ({ #'query, 'arg, DATASPACE_CL }) );


  /*******************
  ** TRANSFORMATIONS
  *******************/

  exact_matches["u"] =
    lambda( ({ 'prev, 'arg, 'first }),
            ({ #'?, 'first, ({ #'users }),
                            ({ #'filter_array, 'prev, #'interactive }) }) );

  exact_matches["l"] =
    lambda( ({ 'prev, 'arg, 'first }),
            ({ #'?, 'first, ({ #'-,
                               ({ #'flatten_array1,
                                  ({ #'m_values,
                                     ({ #'living_name_info })
                                  })
                               }),
                               quote( ({ 0 }) )
                            }),
                            ({ #'filter_array, 'prev, #'living }) }) );

  exact_matches["m"] =
    lambda( ({ 'prev, 'arg, 'first }),
            ({ #'?, 'first, ({ #'mortals }),
                            ({ #'mortals, 'prev }) }) );

  exact_matches["w"] =
    lambda( ({ 'prev, 'arg, 'first }),
            ({ #'?, 'first, ({ #'wizards }),
                            ({ #'wizards, 'prev }) }) );

  exact_matches["i"] =
    lambda( ({ 'prev, 'arg, 'first }),
            MAP_AND_FLATTEN(#'all_inventory) );

  exact_matches["I"] =
    lambda( ({ 'prev, 'arg, 'first }),
            MAP_AND_FLATTEN( #'deep_inventory ) );

  exact_matches["s"] =
    lambda( ({ 'prev, 'arg, 'first }),
            MAP_AND_FLATTEN( #'all_shadows ) );

  exact_matches["e"] =
    lambda( ({ 'prev, 'arg, 'first }),
            ({ #'map_array, 'prev, #'environment }) );

  exact_matches["E"] =
    lambda( ({ 'prev, 'arg, 'first }),
            MAP_AND_FLATTEN( #'all_environment ) );

  /************************
  **   REDUCTIONS
  ************************/

  exact_matches["!="] =
    lambda( ({ 'prev, 'arg, 'first }),
            ({ #'unique_array, 'prev }) );



  // I could make an add_string_prefix() that kept these numbers
  // accurate, but that seems like more trouble and encapsulation than
  // this is worth right now.  It will seem more valuable after I
  // forget to change this once and spend an hour figuring out why my
  // new prefix doesn't work right.  :-)
  max_len_string_prefix = 2;

  // Players
  string_prefix["*"] =
    lambda( ({ 'prev, 'arg, 'first }),
            ({ #'find_player, 'arg }) );

  // Living things
  string_prefix["@"] =
    lambda( ({ 'prev, 'arg, 'first }),
            ({ #'find_living, 'arg }) );

  // Variables
  string_prefix["$"] =
    lambda( ({ 'prev, 'arg, 'first }),
            ({ #'query, 'arg, DATASPACE_CL }) );

  // Symbols
  string_prefix["#'"] =
    lambda( ({ 'prev, 'arg, 'first }),
            MAP_AND_FLATTEN( ({ #'SafeBindSym, 'arg }) ) );

  // Inventory items
  number_prefix["#"] = number_prefix["i"] =
    lambda( ({ 'prev, 'arg, 'first }),
            MAP_AND_FLATTEN_cl("invnum") );

  // Shadows from the top
  number_prefix["s-"] =
    lambda( ({ 'prev, 'arg, 'first }),
            MAP_AND_FLATTEN_cl("shad_1_rev") );

  // Shadows from the bottom
  number_prefix["s"] =
    lambda( ({ 'prev, 'arg, 'first }),
            MAP_AND_FLATTEN_cl("shad_1") );

  // Map Objects
  string_prefix["->"] =
    lambda( ({ 'prev, 'arg, 'first }),
            ({ #'spec_funcall, 'prev, 'arg }) );

  // Add objects
  string_prefix["+"] =
    lambda( ({ 'prev, 'arg, 'first, 'priorities }),
            ({ #'+, 'prev, ({ #'parse_ospec, 'arg, 0, 'priorities }) }) );

  // Remove objects
  string_prefix["!="] =
    lambda( ({ 'prev, 'arg, 'first, 'priorities }),
            ({ #'exclude_array, 'prev,
                   ({ #'parse_ospec, 'arg, 0, 'priorities }) }) );

  // Keep if equal to ..
  string_prefix["=="] =
    lambda( ({ 'prev, 'arg, 'first, 'priorities }),
            ({ #'intersect_array, 'prev,
                   ({ #'parse_ospec, 'arg, 0, 'priorities }) }) );


  /***********************
  **     OrdLevel ranges
  ***********************/
  string_prefix[">"] =
    lambda( ({ 'prev, 'arg, 'first }),
            ({ #'level_range, 'prev,
                   ({ #'+, 1, ({ #'to_ordlevel, 'arg }) }),
                   999 }) );

  string_prefix["<"] =
    lambda( ({ 'prev, 'arg, 'first }),
            ({ #'level_range, 'prev,
                   0,
                   ({ #'-, 1, ({ #'to_ordlevel, 'arg }) }) }) );


  // Room exits
  normal_op["x"] =
    lambda( ({ 'prev, 'args, 'first }),
            ({ #'spec_rooms, 'prev, 'args, 'first }) );

  // Filter closure
  normal_op["f"] =
    lambda( ({ 'prev, 'args, 'first }),
            ({ #'spec_filter_closure, 'prev, 'args }) );

  // mapfile - take each line, map it somehow into objects
  // usage: mapfile.filename.efunname
  // example: mapfile.~/etc/folks.find_player
  normal_op["mapfile"] =
    lambda( ({ 'prev, 'args, 'first }),
            ({ #'spec_mapfile, 'prev, 'args, 'first }) );

  // Sorting closure
  normal_op["sort"] =
    lambda( ({ 'prev, 'args, 'first }),
            ({ #'spec_sort_closure, 'prev, 'args }) );

  // Sorting closure
  normal_op["sort-"] =
    lambda( ({ 'prev, 'args, 'first }),
            ({ #'spec_revsort_closure, 'prev, 'args }) );

  // Shortcut for f.==.->id."blah"
  normal_op["id"] =
    lambda( ({ 'prev, 'args, 'first }),
            ({ #'filter_objects, 'prev, "id", C_IDX('args, 1) }) );

  // Shortcut for f.->test_prop."blah"
  normal_op["prop"] =
    lambda( ({ 'prev, 'args, 'first }),
            ({ #'filter_objects, 'prev, "test_prop", C_IDX('args, 1) }) );

  // shortcut for f.==.#'program_name."blah"
  normal_op["prog"] =
    lambda( ({ 'prev, 'args, 'first }),
            ({ #'filter_array, 'prev, C_IDX( ({ #'cl }) , "progname"),
                   C_IDX('args, 1) }) );

  // IP name matches regexp
  normal_op["ip"] =
    lambda( ({ 'prev, 'args, 'first }),
            ({ #'spec_ip, 'prev, 'args, 'first }) );

  // Users/Players, possibly restricted by level
  normal_op["u"] = normal_op["level"] =
    lambda( ({ 'prev, 'args, 'first }),
            ({ #'spec_level, 'prev, 'args, 'first }) );

}

/*----------------------------- create ---------------------------*/
/*
    Description:
      Initialize this object.
    Parameters:
      None.
    Returns:
      Nothing.
    Notes:
      Called at creation time.
*/
void
create()
{
  seteuid( getuid() );

  // These delimiters can be more than one character.
  spec_delimiter     = ",";
  subspec_delimiter  = ":";
  internal_delimiter = ".";
  default_priorities = (FT_InvItem    FT_EnvItem FT_FindObject
                        FT_FindPlayer FT_File    FT_FindLiving);
  make_closures();
}

/*----------------------------- explode_argspec ---------------------------*/
/*
    Description:
      Explodes (separates) the arguments from a function call
    Parameters:
      argspec - one big string that hopefully contains all the stuff
    Returns:
      List of all the separate value specifications
    Notes:
      None.
*/
static string *
explode_argspec( string argspec )
{
  string *args;
  argspec = trim_left( trim_right( argspec, ' ' ), ' ' );
  args = explode_nested( argspec, "," );
  args = map_array( args, #'trim_left, ' ' );
  args = map_array( args, #'trim_right, ' ' );
  return args;
}



/*----------------------------- value_closure ---------------------------*/
/*
    Description:
      Creates arrays for including into filter closures.
    Parameters:
      arg - A value specifier
    Returns:
      An array, suitable for binding into a closure.
    Notes:
      See make_filter_closure().
*/
static mixed
value_closure( string arg )
{
  string tmp, argspec, *args;
  closure tmpcl;
  int itmp, max;

  if( arg == "THISO" )
    return 'thiso;

  if( arg == "LIST" )
    return 'list;

  if( ( sscanf( arg, "->%s()", tmp )
        || sscanf( arg, "->%s", tmp )
        || ( 2 == sscanf( arg, "->%s(%s)", tmp, argspec ) ) )
      || ( ( arg[0] != '\(' )
           && (( 2 == sscanf( arg, "%s(%s)", tmp, argspec ) )
               || sscanf( arg, "%s()", tmp ) ) ) )
  {
    if( argspec )
    {
      // sscanf will match to the first ')', not the real matching one.
      argspec = arg[ searcha_str_unescaped( arg, '\(') + 1
                   ..searcha_str_unescaped( arg, '\)', strlen(arg)-1, -1) - 1];
      args = map_array( explode_argspec( argspec ), #'value_closure );
      return args
        ? ({ #'funcall, #'call_other, 'thiso, tmp }) + args
        : 0; // there was some kind of error.
    } else
    {
      return ({ #'call_other, 'thiso, tmp });
    }
  }

  if( sscanf( arg, "#'%s()", tmp )
      || sscanf( arg, "#'%s", tmp ) )
  {
    tmpcl = SafeBindSym( tmp );
    return closurep( tmpcl ) ? ({ #'funcall, tmpcl, 'thiso }) : 0;
  }

  max = strlen( arg ) - 1;

  if( ( arg[0] == '\"' ) && ( arg[max] == '\"' ) )
    return arg[1..max-1];

  // We're losing our priorities list here.  Think we'll get away with it?
  if( ( arg[0] == '\(' ) && ( arg[max] == '\)' ) )
    return quote( parse_ospec( arg[1..max-1], 0, 0 ) );
  if( ( arg[0] == '\[' ) && ( arg[max] == '\]' ) )
    return checked_item( parse_ospec( arg[1..max-1], 0, 0 ), 0 );

  if( arg[0] == '0' )
    return to_int( arg );

  if ( itmp = to_int( arg ) )
    return itmp;

  return arg;
}

/* This is public so the closures can use it. */
status
equal( mixed v1, mixed v2 )
{
  if( pointerp(v1) )
    return array_equal( v1, v2 );
  return v1 == v2;
}

/*----------------------------- make_filter_closure ---------------------------*/
/*
    Description:
      Creates a closure from some specified parameters
    Parameters:
      args - A list of subterms from the ospec.
    Returns:
      A closure that takes two arguments:
        thiso - One object to consider for filtering
        list  - The list of all objects being considered
      The closure will evaluate to 1 or 0.
    Notes:
      The closure may produce side effects by calling arbitrary
      functions in the objects passed to it, arbitrary efuns with the
      object as an argument, and by calling parse_ospec().  The
      closure is bound in a safe environment.
*/
static closure
make_filter_closure( string *args )
{
  closure cl;
  mixed m1, m2, m3;

  switch( sizeof( args ) )
  {
    case 1:
      m1 = value_closure( args[0] );
      break;
    case 2:
      m1 = value_closure( args[1] );
      break;
    case 4:
      m3 = value_closure( args[3] );
    default:
      m1 = value_closure( args[1] );
      m2 = value_closure( args[2] );
  }

  if( sizeof( args ) == 1)
    cl = unbound_lambda(({ 'thiso, 'list }), m1);
  else
  {
    switch( args[0] )
    {
      case "!":
        cl = unbound_lambda(({ 'thiso, 'list }), ({ #'!, m1 }) );
        break;
      case ">":
      case "<":
      case ">=":
      case "<=":
        cl = unbound_lambda(({ 'thiso, 'list }),
                            ({ symbol_function( args[0] ), m1, m2 }) );
        break;
      case "?":
      case "?:":
        cl = unbound_lambda(({ 'thiso, 'list }), ({ #'?, m1, m2, m3 }) );
        break;
      case "==":
        cl = unbound_lambda(({ 'thiso, 'list }),
                            ({ #'call_other, THISO, "equal", m1, m2}) );
        break;
      case "!=":
        cl = unbound_lambda(({ 'thiso, 'list }),
                            ({ #'!,({ #'call_other, THISO, "equal",m1,m2}) }));
        break;
      default:
        return 0;
    }
  }
  return closurep( cl ) ? SafeBind( cl ) : 0;
}

/*----------------------------- unnest ---------------------------*/
/*
    Description:
      Strips surrounding parentheses (or something) from a string.
    Parameters:
      ospec - An ospec that may be surrounded by C_OPEN and C_CLOSE chars.
    Returns:
      ospec, not surrounded by C_OPEN and C_CLOSE chars.
    Notes:
      C_OPEN and C_CLOSE are probably parentheses, or something similar.
*/
static string
unnest( string ospec )
{
  int len;
  // Not the fastest way to do this, but it's much simpler than a custom job.
  while( ( member(C_OPEN, ospec[0]) != -1 )
         && ( member(C_CLOSE, ospec[len= strlen(ospec)-1]) != -1 )
         && ( find_close_char( ospec, 0, 0, C_OPEN, C_CLOSE ) == len ))
    ospec = ospec[1..<2];
  return ospec;
}


/*----------------------------- parse_single ---------------------------*/
/*
    Description:
      Third level parsing routine.  Handles things like "m" and "*zamboni"
    Parameters:
      arg   - A term in the ospec
      prev  - An array of objects to use as context
      first - True if this is the first term
    Returns:
      An array of objects referred to by arg in the context of prev
      and first.
    Notes:
      Can have side effects by calling parse_ospec() recursively, and
      through efun and lfun calls specified in arg.
      This is usually called from parse_subspec().
*/
static object *
parse_single(string arg, object *prev, status first, string priorities)
{
  string *args;
  string tmp;
  int numargs, itmp, itmp2;
  mixed mtmp;

  if(!stringp(arg) || arg == "")
    return prev;

  // I split it up like this so I can do faster checks on the parameters.
  args = explode_nested(arg, internal_delimiter);
  numargs = sizeof(args);

  if(!prev)
    prev = ({ });


  if(numargs == 1)
  {
    if( (tmp = unnest( arg )) != arg )
      return parse_ospec( tmp, 0, priorities );

    if( member( exact_matches, arg ) )
    {
      mtmp = funcall( exact_matches[arg], prev, arg, first );
      return pointerp(mtmp) ? mtmp : ({ mtmp });
    }

    if( -1 != ( itmp = searcha_any_str( arg, "0123465789" ) ) )
      if( member( number_prefix, tmp=arg[0..itmp-1] ) )
      {
        itmp = to_int( arg[itmp..] );
        mtmp = funcall( number_prefix[tmp], prev, itmp, first, priorities );
        return pointerp(mtmp) ? mtmp : ({ mtmp });
      }

    // for arg=="blah", and max.. == 2, check for prefixes "bl", then "b".
    itmp = strlen( arg );
    for( itmp = MIN(itmp, max_len_string_prefix) - 1; itmp >= 0; itmp-- )
    {
      if( member( string_prefix, arg[0..itmp] ) )
      {
        mtmp = funcall( string_prefix[arg[0..itmp]], prev, arg[itmp+1..],
                        first, priorities );
        return pointerp(mtmp) ? mtmp : ({ mtmp });
      }
    }

    if (sscanf(arg, "[%d]", itmp))
      return ({ checked_item(prev, itmp) });
  } else
  {

    // If there's a match for the operation named, evaluate it.
    if( member( normal_op, args[0] ) )
    {
      mtmp = funcall( normal_op[args[0]], prev, args, first, priorities );
      return pointerp(mtmp) ? mtmp : ({ mtmp });
    }

    if (arg[0] == '\[')
    {
      if (sscanf(arg, "[%d..<%d]", itmp, itmp2) == 2)
        return prev[itmp..<itmp2];
      if (sscanf(arg, "[%d..%d]", itmp, itmp2) == 2)
        return prev[itmp..itmp2];
      if (sscanf(arg, "[%d..]", itmp))
        return prev[itmp..<1];
    }
  }


  // Do we have strong reason to believe it's a file?
  if( ( member( arg, '/' ) != -1 )
      || ( arg[<2..] == ".c" ) )
    return resolve_filespec( arg, 1 );

  if( !first )
    return map_array(prev, cl["present"], arg);

  /* The DWIM last resort */
  return find_targets( arg, priorities, 1, prev );
}

/*----------------------------- parse_subspec ---------------------------*/
/*
    Description:
      Second level parsing routine.
    Parameters:
      ospec - A subspec string.
      prev  - An array of objects to use as context, or 0.
    Returns:
      An array of objects referred to by ospec in the context of prev.
    Notes:
      prev should normally be 0.
      This is usually called from parse_ospec().
      Can have side effects by calling parse_ospec() recursively.
*/
static object *
parse_subspec(string ospec, object *prev, string priorities)
{
  string *args;
  string tmp;
  int i, size, first;
  object *list;

  args = stringp(ospec)
    ? explode_nested( ospec, subspec_delimiter, 0, C_OPEN, C_CLOSE )
    : args = ({ });

  size=sizeof(args);

  if( ( size == 1 ) && ( (tmp = unnest( ospec )) != ospec ) )
    return parse_ospec( tmp, 0, priorities );

  if(!prev)
  {
    first = 1;
    prev = THISP ? ({ THISP }) : ({ });
  }

  for(i=0; i < size; i++)
  {
    if(args[i] == "")
      continue;

    prev = filter_array(prev, #'objectp);
    prev = parse_single(args[i], prev, first, priorities);
    first = 0;
  }
  return prev;
}

/*----------------------------- parse_ospec ---------------------------*/
/*
    Description:
      Top level parsing routine.
    Parameters:
      ospec - An object specification string.
      prev  - An array of objects to use as context, or 0.
    Returns:
      An array of objects referred to by ospec in the context of prev.
    Notes:
      prev should normally be 0.
      Will set one of the variables "him", "her", "it", or "them".
      Will set the variable "$".
*/
object *
parse_ospec(string ospec, object *prev, string priorities)
{
  object *list;
  object *subval;
  int size, i;
  string *sublists;
  string dataspace;

  if(!stringp(ospec) || !strlen(ospec))
    return 0;

  if( !stringp( priorities ) )
    priorities = default_priorities;

  ospec = unnest( ospec );

  sublists = explode_nested( ospec, spec_delimiter, 0, C_OPEN, C_CLOSE );
  size = sizeof(sublists);

  list = ({ });

  for(i=0; i < size; i++)
  {
    subval = parse_subspec(sublists[i], prev, priorities);
    if(pointerp(subval))
      list += subval;
  }

  list = filter_array(list, #'objectp);
  dataspace = DATASPACE;
  set("$", list, dataspace);
  if( sizeof( list ) > 0)
  {
    if( sizeof( list ) > 1)
      set( "them", list, dataspace );
    else
    {
      if( living( list[0] ) )
        switch( list[0]->query_gender() )
        {
          case "male":
            set( "him", list, dataspace ); break;
          case "female":
            set( "her", list, dataspace ); break;
          default:
            set( "it", list, dataspace );
        }
      else
        set( "it", list, dataspace );
    }
  }

  return list;
}


/*------------------------- evaluate_object_spec ---------------------------*/
/*
    Description:
      Opie conformant access point.
    Parameters:
      ospec  - An object specification string.
      tool   - The tool that is passing us this spec.
    Returns:
      An array of objects referred to by ospec.
    Notes:
      None.
*/
object *
evaluate_object_spec(string ospec, object tool, string priorities)
{
  return parse_ospec(ospec, 0, priorities);
}

/*----------------------------- object_spec_help ---------------------------*/
/*
    Description:
      Tells where help can be found.
    Parameters:
      None.
    Returns:
      The name of the file that contains help for this OSpec system.
    Notes:
      None.
*/
string
object_spec_help()
{
  return AcmeEtcDir "ospec.help";
}

/*------------------------- object_spec_variable_set -----------------------*/
/*
    Description:
      Bridge between Opie and VarSpace
    Parameters:
      key   -
      value -
    Returns:
      Nothing.
    Notes:
      None.
*/
void
object_spec_variable_set(mixed key, mixed value)
{
  if( pointerp( value ) )
    value = filter_array( value, #'objectp );

  set(key, value, DATASPACE);
}

