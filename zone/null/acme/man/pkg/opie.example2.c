
// Here's an example use of opie_p()
/************************************************************************/
#include <acme.h>
#include OpieClient
#include AcmeFindTargetInc

int
tell_folks(string targets, string message)
{
  object *obs;
  obs = opie_p(targets, FT_FindPlayer FT_FindLiving );

  if( !obs )
  {
    notify_fail( "Who? What? Where?" );
    return 0;
  }
  
  message = strformat( message, sprintf("%s tells you: ", PNAME));

  map_array(obs, #'tell_object, message);
  printf("Sent to %d object%s.\n", sizeof(obs), (sizeof(obs)==1) ? "": "s");
  return 1;
}

/************************************************************************/
