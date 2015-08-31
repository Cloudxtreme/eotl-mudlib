#ifndef ACME_ANSI_INC
#define ACME_ANSI_INC

/*
varargs int    get_ansi( string str, int start );
        string remove_ansi( string str );
varargs string extract_ansi( string str, mixed ansi );
        int    ansi_strlen( string str );
varargs string ansi_format( string str, int width, mixed prefix );
varargs string make_ansi_code( string *keys, string *unused );
*/

#define IsLowerCase( x )   ( x >= 'a' && x <= 'z' )
#define IsUpperCase( x )   ( x >= 'A' && x <= 'Z' )
#define IsAlpha( x )       ( IsLowerCase( x ) || IsUpperCase( x ) )

#endif /* ACME_ANSI_INC */
