#include <acme.h>
#include OpieClient

inherit ObjectCode;
create()
{
  ::create();
  set("id", "example");
  set("long", 
      "This is an example of a tool that uses the OPIE system for\n"
      "object spec parsing.\n\n"
      "  foo <ospec>   - print the filename[s] of the object[s].\n"
      "  help ospec  - Get help on the ospec syntax\n");
  set("opie", opieBW);
}

init()
{
  add_action("foo", "foo");
  add_action("help", "help");
}

int
help(string arg)
{
  if(arg == "ospec")
  {
    THISP->more_string(opie_help());
    return 1;
  }
  notify_fail( "Sorry.  Can't find help on that." );
  return 0;
}

int
foo(string ospec)
{
  mixed obs;
  int i;
  if( !stringp( ospec ) )
  {
    write("Loser.  Read the instructions.\n");
    return 1;
  }

  obs = opie( ospec );

  write( "Stuff found: \n" );
  if( pointerp(obs) )
    for( i=0; i < sizeof(obs); i++ )
      printf( "  %s\n", to_string(obs[i]) );
  write( "---\n" );
  return 1;
}
