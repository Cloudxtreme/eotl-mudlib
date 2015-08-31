#ifndef ACME_STRING_INC
#define ACME_STRING_INC

        string  munge_filename(string name);

varargs int     index_past_leading_ws( string str, int start );
        string  trim_left        ( string str, int chr );
        string  trim_right       ( string str, int chr );
        status  is_escaped       ( string str, int index );
        string  unescape         ( string str );
varargs int     match_quote( string str, int start, string quote_chars );
varargs string  unquote    ( string str,            string unquote_chars );
varargs int     member_str_unescaped( string str, int el, int pos, int step );
varargs int     member_any_str( string str, string els, int start, int step );
varargs string  untabify      ( string str, int tabwidth );
varargs string  untabify_list ( mixed ptr, int tabwidth );
varargs string  expand_unescape ( string str, string base, int bits );
varargs string  expand_variables( string str, mapping vars, int varchar,
                   mixed func,  mixed args, int kill_if_no_match );
        string  remove_ansi( string str );
varargs string  indent( mixed src, int nspace );
varargs string  center( mixed src, int width );
varargs string *aexplode( string str, string del );
        string  munge_dirname( string name );

#define UNQUOTE_CHARS "\"'"    /* default quote chars to remove */
#define QUOTE_CHARS "\"'"     /* default quote chars to match on */

/* bits to expand_unescape */
#define O_EXPAND_BS_LITERAL     0x01
#define O_NO_EXPAND_HEX         0x02
#define O_NO_EXPAND_OCTAL       0x04

#define IsLowerCase( x )   ( x >= 'a' && x <= 'z' )
#define IsUpperCase( x )   ( x >= 'A' && x <= 'Z' )
#define IsAlpha( x )       ( IsLowerCase( x ) || IsUpperCase( x ) )

#define DEF_LINEWIDTH           (78)

#endif  // ACME_STRING_INC
