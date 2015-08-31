#ifndef ACME_VARSPACE_INC
#define ACME_VARSPACE_INC

void   vset   ( mixed key, mixed value, mixed space_key );
mixed  vquery ( mixed key, mixed space_key );

varargs void   set   ( mixed key, mixed value, mixed space_key );
varargs mixed  query ( mixed key, mixed space_key );

#endif
