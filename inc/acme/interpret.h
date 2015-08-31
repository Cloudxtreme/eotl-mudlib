#ifndef ACME_INTERPRET_INC
#define ACME_INTERPRET_INC
 
#define BASE_DIGITS                  "0123456789abcdefghijklmnopqrstuvwxyz"
#define ERR_NOT_CLOSURE              0
#define ERR_COULD_NOT_BIND_LFUN      -1
#define ERR_COULD_NOT_BIND_EFUN      -2
 
int     interpret_number_as_base( int base, string number, int failed );
float   interpret_as_float( string number, int failed );
int     interpret_as_number( string number, int failed );
mixed   interpret_as_closure( string cl );
symbol  interpret_as_symbol( string sym );
string  interpret_as_string( string str );
 
#endif
