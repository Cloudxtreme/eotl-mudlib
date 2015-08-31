/*
 * AcmeColoredCode
 * 
 * This function, color_code, assumes that code is being passed to it and
 * colors it accordingly.
 *
 * This code is wholly owned and mainted by Acme.  It is not public domain,
 * nor was it ever intended to be.  Morgaine put a good deal of time and
 * effort into this code, so if it results in any bugs in core files, do
 * NOT ATTEMPT to fix the bug here!  Take the dependence on AcmeColoredCode
 * in the core file out, and mail a member of the Acme Cabal (preferably
 * Morgaine) with a bug fix request.
 *
 * 979506 Morgaine
 * 970620 Morgaine -- made some more mods
 *
 */
#include <acme.h>
#include <ansi.h>
#include <sys/functionlist.h>
#include AcmeColoredCodeInc

private mixed                    code_colors = ({
                                   WHITE,
                                   BOLD_BLACK,
                                   YELLOW,
                                   BOLD_RED,
                                   BOLD_MAGENTA,
                                   BOLD_CYAN,
                                   BOLD_YELLOW,
                                   BOLD_WHITE,
                                   BOLD_GREEN,
                                   BOLD_GREEN,
                                   GREEN,
                                   BOLD_BLUE,
                                   BOLD_BLUE,
                                   BOLD_BLUE,
                                   BOLD_BLUE,
                                   BOLD_BLUE,
                                   CYAN,
                                   BACK_RED BLACK,
                                   WHITE,
                                   BOLD_BLACK
                                 });

private mixed                    simul_efuns = ({ });
private mixed                    efuns = ({ "regexp", "add_action", "all_inventory",
                                   "allocate", "assoc", "break_point", "bind_lambda",
                                   "call_other", "call_out", "call_out_info", "capitalize",
                                   "cat", "clear_bit", "clone_object", "command", "crypt",
                                   "ctime", "deep_inventory", "destruct", 
                                   "disable_commands", "ed", "enable_commands",
                                   "environment", "exec", "explode", "file_name",
                                   "file_size", "filter_array", "find_call_out",
                                   "find_object", "function_exists", "implode",
                                   "input_to", "insert_alist", "interactive",
                                   "intersect_alist", "intp", "living",
                                   "lower_case", "get_dir", "map_array", "member",
                                   "member", "mkdir", "move_object", "negate",
                                   "notify_fail", "objectp", "order_alist", "pointerp",
                                   "present", "query_actions", "query_load_average",
                                   "query_verb", "random", "read_bytes", "read_file",
                                   "remove_call_out", "rename", "replace_program",
                                   "restore_object", "rm", "rmdir", "rusage", "save_object",
                                   "say", "set_bit", "set_heart_beat", "set_light",
                                   "set_this_object", "sizeof", "slice_array", "snoop",
                                   "sort_array", "stringp", "strlen", "tail", "tell_object",
                                   "tell_room", "test_bit", "this_interactive",
                                   "this_object", "this_player", "throw", "time",
                                   "unique_array", "users", "write", "write_bytes",
                                   "write_file", "export_uid", "geteuid", "seteuid",
                                   "getuid", "floatp", "filter_mapping", "m_delete",
                                   "m_indices", "m_sizeof", "m_values", "map_mapping",
                                   "mappingp", "mkmapping", "process_string",
                                   "first_inventory", "next_inventory", "printf", "sprintf",
                                   "quote", "lambda", "symbol_function", "apply",
                                   "funcall", "symbolp", "closurep", "extract",
                                   "extern_call", "previous_object", "strstr",
                                   "program_time", "swap", "query_ip_name", 
                                   "query_ip_number", "query_once_interactive", "to_int",
                                   "to_float", "to_string", "to_array",
                                   "set_extra_wizinfo_size", "caller_stack_depth",
                                   "query_imp_port", "query_input_pending", "sin",
                                   "asin", "cos", "acos", "tan", "atan", "log", "exp",
                                   "sqrt", "mapping_contains", "allocate_mapping",
                                   "copy_mapping", "call_resolved", "referencep",
                                   "raise_error", "get_eval_cost", "garbage_collection",
                                   "typeof", "get_type_info", "add_verb", "add_xverb",
                                   "binary_message", "debug_message", "expand_define",
                                   "functionlist", "get_error_file", "get_extra_wizinfo",
                                   "heart_beat_info", "inherit_list", "query_editing",
                                   "query_idle", "query_snoop", "regexplode", 
                                   "remove_action", "remove_interactive", "rename_object",
                                   "set_auto_include_string", "set_buffer_size",
                                   "set_connection_charset", "set_driver_hook",
                                   "set_extra_wizinfo", "set_is_wizard", 
                                   "set_modify_command", "set_prompt", "set_this_player",
                                   "shadow", "query_shadowing", "shutdown",
                                   "symbol_variable", "to_object", "trace", "traceprefix",
                                   "transpose_array", "unbound_lambda", "unshadow",
                                   "wizlist_info", "efun308", "last_instructions",
                                   "attach_erq_demon", "send_erq", "send_imp", "debug_info",
                                   "filter_objects", "map_objects", "walk_mapping",
                                   "remote_command", "copy_file", "find_objects",
                                   "freeze_interactive", "clonep", "query_arg", "newcrypt",
                                   "query_variable", "set_variable", "query_variables",
                                 });
