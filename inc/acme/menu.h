#ifndef ACME_MENU_INC
#define ACME_MENU_INC

status print_menu ( mixed *menu );
status menu_choice ( string s, mixed *menu );
varargs status prev_menu( int i );

#endif  // ACME_MENU_INC
