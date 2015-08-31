/*
    NAME
        method.c - METHOD package
    DESCRIPTION
        A package to run different methods (funcs or cls)
    NOTES
        None.
    LAST MODIFIED
        Aleron 960403
*/

#include <acme.h>

private inherit AcmeError;

#include AcmeErrorInc
#include AcmeMethodInc

//-------------------------------- run_method -------------------------------//
/*
    description: runs a method (func or cl)
    parameters: method    - the method (a function or a closure)
                in        - the input to the method
                extra     - optional extra param to the method
    returns: void
    notes: if the method is a closure, it is apply'ed to the input
*/
varargs status
run_method( mixed method, mixed in, mixed extra )
{
   if ( closurep( method ) )
   {
      if ( catch( apply( method, ({ in }) +
         ( extra ? ( pointerp( extra ) ? extra : ({ extra }) ) : ({}) ) ) ) )
         return error( ERR_ERROR, sprintf( "Couldn't not apply closure %O\n",
            method ), 0 );
   }
   else if ( stringp( method ) )
   {
      if ( catch( call_other( THISO, method, in, extra ) ) )
         return error( ERR_ERROR, sprintf( "Couldn't not run function: %O\n",
            method ), 0 );
   }
   else
   {
      return error( ERR_ERROR, sprintf( "Invalid method: %O", method ), 0 );
   }
}
