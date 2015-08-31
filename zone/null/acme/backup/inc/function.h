#ifndef ACME_FUNCTION_INC
#define ACME_FUNCTION_INC

string  exists_function ( mixed ob, string func );
status  function_really_exists ( string ob_file, string func );
string *all_function_defs ( object ob, string func );
string  top_function_def ( object ob, string func );

#endif
