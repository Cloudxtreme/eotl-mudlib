#ifndef ACME_ARRAY_INC
#define ACME_ARRAY_INC

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

#endif
