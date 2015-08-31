#ifndef ACME_ARGS_INC
#define ACME_ARGS_INC


string *    explode_args     ( string arg );
mixed  *    eval_args        ( string *args );
varargs int find_close_char  ( string str, int start, status quiet,
                               string open, string close);
string *explode_unescaped    ( string str, string delim);
string *explode_nested       ( string str, string delim, status quiet,
                               string open, string close );

#endif
