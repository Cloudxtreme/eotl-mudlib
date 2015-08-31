/*
    NAME
        graph.c - GRAPH algorithms package
    DESCRIPTION
        A generalized set of graph algorithms.
        check_connectivity()    - checks the connectivity of a graph
    NOTES
        None.
    LAST MODIFIED
        Zamboni 970120
*/

#include <acme.h>

private inherit AcmeAlloc;
private inherit AcmeError;

#include AcmeAllocInc
#include AcmeErrorInc
#include AcmeGraphInc

varargs
void touch ( mixed *graph, int pos, mixed getneighbors, status *seen,
            mixed *keys );


/* PUBLIC FUNCTIONS */

/*---------------------------- check_connectivity ---------------------------*/
/*
    description: checks the connectivity of a graph
    parameters: graph         - either an array or mapping representing a graph
                getneightbors - a method (func name or closure) to get the
                                neighboring nodes from the graph structure
                node          - check the connectivity of the graph with this
                                node removed.  if graph is an array, node is
                                int index.  if graph is a mapping, node is a
                                key.  furthermore, if node is GRAPH_ALL, then
                                it checks connectivity on the whole graph
    returns: status     - whether the graph is fully connected or not
    notes: the getneighbors method should return an array of int index if
           the graph is an array, else if the graph is a mapping, it should
           return an array of keys
*/
// if graph is array, node is int pos, else if graph is mapping, is key
status check_connectivity ( mixed graph, mixed getneighbors, mixed node )
{
   status   *seen;
   mixed     keys;
   int       i, n_nodes;

   n_nodes = sizeof ( graph );
   if ( n_nodes < 2 )
      return 1;
   
   keys = 0;
   seen = calloc ( n_nodes );

   if ( mappingp ( graph ) )
      keys = m_indices ( graph );

   if ( GRAPH_ALL == node )
      i = -1;
   else if ( keys )
      i = member ( keys, node );
   else
      i = to_int ( node );

   /* if node is negative, then we set it to 1 so when we ! it, it starts at 0
    | otherwise, we dirty the node we're looking at and then ! it so we
    | don't accidentially start on the same room we just dirtied
    */
   if ( -1 == i )
      i = 1;
   else
      seen[ i ] = 1;
   i = !i;
   
   touch ( graph, i, getneighbors, seen, keys );

   for ( i = 0; i < n_nodes; i++ )
      if ( 0 == seen[ i ] )
         return 0;
   return 1;
}

/* PRIVATE FUNCTIONS */

/*---------------------------------- touch ----------------------------------*/
/*
    description: touches a node and dirties it
    parameters: graph         - either an array or mapping representing a graph
                pos           - position of the node we're on
                getneightbors - a method (func name or closure) to get the
                                neighboring nodes from the graph structure
                seen          - dirty array
                keys          - if graph is a mapping, lookup table to map
                                graph to seen array, else keys is 0
    returns: void
    notes: None.
*/
void touch ( mixed *graph, int pos, mixed getneighbors, status *seen,
            mixed *keys )
{
   mixed *neighbors;
   mixed  tmppos;
   int i, max;

   if ( seen [ pos ] )
      return;
   seen [ pos ] = 1;

   if ( keys )
      tmppos = keys[ pos ];
   else
      tmppos = pos;

   if ( stringp ( getneighbors ) )
      neighbors = call_other ( THISO, getneighbors, graph[ tmppos ] );
   else if ( closurep ( getneighbors ) )
      neighbors = funcall ( getneighbors, graph[ tmppos ] );
   else
      return error ( ERR_ERROR, "touch: getneighbors is not a proper method " +
             "(function name or closure )\n" );

   max = sizeof ( neighbors );

   if ( keys )
      for ( i = 0; i < max; i++ )
         touch ( graph, member ( keys, neighbors[ i ] ), getneighbors, seen,
                keys );
   else
      for ( i = 0; i < max; i++ )
         touch ( graph, neighbors[ i ], getneighbors, seen, keys );
}
