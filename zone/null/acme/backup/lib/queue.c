/*
    NAME
        queue.c - QUEUE package
    DESCRIPTION
        Implementation of the queue datatype and its methods.
        new()      - create a new queue
        emtpy()    - test if a queue is emtpy
        enqueue()  - enter an element into the queue
        dequeue()  - remove an element from the queue
        front()    - return the front of the queue without removing it
    NOTES
        None.
    LAST MODIFIED
        Aleron 960401
*/

#include <acme.h>

private inherit AcmeError;

#include AcmeErrorInc
#include AcmeQueueInc

// private defines
#define InitQueue    ({({})})
#define ValidHdl(h)  (pointerp(h) && sizeof(h) == 1 && pointerp(h[0]))

//----------------------------------- new -----------------------------------//
/*
    description: create a new queue
    parameters: None.
    returns: QueueHdl - a handle to the new queue
    notes: None.
*/
QueueHdl
new()
{
   return InitQueue;
}

//---------------------------------- emtpy ----------------------------------//
/*
    description: test if a queue is emtpy
    parameters: hdl       - handle to the queue to test
    returns: status       - whether or not the queue is empty
    notes: None.
*/
status
empty( QueueHdl hdl )
{
   return ( sizeof( hdl[0] ) == 0 );
}

//--------------------------------- enqueue ---------------------------------//
/*
    description: enter an element into the queue
    parameters: elem      - the element enter
                hdl       - the queue handle
    returns: void
    notes: None.
*/
void
enqueue( mixed elem, QueueHdl hdl )
{
   if ( !ValidHdl( hdl ) )
      return error( ERR_ERROR, "Invalid queue handle." );
   hdl[ 0 ] += ({ elem });
}

//--------------------------------- dequeue ---------------------------------//
/*
    description: remove an element from the queue
    parameters: hdl       - the queue handle
    returns: mixed        - the queue element popped (or QUEUE_EMPTY)
    notes: None.
*/
mixed
dequeue( QueueHdl hdl )
{
   mixed  elem;
   int    queue_sz;

   if ( !ValidHdl( hdl ) )
      return error( ERR_ERROR, "Invalid queue handle." );

   if ( !( queue_sz = sizeof( hdl ) ) )
      return QUEUE_EMPTY;

   queue_sz--;
   elem = hdl[0][ queue_sz ];
   hdl[0] = deletea( hdl[0], queue_sz, queue_sz );
   return elem;
}

//---------------------------------- front ----------------------------------//
/*
    description: return the front of the queue without removing it
    parameters: hdl       - the queue handle
    returns: mixed        - the top queue element (or QUEUE_EMPTY)
    notes: None.
*/
mixed
front( QueueHdl hdl )
{
   int queue_sz;

   if ( !ValidHdl( hdl ) )
      return error( ERR_ERROR, "Invalid queue handle." );

   if ( !( queue_sz = sizeof( hdl ) ) )
      return QUEUE_EMPTY;

   return hdl[0][ queue_sz-1 ];
}
