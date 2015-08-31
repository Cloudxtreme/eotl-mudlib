/*
    NAME
        struct.c - STRUCTure package
    DESCRIPTION
        A package to deal with structures (multilevel arrays).
        make_assignptrs()   - makes a flat array of assignment pointers to
                              a structure
    NOTES
        None.
    LAST MODIFIED
        Aleron  960202
*/

#include "/zone/null/acme/inc/acme.h"

private inherit AcmeError;

#include AcmeClosuresInc
#include AcmeErrorInc
#include AcmeStructInc

/* PUBLIC FUNCTIONS */

/*----------------------------- make_assignptrs -----------------------------*/
/*
    description: returns a flat array of closures that can assign values into
                 any array of the same structure as the input array
    parameters: array     - the input array
    returns: closure * (the flat array of closure "assignment pointers")
    notes: this will work on 1D input arrays too (but why would you want to?)

           the traversal is done depth-first
           the assignment pointers can be used by a funcall or apply

           ie: if the input array foo = ({ ({ a, b }), c }) 
               and bar = make_assignptrs ( foo );
               then funcall ( bar[1], foo, z ) would cause foo to be
               ({ ({ a, z }), c })
*/
closure *make_assignptrs ( mixed *array )
{
    return assignptrs ( array, ({}), 'a );
}


/* PRIVATE FUNCTIONS */

private
closure *assignptrs ( mixed *array, closure *ptrs, mixed sym )
{
    int i, max;

    max = sizeof ( array );
    for ( i = 0; i < max; i++ )
    {
        if ( pointerp ( array[i] ) )
        {
            ptrs = assignptrs ( array[i], ptrs, ({ #'[, sym, i }) );
        }
        else
        {
            ptrs += ({
                lambda ( ({ 'a, 'val }),
                    ({ #'=, ({ #'[, sym, i }), 'val }) )   
            });
        }
     }
    return ptrs;
}
