/*
    NAME

        misc.c - Miscellaneous stuff

    DESCRIPTION        

        format_objects    - Formats a list of objects for humans to read
        print_objects     - Prints the output of format_objects()

    NOTES

        None.
     
    LAST MODIFIED
        Devo 980109

*/
#include <acme.h>
#include AcmeMiscInc

/*----------------------------- format_objects ---------------------------*/
/*
    Description:
      Formats a list of objects for people to read.
    Parameters:
      olist - list of objects
      brief - if true, omits object names
    Returns:
      A string containing the formatted list.
    Notes:
      None.
*/
/*----------------------------- print_objects ---------------------------*/

varargs string
format_objects( object *olist, status brief )
{
  string out, name;
  int i, size, width;
 
  out = "";
  width = THISP->query_page_width()||78;
 
  for(i=0, size = sizeof(olist); i < size; i++)
  {
    if(objectp(olist[i]))
    {
      
      if( brief )
      {
        out += sprintf("%3d: %-.*s\n", 
               i, 
               width-5,
               compress_path(file_name(olist[i])) );      
      } 
      else 
      {
        if(interactive(olist[i]))
        {
          name = olist[i]->query_real_name();
          if(stringp(name))
            name = CAP(name);
          else
            name = "(no name)";
        }
        else
          name = (olist[i]->short()
                  || olist[i]->query("name")
                  || olist[i]->query_name()
                  || "");      
 
        out += sprintf("%3d: %-:*s %s\n", 
                 i, 
                 (width-5)/2,
                 compress_path(file_name(olist[i])), 
                 strlen( name ) > (width-5)/2 ? "\n     " + name : name
               );      
      }
    }
  }
  return out;
}

/*
    Description:
      Prints a formatted list of objects using format_objects.
    Parameters:
      olist - list of objects
      brief - if true, omits object names
    Returns:
      Nothing.
    Notes:
      None.
*/
varargs void
print_objects(object *olist, status brief)
{
  THISP->more_string(format_objects(olist, brief));
}