private mixed                    Ckeywords = ({ "break", "continue", "do", "else",
                                   "for", "if", "return", "switch", "while", "auto",
                                   "extern", "register", "static", "typedef", "struct",
                                   "union", "enum", "signed", "unsigned", "short", "long"
                                   "int", "char", "float", "double", "void", "volatile",
                                   "const" });
private mixed                    keywords = ({ "do", "while", "for", "if", "else", 
                                   "return", "int", "string", "object", "closure",
                                   "symbol", "mapping", "mixed", "float", "private",
                                   "public", "protected", "static", "nomask", "varargs",
                                   "functions", "variables", "inherit", "virtual", 
                                   "switch", "case", "break", "void", "status",
                                   "continue", "sscanf", "parse_command", "default",
                                   "catch" });
private mixed                    cl_keywords = ({ "do", "while", "return", "break",
                                   "switch", "continue", "sscanf", "parse_command",
                                   "catch" });

private string                   wordstring = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQ"
                                   "RSTUVWXYZ0123456789_";
private static closure                  color_expand;
                                                        
varargs string color_code( string code, mixed c_cols, int cont, int language )
{
    mixed ret;
    int i, sz, type, t1, t2;
    
    /*** this variable implies that code is in the middle of a color scheme, ie,
         the code started with a comment so search for the end. ***/
    if( cont )
    {
        switch( cont - 1 )
        {

        /*** this label implies C style comments ***/
        case COMMENT_COLOR:

            ret = ({ COMMENT_COLOR, code[ 0 .. ( i = strstr( code, "*/" ) ) > -1 ? ++i :
                strlen( code ) ] });
            
            /*** cont doesn't need to be reset, because its already set to COMMENT_COLOR ***/
            if( i < 0 )
            {
                ret = map_array( ret, color_expand, c_cols || code_colors );
                return( implode( ret, "" ) + NOR3 );
            }

            i++;
            break;

        /*** this label implies C++ style comments ***/
        case __COMMENT_CPP:

            ret = ({ COMMENT_COLOR, code[ 0 .. ( i = member( code, '\n' ) ) > -1 ? i :
                strlen( code ) ] });

            /*** cont doesn't need to be reset, because its already set to __COMMENT_CPP ***/
            if( i < 0 )
            {
                ret = map_array( ret, color_expand, c_cols || code_colors );
                return( implode( ret, "" ) + NOR3 );
            }

            i++;
            break;

        /*** this label implies strings ***/
        case STRING_COLOR:
             
            ret = ({ STRING_COLOR, "" });

            /*** find the end of the string ***/
            for( sz = strlen( code ); i < sz; i++ )
            {
                /*** was this string escaped? ***/
                if( code[ i ] == '\\' )
                    ret[ <1 ] += code[ i .. ++i ]; 

                /*** otherwise, add it and check for the ending quote ***/
                else
                {
                    ret[ <1 ] += code[ i .. i ];
                    if( ( code[ i ] == '"' ) || ( code[ i ] == '\n' ) )
                        break;
                }
            }

            /*** cont doesn't need to be reset, because its already set to STRING_COLOR ***/
            if( i >= sz )
            {
                ret = map_array( ret, color_expand, c_cols || code_colors );
                return( implode( ret, "" ) + NOR3 );
            }

            i++;
            break;

        /*** this label implies preprocessor directives ***/
        case PREPROCESSOR_COLOR:

            ret = ({ PREPROCESSOR_COLOR, "" });

            /*** find the end of the define ***/
            for( sz = strlen( code ); i < sz; i++ )
            {
                /*** don't interpret escaped characters ***/
                if( code[ i ] == '\\' )
                    ret[ <1 ] += code[ i .. ++i ];

                /*** check for C++ style comments ***/
                else if( code[ i .. i + 1 ] == "//" )
                {
                    ret += ({ COMMENT_COLOR, code[ i .. ( i = member( code, '\n', i ) ) > -1 ?
                        i : strlen( code ) ] });
                    if( i < 0 )
                    {
                        /*** cont needs to be reset to __COMMENT_CPP ***/
                        cont = __COMMENT_CPP + 1;
                        ret = map_array( ret, color_expand, c_cols || code_colors );
                        return( implode( ret, "" ) + NOR3 );
                    }

                    break;
                }

                /*** check for C style comments ***/
                else if( code[ i .. i + 1 ] == "/*" )
                {
                    t1 = strstr( code, "*/", i );
                    t2 = member( code, '\n', i );

                    /*** did we never come to the end of the comment? ***/
                    if( t1 == -1 )
                    {
                        /*** cont needs to be reset to COMMENT_COLOR ***/
                        cont = COMMENT_COLOR + 1;
                        ret += ({ COMMENT_COLOR, code[ i .. ] });
                        ret = map_array( ret, color_expand, c_cols || code_colors );
                        return( implode( ret, "" ) + NOR3 );
                    }

                    /*** did we never come to the end of the line? ***/
                    else if( t2 == -1 )
                    {
                        /*** cont is already PREPROCESSOR_COLOR ***/
                        ret += ({ COMMENT_COLOR, code[ i .. t2 + 1 ], PREPROCESSOR_COLOR,
                            code[ t2 + 2 .. ] });
                        ret = map_array( ret, color_expand, c_cols || code_colors );
                        return( implode( ret, "" ) + NOR3 );
                    }

                    /*** did the line end before the comment? ***/
                    else if( t2 < t1 )
                    {
                        ret += ({ COMMENT_COLOR, code[ i .. ( i = t1 + 1 ) ] });
                        break;
                    }

                    /*** did the comment end before the line? ***/
                    else if( t1 > t2 )
                    {
                        ret += ({ COMMENT_COLOR, code[ i .. ( i = t1 + 1 ) ] });
                        continue;
                    }
                }

                 /*** otherwise, add it and perform the necessary checks ***/
                 else
                 {
                     ret[ <1 ] += code[ i .. i ];
                     if( code[ i ] == '\n' )
                         break;
                 }
            }

            i++;
            break;

        /*** otherwise, just set ret at whatever cont suggested ***/
        default:

            ret = ({ cont - 1, "" });
        }
    }
    else ret = ({ });
            
    /*** main loop ***/
    for( sz = strlen( code ); i < sz; i++ )
    {

        /*** set cont to 0 if it makes it back into the loop ***/
        cont = 0;

        switch( code[ i ] )
        {

        /*** handle comment coloring / divide operators coloring ***/

        case '/': 
            
            /*** there are two kinds of comments, C style and C++ style, both of which
                 are supported in LPC. ***/
            
            /*** this is a C style comment ***/
            if( code[ i + 1 ] == '*' )
            {
                cont = COMMENT_COLOR + 1;
                if( !sizeof( ret ) || ( ret[ <2 ] != COMMENT_COLOR ) )
                    ret += ({ COMMENT_COLOR, "" });
                ret[ <1 ] += code[ i .. ( i = strstr( code, "*/", i ) ) > -1 ? ++i : sz ];
                if( i < 0 )
                    i = sz;
                break;
            }
            
            /*** this is a C++ style comment ***/
            else if( ( language != LANGUAGE_C ) && ( code[ i + 1 ] == '/' ) )
            {
                cont = __COMMENT_CPP + 1;
                if( !sizeof( ret ) || ( ret[ <2 ] != COMMENT_COLOR ) )
                    ret += ({ COMMENT_COLOR, "" });
                ret[ <1 ] +=  code[ i .. ( i = member( code, '\n', i ) ) > -1 ? i : sz ];
                if( i < 0 )
                    i = sz;
                break;
            }
       
        /*** handle operator coloring ***/
        case '!': case '%': case '^': case '&': case '*': case '(': case ')': case '-':
        case '=': case '+': case '|': case '{': case '[': case '}': case ']': case ':':
        case ';': case ',': case '<': case '.': case '>': case '?': case '~':
    
            /*** if this is a floating point, color as a float ***/
            if( ( code[ i ] == '.' ) && sizeof( ret ) && ( ret[ <2 ] == INT_COLOR ) )
            {
                ret[ <2 ] = FLOAT_COLOR;
                ret[ <1 ] += ".";
                break;
            }

            /*** otherwise, color as an operator ***/
            if( !sizeof( ret ) || ( ret[ <2 ] != OPERATOR_COLOR ) )
            {
                /*** since operators serve as delimiters in C style languages,
                     here is a good place to check if we need to color for keyword,
                     efun or simul_efun ***/
                if( sizeof( ret ) && ( ret[ <2 ] == CODE_COLOR ) )
                {
                    if( language == LANGUAGE_LPC )
                    {
                        if( member( keywords, ret[ <1 ] ) > -1 )
                            ret[ <2 ] = KEYWORD_COLOR;
                        else if( member( simul_efuns, ret[ <1 ] ) > -1 )
                            ret[ <2 ] = SIMUL_EFUN_COLOR;
                        else if( member( efuns, ret[ <1 ] ) > -1 )
                            ret[ <2 ] = EFUN_COLOR;
                    }
                    else if( language == LANGUAGE_C )
                    {
                        if( member( Ckeywords, ret[ <1 ] ) > -1 )
                            ret[ <2 ] = KEYWORD_COLOR;
                        else if( code[ i ] == '(' )
                            ret[ <2 ] = EFUN_COLOR;
                    }                   
                }
                else if( ( code[ i ] == '(' ) && ( language == LANGUAGE_C ) && 
                  ( sizeof( ret ) >= 4 ) && ( ret[ <4 ] == CODE_COLOR ) &&
                  ( ret[ <2 ] == NOR3 ) )
                    ret[ <4 ] = EFUN_COLOR;

                ret += ({ OPERATOR_COLOR, "" });
            }
            ret[ <1 ] += code[ i .. i ];
            break;

        /*** handle integer coloring ***/
        case '0': case '1': case '2': case '3': case '4': 
        case '5': case '6': case '7': case '8': case '9':

            /*** if this was already specified as octal, and code[ i ] is between
                 0 and 7, return ***/
            if( sizeof( ret ) && ( ret[ <2 ] == OCTAL_COLOR ) && ( code[ i ] >= '0' ) &&
              ( code[ i ] <= '7' ) )
            {
                ret[ <1 ] += code[ i .. i ];
                break;
            }

            /*** if this was already specified as code coloring, integer coloring,
                 hex coloring, float coloring, or symbol coloring, let it pass ***/
            if( sizeof( ret ) && ( ( ret[ <2 ] == CODE_COLOR ) || 
              ( ret[ <2 ] == INT_COLOR ) || ( ret[ <2 ] == HEX_COLOR ) || 
              ( ret[ <2 ] == FLOAT_COLOR ) || ( ret[ <2 ] == SYMBOL_COLOR ) ) )
            {
                 ret[ <1 ] += code[ i .. i ];
                 break;
            }
            
            /*** is this a hex number? ***/
            if( ( code[ i ] == '0' ) && ( code[ i + 1 ] == 'x' ) )
            {
                if( !sizeof( ret ) || ( ret[ <2 ] != HEX_COLOR ) )
                    ret += ({ HEX_COLOR, "" });
                ret[ <1 ] += "0x";
                i++;
                break;
            }

            /*** octal? ***/
            if( ( language != LANGUAGE_LPC ) && ( code[ i ] == '0' ) && 
              ( code[ i + 1 ] >= '0' ) && ( code[ i + 1 ] <= '7' ) )
            {
                if( !sizeof( ret ) || ( ret[ <2 ] != OCTAL_COLOR ) )
                    ret += ({ OCTAL_COLOR, "" });
                ret[ <1 ] += "0";
                break;
            }

            /*** otherwise, its an integer ***/
            if( !sizeof( ret ) || ( ret[ <2 ] != INT_COLOR ) )
                ret += ({ INT_COLOR, "" });
            ret[ <1 ] += code[ i .. i ];
            break;

       /*** handle hex digits A through F ***/
       case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
       case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':

           /*** was this a hex number? ***/
           if( sizeof( ret ) && ( ret[ <2 ] == HEX_COLOR ) )
           {
               ret[ <1 ] += code[ i .. i ];
               break;
           }

       /*** handle code characters (a-z, A-Z, _) ***/
       case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
       case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v':
       case 'w': case 'x': case 'y': case 'z': case 'G': case 'H': case 'I': case 'J':
       case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
       case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
       case '_':

           /*** append to the return array ***/
           if( !sizeof( ret ) || ( ( ret[ <2 ] != CODE_COLOR ) && 
             ( ret[ <2 ] != SYMBOL_COLOR ) ) )
               ret += ({ CODE_COLOR, "" });
           ret[ <1 ] += code[ i .. i ];
           break;
      
       /*** handle characters / symbols ***/
       case '\'':

           /*** if this a quoted array, color it as an operator ***/
           if( ( code[ i + 1 ] == '(' ) && ( code[ i + 2 ] == '{' ) )
           {
               if( !sizeof( ret ) || ( ret[ <2 ] != OPERATOR_COLOR ) )
                   ret += ({ OPERATOR_COLOR, "" });
               ret[ <1 ] += "'({";
               i += 2;
               break;
           }

           /*** null characters are illegal ***/
           else if( code[ i + 1 ] == '\'' )
           {
               if( !sizeof( ret ) || ( ret[ <2 ] != ERROR_COLOR ) )
                   ret += ({ ERROR_COLOR, "" });
               ret[ <1 ] += "''";
               i++;
               break;
           }

           /*** an escaped single quote ***/
           else if( code[ i + 1 .. i + 3 ] == "\\''" )
           {
               if( !sizeof( ret ) || ( ret[ <2 ] != CHARACTER_COLOR ) )
                   ret += ({ CHARACTER_COLOR, "" });
               ret[ <1 ] += "'\\''";
               i += 3;
               break;
           }

           /*** a simple character ***/
           else if( code[ i + 2 ] == '\'' )
           {
               if( !sizeof( ret ) || ( ret[ <2 ] != CHARACTER_COLOR ) )
                   ret += ({ CHARACTER_COLOR, "" });
               ret[ <1 ] += code[ i .. i += 2 ];
               break;
           }
           
           /*** an escaped character ***/
           else if( ( code[ i + 1 ] == '\\' ) && ( code[ i + 3 ] == '\'' ) )
           {
               if( !sizeof( ret ) || ( ret[ <2 ] != CHARACTER_COLOR ) )
                   ret += ({ CHARACTER_COLOR, "" });
               ret[ <1 ] += code[ i .. i += 3 ];
               break;
           }

           /*** a symbol ***/
           else
           {
               if( !sizeof( ret ) || ( ret[ <2 ] != SYMBOL_COLOR ) )
                   ret += ({ SYMBOL_COLOR, "" });
               ret[ <1 ] += "'";
               break;
           }

       /*** handle preprocessor / closures ***/
       case '#':
           
           /*** is it a preprocessor directive? ***/
           if( ( !i || ( code[ i - 1 ] == '\n' ) ) && ( code[ i + 1 ] != '\'' ) )
           {
               cont = PREPROCESSOR_COLOR + 1;
               if( !sizeof( ret ) || ( ret[ <2 ] != PREPROCESSOR_COLOR ) )
                   ret += ({ PREPROCESSOR_COLOR, "" });

               /*** find the end of the define ***/
               for( ; i < sz; i++ )
               {
                   /*** don't interpret escaped characters ***/
                   if( code[ i ] == '\\' )
                       ret[ <1 ] += code[ i .. ++i ];

                   /*** check for C++ style comments ***/
                   else if( code[ i .. i + 1 ] == "//" )
                   {
                       ret += ({ COMMENT_COLOR, code[ i .. ( i = member( code, '\n', i ) ) > -1 ?
                           i : strlen( code ) ] });
                       if( i < 0 )
                       {
                           /*** cont needs to be reset to __COMMENT_CPP ***/
                           cont = __COMMENT_CPP + 1;
                           ret = map_array( ret, color_expand, c_cols || code_colors );
                           return( implode( ret, "" ) + NOR3 );
                       }

                       break;
                   }

                   /*** check for C style comments ***/
                   else if( code[ i .. i + 1 ] == "/*" )
                   {
                       t1 = strstr( code, "*/", i );
                       t2 = member( code, '\n', i );

                       /*** did we never come to the end of the comment? ***/
                       if( t1 == -1 )
                       {
                           /*** cont needs to be reset to COMMENT_COLOR ***/
                           cont = COMMENT_COLOR + 1;
                           ret += ({ COMMENT_COLOR, code[ i .. ] });
                           ret = map_array( ret, color_expand, c_cols || code_colors );
                           return( implode( ret, "" ) + NOR3 );
                       }

                       /*** did we never come to the end of the line? ***/
                       else if( t2 == -1 )
                       {
                           /*** cont is already PREPROCESSOR_COLOR ***/
                           ret += ({ COMMENT_COLOR, code[ i .. t2 + 1 ], PREPROCESSOR_COLOR,
                               code[ t2 + 2 .. ] });
                           ret = map_array( ret, color_expand, c_cols || code_colors );
                           return( implode( ret, "" ) + NOR3 );
                       }

                       /*** did the line end before the comment? ***/
                       else if( t2 < t1 )
                       {
                           ret += ({ COMMENT_COLOR, code[ i .. ( i = t1 + 1 ) ] });
                           break;
                       }

                       /*** did the comment end before the line? ***/
                       else if( t2 > t1 )
                       {
                           ret += ({ COMMENT_COLOR, code[ i .. ( i = t1 + 1 ) ], PREPROCESSOR_COLOR, "" });
                           continue;
                       }
                   }

                   /*** otherwise, add it and make the appropriate checks ***/
                   else
                   {
                       ret[ <1 ] += code[ i .. i ];
                       if( code[ i ] == '\n' )
                           break;
                   }
               }

               break;
           }
 
           /*** is it NOT a closure? ***/
           else if( ( language != LANGUAGE_LPC ) || ( code[ i + 1 ] != '\'' ) )
           {
               if( !sizeof( ret ) || ( ret[ <2 ] != ERROR_COLOR ) )
                   ret += ({ ERROR_COLOR, "" });
               ret[ <1 ] += "#";
               break;
           }

           if( !sizeof( ret ) || ( ret[ <2 ] != CL_CODE_COLOR ) )
               ret += ({ CL_CODE_COLOR, "" });
           ret[ <1 ] += "#'";
           i++;

           /*** interpret the closure type ***/
           switch( code[ i + 1 ] )
           {
           
           /*** closure operators that exist in #'X form and in #'X= form ***/
           case '!': case '%': case '^': case '*': case '/': case '=':
               
               /*** append it to the return array ***/
               ret[ <2 ] = CL_OPERATOR_COLOR;
               ret[ <1 ] += code[ ++i .. i ];
               
               if( code[ i + 1 ] == '=' )
               {
                   ret[ <1 ] += "=";
                   i++;
               }
               break;

           /*** closure operators that exist in #'X form, #'X= form, and in #'XX form ***/
           case '&': case '|': case '-': case '+':

               /*** append to the return array ***/
               ret[ <2 ] = CL_OPERATOR_COLOR;
               ret[ <1 ] += code[ ++i .. i ];

               if( code[ i + 1 ] == '=' )
               {
                   ret[ <1 ] += "=";
                   i++;
               }
               else if( code[ i + 1 ] == code[ i ] )
                   ret[ <1 ] += code[ ++i .. i ];
               break;

           /*** closure operators that exist in #'X form, #'X= form, #'XX form, and in
                #'XX= form ***/
           case '<': case '>':

               /*** append to the return array ***/
               ret[ <2 ] = CL_OPERATOR_COLOR;
               ret[ <1 ] += code[ ++i .. i ]; 
 
               if( code[ i + 1 ] == '=' )
               {
                   ret[ <1 ] += "=";
                   i++;
               }
               else if( code[ i + 1 ] == code[ i ] )
               {
                   ret[ <1 ] += code[ ++i .. i ]; 
 
                   if( code[ i + 1 ] == '=' )
                   {
                       ret[ <1 ] += "=";
                       i++;
                   }
               }
               break;

           /*** closure operators #'({ and #'([ ***/
           case '(':

               /*** aggregate ***/
               if( code[ i + 2 ] == '{' )
               {
                   ret[ <2 ] = CL_OPERATOR_COLOR;
                   ret[ <1 ] += "({";
                   i += 2;
               }

               /*** m_caggregate ***/
               else if( code[ i + 2 ] == '[' )
               {
                   ret[ <2 ] = CL_OPERATOR_COLOR;
                   ret[ <1 ] += "([";
                   i += 2;
               }

               /*** error ***/
               else
                   ret[ <2 ] = ERROR_COLOR; 
               break;

           /*** indexing operators ***/
           case '[':
               
               /*** append it to the return array ***/
               ret[ <2 ] = CL_OPERATOR_COLOR;
               ret[ <1 ] += "[";
               i++;

               if( ( code[ i + 1 ] == ',' ) && ( code[ i + 2 ] == ']' ) )
               {
                   ret[ <1 ] += ",]";
                   i += 2;
                   break;
               }
               
               if( code[ i + 1 ] == '<' )
               {
                   ret[ <1 ] += "<";
                   i++;
               }

               if( ( code[ i + 1 ] == '.' ) && ( code[ i + 2 ] == '.' ) )
               {
                   ret[ <1 ] += "..";
                   i += 2;

                   if( code[ i + 1 ] == ']' )
                   {
                       ret[ <1 ] += "]";
                       i++;
                   }
                   else if( ( code[ i + 1 ] == '<' ) && ( code[ i + 2 ] == ']' ) )
                   {
                       ret[ <1 ] += "<]";
                       i += 2;
                   }
               }

               break;

           /*** comma operator ***/
           case ',':

               /*** append it to the return array ***/
               ret[ <2 ] = CL_OPERATOR_COLOR;
               ret[ <1 ] += ",";
               i++;
               break;

           /*** #'? and #'?! operators ***/
           case '?':

               /*** append it to the return array ***/
               ret[ <2 ] = CL_OPERATOR_COLOR;
               ret[ <1 ] += "?";
               i++;

               if( code[ i + 1 ] == '!' )
               {
                   ret[ <1 ] += "!";
                   i++;
               }
               break;

           /*** bitwise NOT ***/
           case '~':

               /*** append it to the return array ***/
               ret[ <2 ] = CL_OPERATOR_COLOR;
               ret[ <1 ] += "~";
               i++;
               break;

           /*** efun/simul_efun/lfun/global variable ***/
           case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h':
           case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p':
           case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x':
           case 'y': case 'z': case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
           case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
           case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V':
           case 'W': case 'X': case 'Y': case 'Z': case '_':

               /*** are they using efun:: notation? ***/
               if( ( code[ i + 1 ] == 'e' ) && ( code[ i + 2 .. i + 6 ] == "fun::" ) )
               {
                   ret[ <1 ] += "efun::";
                   i += 6;

                   while( member( wordstring, code[ ++i ] ) > -1 )
                       ret[ <1 ] += code[ i .. i ];
                   i--;

                   /*** with the efun:: modifier, only cl_keywords and efuns are valid ***/
                   if( member( cl_keywords, ret[ <1 ][ 8 .. ] ) > -1 )
                       ret[ <2 ] = CL_KEYWORD_COLOR;

                   else if( member( efuns, ret[ <1 ][ 8 .. ] ) > -1 )
                       ret[ <2 ] = CL_EFUN_COLOR;

                   /*** therefore, if its something else, color it as an error ***/
                   else
                       ret[ <2 ] = ERROR_COLOR;

                   break;
               }

               /*** append it to the return array ***/
               ret[ <1 ] += code[ ++i .. i ];

               while( member( wordstring, code[ ++i ] ) > -1 )
                   ret[ <1 ] += code[ i .. i ];
               i--;

               /*** is this a keyword? ***/
               if( member( cl_keywords, ret[ <1 ][ 2 .. ] ) > -1 )
                   ret[ <2 ] = CL_KEYWORD_COLOR;
               
               /*** a simul_efun? ***/
               else if( member( simul_efuns, ret[ <1 ][ 2 .. ] ) > -1 )
                   ret[ <2 ] = CL_SIMUL_EFUN_COLOR;

               /*** an efun? ***/
               else if( member( efuns, ret[ <1 ][ 2 .. ] ) > -1 )
                   ret[ <2 ] = CL_EFUN_COLOR;
               break;

           default:

               /*** its an error ***/
               ret[ <2 ] = ERROR_COLOR;
               break;
           }
           break;
       
       /*** strings ***/
       case '"':
           
           if( !sizeof( ret ) || ( ret[ <2 ] != STRING_COLOR ) )
               ret += ({ STRING_COLOR, "" });
           ret[ <1 ] += "\"";
           cont = STRING_COLOR + 1;

           /*** find the end of the string ***/
           for( ++i; i < sz; i++ )
           {
               /*** was this string escaped? ***/
               if( code[ i ] == '\\' )
                   ret[ <1 ] += code[ i .. ++i ];

               /*** otherwise, add it and check for the ending quote ***/
               else
               {
                   ret[ <1 ] += code[ i .. i ];
                   if( ( code[ i ] == '"' ) || ( code[ i ] == '\n' ) )
                       break;
               }
           }
           break;

       /*** whitespaces ***/
       case ' ': case '\t': case '\n':
           
           /*** whitespaces often seperate keywords from the rest of the code,
                so check for them ***/
           if( sizeof( ret ) && ( ret[ <2 ] == CODE_COLOR ) )
           {
               if( language == LANGUAGE_LPC )
               {
                   if( member( keywords, ret[ <1 ] ) > -1 )
                       ret[ <2 ] = KEYWORD_COLOR;
                   else if( member( simul_efuns, ret[ <1 ] ) > -1 )
                       ret[ <2 ] = SIMUL_EFUN_COLOR;
                   else if( member( efuns, ret[ <1 ] ) > -1 )
                       ret[ <2 ] = EFUN_COLOR;
               }
               else if( language == LANGUAGE_C )
               {
                   if( member( Ckeywords, ret[ <1 ] ) > -1 )
                       ret[ <2 ] = KEYWORD_COLOR;
               }
           }

           if( !sizeof( ret ) || ( ( code[ i ] != '\n' ) && ( ret[ <2 ] != NOR3 ) ) )
               ret += ({ NOR3, "" });
           ret[ <1 ] += code[ i .. i ];
           break;

       /*** if nothing matches, color as an error ***/
       default:

           if( !sizeof( ret ) || ( ret[ <2 ] != ERROR_COLOR ) )
               ret += ({ ERROR_COLOR, "" });
           ret[ <1 ] += code[ i .. i ];
           break;
       }
       
    }

    /*** do they have their own set of code colors? ***/
    c_cols = c_cols || code_colors;

    /*** expand the color-codes into strings ***/
    ret = map_array( ret, color_expand, c_cols );

    /*** end ***/
    return( implode( ret, "" ) + NOR3 );
}

void create()
{
    if( find_object( SIMUL_EFUN ) )
        simul_efuns = functionlist( find_object( SIMUL_EFUN ) );
    else if( find_object( SPARE_SIMUL_EFUN ) )
        simul_efuns = functionlist( find_object( SPARE_SIMUL_EFUN ) );
    
    color_expand = lambda( ({ 'x, 'colors }),
      ({ #'?,
        ({ #'intp, 'x }),
        ({ #'+, NOR3, ({ #'[, 'colors, 'x }) }),
        'x
      }) );
}

nomask query_code_colors()
{
    return( code_colors + ({ }) );
}
