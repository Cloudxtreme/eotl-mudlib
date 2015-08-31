#include <acme.h>

private inherit AcmeMethod;
private inherit AcmeQueue;

#include AcmeMethodInc
#include AcmeQueueInc
#include AcmeBreadthInc

/*------------------------------ breadth_first ------------------------------*/
/*
    description: breadth first traversal
    parameters: rootnode  - the root of the tree/graph that we're traversing
                maxdepth  - maximum depth to traverse to
                next      - method to get next nodes/leaves
                method    - optional method to apply to each node
                seen      - optional custom method to check to see if a node
                            has already been seen (in order to avoid revisits).
                            the default checking should work for almost all
                            cases, so unless you're doing something utterly
                            weird, you can leave this alone
    returns: mixed        - (1), if it completes the traversal
                            ACME_METHOD_ERROR, if any of the methods fail
                            otherwise, it returns whatever the method returns
    notes: the user-defined next, method and seen methods should have
           signatures (whether they're functions or closures) equivalent to
           having the following prototypes:

           // returns an array of the next nodes based on the current node
           mixed next( mixed current );

           // do something based on the current node and return either
           // CONTINUE_TRAVERSAL to not interrupt the traversal, or any
           // other value which will cause the traversal to end and return
           // that value
           mixed method( mixed current, int depth );

           // returns 1 if the current node has been seen
           status seen( mixed current, mixed *seennodes );
*/
varargs mixed
breadth_first( mixed rootnode, int maxdepth, mixed next, mixed method,
              mixed seen )
{
   QueueHdl  qhdl;
   mixed     nextnodes;     // array of next nodes to look at
   mixed    *seennodes;     // nodes we've already seen
   mapping   depth;         // list of recursion depth for each node
   mixed     current;       // current node
   mixed     ret;           // return value
   int       i, max;

   seennodes = ({});
   qhdl = newqueue();
   enqueue( rootnode, qhdl );
   depth = ([ rootnode : 0 ]);
   while ( !emptyqueue( qhdl ) )
   {
      current = dequeue( qhdl );
      if ( !seen )  // use the default check
      {
         ret = ( member( seennodes, current ) != -1 );
      }
      // otherwise, use the custom seen method and return error if any
      else if ( ACME_METHOD_ERROR ==
               ( ret = run_method( seen, current, seennodes )))
      {
         return ret;
      }
      // if we've already seen this node, skip it
      if ( ret )
      {
         continue;
      }

      // tack it on the seen list
      seennodes += ({ current });

      // apply the method to the current node
      if ( method && CONTINUE_TRAVERSAL != 
           ( ret = run_method( method, current, depth[ current ] )))
      {
         return ret;
      }

      if ( depth[ current ] < maxdepth )
      {
         if ( ACME_METHOD_ERROR == ( nextnodes = run_method( next, current )))
            return nextnodes;
         max = sizeof( nextnodes );
         for ( i = 0; i < max; ++i )
         {
            enqueue( nextnodes[ i ], qhdl );
            if ( !depth[ nextnodes[ i ] ] )
               depth[ nextnodes[ i ] ] = depth[ current ] + 1;
         }
      }
   }
   return 1;
}
