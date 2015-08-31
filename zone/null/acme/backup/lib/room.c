/*
    NAME
        room.c - Useful functions for dealing with rooms
    DESCRIPTION
        visible_directions()  - Returns all visible exit directions
	visible_exits()	      - Returns destinations of all visible exits
	invisible_directions()- Returns all invisible exit directions
	invisible_exits()     - Returns destinations of all invisible exits
	all_directions()      - Returns all exits directions
	all_exits()	      - Returns all destinations of all exits
	get_exit()	      - Returns destination of a particular exit
    NOTES
        None.
    LAST MODIFIED
        Zamboni 960111

*/

#include <acme.h>
#include AcmeRoomInc

private inherit AcmeArray;
#include AcmeArrayInc

private mapping
vis_or_invis_exits_mapping(object room, string key)
{
  if(!objectp(room))
    return 0;  
  return room->query(key);
}


private string *
vis_or_invis_exits(object room, status vis, status dir)
{
  mixed exits;
  exits = vis_or_invis_exits_mapping(room, (vis ? "exits" : "invis_exits"));

  if(!mappingp(exits))
    return 0;

  return (dir ? m_indices(exits) : m_values(exits));
}


string *
visible_directions(object room)
{
  return vis_or_invis_exits(room, 1, 1);
}

string *
visible_exits(object room)
{
  return vis_or_invis_exits(room, 1, 0);
}

string *
invisible_directions(object room)
{
  return vis_or_invis_exits(room, 0, 1);
}

string *
invisible_exits(object room)
{
  return vis_or_invis_exits(room, 0, 0);
}

string *
all_exits(object room)
{
  return add_array(visible_exits(room), invisible_exits(room));
}

string *
all_directions(object room)
{
  return add_array(visible_directions(room), 
		   invisible_directions(room));
}

string
get_exit(object room, string dir)
{
  mixed exits;

  if(!objectp(room))
    return 0;

  exits = room->query("exits");

  if(mappingp(exits) && exits[dir])
    return exits[dir];

  exits = room->query("invis_exits");
  if(mappingp(exits))
    return exits[dir];
  return 0;
}

