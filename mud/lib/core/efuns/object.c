

#include "objects/compile_object.c"
#include "objects/find_object.c"
#include "objects/load_object.c"
#include "objects/file_name.c"
#include "objects/destruct.c"

// object new(string path)
// {
//   object ob;

//   ob = ::find_object(path);
  
//   if (!ob)
//     ob = compile_object(path);

//   return clone_object(ob);  
// }

// /secure/simul_efun/base_name.c
// from the RotD Mudlib
// returns the file name of the master copy of an object (no #???)
// created by Descartes of Borg 930822

static nomask string base_name(object ob) 
{
  string file, tmp;

  // seteuid(geteuid(previous_object()));

  if (sscanf(file_name(ob), "%s#%s", file, tmp) != 2) 
    file = file_name(ob);

  // seteuid(0);

  return file;
}

static nomask int clone_number(object ob) 
{
  string str;
  int num;

  if (!ob && !(ob = this_object())) 
    return 0;
  
  if (sscanf(object_name(ob), "%s#%d", str, num) == 2) 
    return num;

  return 0;
}
