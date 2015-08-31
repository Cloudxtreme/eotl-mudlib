#ifndef ACME_FILE_INC
#define ACME_FILE_INC

varargs string expand_path( string pattern, object who );
varargs string *expand_pattern( string pattern, object who );
varargs private string *_expander( string *pat, string *dirs,
                                   closure add_slash );
int    file_exists ( string file );
string basename    ( string file );
string dirname     ( string file );
string find_file   ( string file, string *path );
string *get_dirs   ( string path );
string *get_files  ( string path );      
string make_absolute ( string filename, string pwd, string home );

#endif
