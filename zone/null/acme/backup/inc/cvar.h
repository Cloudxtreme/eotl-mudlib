#ifndef ACME_CVAR_INC
#define ACME_CVAR_INC

#define cvar(x)   choose_variant ( x )

mixed choose_variant ( mixed *choices );    // chooses among various weighted
                                            // options

#endif  // ACME_CVAR_INC
