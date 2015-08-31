#ifndef ACME_OUTPUT_INC
#define ACME_OUTPUT_INC

#ifndef SILENTP
# define SILENTP   "silent"
#endif  /* SILENTP */

#ifndef SilentP
# define SilentP   SILENTP
#endif  /* SilentP */

// standard output function
varargs
void output ( string s, string o, object tob, string t, string tenv );

#endif  // ACME_OUTPUT_INC
