#ifndef ACME_ROOM_INC
#define ACME_ROOM_INC

string *visible_directions   ( object room );
string *visible_exits        ( object room );
string *invisible_directions ( object room );
string *invisible_exits      ( object room );
string *all_exits            ( object room );
string *all_directions       ( object room );
string  get_exit             ( object room, string dir );

#endif
