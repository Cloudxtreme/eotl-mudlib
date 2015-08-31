/*
** client.h
** Include this if you want the opie calls to look local.
**
*/

#include "opie.h"

#define opie(arg)      (call_other(opieServer, "opie", arg))
#define opie1(arg)     (call_other(opieServer, "opie1", arg))

#define opie_p(arg, pr)  (call_other(opieServer, "opie", arg, pr))
#define opie1_p(arg, pr) (call_other(opieServer, "opie1", arg, pr))

#define opie_help()    (call_other(opieServer, "help"))
#define opie_set(k,v)  (call_other(opieServer, "set", k, v))

#define opie_which()   (call_other(opieServer, "choose_engine"))
