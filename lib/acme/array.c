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
        unique_array_slow() - unique_array() without side effects
        add_array()       - Add two arrays together, checks for nulls.
        intersect_array() - Array-AND
        exclude_array()   - Array subtraction
        prepend_array()   - prepends something to each item in an array
        anti_searcha()    - returns position of first element that isn't <el>
        searcha_any()     - like member, but will match on any of a set
        anti_searcha_any() - returns position of first element that 
                             doesn't match set
        to_int_range()    - returns an array of integers that match a range
        typecast_array()  - make all values in an array the same type
        random_array()    - generate an array of random values
        sort_alist()      - sorts an alist respective to the values of the keys
        acc_array() - compound elements through a closure
    NOTES
        None.
    LAST MODIFIED
        Devo 980123

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
** Returns a copy of the array with no duplicates in the same
** order as original array.
*/
varargs mixed *
unique_array_slow( mixed *arr, int keep_last )
{
  mixed *out;
  int i, j, found;
  out = ({ });
  if( keep_last )
  {
    for( i=sizeof(arr)-1; i>=0; i-- )
      if( member( out, arr[i] ) == -1 )
        out = ({ arr[i] }) + out;
  }
  else
  {
    for( i=0, j=sizeof(arr); i<j; i++ )
      if( member( out, arr[i] ) == -1 )
        out += ({ arr[i] });
  }
  return out;
}

/* 
** Array-AND
**  Order is the same as the first array. 
**  Also uniquifies the result.
*/
mixed
intersect_array(mixed *orig1, mixed *orig2)
{
  return( orig1 & orig2 );
/*
  mixed hash;
  hash = mkmapping(orig2);
  return unique_array(filter_array(orig1, 
                                   lambda(({ 'e }), 
                                          ({ #'member, hash, 'e }))));
*/
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
** Like member, but instead of searching for a single item** for any of a set.  Use AcmeStrings->searcha_any_str() for strings if 
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
    if(member(elements, list[start]) != -1)
      return start;
  
  return -1;
}


/*
** Like searcha_any, but instead of searching for a match, searches for
** a non-match.
*/
varargs int
anti_searcha_any(mixed list, mixed elements, int start, int step)
{
  int size;
 
  if(stringp(list))
    size = strlen(list);
  else
    size = sizeof(list);
 
  if(!step)
    step = 1;
 
  for(; (start < size); start += step)
    if(member(elements, list[start]) == -1)
      return start;
  
  return -1;
}

#ifndef is_int
#define is_int(s) ( to_int(s) || ( strlen(s) && s[0]=='0' ) )
#endif 

/*
** Returns an array of integers that match the specified range.
*/
int *
to_int_range( string range )
{
  int *out;
  int i, j;
  string *rargs;
  if( !stringp(range) )
    return 0;
  if( anti_searcha_any(range, "01234567890,- ") != -1 )
    return 0;
  rargs = explode(range, ",");
  out = ({ });
  for(i=0,j=sizeof(rargs); i<j; i++)
  {
    string *tmp;
    int start, end, x;
    tmp = explode(rargs[i], "-");
    if( sizeof(tmp) == 0 )
      continue;
    if( sizeof(tmp) == 1 && 
        strlen( tmp[0] ) && 
        is_int( tmp[0] )    )
    {
      out += ({ to_int(tmp[0]) });
      continue;
    }
    if( sizeof(tmp) > 1 )
    {
      tmp = tmp[<2..<1];
      if( !is_int(tmp[0]) || !is_int(tmp[1]) )
        continue;
      start = to_int(tmp[0]);
      end = to_int(tmp[1]);
      if( start > end )
        continue;
      for( x=start; x<=end ; x++ )
        out += ({ x });
    }
  }
  return out;
}

#include <sys/lpctypes.h>
 
/*------------------------ typecast_array ---------------------------*/
/*
    Description:
      Attempts to make every value in an array a given type.
    Parameters:
      arr      - the array to be typecasted
      type     - the type to cast the array to
    Returns:
      An array where all elements have been converted to type,
      or if they could not be converted to type, extracted.
    Notes:
      None.  
*/
 
private static closure typecast_cl;
private static closure ta_tmp_cl;
 
