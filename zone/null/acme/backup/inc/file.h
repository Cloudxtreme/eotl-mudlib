#ifndef ACME_FILE_INC
#define ACME_FILE_INC

string expand_path ( string file );
int    file_exists ( string file );
string basename    ( string file );
string dirname     ( string file );
string find_file   ( string file, string *path );

#endif
