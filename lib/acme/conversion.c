/*
    NAME
        conversion.c - CONVERSION package
    DESCRIPTION
        A package to deal with conversions that the standard mudlib doesn't do
        array2map()    - transforms a 1-D array into a mapping
        array2D2map()  - transforms a 2-D array into a mapping
    NOTES
        None.
    LAST MODIFIED
       Aleron 960216
*/
#include <acme.h>

inherit AcmeError;

#include AcmeErrorInc
#include AcmeConversionInc

/*-------------------------------- array2map --------------------------------*/
/*
    description: transforms a 1-D array into a mapping
    parameters: arr     - 1-D array to transform into a mapping
    returns: a mapping where the first array elem maps to the second, third
             to the fourth and so on.
    notes: ie: ({ a, b, c, d }) -> ([ a: b, c: d ])
           If the array has an odd number of members, the last element is
           ignored.
*/
mapping array2map( mixed arr )
{
   mapping ret;
   int     i, max;

   ret = ([]);
   if ( pointerp( arr ) )
      for ( max = sizeof( arr ), i = 0; i+1 < max; i+= 2 )
         ret += ([ arr[i] : arr[i+1] ]);
   return ret;
}

/*------------------------------- array2D2map -------------------------------*/
/*
    description: transforms a 2-D array into a mapping
    parameters: arr     - 2-D array to transform into a mapping
    returns: a mapping
    notes: ie: ({ ({ 1, 2 }), ({ 3, 4 }) }) -> ([ 1: 2, 3: 4 ])
*/
mapping array2D2map( mixed arr )
{
   mapping ret;
   int     i, max;

   ret = ([]);
   if ( pointerp( arr ) )
      for ( max = sizeof( arr ), i = 0; i < max; ++i )
         if ( pointerp( arr[i] ) && sizeof( arr[i] ) == 2 )
            ret += ([ arr[i][0] : arr[i][1] ]);
         else
            return error( ERR_ERROR, "Input array is not 2-dimensional!", 0 );
   return ret;
}
