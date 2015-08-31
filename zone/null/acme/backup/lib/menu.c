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
        Aleron  960214
*/

#include <acme.h>

private inherit AcmeError;

#include AcmeErrorInc
#include AcmeMenuInc

#define TITLE(s)         write("\n"+s+"\n\n")
#define CHOICE(i,s)      write(i+") "+s+"\n")
#define LINE()           write("\n==================================================\n")
#define PROMPT()         write( "Please enter the number of your choice: " )
#define OUTPUT(s)        write("\n"+s+"\n")
#define INVALIDCHOICE()  OUTPUT("That is an invalid choice.")

private mixed menustack;

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
   
   LINE();
   TITLE( menu[0] );
   max = sizeof( menu );
   for ( i = 1; i < max; i++ )
      CHOICE( i, menu[i][0] );
   LINE();
   PROMPT();
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
      menustack = ( menustack ? menustack + ({ ({ method, arg }) }) :
                   ({ ({ method, arg }) }) );
      apply( method, arg );
   }
   else if ( stringp( method ) )
   {
      menustack = ( menustack ? menustack + ({ ({ method, arg }) }) :
                   ({ ({ method, arg }) }) );
      call_other( THISO, method, arg );
   }
   else
      return 0;
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
      INVALIDCHOICE();
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
   menustack = 0;   // clear the menu stack
   OUTPUT( s );
   return 1;
}

/*-------------------------------- prev_menu --------------------------------*/
/*
    description: runs the last i-th menu
    parameters: i  - the i-th last menu
    returns: sucess status
    notes: if i is not set, then do the last menu
*/
varargs status prev_menu( int i )
{
   int n;

   if ( !i )
      i = 1;
   if ( !menustack || !pointerp( menustack ) ||
       !( 0 < i && ( n = sizeof( menustack ) ) >= i ) )
      return 0;
   if ( i > 1 )
      menustack = deletea( menustack, n-i+1, n-1 );
   return do_choice( menustack[n-i][0], menustack[n-i][1] );
}
