inherit "/usr/zamboni/cmds/Call";

// disallow shadows
status
query_prevent_shadow()
{
   return 1;
}
