/*
    NAME

        varspace.c - A two-tiered mapping ( A 2-D VariableCode )

    DESCRIPTION
        vset()             - set a variable
        vquery()           - test a variable
        set()              - backwardly compatible with VariableCode
        query()            - backwardly compatible with VariableCode

    NOTES

        There used to be a default spacekey of PRNAME, and a function
        you could override to change the default.  That has been
        abandoned.  Now if you don't specify the spacekey, it will
        simply be 0.  This has the nice side effect of making this
        package backward compatible with VariableCode, though the
        intent of the change was just to make it cleaner.
     
    LAST MODIFIED
        Zamboni 960402


*/

#include <acme.h>
#include AcmeVarSpaceInc

mapping variables;

void
vset(mixed key, mixed value, mixed space_key)
{
  if(!mappingp(variables))
    variables = ([ ]);
  
  if(!member(variables, space_key)
     || !mappingp(variables[space_key]))
    variables[space_key] = ([]);
  
  variables[space_key][key] = value;
}


mixed
vquery(mixed key, mixed space_key)
{
  if(!mappingp(variables)
     || !member(variables, space_key)
     || !mappingp(variables[space_key]))
    return 0;
  return variables[space_key][key];
}

varargs void
set(mixed key, mixed value, mixed space_key)
{
  vset( key, value, space_key );
}


varargs mixed
query(mixed key, mixed space_key)
{
  return vquery( key, space_key );
}
