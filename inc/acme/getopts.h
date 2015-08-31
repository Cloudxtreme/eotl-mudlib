#ifndef ACME_GETOPTS_INC
#define ACME_GETOPTS_INC

mixed *getopts     ( string *args, string validopts );
mixed *getopts_long( string *args, string validopts, mapping longopts);

#define OtherArgs(arr)     (arr[0])
#define Options(arr)       (arr[1])
#define BadOpts(arr)       (arr[2])

#endif  // ACME_GETOPTS_INC
