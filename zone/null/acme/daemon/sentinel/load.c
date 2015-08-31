#include <acme.h>

inherit AcmeForceLoad;

#include AcmeForceLoadInc

// this is using the bastardized cmd format
status
do_cmd( string name, string targ )
{
   force_load( targ );
   return 1;
}

// disallow shadows
status
query_prevent_shadow()
{
   return 1;
}
