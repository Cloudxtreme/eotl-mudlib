/*
    NAME
        math.c - Handy integer manipulation routines
    DESCRIPTION
        from_hex()         - Convert a string of hex digits to a decimal int
        from_octal()       - Convert a string of octal digits to a decimal int
        pow()              - Calculate exponents (macro)
    NOTES
        None.
    LAST MODIFIED
        Morgaine 960406
*/

#include <acme.h>
#include AcmeMathInc

/* converts a string of hex chars to an integer 
** done is not a required argument, but if passed by reference, it
** will be set to: -1, we didnt finish it all, 1, we didnt do any of it,
** or 0, we converted the whole string.
**
** it should be noted that since an integer is 32 bits, a hex number can
** only be 8 digits in length.  If it is larger than that, done will be
** set to -2 and the number up to that point returned.
*/
 
varargs int from_hex( string str, int done )
{
    int ret, tmp;
 
    if( !stringp( str ) || !strlen( str ) )
        return( done = 1, 0 );
    str = lower_case( str );
    do
    {
        if( ( tmp = member( "0123456789abcdef", str[ 0 ] ) ) < 0 )
            return( done = ret ? -1 : 1, ret );
        ret <<= 4;
        ret |= tmp;
        if( ( ret & 0xF8000000 ) && ( strlen( str ) > 1 ) )
            return( done = -2, ret );
    }
    while( strlen( str = str[ 1 .. ] ) );   /* wah while( *++str ); :( */
    
    return( ret );
}
 
/* converts a string of octal chars to an integer.  same thing as from_hex */
 
varargs int from_octal( string str, int done )
{
    int ret, tmp;
 
    if( !stringp( str ) || !strlen( str ) )
        return( done = 1, 0 );
    do
    {
        if( ( tmp = member( "01234567", str[ 0 ] ) ) < 0 )
            return( done = ret ? 1 : -1, ret );
        ret <<= 3;
        ret |= tmp;
        if( ( ret & 0xF8000000 ) && ( strlen( str ) > 1 ) )
            return( done = -2, ret );
    }
    while( strlen( str = str[ 1 .. ] ) );
    
    return( ret );
}

varargs int from_binary( string str, int done )
{
    int ret, tmp;
 
    if( !stringp( str ) || !strlen( str ) )
        return( done = 1, 0 );
    do
    {
        if( ( tmp = member( "01", str[ 0 ] ) ) < 0 )
            return( done = ret ? 1 : -1, ret );
        ret <<= 1;
        ret |= tmp;
        if( ( ret & 0xF8000000 ) && ( strlen( str ) > 1 ) )
            return( done = -2, ret );
    }
    while( strlen( str = str[ 1 .. ] ) );
 
    return( ret );
}
 
string to_octal( int num )
{
    return( sprintf( "%o", num ) );
}

varargs string to_hex( int num, int lower )
{
    return( sprintf( lower ? "%x" : "%X", num ) );
}

string to_binary( int num )
{
    string out;
    int    i, valid, digit;
 
    for( out = "", i = 0; i < 32; i++ )
    {
        if( !( digit = num & 0x80000000 ) && valid )
            out += "0";
        else if( digit )
        {
            valid = TRUE;
            out += "1";
        }
        num <<= 1;
    }
 
    return( strlen( out ) && out || "0" );
}
