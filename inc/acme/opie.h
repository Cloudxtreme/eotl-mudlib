#undef opieVar
#undef opieFunc
#undef opieHelpFunc
#undef opieSetFunc
#undef opieDir
#undef opieServer
#undef opieAcmeServer
#undef opieTracer
#undef opieBW
#undef opie
#undef opie1
#undef opie_p
#undef opie_help
#undef opie_which
#undef opie_set
#define opieVar "opie"

#define opieFunc     "evaluate_object_spec"
#define opieHelpFunc "object_spec_help"
#define opieSetFunc  "object_spec_variable_set"

#define opieDir    "/zone/null/acme/opie/"
#define opieServer opieDir "opie.c"

#define opieAcmeSpec   opieDir "i/acme.c"
#define opieTracer     opieDir "i/tracer.c"
#define opieBW         opieDir "i/bw.c"
