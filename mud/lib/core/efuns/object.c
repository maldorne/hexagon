
#include <type.h>

#include "objects/compile_object.c"
#include "objects/find_object.c"
#include "objects/load_object.c"
#include "objects/file_name.c"

#include "objects/restore_object.c"
#include "objects/save_object.c"


// object new(string path)
// {
//   object ob;

//   ob = ::find_object(path);
  
//   if (!ob)
//     ob = compile_object(path);

//   return clone_object(ob);  
// }


// clonep - determine whether or not a given variable points to a cloned object
// int clonep();
// int clonep(mixed arg);

// Returns true (1) iff the argument is objectp() ... 
// The clonep() efun will not return true when called on
// objects that are the blueprint copy (those that are loaded via call_other()
// or load_object()).

// Note that if clonep() returns true, then file_name() will return a string
// containing a '#'.  clonep() defaults to this_object().

static nomask int clonep(varargs mixed what)
{
  if (!what)
    what = this_object();

  if (typeof(what) != T_OBJECT)
    return 0;

  return (strsrch(object_name(what), "#") >= 0);
}        

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
