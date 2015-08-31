/*
    NAME
        alloc.c - ALLOCate
    DESCRIPTION
        Memory allocation functions built ontop of the mud's allocate().
        ialloc()        - allocates and initializes an array
        calloc()        - allocates and initializes an array to 0
    NOTES
        None.
    LAST MODIFIED
        Zamboni 960111

*/


#include <acme.h>
#include AcmeAllocInc

/*---------------------------------- ialloc ---------------------------------*/
/*
    description: allocates and initalizes an array
    parameters: size      - size of the array to allocate
                init      - value to initialize the array elements to
    returns: mixed *    (the initalized array)
    notes: If you want to pass an array as the init (ie: ({"test"}) ), you
           need to pass it in as a symbol (ie: '({"test"}) )
*/
mixed ialloc ( int size, mixed init )
{
   if ( pointerp ( init ) )
      init = 'init;
   return map_array ( allocate( size ),
      lambda ( ({ 'e }), ({ #'=, 'e, init }) ) );
}

/*---------------------------------- calloc ---------------------------------*/
/*
    description: allocates and initalizes an array to 0
    parameters: size      - size of the array to allocate
    returns: mixed *    (the cleared array)
    notes: None.
*/
mixed calloc ( int size )
{
    return ialloc ( size, 0 );
}
