/*
    NAME
        ansi.c - Handy string manipulation routines dealing with ansi

    DESCRIPTION
        get_ansi()          - returns the length of an ansi-escape sequence
        remove_ansi()       - removes ansi from a string
        extract_ansi()      - same as remove_ansi(), but allows preservation
                              of removed ansi
        ansi_strlen()       - returns the number of length of a string
                              without ansi.
        ansi_format()       - same as format(), but works form strings with
                              ansi in them.

    NOTES
        None.

    LAST MODIFIED
        Morgaine 970512
*/

#include <acme.h>
#include <ansi.h>
private inherit AcmeArray;
#include AcmeArrayInc
#include AcmeAnsiInc

#ifndef TAB_LEN
#define TAB_LEN        8
#endif
#ifndef DEFAULT_WIDTH
#define DEFAULT_WIDTH  77
#endif

varargs int get_ansi( string str, int start )
{
    int i, sz;

    if( str[ i = start ] != 27 )
        return( 0 );

    switch( str[ ++i ] )
    {
    case '[':
        for( ++i, sz = strlen( str ); i < sz && !IsAlpha( str[ i ] ); i++ );
        break;
    case '#': 
    case ')': 
    case '(':
        i++;
        break;
    }

    return( i - start );
}

string remove_ansi( string str )
{
    int i, t, sz;

    if( ( i = member( str, 27 ) ) < 0 )
        return( str );
    sz = strlen( str );
    do
    {
        if( t = get_ansi( str, i ) )
            str = str[ 0 .. i - 1 ] + str[ i + t + 1 .. ];
    }
    while( ( i = member( str, 27 ) ) > -1 );

    return( str );
}

varargs string extract_ansi( string str, mixed ansi )
{
    int i, t, sz;

    ansi = ({ });
    if( ( i = member( str, 27 ) ) < 0 )
        return( str );
    sz = strlen( str );
    do
    {
        if( t = get_ansi( str, i ) )
        {
            ansi += ({ i, str[ i .. i + t ] });
            str = str[ 0 .. i - 1 ] + str[ i + t + 1 .. ];
        }
    }
    while( ( i = member( str, 27 ) ) > -1 );

    return( str );
}

int ansi_strlen( string str )
{
    return( strlen( remove_ansi( str ) ) );
}


// Function: ansi_format
// Does the same things as format(), but supports ANSI colors.
varargs string ansi_format( string msg, int width, string prefix )
{
   string output;
   string fmt;
   string infix;
   int    len, cur, pos, last_blank, last_endl, last_word_len, pref_len;

   if( !stringp( msg ) ) raise_error(
      "Bad argument 1 to ansi_format( string, int, string )\n" );
   if( member( msg, 27 ) < 0 )
       return( format( msg, width, prefix ) );
   if( width <= 0 ) width = DEFAULT_WIDTH;
   if( !stringp( prefix ) ) prefix = "";
   len        = strlen( msg ) - 1;
   width     -= ( pref_len = strlen( output = prefix ) );
   fmt        = "%s\n";
   for( cur = strlen( prefix ); cur--; ) fmt += " ";
   infix      = ( msg[len] == '\n' ? "" : endl );
   last_blank = last_endl = -1;
   last_word_len = cur = pos = 0;
   for( ; cur <= len; cur++, pos++, last_word_len++ )
   {
      while( msg[cur] == 27 ) if( !( cur = 1 + member( msg, 'm', cur ) ) )
         return msg;
      switch( msg[cur] )
      {
      case '\n':
         last_blank = cur++;
         last_word_len = 1;
         pos = width;
         break;
      case '\t':
         pos = ( ( pos + pref_len ) / TAB_LEN + 1 ) * TAB_LEN - pref_len - 1;
      case ' ':
         last_blank    = cur;
         last_word_len = 0;
         break;
      }
      if( pos >= width )
      {
         if( last_blank == last_endl )
         {
            // The word is VERY long. Split it.
            if( last_blank != len )
               output += sprintf( fmt, msg[last_endl+1..cur-1] );
            else
               output += sprintf( "%s\n", msg[last_endl+1..cur-1] );
            pos = 0;
            last_endl = last_blank = cur - 1;
            last_word_len = -1;
         }
         else
         {
            if( last_blank != len )
               output += sprintf( fmt, msg[last_endl+1..last_blank-1] );
            else
               output += sprintf( "%s\n", msg[last_endl+1..last_blank-1] );
            pos = last_word_len - 1;
            last_endl = last_blank;
            last_word_len = -1;
         }
      }
   }
   if( len >= last_endl - 2 )
      return output + msg[last_endl+1..len] + infix;
   return output + infix;
}

