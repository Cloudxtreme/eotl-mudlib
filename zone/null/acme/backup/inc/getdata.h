#ifndef ACME_GETDATA_INC
#define ACME_GETDATA_INC

#define GD_INT     1
#define GD_FLOAT   2
#define GD_STRING  3        // single line string
#define GD_TEXT    4        // multiline string

varargs void get_data ( mixed type, mixed prompt, mixed method, mixed extra );

#endif  // ACME_GETDATA_INC
