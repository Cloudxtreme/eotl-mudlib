/*
    NAME
        menu.c - MENU package
    DESCRIPTION
        A package to create a standardized menuing system.
        print_menu()    - outputs the menu
        menu_choice()   - acts on player's choice
        exit_menu()     - exiting out of a menu
        prev_menu()     - runs a previous menu
    NOTES
        The menu must be in the following format:
        ({ string menu_title,
        ({ string menu_choice, mixed method, mixed *args }),
        ({ string menu_choice, mixed method, mixed *args }),
                .
                .
        })
        method can be either a closure or a function name
    LAST MODIFIED
        Aleron  980403
*/
 
#include <acme.h>
 
private inherit AcmeError;
private inherit AcmeStack;
 
#include AcmeErrorInc
#include AcmeStackInc
#include AcmeMenuInc
 
#define OUTPUT(s)        write("\n"+s+"\n")
 
private StackHdl menustack;
 
/*--------------------------- overloadable functions ------------------------*/
/*
    name: get_header, get_footer
    description: generates a seperator to precede/follow the menu.
    returns: the header/footer string.
    notes: because get_footer just returns get_header, if you intend both to be
       the same, it is uncessary to overload them both.  A newline must be
       included at the end
*/
string get_header( string menuname )
{
    return( "\n==================================================\n" );
}
 
string get_footer( string menuname )
{
    return( get_header( menuname ) );
}
 
/*
    name: get_choice
    description: generates the choice line
    returns: the choice line
    notes: remember to return a trailing newline, it is not appended
        automatically.
*/
string get_choice( int choice_num, string choice_str, string menuname )
{
    return( choice_num + ") " + choice_str + endl );
}
 
/*
    name: get_prompt
    description: generates the choice prompt.
    notes: again, no newline is autoappended, but as this is a prompt,
        you may not want one.
*/
string get_prompt( string menuname )
{
    return( "Please enter the number of your choice: " );
}
 
/*
    name: invalid_choice
    description: handles invalid choices.
    returns: 1 or 0, depending on whether you want to resume exit the menu
        or not, respectively.
    notes: none
*/
int invalid_choice( string menuname, string choice )
{
    write( "\nThat is an invalid choice.\n" );
    return( 0 );
}
 
/*
    name: get_title
    description: generates the title header.
    returns: the title header.
    notes: No newline is autoappended.
*/
string get_title( string title )
{
    return( endl + title + endl endl );
}
 
/*-------------------------------- print_menu -------------------------------*/
/*
    description: prints out a menu
    parameters: menu
    returns: status 1
    notes: None.
*/
status print_menu( mixed *menu )
{
   int i, max;
 
   if (!menustack || !pointerp(menustack))
     menustack = newstack();
   push(menu, menustack);
   write( get_header( menu[ 0 ] ) );
   write( get_title( menu[0] ) );
   max = sizeof( menu );
   for ( i = 1; i < max; i++ )
      write( get_choice( i, menu[i][0], menu[ 0 ] ) );
   write( get_footer( menu[ 0 ] ) );
   write( get_prompt( menu[ 0 ] ) );
   input_to( "menu_choice", 0, menu );
}
 
 
//-------------------------------- do_choice --------------------------------//
/*
    description: runs the method
    parameters: method    - the method to activate
                arg       - the method's arguments
    returns: status (if the method was valid)
    notes: None.
*/
static status do_choice( mixed method, mixed arg )
{
   if ( closurep( method ) )
   {
      apply( method, arg );
   }
   else if ( stringp( method ) )
   {
      call_other( THISO, method, arg );
   }
   else
   {
     return 0;
   }
   return 1;
}
 
/*------------------------------- menu_choice -------------------------------*/
/*
    description: calls the function to handle the choice a player chose
    parameters: s         - the player's choice
                menu      - the menu the player was choosing from
    returns: status 1
    notes: None.
*/
status menu_choice( string s, mixed *menu )
{
   int i;
 
   i = to_int( s );
   if ( i < 1 || i >= sizeof( menu ) )
   {
      if( !invalid_choice( menu[ 0 ], s ) )
          print_menu( menu );
   }
   else
   {
      if ( !do_choice( menu[i][1], menu[i][2] ) )
         return error( ERR_ERROR, "Invalid method for menu choice "+i, 0 );
   }
   return 1;
}
 
/*-------------------------------- exit_menu --------------------------------*/
/*
    description: exits the menuing system
    parameters: s  - the message the player gets after exitting the menu
    returns: 1
    notes: None.
*/
status exit_menu( string s )
{
   menustack = newstack();   // clear the menu stack
   OUTPUT( s );
   return 1;
}
 
/*-------------------------------- prev_menu --------------------------------*/
/*
    description: runs the last n-th menu
    parameters: n  - the n-th last menu
    returns: sucess status
    notes: if n is not set, then do the last menu
*/
varargs status prev_menu( int n )
{
   int i;
   mixed menu;
 
   if ( !n )
      n = 1;
   for (i = 0; i < n && !emptystack(menustack); i++)
     menu = pop(menustack);

   return print_menu(menu);
}
