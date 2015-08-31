/*
    NAME

        kqml.c - KQML message handling

    DESCRIPTION

        This package is for parsing and packing KQML messages.  I
        doubt we'll ever have any use for it on the mud, but sometimes
        you just feel like a nut.

        parse - parse a KQML message into its component parts
        pack  - pack component parts into a KQML message

        parse_character        - These are all internal parsing 
        parse_word             - routines which you can overload to 
        parse_whitespace       - add additional actions to the parse.
        parse_string           - See the documention below for more
        parse_quotation        - information.
        parse_comma_expression - 
        parse_expression       - 


    NOTES

        KQML (Knowledge Query and Manipulation Language) is an agent
        communication language.  See http://www.cs.umbc.edu/kse/
     
    LAST MODIFIED
        Zamboni 970120


*/


#include <acme.h>

private inherit AcmeStrings; // I use searcha_str_unescaped() in the parsing
#include AcmeStringsInc
#include AcmeKQMLInc

/*******************************************************************/
/* Parsing
**
*/

int stream_len; /* I could recompute this everywhere, but it's
                ** constant for the duration of a single parse.  I
                ** could pass it around, but that's ugly.  It's a
                ** global.  So shoot me.  No one is going to use this
                ** library anyway.  :-D 
                */

/****************************************
 Documentation for parse_foo()  ( except parse_performative() )

    Description:
      Attempts to parse the <foo> element from the stream at position <i>.
    Parameters:
      stream    - The input stream
      i         - Position of the cursor (where to scan from)
      result    - The resulting expression
    Returns:
      0 if it failed to parse the element from stream[i..].
      Otherwise, a positive number indicating which grammar
      alternation was matched.  (e.g. parse_expression() would
      return 2 if it matched a <quotation>.)  
    Notes:
      <i> and <result> should be passed by reference.  <i> will be
      incremented to point to the end of the matched term.  The term
      itself will be stored in <result>.  If the parse fails (and 0
      is returned), <i> will be unchanged and the value of <result>
      is undefined.  

      Here's an example of a program that prints "Yowsa" every time
      it matches the second form of <string>: 

      #include <acme.h>
      inherit AcmeKQML;
      #include AcmeKQMLInc

      static int
      parse_string( string stream, int i, string kstring )
      {
        int retval;
        retval = ::parse_string( stream, &i, &kstring );
        if( retval == 2 )
          write( "Yowsa!\n" );
        return retval;
      }

*/



// Some necessary forward declarations
static int parse_comma_expression( string stream, int i, string expr );
static int parse_expression( string stream, int i, string expression );

#define ASSERT( a )         if( !(a) ) { return 0; }
#define ASSERT_ELSE( a, e ) if( !(a) ) { e; return 0; }

#define SPECIAL "<>=+-*/&^~_@$%:.!?"  // used in parse_character()

// <character> ::= <alphabetic> | <numeric> | <special>
static int
parse_character( string stream, int i, string character )
{
  int char, lchar;  // lchar is lowercase, if it happens to be a letter.
  char = stream[i];
  ASSERT( char );

  if((                       (char >= '0')  && ( char <= '9') )
     || ( (( lchar = char | 0x20 ) >= 'a' ) && (lchar <= 'z') )
     || ( member( SPECIAL, char) != -1 ))
  {
    character = stream[i..i++];
    return 1;
  }
  return 0;
}


// <word> ::= <character><character>*
static int
parse_word( string stream, int i, string word )
{
  int oldi;
  string character;
  
  oldi = i;
  while( parse_character( stream, &i, &character ) );  // empty on purpose
  
  ASSERT( i > oldi );
  word = stream[oldi..i-1];
  return 1;
}


#define is_ws(ch) ((ch == ' ') || (ch == '\n') || (ch == '\t'))

// They don't define whitespace.
static int
parse_whitespace( string stream, int i, string whitespace )
{
  int j;
  
  for(j=i; (j < stream_len) && is_ws(stream[j]); j++); // body emtpy on purpose
  ASSERT( j > i );
  
  whitespace = stream[i..j-1];
  i = j;
  return 1;
}

// <string> ::= "<stringchar>*" | #<digit><digit>*"<ascii>*
// Note that in the second form, the length of the ascii string must
// match the number specified by the digits.  It's not really BNF.
static int
parse_string( string stream, int i, string kstring )
{
  int len, end;
  
  if( stream[i] == '\"' )
  {
    end = searcha_str_unescaped(stream, '\"', i+1);
    if( end == -1 )
      return 0;

    kstring = stream[i..end];
    i = end + 1;
    return 1;
  }

  if( sscanf(stream[i..], "#%d\"", len ) )
  {
    end = member( stream, '\"', i ) + 1;
    if( end+len > stream_len )  // We run out too soon
      return 0;
    
    kstring =  stream[i..end+len-1];
    i = end + len;
    return 2;
  }

  return 0;
}


// <quotation> ::= '<expression> | `<comma-expression>
static int
parse_quotation( string stream, int i, string quotation )
{
  // '<expression>
  if( stream[i] == '\'' )
  {
    i++;
    if(parse_expression( stream, &i, &quotation ))
    {
      quotation = "'" + quotation;
      return 1;
    }    
    i--;
    return 0;
  }
  
  // `<comma-expression>
  if( stream[i] == '\`' )
  {
    i++;
    if(parse_comma_expression( stream, &i, &quotation ))
    {
      quotation = "`" + quotation;
      return 2;
    }    
    i--;
    return 0;
  }  
  return 0;
}