varargs string make_ansi_code( string *keys, string *unused )
{
    string ret, fore, back;
    mixed  tmp;
 
    keys = unique_array( keys );
    tmp = keys & ({ "default", "norm", "flash", "blink", "bold",
              "underline", "reverse", "black", "red", "green", "yellow",
              "brown", "blue", "magenta", "purple", "cyan", "white",
              "black,black", "black,red", "black,green", "black,yellow", 
              "black,brown", "black,blue", "black,magenta", "black,purple", 
              "black,cyan", "black,white", "red,black", "red,red", "red,green", 
              "red,yellow", "red,brown", "red,blue", "red,magenta", 
              "red,purple", "red,cyan", "red,white", "green,black", 
              "green,red", "green,green", "green,yellow", "green,brown", 
              "green,blue", "green,magenta", "green,purple", "green,cyan", 
              "green,white", "yellow,black", "yellow,red", "yellow,green", 
              "yellow,yellow", "yellow,brown", "yellow,blue", "yellow,magenta", 
              "yellow,purple", "yellow,cyan", "yellow,white", "brown,black", 
              "brown,red", "brown,green", "brown,yellow", "brown,brown", 
              "brown,blue", "brown,magenta", "brown,purple", "brown,cyan", 
              "brown,white", "blue,black", "blue,red", "blue,green", 
              "blue,yellow", "blue,brown", "blue,blue", "blue,magenta", 
              "blue,purple", "blue,cyan", "blue,white", "magenta,black", 
              "magenta,red", "magenta,green", "magenta,yellow", 
              "magenta,brown", "magenta,blue", "magenta,magenta", 
              "magenta,purple", "magenta,cyan", "magenta,white",
              "purple,black", "purple,red", "purple,green", "purple,yellow", 
              "purple,brown", "purple,blue", "purple,magenta", "purple,purple", 
              "purple,cyan", "purple,white", "cyan,black", "cyan,red", 
              "cyan,green", "cyan,yellow", "cyan,brown", "cyan,blue", 
              "cyan,magenta", "cyan,purple", "cyan,cyan", "cyan,white",
              "white,black", "white,red", "white,green", "white,yellow", 
              "white,brown", "white,blue", "white,magenta", "white,purple", 
              "white,cyan", "white,white" });
    unused = keys - tmp;
    tmp = mkmapping( tmp );
 
    ret = ESC "[";
 
    if( member( tmp, "norm" ) || member( tmp, "default" ) )
    {
        ret += _CLEAR + ";";
        m_delete( tmp, "norm" );
        m_delete( tmp, "default" );
    }
 
    if( member( tmp, "bold" ) )
    {
        ret += _BOLD + ";";
        m_delete( tmp, "bold" );
    }
 
    if( member( tmp, "underline" ) )
    {
        ret += _UNDERLINE + ";";
        m_delete( tmp, "underline" );
    }
 
    if( member( tmp, "blink" ) || member( tmp, "flash" ) )
    {
        ret += _BLINK + ";";
        m_delete( tmp, "blink" );
        m_delete( tmp, "flash" );
    }
 
    if( member( tmp, "reverse" ) )
    {
        ret += _INVERSE + ";";
        m_delete( tmp, "reverse" );
    }
 
    if( !sizeof( tmp ) )
    {
        ret = ret[ 0 .. <2 ] + "m";
        if( ( strlen( ret ) == 4 ) && ( ret[ 2 .. 2 ] == _CLEAR ) )
            ret = NOR3;
        return( ret );
    }
 
    /*** everything else must be color
         since only one will be in the string, choose one arbitrarily ***/
    fore = m_indices( tmp )[ <1 ];
    sscanf( fore, "%s,%s", fore, back );
    
    switch( fore )
    {
    case "black":
        ret += _BLACK + ";";
        break;
    case "red":
        ret += _RED + ";";
        break;
    case "green":
        ret += _GREEN + ";";
        break;
    case "yellow":
    case "brown":
        ret += _YELLOW + ";";
        break;
    case "blue":
        ret += _BLUE + ";";
        break;
    case "magenta":
    case "purple":
        ret += _MAGENTA + ";";
        break;
    case "cyan":
        ret += _CYAN + ";";
        break;
    case "white":
        ret += _WHITE + ";";
        break;
    }
 
    switch( back )
    {
    case 0:
        ret = ret[ 0 .. <2 ] + "m";
        break;
    case "black":
        ret += _BACK_BLACK + "m";
        break;
    case "red":
        ret += _BACK_RED + "m";
        break;
    case "green":
        ret += _BACK_GREEN + "m";
        break;
    case "yellow":
    case "brown":
        ret += _BACK_YELLOW + "m";
        break;
    case "blue":
        ret += _BACK_BLUE + "m";
        break;
    case "magenta":
    case "purple":
        ret += _BACK_MAGENTA + "m";
        break;
    case "cyan":
        ret += _BACK_CYAN + "m";
        break;
    case "white":
        ret += _BACK_WHITE + "m";
        break;
    }
 
    return( ret );
}
