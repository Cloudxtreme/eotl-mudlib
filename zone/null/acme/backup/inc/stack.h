#ifndef ACME_STACK_INC
#define ACME_STACK_INC

#define STACK_EMPTY   "ACME_STACK_EMTPY"

// use the StackHdl type rather than mixed since the implementation may
// change at any time
#define StackHdl mixed *

StackHdl new    ();
status   empty  ( StackHdl hdl );
void     push   ( mixed element, StackHdl hdl );
mixed    pop    ( StackHdl hdl );
mixed    top    ( StackHdl hdl );

#endif  // ACME_STACK_INC
