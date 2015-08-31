#ifndef ACME_ARRAY_INC
#define ACME_ARRAY_INC

#ifndef is_int
#define is_int(s) ( to_int(s) || ( strlen(s) && s[0]=='0' ) )
#endif 

status array_eq          ( mixed *a1, mixed *a2 );
status array_equal       ( mixed *a1, mixed *a2 );
mixed *add_array         ( mixed *orig1, mixed *orig2 );
mixed *reverse_array     ( mixed *list );
mixed *checked_range     ( mixed *list, int i, int j );
mixed  checked_item      ( mixed *list, int i );
mixed  checked_item_reverse(mixed *list, int i);
mixed  wrapped_item      ( mixed *list, int i );
mixed *flatten_array1    ( mixed *list );
mixed *flatten_array     ( mixed *list );
mixed *unique_array      ( mixed *list );
mixed *intersect_array   ( mixed *orig1, mixed *orig2 );
mixed *exclude_array     ( mixed *orig1, mixed *orig2 );
varargs int anti_searcha ( mixed list, mixed el, int start, int step );
varargs int searcha_any  ( mixed list, mixed els, int start, int step );
varargs int anti_searcha_any( mixed list, mixed el, int start, int step );
int *to_int_range        ( string range );
mixed *typecast_array    ( mixed *arr, int type );
int *random_array        ( int size );
mixed *unique_array_slow ( mixed *arr, int keep_last );
varargs mixed *sort_alist( mixed *arr, mixed call, object ob );
varargs mixed acc_array( mixed elements, closure cl, mixed start, mixed args );

#endif
