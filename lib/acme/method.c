/*
    NAME
        method.c - METHOD package
    DESCRIPTION
        A package to run different methods (funcs or cls)
    NOTES
        None.
    LAST MODIFIED
        Aleron 970422
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
    returns: mixed        - ACME_METHOD_ERROR if it failed to run the method
                            otherwise, it returns whatever the method returns
    notes: if the method is a closure, it is apply'ed to the input
*/
varargs mixed
run_method( mixed method, mixed in, mixed extra )
{
   mixed err;
   mixed ret;

   if ( closurep( method ))
   {
      if ( err = catch( ret = apply( method, ({ in }) +
         ( extra ? ( pointerp( extra ) ? extra : ({ extra }) ) : ({}) ))))
         return error( ERR_ERROR,
            sprintf( "Could not apply closure: %O\n%O\n", method, err ),
            ACME_METHOD_ERROR );
   }
   else if ( stringp( method ) )
   {
      if ( err = catch( ret = call_other( THISO, method, in, extra ) ) )
         return error( ERR_ERROR,
            sprintf( "Could not run function: %O\n%O\n", method, err ),
            ACME_METHOD_ERROR );
   }
   else
   {
      return error( ERR_ERROR, sprintf( "Invalid method: %O", method ),
         ACME_METHOD_ERROR );
   }
   return ret;
}
