/*
    NAME

        closure_eval_server.c - Something to bind untrusted closures

    DESCRIPTION

        A server that will bind arbitrary unbound closures and symbols
        so that you can evaluate untrusted closures without risking
        anything.  Useful for providing ways for users to construct
        closures at runtime without having to check them for
        undesirable operations.

    NOTES

        This object is designed to evaluate closures in a secure
        environment (one with no access.)  That means the AccessObject
        MUST assign it a harmless uid.  (Users of the server don't
        need to worry about this.)

    LAST MODIFIED

        Zamboni 960111

*/

 
/* name: secure_closure
 * arguments: cl - an unbound lambda closure.
 * description: binds unbound_closure to THISO and returns it (bound).
 */
nomask closure 
secure_closure( closure cl )
{
  if( !closurep( cl ) )
    return 0;

  cl = bind_lambda( cl );
  return cl;
}
 
/* name: secure_closure
 * arguments: unbound_closure is an unbound lambda closure.
 * description: binds unbound_closure to THISO, saves the closure and returns
 *       it (bound).
 */
nomask closure 
secure_fsym( string fsym )
{
  if( !stringp( fsym ) )
    return 0;

  return symbol_function( fsym );
}
 
/* name: create
 * description: initialize this object's variables.
 */
nomask void 
create()
{
  seteuid( getuid() );
}

