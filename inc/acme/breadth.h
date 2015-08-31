#ifndef ACME_BREADTH_INC
#define ACME_BREADTH_INC

#define CONTINUE_TRAVERSAL   ("keep on truckin'")

varargs mixed
breadth_first( mixed rootnode, int maxdepth, mixed next, mixed method,
              mixed seen );

#endif  // ACME_BREADTH_INC
