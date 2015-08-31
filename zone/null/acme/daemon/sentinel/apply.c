inherit "/usr/zamboni/cmds/apply.c";

// disallow shadows
status
query_prevent_shadow()
{
   return 1;
}
