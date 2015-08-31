#ifndef ACME_ERROR_INC
#define ACME_ERROR_INC

#define ERR_USAGE     0
#define ERR_NOTE      1
#define ERR_WARNING   2
#define ERR_ERROR     3

// standard error msg
varargs mixed error ( int severity, string msg, mixed ret );

#endif  // ACME_ERROR_INC
