#ifndef ACME_OSPEC_INC
#define ACME_OSPEC_INC

object *parse_ospec         (string ospec, object *prev, string priorities);
object *evaluate_object_spec(string ospec, object tool,  string priorities);
string object_spec_help ();
void   object_spec_variable_set ( mixed key, mixed value );

#endif
