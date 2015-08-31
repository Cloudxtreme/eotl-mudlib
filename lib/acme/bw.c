
#include <acme.h>
private inherit AcmeFunction;
private inherit AcmeShadow;

#include AcmeFunctionInc
#include AcmeShadowInc

object *
find_bw_shields(object who)
{
  object *shads;
  shads = all_shadows(who);
  if(!shads)
    return 0;
  return filter_array(shads, #'exists_function, "query_bw_shield");
}

object *
transplant_bw_shields(object from, object to)
{
  object *shields;
  
  shields = find_bw_shields(from);
  
  map_array(shields, #'remove_shadow);
  map_objects(shields, "sh_init", to);

  return shields;
}

object *
steal_bw_shields(object from)
{
  return transplant_bw_shields(from, THISP);
}

void
dest_bw_shields(object who)
{
  object *shields;
  shields = steal_bw_shields(who);
  map_array(shields, #'remove_shadow);
  map_array(shields, #'destruct);
}
