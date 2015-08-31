#ifndef ACME_CRYPT_INC
#define ACME_CRYPT_INC

#define NCHARS    (256)              // size of ascii charset
#define ENCRYPT     (1)
#define DECRYPT    (-1)
#define MAX_BYTE_TRANSFER 500000     // from /secure/Parser/eotl/config.h

string crypt ( string s, string key, int type);   // en-/de- CRYPT strings
// en-/de- CRYPT FILEs
status cryptfile ( string infile, string outfile, string key, int type );

#endif  // ACME_CRYPT_INC
