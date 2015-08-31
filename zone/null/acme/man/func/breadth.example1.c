#include <acme.h>

inherit AcmeBreadth;
#include AcmeBreadthInc

mixed
next( mixed currentrm )
{
   mapping  exits;
   string  *dirs;
   mixed    ret;
   int      i, max;

   ret = ({});

   // this will only occur in a room that I don't have permissions to query
   if ( !( exits = currentrm->query( "exits" )))
      return ({});

   // get all the exit names
   dirs = m_indices( exits );

   max = sizeof( dirs );
   for ( i = 0; i < max; i++ )
   {
      // build up an array of the neighboring rooms
      ret += ({ exits[ dirs[ i ] ] });
   }

   return ret;
}

// print out the filename of the room and continue traversing
mixed
print( mixed current, int depth )
{
   printf( "%d%' '*s%O\n", depth, depth*2, "", current );
   return CONTINUE_TRAVERSAL;
}

status
list( int maxdepth )
{
   return breadth_first( file_name( ENV( THISP )), maxdepth, "next", "print" );
}

void
create()
{
   seteuid( getuid());
}
