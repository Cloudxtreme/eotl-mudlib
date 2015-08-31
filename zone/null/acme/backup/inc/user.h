#ifndef ACME_USER_INC
#define ACME_USER_INC

object *level_range        ( object *ulist, int min, int max );
object *level_range_fast   ( object *ulist, int min, int max );
int     to_ordlevel        ( mixed arg );
object *filter_by_hostname ( object *ulist, string pattern );
varargs object *wizards    ( object *ulist );
varargs object *mortals    ( object *ulist );

#endif
