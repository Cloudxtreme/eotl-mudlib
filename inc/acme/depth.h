#ifndef ACME_DEPTH_INC
#define ACME_DEPTH_INC

#define CONTINUE_TRAVERSAL   ("keep on truckin'")

varargs mixed
depth_first( mixed rootnode, int maxdepth, mixed next, mixed prefix,
             mixed infix, mixed postfix, mixed seen );

#endif  // ACME_DEPTH_INC
