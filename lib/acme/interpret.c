/*
** AcmeInterpret
**
** Several functions to convert strings (user input?) to machine
** readable values (ie, string to closure, etc)
**
** 980822 Morgaine
*/

#include <acme.h>
#include AcmeInterpretInc

int interpret_number_as_base( int base, string number, int failed )
{
    string base_digits;
    int i, modifier, value, ret;

    /*** specify which digits are ok for this particular base ***/
    base_digits = BASE_DIGITS[ 0 .. base - 1 ];

    /*** put the digits in the number into lower case ***/
    number = lower_case( number );

    /*** loop (from the back!) ***/
    for( modifier = 1, i = strlen( number ); i--; )
    {
        /*** is this a valid digit? ***/
        if( ( value = member( base_digits, number[ i ] ) ) < 0 )
        {
            failed = TRUE;
            return( 0 );
        }

        ret += value * modifier;
        modifier *= base;
    }

    return( ret );
}

float interpret_as_float( string number, int failed )
{
    int i, sz;

    for( sz = strlen( number ); i < sz; i++ )
    {
        if( member( "0123456789.", number[ i ] ) < 0 )
            return( failed = TRUE, 0 );
    }

    return( to_float( number ) );
}

int interpret_as_number( string number, int failed )
{
    int base;
    mixed ret;
    int negative;

    if( number[ 0 ] == '-' )
    {
        number = number[ 1 .. ];
        negative = TRUE;
    }

    if( member( number, '.' ) > -1 )
        ret = interpret_as_float( number, &failed );
    else if( number[ 0 .. 1 ] == "0x" )
        ret = interpret_number_as_base( 16, number[ 2 .. ], &failed );
    else if( number[ 0 ] == '0' )
        ret = interpret_number_as_base( 8, number[ 1 .. ], &failed );
    else if( sscanf( number, "%dr%s", base, number ) == 2 )
        ret = interpret_number_as_base( base, number, &failed );
    else
        ret = interpret_number_as_base( 10, number, &failed );

    if( negative && !failed )
        ret = negate( ret );

    return( ret );
}

mixed interpret_as_closure( string cl )
{
    closure ret;
    mixed   ob;

    if( cl[ 0 .. 1 ] != "#'" )
        return( ERR_NOT_CLOSURE );

    switch( cl )
    {
    case "#'!":
        ret = #'!;
        break;

    case "#'!=":
        ret = #'!=;
        break;

    case "#'%":
        ret = #'%;
        break;

    case "#'%=":
        ret = #'%=;
        break;

    case "#'^":
        ret = #'^;
        break;

    case "#'^=":
        ret = #'^=;
        break;

    case "#'&":
        ret = #'&;
        break;

    case "#'&=":
        ret = #'&=;
        break;

    case "#'&&":
        ret = #'&&;
        break;

    case "#'*":
        ret = #'*;
        break;

    case "#'*=":
        ret = #'*=;
        break;

    case "#'({":
        ret = #'({;
        break;

    case "#'([":
        ret = #'([;
        break;

    case "#'-":
        ret = #'-;
        break;

    case "#'-=":
        ret = #'-=;
        break;

    case "#'--":
        ret = #'--;
        break;

    case "#'+":
        ret = #'+;
        break;

    case "#'+=":
        ret = #'+=;
        break;

    case "#'++":
        ret = #'++;
        break;

    case "#'=":
        ret = #'=;
        break;

    case "#'==":
        ret = #'==;
        break;

    case "#'|":
        ret = #'|;
        break;

    case "#'|=":
        ret = #'|=;
        break;

    case "#'||":
        ret = #'||;
        break;

    case "#'[":
        ret = #'[;
        break;

    case "#'[..":
        ret = #'[..;
        break;

    case "#'[..]":
        ret = #'[..];
        break;

    case "#'[..<]":
        ret = #'[..<];
        break;

    case "#'[<":
        ret = #'[<;
        break;

    case "#'[<..":
        ret = #'[<..;
        break;

    case "#'[<..]":
        ret = #'[<..];
        break;

    case "#'[<..<]":
        ret = #'[<..<];
        break;

    case "#'[,]":
        ret = #'[,];
        break;

    case "#',":
        ret = #',;
        break;

    case "#'<":
        ret = #'<;
        break;

    case "#'<=":
        ret = #'<=;
        break;

    case "#'<<":
        ret = #'<<;
        break;

    case "#'<<=":
        ret = #'<<=;
        break;

    case "#'>":
        ret = #'>;
        break;

    case "#'>=":
        ret = #'>=;
        break;

    case "#'>>":
        ret = #'>>;
        break;

    case "#'>>=":
        ret = #'>>=;
        break;

    case "#'?":
        ret = #'?;
        break;

    case "#'?!":
        ret = #'?!;
        break;

    case "#'~":
        ret = #'~;
        break;

    default:
        if( sscanf( cl, "#'%s->%s", ob, cl ) == 2 )
        {
            if( !closurep( ret = symbol_function( cl, ob ) ) )
                return( ERR_COULD_NOT_BIND_LFUN );
        }
        else if( !closurep( ret = symbol_function( cl[ 2 .. ] ) ) )
            return( ERR_COULD_NOT_BIND_EFUN );
    }

    return( ret );
}

symbol interpret_as_symbol( string sym )
{
    symbol ret;
    int quotes, i, sz;

    for( quotes = 0; sym[ quotes ] == '\''; quotes++ );

    if( !( i = quotes ) )
        return( 0 );

    if( ( sym[ i ] < 'a' || sym[ i ] > 'z' ) && ( sym[ i ] < 'A' || 
      sym[ i ] > 'Z' ) )
        return( 0 );

    for( i++, sz = strlen( sym ); i < sz; i++ )
    {
        if( ( sym[ i ] < 'a' || sym[ i ] > 'z' ) && ( sym[ i ] < 'A' ||
          sym[ i ] > 'Z' ) && ( sym[ i ] < '0' || sym[ i ] > '9' ) &&
          ( sym[ i ] != '_' ) )
            return( 0 );
    }

    for( i = 1, ret = quote( sym[ quotes .. ] ); i < quotes; i++ )
        ret = quote( ret );

    return( ret );
}

string interpret_as_string( string str )
{
    string ret;
    int i, sz, failed, newchar;

    if( str[ i++ ] != '"' )
        return( 0 );

    for( ret = "", sz = strlen( str ); i < sz; i++ )
    {
        if( str[ i ]  == '\\' )
        {
            switch( str[ ++i ] )
            {
            case 0:
                return( 0 );

            case 'a':
                ret += "\a";
                break;

            case 'b':
                ret += "\b";
                break;

            case 'f':
                ret += "\f";
                break;

            case 'n':
                ret += "\n";
                break;

            case 'r':
                ret += "\r";
                break;

            case 't':
                ret += "\t";
                break;

            case 'x':
                newchar = interpret_number_as_base( 16, str[ i + 1 .. i + 2 ], 
                    &failed );

                if( failed )
                {
                    ret += "x";
                    failed = FALSE;
                    break;
                }

                ret += sprintf( "%c", newchar );
                i += 2;
                break;

            default:
                newchar = interpret_number_as_base( 8, str[ i .. i + 2 ],
                    &failed );

                if( failed )
                {
                    ret += str[ i .. i ];
                    failed = FALSE;
                    break;
                }

                ret += sprintf( "%c", newchar );
                i += 2;
                break;
            }

            continue;
        }
        else if( str[ i ] == '"' )
            return( ret );
        else
            ret += str[ i .. i ];
    }
}
