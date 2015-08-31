#ifndef ACME_FORMAT_STRING_INC
#define ACME_FORMAT_STRING_INC

varargs closure parse_format( string format, mapping infomap, symbol *args );

#define FS_DEFAULT_ARG  0
#define FS_SPRINTF_FMT  1
#define FS_FMT_LAMBDA   2

#define FS_ARG_OPEN     "{["
#define FS_ARG_CLOSE    "}]"

#endif  // ACME_FORMAT_STRING_INC
