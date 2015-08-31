#include <acme.h>

private inherit AcmeMethod;
private inherit AcmeStack;

#include AcmeMethodInc
#include AcmeStackInc
#include AcmeDepthInc

// Rather than muck about inheriting push() and pop() and then adding behavior
// I'll just use the PUSH() and POP() macros.  It should be marginally faster
// too since it'll all be inline

// IN: N = node, S = stack handle, D = depth
#define PUSH(N,S,D)     push(N,S); seennodes[N]=-1; depth[N]=D;

// IN:  N = node, S = stack handle    OUT: N = node
#define POP(N,S)        N=pop(S); seennodes[N]=1;


/*------------------------------- depth_first -------------------------------*/
/*
    description: depth first traversal
    parameters: rootnode   - the root of the tree/graph to traverse
                maxdepth   - maximum depth to traverse to
                next       - method to get next nodes/leaves
                prefix     - optional prefix method to apply to each node
                infix      - optional infix method to apply to each node
                postfix    - optional postfix method to apply to each node
                seen       - optional custom method to check if a node has
                             already been seen (in order to avoid revisits).
                             the default checking should work for almost all
                             cases, so unless you're doing something utterly
                             weird, you can leave this alone
    returns: mixed         - (1) if it completes the traversal
                             ACME_METHOD_ERROR, if any of the methods fail
                             otherwise, it returns whatever the method returns
    notes: The infix method gets applied on the current node between each
           and everyone of its children...this could be dangerous if you
           don't use it correctly.

           The user-defined next, pre-/in-/post-fix methods and seen methods
           should have signatures (whether they're functions or closures)
           equivalent to having the following prototypes:

           // returns an array of the next nodes based on the current node
           mixed next( mixed current );

           // do something based on the current node and return either
           // CONTINUE_TRAVERSAL to not interrupt the traversal, or any
           // other value which will cause the traversal to end and return
           // that value
           mixed pre-in-post-fix( mixed current, int depth );

           // returns 1 if the current node has been seen
           // seennodes is a mapping from node to value, where the value
           // will be 1 if seen, 0 if unseen and -1 if currently on the
           // stack (and thusly visted once, but not yet seen)
           status seen( mixed current, mapping seennodes );
*/
varargs mixed
depth_first( mixed rootnode, int maxdepth, mixed next, mixed prefix,
             mixed infix, mixed postfix, mixed seen )
{
   StackHdl  shdl;
   mixed     nextnodes;     // array of next nodes to look at
   mapping   seennodes;     // nodes we've already seen, 0 is unseen, 1 seen,
                            // and -1 means it's currently on the stack
   mapping   depth;         // list of recursion depth for each node
   mixed     current;       // current node
   mixed     ret;           // return value
   status    dirty;         // we added to the stack
   int       i;

   // init
   shdl      = newstack();
   seennodes = ([]);
   depth     = ([]);

   PUSH( rootnode, shdl, 0 );

   // apply the prefix method if it exists
   if ( prefix && ( CONTINUE_TRAVERSAL !=
      ( ret = run_method( prefix, rootnode, depth[ rootnode ]))))
   {
      return ret;
   }

   while ( !emptystack( shdl ))
   {
      current = top( shdl );
      dirty   = 0;

#ifdef DEBUG      
      printf("current= %O\tshdl= %O\nseennodes= %O\ndepth= %O\n----------\n",
             current, shdl, seennodes, depth);
#endif  // DEBUG

      if ( depth[ current ] < maxdepth )
      {
         // push the current node's children/neighbors on the stack
         if ( ACME_METHOD_ERROR == ( nextnodes = run_method( next, current )))
         {
            return nextnodes;
         }
         // by using this linear stack method, we'd end up pushing the
         // children in the reverse order that the recursive method would
         // do it...so in order to maintain what the user expects, we'll
         // decend our list of nextnodes
         for ( i = sizeof( nextnodes ) - 1; i >= 0; --i )
         {
            // only push this child if we haven't seen or visited it
            if ( !seen )  // use the default check
            {
               // seen or visited?
               ret = ( 0 != seennodes[ nextnodes[ i ] ] );
            }
            // otherwise, use the custom seen method and return error if any
            else if ( ACME_METHOD_ERROR ==
                     ( ret = run_method( seen, nextnodes[ i ], seennodes )))
            {
               return ret;
            }
            if ( !ret )
            {
               dirty = 1;
               PUSH( nextnodes[ i ], shdl, depth[ current ] + 1 );

               // apply the prefix method if it exists
               if ( prefix && ( CONTINUE_TRAVERSAL !=
                  ( ret = run_method( prefix, nextnodes[ i ],
                                     depth[ nextnodes[ i ] ]))))
               {
                  return ret;
               }
            }
         }
      }
      if ( !dirty )  // if this is effectively a leaf node, then pop it
      {
         POP( current, shdl );

         // apply the postfix method if it exists
         if ( postfix && ( CONTINUE_TRAVERSAL !=
            ( ret = run_method( postfix, current, depth[ current ] ))))
         {
            return ret;
         }
      }
   }
   return 1;
}
