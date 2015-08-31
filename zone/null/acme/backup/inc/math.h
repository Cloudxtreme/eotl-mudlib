#ifndef ACME_MATH_INC
#define ACME_MATH_INC

#ifndef pow
#define pow( x, y )   to_int( exp( y * log( to_float( x ) ) ) )
#endif

varargs int           from_hex( string str, int done );
varargs int           from_octal( string str, int done );

#endif
