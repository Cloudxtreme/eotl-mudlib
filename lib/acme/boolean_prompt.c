#include <acme.h>

private inherit AcmeError;

#include AcmeErrorInc

varargs void boolean_prompt( string question, mixed yesmethod, mixed nomethod, 
    mixed quitmethod, int default_choice )
{
    string methstr, defstr, qstr;

    if( ( default_choice < 0 ) || ( default_choice > 3 ) )
        return( error( ERR_ERROR, "default_choice out of range!", 0 ) );

    if( !default_choice )
        defstr = "";
    else
        defstr = " ynq"[ default_choice .. default_choice ];

    methstr = " (ynq)";

    if( stringp( yesmethod ) )
        yesmethod = ({ symbol_function( yesmethod, THISO ) });
    else if( closurep( yesmethod ) )
        yesmethod = ({ yesmethod });
    else if( !pointerp( yesmethod ) || !sizeof( yesmethod ) )
        return( error( ERR_ERROR, "Invalid yesmethod!", 0 ) );
    else if( stringp( yesmethod[ 0 ] ) )
        yesmethod[ 0 ] = symbol_function( yesmethod[ 0 ], THISO );
    else if( !closurep( yesmethod[ 0 ] ) )
        return( error( ERR_ERROR, "Invalid yesmethod!", 0 ) );

    if( stringp( nomethod ) )
        nomethod = ({ symbol_function( nomethod, THISO ) });
    else if( closurep( nomethod ) )
        nomethod = ({ nomethod });
    else if( !pointerp( nomethod ) || !sizeof( nomethod ) )
        return( error( ERR_ERROR, "Invalid nomethod!", 0 ) );
    else if( stringp( nomethod[ 0 ] ) )
        nomethod[ 0 ] = symbol_function( nomethod[ 0 ], THISO );
    else if( !closurep( nomethod[ 0 ] ) )
        return( error( ERR_ERROR, "Invalid nomethod!", 0 ) );

    if( !quitmethod )
    {
        if( default_choice == 3 )
            return( error( ERR_ERROR, "DEFAULT_QUIT specified as default; "
                "no quit method was passed!", 0 ) );
        methstr = " (yn)";
    }
    else if( stringp( quitmethod ) )
        quitmethod = ({ symbol_function( quitmethod, THISO ) });
    else if( closurep( quitmethod ) )
        quitmethod = ({ quitmethod });
    else if( !pointerp( quitmethod ) || !sizeof( quitmethod ) )
        return( error( ERR_ERROR, "Invalid quitmethod!", 0 ) );
    else if( stringp( quitmethod[ 0 ] ) )
        quitmethod[ 0 ] = symbol_function( quitmethod[ 0 ], THISO );
    else if( !closurep( quitmethod[ 0 ] ) )
        return( error( ERR_ERROR, "Invalid quitmethod!", 0 ) );

    write( qstr = ( question +  methstr + " [" + defstr + "] " ) );
    input_to( "answer_question", 0, qstr, yesmethod, nomethod, quitmethod, 
        default_choice );
}

static void answer_question( string arg, string qstr, mixed yesmethod,
    mixed nomethod, mixed quitmethod, int default_choice )
{
    while( ( arg[ 0 ] == ' ' ) || ( arg[ 0 ] == '\t' ) )
        arg = arg[ 1 .. ];

    switch( lower_case( arg[ 0 .. 0 ] ) )
    {
    case "":
        if( !default_choice )
            break;
        answer_question( " ynq"[ default_choice .. default_choice ], qstr,
            yesmethod, nomethod, quitmethod, default_choice );
        return;

    case "y":
        if( apply( yesmethod[ 0 ], yesmethod[ 1 .. ] ) )
            return;
        break;

    case "n":
        if( apply( nomethod[ 0 ], nomethod[ 1 .. ] ) )
            return;
        break;

    case "q":
        if( quitmethod )
        {
            if( apply( quitmethod[ 0 ], quitmethod[ 1 .. ] ) )
                return;
            break;
        }

    default:
        write( "Invalid choice.\n" );
        break;
    }

    write( qstr );
    input_to( "answer_question", 0, qstr, yesmethod, nomethod, quitmethod,
        default_choice );
}
