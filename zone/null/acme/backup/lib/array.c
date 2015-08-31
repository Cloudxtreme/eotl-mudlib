/*
    NAME
        array.c - Array handling routines
    DESCRIPTION
        Useful routines for dealing with arrays
        array_eq()        - Compare arrays
        array_equal()     - Compare arrays recursively
        reverse_array()   - reverse the order of an array
        checked_range()   - Same as list[i..j] but with range cropping
        checked_item()    - Same as list[i] but with range rejecting
        checked_item_reverse() - Same but counts from the end
        wrapped_item()    - Same as checked_item() but wraps around
        flatten_array1()  - Flatten out an array by only one level
        flatten_array()   - Flatten out an array entirely (depth first)
        unique_array()    - Returns a copy of the array with no duplicates
        add_array()       - Add two arrays together, checks for nulls.
        intersect_array() - Array-AND
        exclude_array()   - Array subtraction
        prepend_array()   - prepends something to each item in an array
        anti_searcha()    - returns position of first element that isn't <el>
        searcha_any()     - like searcha, but will match on any of a set
    NOTES
        None.
    LAST MODIFIED
        Zamboni 960319

*/


/*------------------------------- array_eq ------------------------------*/
/*
    Description:
      Check to see if the contents of two arrays are the same
    Parameters:
      a1 - an array
      a2 - an array
    Returns:
      True if a1[n] == a2[n] for all n.
    Notes:
      This is not recursive.  If a1 and a2 contain different arrays
      which each contain the same things, this will return 0.  Try
      array_equal() if that's not what you want.
*/
status
array_eq( mixed *a1, mixed *a2 )
{
  int i, max;
  if( (max = sizeof(a1)) != sizeof(a2) )
    return 0;
  for( ;i < max; i++ )
    if( a1[i] != a2[i] )
      return 0;
  return 1;
}

/*------------------------------- array_eq ------------------------------*/
/*
    Description:
      Check to see if the contents of two arrays are the same
    Parameters:
      a1 - an array
      a2 - an array
    Returns:
      True if a1[n] == a2[n] for all n.
    Notes:
      This is recursive.  If a1 and a2 contain different arrays
      which each contain the same things, this will return 1.
*/
status
array_equal( mixed *a1, mixed *a2 )
{
  int i, max;
  if( (max = sizeof(a1)) != sizeof(a2) )
    return 0;
  for( ;i < max; i++ )
    if(( a1[i] != a2[i] )
       && !( pointerp(a1[i])
             && pointerp(a2[i])
             && array_equal(a1[i], a2[i])) )
      return 0;
  return 1;
}



mixed *
add_array(mixed *orig1, mixed *orig2)
{
  if(!pointerp(orig1))
    return orig2;
  if(!pointerp(orig2))
    return 0;
  return orig1 + orig2;
}

/* 
** Reverse the order in an array.
** Since arrays are passed by reference, this changes the original.
** Returns a pointer to the array just to be nice.
*/
mixed *
reverse_array(mixed *list)
{
  int i, size, middle;
  mixed temp;

  size = sizeof(list);
  middle = (size / 2);
  
  for(i=0; i < middle; i++)
  {
    temp             = list[i];
    list[i]          = list[size -i -1];
    list[size -i -1] = temp;
  }

  return list;
}


/*
** Same as list[i..j] but with range cropping.
*/
mixed
checked_range(mixed *list, int i, int j)
{
  if(i < 0)
    i = 0;
  if(j >= sizeof(list))
    j = sizeof(list) - 1;

  if(j < i)
    return ({ });
  
  return list[i..j];
}

/*
** Same as list[i] but won't give an error on illegal indeces.
*/
mixed
checked_item(mixed *list, int i)
{
  if(!pointerp(list) 
     || (i < 0)
     || (i >= sizeof(list)))
    return 0;
  return list[i];
}

/*
** Same as checked_item, but counts from the end of the array.
*/
mixed
checked_item_reverse(mixed *list, int i)
{
  if(!pointerp(list) 
     || (i < 0)
     || (i >= sizeof(list)))
    return 0;
  return list[<(i+1)];
}

/*
** Same as checked_item, but wraps around instead of rejecting
*/
mixed
wrapped_item(mixed *list, int i)
{
  int size;
  if(!(size = sizeof(list)))
    return 0;
  return list[(i + size) % size];
}


/*
** Turns ({ ({ a, b }), c, ({ d, ({ e }) }) }) 
** into  ({ a, b, c, d, ({ e }) }) 
*/
mixed
flatten_array1(mixed *list)
{
  mixed *newlist;
  int i, size;

  if(!pointerp(list))
    return ({ });

  size = sizeof(list);
  newlist = ({ });
  for(i=0; i < size; i++)
  {
    if(pointerp(list[i]))
      newlist += list[i];
    else
      newlist += ({ list[i] });
  }
  return newlist;
}

/*
** Turns ({ ({ a, b }), c, ({ d, ({ e }) }) }) 
** into  ({ a, b, c, d, e, })
*/
mixed
flatten_array(mixed *list)
{
    mixed *newlist;
    int i, max;;

    if (!pointerp(list))
        return ({ list });
    newlist = ({});
    max = sizeof(list);
    for (i=0; i < max; i++)
    {
        newlist += flatten_array(list[i]);
    }
    return newlist;
}

/*
** Returns a copy of the array with no duplicates
*/
mixed
unique_array(mixed *array)
{
  return m_indices(mkmapping(array));
}

/* 
** Array-AND
**  Order is the same as the first array. 
**  Also uniquifies the result.
*/
mixed
intersect_array(mixed *orig1, mixed *orig2)
{
  mixed hash;
  hash = mkmapping(orig2);
  return unique_array(filter_array(orig1, 
                                   lambda(({ 'e }), 
                                          ({ #'member, hash, 'e }))));
}

/* 
**  Removes from the first array all occurances 
**  of any items in the second
*/
mixed *
exclude_array(mixed *orig1, mixed *orig2)
{
  mixed hash;
  hash = mkmapping(orig2);
  return unique_array(filter_array(orig1, 
                                   lambda(({ 'e }), 
                                          ({ #'!, 
                                               ({ #'member, hash, 'e }) }))));
}

/*
** Prepends something to each item in an array
*/
mixed *
prepend_array(mixed item, string *list)
{
  int i, size;
  for(i=0, size = sizeof(list); i < size; i++)
    list[i] = item + list[i];
  return list;
}


/* returns the index of the first element that isn't <el> */
varargs int
anti_searcha(mixed list, mixed el, int start, int step)
{
  int size;

  if(stringp(list))
    size = strlen(list);
  else
    size = sizeof(list);

  if(!step)
    step = 1;

  if(step > 0)
  {
    for( ; (start < size) && (list[start] == el); start += step);
    return (start < size) ? start : -1;
  } else
  {
    for( ; (start >= 0)   && (list[start] == el); start += step);
    return (start >= 0)   ? start : -1;
  }
}


/*
** Like searcha, but instead of searching for a single item** for any of a set.  Use AcmeStrings->searcha_any_str() for strings if 
** you want to ignore escaped characters in strings.
*/
varargs int
searcha_any(mixed list, mixed elements, int start, int step)
{
  int size;

  if(stringp(list))
    size = strlen(list);
  else
    size = sizeof(list);

  if(!step)
    step = 1;

  for(; (start < size); start += step)
    if(searcha(elements, list[start]) != -1)
      return start;
  
  return -1;
}


