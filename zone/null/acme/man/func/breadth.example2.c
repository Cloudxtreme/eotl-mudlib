#include <acme.h>

inherit AcmeBreadth;
#include AcmeBreadthInc

string Targetname;

// in this example, our node structure corresponds to
// ([ room file name : ({ exits to get to this room from our start }) ])
#define RM(node)       (m_indices(node))[0]
#define PATH(rm,node)  (node[rm])

mixed
next( mixed current )
{
   mapping  exits;
   string  *dirs;
   mixed    ret;
   mixed    rm;
   string  *path;
   int      i, max;

   ret = ({});

   // unpack the room and the path
   rm   = RM( current );
   path = PATH( rm, current );

   // this will only occur in a room that I don't have permissions to query
   if ( !( exits = rm->query( "exits" )))
      return ({});

   // get all the exit names
   dirs = m_indices( exits );
  
   max = sizeof( dirs );
   for ( i = 0; i < max; i++ )
   {
      // build up an array of the neighboring rooms with the histories
      ret += ({ ([ exits[ dirs[ i ] ] : path + ({ dirs [ i ] }) ]) });
   }
   return ret;
}

mixed
search( mixed current, int depth )
{
   mixed   rm;
   string *path;
   object *inva;
   int     i, max;

   // unpack the room and the path
   rm   = RM( current );
   path = PATH( rm, current );

   // get all the items in the room
   inva = inventorya( FINDO( rm ));

   max = sizeof( inva );
   for ( i = 0; i < max; i++ )
   {
      if ( living( inva[i] ) && inva[i]->id( Targetname ) )
      {
         return path;
      }
   }
   return CONTINUE_TRAVERSAL;
}

void
find( string targ, int maxdepth )
{
   string *path;
   string  out;
   int     i, max;

   if ( !( Targetname = targ ) )
   {
      printf( "Error: No target\n" );
      return;
   }

   path = breadth_first( ([ file_name( ENV( THISP )) : ({}) ]), maxdepth,
                        "next", "search" );
   if ( !pointerp( path ) )  // either an error or didn't find the target
   {
      printf( "Couldn't find %s\n", capitalize( targ ) );
      return;
   }
   max = sizeof( path ) - 1;
   if ( !max )   // target is only one room away
   {
      out = path[ 0 ];
   }
   else
   {
      out = "";
      for ( i = 0; i < max; i++ )
      {
         out += path[ i ] + ", ";
      }
      out += "and " + path[ max ];
   }
   printf( "%s is %s from you.\n", capitalize( targ ), out );
}

void
create()
{
   seteuid( getuid());
}
