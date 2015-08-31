
#ifndef ACME_CMD_INC
#define ACME_CMD_INC

#include "acme.h"
#include OpieClient
#include AcmeFindTargetInc

void    create ();
string  cmd_help ( string func );
string *cmd_usage ( string func );
status  do_cmd ( string name, string cmd_name );
varargs string UsageString ( string name, string func );
void    Output(string message);

#define FailUsage(name) return (notify_fail(UsageString(name)), 0)
#define FailUsageF(name,Func) return (notify_fail(UsageString(name, Func)), 0)

#define Msg_NoObs(msg) (sprintf("No objects found: %s", msg))
#define Fail(msg) return error(ERR_ERROR, msg, 1)
#define FailNoObs(spec)    Fail(Msg_NoObs(spec))

#define GetObjects(var, spec)  if(!(var = opie(spec)) || !sizeof(var)) \
                                { FailNoObs(spec); } else 1

#define GetObjects_p(var,spec,p)  if(!(var = opie_p(spec,p)) || !sizeof(var)) \
                                  { FailNoObs(spec); } else 1

#endif