mixed *
typecast_array( mixed *arr, int type )
{
  if( !pointerp( arr ) )
    return ({ });
  if( !closurep( typecast_cl ) )
  {
    ta_tmp_cl = lambda( 0, ({ #'return }) );   // The only time we'll see
                                               // this value is here.
    typecast_cl = lambda( ({ 'x, 'y }),        
      ({ #'?,
         ({ #'==, ({ #'efun::typeof, 'x }), 'y }),
         'x,
         ({ #'catch, 
            ({ #'=, 
               'ret, 
               ({ #'switch,
                  'y,   
                  T_NUMBER,  ({ #'to_int, 'x }),    #'break,
                  T_STRING,  ({ #'to_string, 'x }), #'break,
                  T_POINTER, ({ #'to_array, 'x }),  #'break,
                  T_OBJECT,  ({ #'to_object, 'x }), #'break,
                  T_FLOAT,   ({ #'to_float, 'x }),  #'break,
                  #'default, 0,                     #'break
               })
            })
         }),
         ta_tmp_cl,
         'ret,
         'ret,
         ({ #'intp, 'x }),
         'ret,
         ({ #'floatp, 'x }),
         'ret,
         ({ #'&&,
            ({ #'&&, ({ #'stringp, 'x }), ({ #'strlen, 'x }) }),
            ({ #'==, ({ #'[, 'x, 0 }), '0' })
         }),
         'ret,
         ta_tmp_cl
      })
    );
  }
  return map_array( arr, typecast_cl, type ) - ({ ta_tmp_cl });
}
 
/*------------------------- random_array ---------------------------*/
/*
    Description:
      Returns an array of random values.
    Parameters:
      size - the size of the array to be returned
    Returns:
      an array with values from 0 to size-1 scrambled into a random order
    Notes:
      None.  
*/
int *
random_array( int size )
{
  int *out, i, j, tmp;
  out = allocate(size);
  for( i=0; i<size; i++ )
    out[i] = i;
  for( i=size-1; i > 0; i-- )
  {
    j = random(i+1);
    tmp = out[i];
    out[i] = out[j];
    out[j] = tmp;
  }
  return out;
}

/*------------------------------ sort_alist ------------------------------*/
/*
    Description:
      Sort multiple arrays via the same sorting pattern
    Parameters:
      arr - an array of at least 2 dimisions
      call - a closure or a string representing a function
      ob - if call is a function, the object in which the function is to
           be called
    Returns:
      A copy of arr in which all the arrays in arr have been sorted as
      if their contents were arr[0].
      ( ie. ({ ({ 3, 2, 1 }), ({ 4, 5, 6 }) }) -> 
            ({ ({ 1, 2, 3 }), ({ 6, 5, 4 }) }) )
    Notes:
      None.
*/
varargs mixed *
sort_alist( mixed *arr, mixed call, object ob )
{
  mixed *keys;
  closure cl;
  int i, size;
  if( !size = sizeof(arr) )
    raise_error( "Bad argument 1 to sort_alist()\n" );
  if( closurep(call) )
    cl = call;
  else
    cl = symbol_function( call, ob );
  if( !closurep( cl ) )
    raise_error( "Bad argument 2 to sort_alist()\n" );
  for( i=0; i<size; i++ )
    if( !pointerp(arr[i]) || sizeof(arr[i]) != sizeof(arr[0]) )
      raise_error( "Bad argument 1 to sort_alist()\n" );
  arr = transpose_array( arr );
  arr = sort_array( arr, lambda( ({ 'x, 'y }),
    ({ #'funcall, cl, ({ #'[, 'x, 0 }), ({ #'[, 'y, 0 }) }) ) );
  arr = transpose_array( arr );
  return arr;
}

/* affect the elements of an array through a closure */
private static closure affect_cl;

varargs mixed
acc_array( mixed elements, closure cl, mixed start, mixed args )
{
    if( !closurep( affect_cl ) )
    {
        affect_cl = lambda( ({ 'x, 'y, 'a, 'c }),
          ({ #'=, 'y, ({ #'apply, 'c, 'x, 'y, 'a }) }) );
    }
 
    map_array( elements, affect_cl, &start, args || ({ }), cl );
    return( start );
}
