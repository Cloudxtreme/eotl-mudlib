/*
    NAME
        getdata.c - GET DATA package
    DESCRIPTION
        A standardized set of methods to get input from a user to a function.
        get_data()  - gets the data and passes it onto a method in THISO
    NOTES
        Look in the header file to see what types are supported.
    LAST MODIFIED
        Aleron  960403
*/

#include <acme.h>

private inherit AcmeError;
private inherit AcmeStruct;
private inherit AcmeArray;
private inherit AcmeMethod;

#include AcmeErrorInc
#include AcmeStructInc
#include AcmeArrayInc
#include AcmeMethodInc
#include AcmeGetDataInc

private mixed gd_data;
private mixed gd_type;
private mixed gd_prompt;
private mixed gd_assign;
private mixed gd_method;
private mixed gd_extra;
private mixed gd_size;
private mixed gd_i;

static void get_data_i ();
void got_data_i ( string in );

/* PUBLIC FUNCTIONS */

#define NULLVALTEST(v,s)  if (!v) return error(ERR_ERROR,"get_data: null "+s)
#define XOR(a,b)          ((a && !b) || (!a && b))
/*--------------------------------- get_data --------------------------------*/
/*
    description: gets data from a user to a method
    parameters: type      - a type (see .h for types) or array of types of
                            the data to get
                prompt    - a prompt or array of prompts
                method    - method to pass the data onto
                extra     - extra param to pass to method
    returns: void
    notes: the structure of the type and prompt have to match if they're arrays
           the data passed to the method will be in the same structure that
           the input/prompt arrays are in
*/
varargs void get_data ( mixed type, mixed prompt, mixed method, mixed extra )
{
    NULLVALTEST ( type, "type" );
    NULLVALTEST ( prompt, "prompt" );
    NULLVALTEST ( method, "method" );
    if ( XOR ( pointerp ( type ), pointerp ( prompt ) ) )
        return error ( "get_data: type and prompt are not both arrays" );

    gd_i = 0;
    gd_method = method;
    gd_extra = extra;
    if ( pointerp ( type ) && pointerp ( prompt ) )
    {
        gd_data = type + ({});  // copy the array 
        gd_type = flatten_array ( type );
        gd_size = sizeof ( gd_type );
        gd_prompt = flatten_array ( prompt );
        if ( gd_size != sizeof ( gd_prompt ) )
            return error ( "get_data: mismatched type and prompt arrays" );
        gd_assign = make_assignptrs ( type );
        if ( gd_size != sizeof ( gd_type ) )
            return error ( "get_data: evil voodoo happened" );
    }
    else
    {
        gd_type = type;
        gd_prompt = prompt;
        gd_assign = 0;
        gd_size = 0;
    }
    get_data_i ();
}

/* PRIVATE FUNCTIONS */

//-------------------------------- get_data_i -------------------------------//
/*
    description: prompts and gets the data to got_data_i
    parameters: None.
    returns: void
    notes: None.
*/
static
void get_data_i ()
{
    int type;
    
    type = ( gd_assign ? gd_type[ gd_i ] : gd_type );

    /* prompt the user */
    write ( ( gd_assign ? gd_prompt[ gd_i ] : gd_prompt ) );
    
    /* go get the next data element */
    switch ( type )
    {
    case GD_INT:
    case GD_FLOAT:
    case GD_STRING:
        input_to ( "got_data_i" );
        break;
    case GD_TEXT:
        EDITOR->get_text ( THISO, "got_data_i" );
        break;
    default:
        error ( ERR_ERROR, "get_data_i: incorect type " + type );
        break;
    }
}

//-------------------------------- got_data_i -------------------------------//
/*
    description: what to do to once we've gotten the data?
    parameters: in        - the data gotten from get_data_i
    returns: void
    notes: None.
*/
void got_data_i ( string in )
{
    int type;
    mixed data;

    type = ( gd_assign ? gd_type[ gd_i ] : gd_type );

    switch ( type )
    {
    case GD_INT:
        data = to_int ( in );
        break;
    case GD_FLOAT:
        data = to_float ( in );
        break;
    case GD_STRING:
    case GD_TEXT:
        data = to_string ( in );
        break;
    default:
        error ( ERR_ERROR, "get_data: inncorect type " + type );
        return;
        break;
    }
    if ( gd_assign )
    {
        funcall ( gd_assign[ gd_i ], gd_data, data );
        gd_i++;
    }
    else
        gd_data = data;

    if ( !gd_assign || !( gd_i < gd_size ) )
    {
        run_method( gd_method, gd_data, gd_extra );
        return;
    }

    get_data_i ();
}
