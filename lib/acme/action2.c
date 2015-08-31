/*
   NAME
       action.c - action handling package
 
   DESCRIPTION
       Contains several functions for handling query_actions() in a more
       specialized manner.  It includes:
 
       has_action               -- check if an action exists and return 
				   boolean
       action_type_char         -- return a string based on action type
       get_actions_regexp       -- filter actions based on verb regexp
				   and optional adding objects
       get_actions_literal      -- filter actions based on literal verb
				   comparison and optional adding objects
       get_all_actions          -- filter actions based on optional
				   adding objects
       get_actions_transposed   -- transpose query_actions()
 
   LAST MODIFIED
       960320 Morgaine
*/
 
#include <acme.h>
#include AcmeActionInc
 
private closure cl_regexp, cl_literal, cl_from;
 
status has_action( object who, string verb )
{
    return( !!query_actions( who, verb ) );
}
 
string action_type_char( int type )
{
    switch( type )
    {
	default:
	    return( "?" );
	case SENT_PLAIN:      return( " " );
	case SENT_SHORT_VERB: return( "S" );
	case SENT_NO_SPACE:   return( "X" );
	case SENT_NO_VERB:    return( "N" );
	case SENT_MARKER:     return( "M" );
    }
}
 
mixed get_actions_regexp( object who, string pattern, object *from )
{
    if( !objectp( who ) )
	raise_error( "bad argument 1 to get_actions_regexp\n" );
    if( !stringp( pattern ) )
	raise_error( "bad argument 2 to get_actions_regexp\n" );
 
    if( !pointerp( from ) )
	from = ({ });
    if( !closurep( cl_regexp ) )
    {
	cl_regexp = lambda( ({ 'x, 'p, 'f }),
	  ({ #'&&,
	    ({ #'sizeof, 
	      ({ #'regexp, 
	      ({ #'({, ({ #'[, 'x, VERB }) }), 'p }) 
	    }),
	    ({ #'>, ({ #'member, 'f, ({ #'[, 'x, OBJECT }) }), -1 })
	  }) );
    }
    return( filter_array( get_actions_transposed( who ), cl_regexp, pattern,
	from ) );
}
 
mixed get_actions_literal( object who, string verb, object *from )
{
    if( !objectp( who ) )
	raise_error( "bad argument 1 to get_actions_literal\n" );
    if( !stringp( verb ) )
	raise_error( "bad argument 2 to get_actions_literal\n" );
 
    if( !pointerp( from ) )
	from = ({ });
    if( !closurep( cl_literal ) )
    {
	cl_literal = lambda( ({ 'x, 'v, 'f }),
	  ({ #'&&,
	    ({ #'==, ({ #'[, 'x, VERB }), 'v }),
	    ({ #'>, ({ #'member, 'f, ({ #'[, 'x, OBJECT }) }), -1 })
	  }) );
    }
    return( filter_array( get_actions_transposed( who ), cl_literal, verb,
	from ) );
}
 
mixed get_all_actions( object who, object *from )
{
    if( !objectp( who ) )
	raise_error( "bad argument 1 to get_all_actions\n" );
    if( !pointerp( from ) )
	return( get_actions_transposed( who ) );
    if( !closurep( cl_from ) )
    {
	cl_from = lambda( ({ 'x, 'f }),
	  ({ #'>, ({ #'member, 'f, ({ #'[, 'x, OBJECT }) }), -1 }) );
    }
    return( filter_array( get_actions_transposed( who ), cl_from, from ) );
}
 
mixed get_actions_transposed( object who )
{
    mixed tp;
    int i;
 
    if( !objectp( who ) )
	raise_error( "bad argument 1 to get_actions_transposed\n" );
    for( i = 0, tp = ({ }); i < NUM_IN_MASK; i++ )
	tp += ({ query_actions( who, 1 << i ) });
    return( transpose_array( tp ) );
}
