/*
    NAME
        output.c - OUTPUT library package
    DESCRIPTION
        A package to deal with all kinds of output.
        output()        - function to send text different locales in a manner
                          consistent with feelings
    NOTES
        None.
    LAST MODIFIED
        Aleron 960823

*/

#include <acme.h>
#include AcmeOutputInc

#define NULLSTR(s) (!s||""==s)
#define PRINT(x)   sprintf("%s\n",x)

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
    notes: If the silent flag is set, it will only output the self message
*/
varargs void
output( string s, string o, object tob, string t, string tenv, status silent )
{
   object *exclude;

   if ( !NULLSTR( s ) )
      tell_object( THISP, PRINT( s ) );

   if ( silent )               // if silent, we can return now
      return;

   exclude = ({ THISP });      // exclude THISP from room tells
   if ( tob )
      exclude += ({ tob });    // exclude tob also

   if ( tob && !NULLSTR( t ) )
   {
      tell_object( tob,  PRINT( t ) );
   }

   // now handle the room tells
   if ( !NULLSTR( o ) )
      tell_room( ENV( THISP ), PRINT( o ), exclude );
   
   if ( tob && !NULLSTR( tenv ) )
      tell_room( ENV( tob ), PRINT( tenv ), exclude );
}
