/*
    NAME
        stack.c - STACK package
    DESCRIPTION
        Implementation of the stack datatype and its methods.
        newstack()    - create a new stack
        emptystack()  - test if a stack is empty
        push()        - push a stack element on to the stack indicated by hdl
        pop()         - pop a stack element from the top of a stack and return it
        top()         - return the top of the stack without popping it
    NOTES
        None.
    LAST MODIFIED
        Aleron 960401
*/


#include <acme.h>

private inherit AcmeError;

#include AcmeErrorInc
#include AcmeStackInc

// private defines
#define InitStack    ({({})})
#define ValidHdl(h)  (pointerp(h) && sizeof(h) == 1 && pointerp(h[0]))

//---------------------------------- newstack -------------------------------//
/*
    description: create a new stack
    parameters: None.
    returns: StackHdl - a handle to the new stack
    notes: None.
*/
StackHdl
newstack()
{
   return InitStack;
}

//------------------------------- emptystack --------------------------------//
/*
    description: test if a stack is empty
    parameters: hdl       - handle to the stack to test
    returns: status       - whether or not the stack is empty
    notes: None.
*/
status
emptystack( StackHdl hdl )
{
   return ( sizeof( hdl[0] ) == 0 );
}

//----------------------------------- push ----------------------------------//
/*
    description: push a stack element on to the stack indicated by hdl
    parameters: elem      - the element to push
                hdl       - the stack handle
    returns: void
    notes: None.
*/
void
push( mixed elem, StackHdl hdl )
{
   if ( !ValidHdl( hdl ) )
      return error( ERR_ERROR, "Invalid stack handle." );
   hdl[ 0 ] += ({ elem });
}

//----------------------------------- pop -----------------------------------//
/*
    description: pop a stack element from the top of a stack and return it
    parameters: hdl       - the stack handle
    returns: mixed        - the stack element popped (or STACK_EMPTY)
    notes: None.
*/
mixed
pop( StackHdl hdl )
{
   mixed  elem;
   int    stack_sz;

   if ( !ValidHdl( hdl ) )
      return error( ERR_ERROR, "Invalid stack handle." );

   if ( !( stack_sz = sizeof( hdl[0] ) ) )
      return STACK_EMPTY;

   stack_sz--;
   elem = hdl[0][ stack_sz ];
   hdl[0] = deletea( hdl[0], stack_sz, stack_sz );
   return elem;
}

//----------------------------------- top -----------------------------------//
/*
    description: return the top of the stack without popping it
    parameters: hdl       - the stack handle
    returns: mixed        - the top stack element (or STACK_EMPTY)
    notes: None.
*/
mixed
top( StackHdl hdl )
{
   int stack_sz;

   if ( !ValidHdl( hdl ) )
      return error( ERR_ERROR, "Invalid stack handle." );

   if ( !( stack_sz = sizeof( hdl[0] ) ) )
      return STACK_EMPTY;

   return hdl[0][ stack_sz-1 ];
}
