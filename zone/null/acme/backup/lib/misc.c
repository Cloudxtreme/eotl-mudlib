
string
format_objects(object *olist)
{
  string out, name;
  int i, size, count;

  out = "";
  
  for(i=0, size = sizeof(olist); i < size; i++)
  {
    if(objectp(olist[i]))
    {
      count++;
      
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
		      
      out += sprintf("%2d: %-40s %s\n", i, file_name(olist[i]), name);      
    }
  }
//  out += sprintf("Printed %d of %d objects.\n", count, size);  
  return out;
}

void
print_objects(object *olist)
{
  THISP->more_string(format_objects(olist));
}
