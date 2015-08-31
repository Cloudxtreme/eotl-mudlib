/*
    NAME
        user.c - Useful functions for dealing with users
    DESCRIPTION
        level_range()        - filter a list of players by level
        level_range_fast()   - same thing, but with side effects
        filter_by_hostname() - filter a list of players by hostname regexp
        to_ordlevel()        - Convert level names, OrdLevels, or quoted
                               OrdLevels to Ordlevels
        wizards()            - Filters out all non-wizards from a specified
                               list or from all users
        mortals()            - Filters out all non-mortals from a specified
                               list or from all users

    NOTES

    LAST MODIFIED
        Zamboni 960229

*/

private static closure filter_by_hostname_cl;

/*------------------------------- level_range -------------------------------*/
/*
    Description: 
      A user-object filter that works on level.
      For example, level_range(users(), OrdLevel(Guest), OrdLevel(God))
      would return an array containing all the wizards online.
    Parameters: 
      ulist     - array of some users
      min       - minimum OrdLevel of users to keep
      max       - maximum OrdLevel of users to keep
    Returns:
      New array of all users in <folks> who were within the range specified
    Notes: 
      None.
*/
object *
level_range(object *ulist, int min, int max)
{
  int i, size, ol;
  object *newlist;

  if(!pointerp(ulist) || !(size = sizeof(ulist)))
    return 0;
  
  newlist = ({ });
  for(i=0; i < size; i++)
    if(((ol = GetOrdLevel(ulist[i])) >= min)
       && (ol <= max))
      newlist += ({ ulist[i] });
  return newlist;
}

/*---------------------------- level_range_fast -----------------------------*/
/*
    Description: 
      see level_range()
    Parameters: 
      ulist     - array of some users
      min       - minimum OrdLevel of users to keep
      max       - maximum OrdLevel of users to keep
    Returns:
      folks, stripped of all users who were not within the range
      specified
    Notes: 
      Unlike level_range(), this routine screws with the passed array
      rather than just returning a newly constructed version.  It's
      probably faster, especially for lists in which few members are
      kept.
*/
object *
level_range_fast(object *ulist, int min, int max)
{
  int i, size, ol;

  size = sizeof(ulist);
  for(i=0; i < size; i++)
    if(((ol = GetOrdLevel(ulist[i])) < min)
       || (ol > max))
      ulist[i] = 0;
  ulist = filter_array(ulist, #'objectp);
  return ulist;
}

/*---------------------------- to_ordlevel -----------------------------*/
/*
    Description: 
      
    Parameters: 
      arg       - The name of a level ("Wizard"), an OrdLevel (3), 
                  or a quoted OrdLevel ("3")
    Returns:
      the corresponding OrdLevel
    Notes: 
      None.
*/
int
to_ordlevel(mixed arg)
{
  int val;
  if(intp(arg))
    return arg;
  if(stringp(arg))
  {
    if(sscanf(arg, "%d", val))
      return val;
    return OrdLevel(lower_case(arg));
  }
  return -1;
}


private void
make_hostname_closure()
{
  filter_by_hostname_cl =
    lambda(({ 'o, 'pattern }),
	   ({ #'sizeof,
             ({ #'&& ,
               ({ #'interactive, 'o }),
	       ({ #'regexp,
		 ({ #'({ , ({ #'lower_case,
			      ({ #'query_ip_name, 'o }) }) }),
		 'pattern }) }) }) );
}


/*-------------------------- filter_by_hostname ---------------------------*/
/*
    Description: 
      Filter a list of users by a hostname regexp.
    Parameters: 
      ulist     - array of some objects, usually players
    Returns:
      A list of everything in ulist that was interactive and whose
      hostname was matched by <pattern>.
    Notes: 
      None.
*/
object *
filter_by_hostname(object *ulist, string pattern)
{
  if(!closurep(filter_by_hostname_cl))
    make_hostname_closure();
  return filter_array( ulist, filter_by_hostname_cl, pattern );
}

/*-------------------------- wizards ---------------------------*/
/*
    Description: 
      Filters out all non-wizards from a specified list or from all users
    Parameters: 
      ulist     - array of some objects, usually players
    Returns:
      ulist || users() minus all non-wizards
    Notes: 
      None.
*/
varargs object *
wizards(object *ulist)
{
  if(!ulist)
    ulist = users();
  return level_range_fast( ulist, 1, 99 );
}

/*-------------------------- mortals ---------------------------*/
/*
    Description: 
      Filters out all non-mortals from a specified list or from all users
    Parameters: 
      ulist     - array of some objects, usually players
    Returns:
      ulist || users() minus all non-mortals
    Notes: 
      None.
*/
varargs object *
mortals(object *ulist)
{
  if(!ulist)
    ulist = users();
  return level_range_fast( ulist, 0, 0 );
}