// <comma-expression> ::= <word> | <quotation> | <string> |
//       ,<comma-expression> (<word> {<whitespace> <comma-expression>}*)
static int
parse_comma_expression( string stream, int i, string expression )
{
  // <word> | <quotation> | <string>
  if( parse_word( stream, &i, &expression ) )
    return 1;
  if( parse_quotation( stream, &i, &expression ) )
    return 2;
  if( parse_string( stream, &i, &expression ) )
    return 3;
  
  // ,<comma-expression> (<word> {<whitespace> <comma-expression>}*)
  if( stream[i] == ',' )
  {
    int oldi;
    string term;
    
    oldi = i;
    ASSERT_ELSE( parse_comma_expression( stream, &i, &term ) , (i = oldi) );
    ASSERT_ELSE( (stream[i] == '\(') , (i = oldi) );
    i++;
    ASSERT_ELSE( parse_word( stream, &i, &term ) , (i = oldi) );
    while( stream[i] != '\)' )
    {
      ASSERT_ELSE( parse_whitespace( stream, &i, &term ) , (i = oldi) );
      ASSERT_ELSE( parse_comma_expression( stream, &i, &term ) , (i = oldi) );
    }
    ASSERT_ELSE( (stream[i] == '\)') , (i = oldi) );
    expression = stream[oldi..++i];
    return 4;
  }  
  return 0;
}


// <expression> ::= <word> | <quotation> | <string> |
//                  (<word> {<whitespace> <expression>}*)
static int
parse_expression( string stream, int i, string expression )
{
  string term;
  int oldi;

  // <word> | <quotation> | <string>
  if( parse_word( stream, &i, &expression ) )
    return 1;
  if( parse_quotation( stream, &i, &expression ) )
    return 2;
  if( parse_string( stream, &i, &expression ) )
    return 3;
  
  // (<word> {<whitespace> <expression>}*)
  if( stream[0] == '\(' )
  {
    oldi = i++;

    // <word>
    ASSERT_ELSE( parse_word( stream, &i, &term ), (i = oldi) );

    // {<whitespace> <expression>}*
    while( stream[i] != '\)' )
    {
      ASSERT_ELSE( parse_whitespace( stream, &i, &term ), (i = oldi) );
      ASSERT_ELSE( parse_expression( stream, &i, &term ), (i = oldi) );
    }
    ASSERT_ELSE( stream[i] == '\)', (i = oldi) );
    expression = stream[oldi..++i];
    return 4;
  }

  return 0;
}

/*
 <performative> ::= (<word> {<whitespace> :<word> <whitespace> <expression>}*)
*/
static int
parse_performative( string stream, string performative, mapping params )
{
  int i;
  string parameter, val, trash;
  
  stream_len = strlen( stream );
  
  if( stream[0] != '(' 
      || stream[stream_len-1] != ')' )
    return 0;

  i = 1;
  ASSERT( parse_word( stream, &i, &performative ) );

  while( stream[i] != '\)' )
  {
    ASSERT( parse_whitespace( stream, &i, &trash ) );
    ASSERT( stream[i++] == ':' );
    ASSERT( parse_word      ( stream, &i, &parameter ) );
    ASSERT( parse_whitespace( stream, &i, &trash ) );
    ASSERT( parse_expression( stream, &i, &val ) );

    params[parameter] = val;
  }
  ASSERT( i == (stream_len-1) );
  return 1;
}

/*----------------------------- parse ---------------------------*/
/*
    Description:
      Extract KQML message into a performative and parameters
    Parameters:
      stream       - Message to be parsed
      performative - Output performative (must be passed by reference)
      params       - mapping in which to store parameters
    Returns:
      nonzero if stream was a proper KQML message
    Notes:
      See KQML documention on web for more information.
      I may be unwrapping the terms too much.
*/
int
parse( string stream, string performative, mapping params )
{
  return parse_performative( stream, &performative, params );
}



/*******************************************************************/
/* Packing
**
*/


private string
valid_word( string word )
{
  return parse_word( word, 0, &word ) ? word : "";
}

private string
valid_expression( string expression )
{
  return parse_expression( expression, 0, &expression ) ? expression : "";
}

#define NICE

private string
pack_expression( string expression )
{
  int len;
  len = strlen( expression );
  if( strlen(valid_expression( expression )) == len )
    return expression;

#ifdef NICE
  // Unnecessary, but It'd be nice to have more readable strings.
  // Of course they take longer to parse...
  if( searcha_any_str( expression, "\"\\" ) == -1 )
    return "\"" + expression + "\"";
#endif

  return sprintf( "#%d\"%s", len, expression );
}

private string
pack_parameter( string key, string value )
{
  return " :" + valid_word( key ) + " " + valid_expression( value );
}


/*----------------------------- pack ---------------------------*/
/*
    Description:
      Pack component parts into a KQML message
    Parameters:
      performative - The performative
      params       - A mapping containing parameter:value pairs
    Returns:
      A KQML message.
    Notes:
      The performative and parameter names will be truncated if
      necessary to ensure compliance with the KQML message grammar.
      pack() should do exactly the reverse of parse(), except that the
      order of the parameters may be changed.  This does not affect
      the semantics of the message.  Expressions may also be quoted
      differently.  I don't know if that's bad.
*/
string
pack( string performative, mapping params )
{
  string result;
  string *param_keys;
  int i;
  
  result = "(" + valid_word( performative );
  param_keys = m_indices( params );
  for( ; i < sizeof( param_keys ); i++ )
  {
    result += pack_parameter( param_keys[i], params[param_keys[i]] );
  }
  
  return result + ")";
}
