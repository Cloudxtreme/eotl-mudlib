/*
    NAME
        error.c - ERROR library package
    DESCRIPTION
        A package to deal with errors.
        error()   - sends error messages to the player
    NOTES
        None.
    LAST MODIFIED
        Aleron 960320

*/

#include <acme.h>
#include AcmeErrorInc

#define ESTRING ( { "usage", "note", "warning", "error" } )

/*---------------------------------- error ----------------------------------*/
/*
    description: a standardized way of sending error messages to the player
    parameters: severity  - how bad the error is
                msg       - the message to be sent
                ret       - the value for error to return
    returns: ret (so you can do something like "return error(blahbalh, 0);")
    notes: see ESTRING array for error severities
*/
varargs mixed error ( int severity, string msg, mixed ret )
{
    printf( "%s: %s\n", ESTRING[ severity ], msg );
    return ret;
}
