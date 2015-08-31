/*
    NAME
        output.c - OUTPUT library package
    DESCRIPTION
        A package to deal with all kinds of output.
        output()        - function to send text different locales
    NOTES
        None.
    LAST MODIFIED
        Aleron 960328

*/

#include <acme.h>
#include AcmeOutputInc

/*---------------------------------- output ---------------------------------*/
/*
    description: A generalized output function to be able to send text to
                 THISP, a target, and the target's room.
    parameters: s         - self message
                o         - others message
                tob       - target, if any
                t         - target message
                tenv      - target environment message
    returns: void
    notes: If the property SilentP is set in THISO, it will only output the
           self message.
*/
varargs void
output( string s, mixed o, object tob, string t, string tenv )
{
    status silent;

    if ( s )
        write ( s + "\n" );
    silent = ( function_exists ( "test_prop", THISO ) &&
               THISO->test_prop ( SilentP ) ) ? 1 : 0;
    if ( !tob && !silent && o && "" != o )
        say( o + "\n", THISP );
    else{
        if ( !silent ){
            if ( tenv && ENV ( tob ) != ENV ( THISP ) )
                tell_room ( ENV ( tob ), tenv + "\n", ( { tob, THISP } ) );
            tell_object ( tob, t + "\n" );
        }
        say ( o + "\n", ({ tob, THISP }) );
    }
}
