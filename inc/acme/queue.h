#ifndef ACME_QUEUE_INC
#define ACME_QUEUE_INC

#define QUEUE_EMPTY   "ACME_QUEUE_EMTPY"

// use the QueueHdl type rather than mixed since the implementation may
// change at any time
#define QueueHdl mixed *

QueueHdl newqueue     ();
status   emptyqueue   ( QueueHdl hdl );
void     enqueue      ( mixed element, QueueHdl hdl );
mixed    dequeue      ( QueueHdl hdl );
mixed    front        ( QueueHdl hdl );

#endif  // ACME_QUEUE_INC
