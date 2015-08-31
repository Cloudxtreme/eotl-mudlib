
#include <acme.h>
#include AcmeActionInc

status
has_action(object who, string verb)
{
  return !!query_actions(who, verb);
}


string 
action_type_char(int type)
{
  if(type == SENT_PLAIN)      return " ";
  if(type == SENT_SHORT_VERB) return "S";
  if(type == SENT_NO_SPACE)   return "X";
  if(type == SENT_NO_VERB)    return "N";
  if(type == SENT_MARKER)     return "M";
  return "?";
}


/* 
** returns an array of all actions in <who> that match the 
** regexp <pattern> defined by any object in <from> (if supplied).
** Each item in the array is an array:
**  ({ string verb, int type, ? short_verb, object ob, string function })
** See action_type_char for slightly more information on the type.
*/
mixed
get_actions_regexp(object who, string pattern, object *from)
{
  int i, num_actions, matching;
  mixed actions, matched;
  string *tmp;

  matched = ({ });
  actions = query_actions(who, MASK);

  num_actions = sizeof(actions) / NUM_IN_MASK;

  for(i=0; i < num_actions; i++)
  {
    if(tmp = regexp( ({ actions[i * NUM_IN_MASK + VERB] }), pattern))
    {
      if(sizeof(tmp)
       && (!from || (index(from, actions[i * NUM_IN_MASK + OBJECT]) != -1)))
      {
	matched += ({ ({ actions[i * NUM_IN_MASK + VERB],
			 actions[i * NUM_IN_MASK + TYPE],
			 actions[i * NUM_IN_MASK + SHORT_VERB],
			 actions[i * NUM_IN_MASK + OBJECT],
			 actions[i * NUM_IN_MASK + FUNCTION]
		       }) });
      }    
    }
  }
  return matched;
}

/*
** Same as get_actions_regexp but doesn't use regular expressions.
*/
mixed
get_actions_literal(object who, string verb, object *from)
{
  int i, num_actions, matching;
  mixed actions, matched;

  matched = ({ });
  actions = query_actions(who, MASK);

  num_actions = sizeof(actions) / NUM_IN_MASK;

  for(i=0; i < num_actions; i++)
  {
    if((!verb || (actions[i * NUM_IN_MASK + VERB] == verb))
       && (!from || (index(from, actions[i * NUM_IN_MASK + OBJECT]) != -1)))
    {
      	matched += ({ ({ actions[i * NUM_IN_MASK + VERB],
			 actions[i * NUM_IN_MASK + TYPE],
			 actions[i * NUM_IN_MASK + SHORT_VERB],
			 actions[i * NUM_IN_MASK + OBJECT],
			 actions[i * NUM_IN_MASK + FUNCTION]
		       }) });
    }
  }
  return matched;
}
