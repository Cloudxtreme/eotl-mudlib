#ifndef ACME_KQML_INC
#define ACME_KQML_INC

// You must pass <performative> by reference.  e.g. parse(s, &p, m)
status parse( string stream, string performative, mapping params );

// No pass by reference necessary
string pack( string performative, mapping params );

// Pass <i> and <term> by reference
static int parse_character       ( string stream, int i, string term );
static int parse_word            ( string stream, int i, string term );
static int parse_whitespace      ( string stream, int i, string term );
static int parse_string          ( string stream, int i, string term );
static int parse_quotation       ( string stream, int i, string term );
static int parse_comma_expression( string stream, int i, string term );
static int parse_expression      ( string stream, int i, string term );

// Pass <perf> by reference
static int parse_performative    ( string stream, string perf, mapping params);


#endif
