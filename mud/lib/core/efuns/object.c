
// object new(string path)
// {
//   object ob;

//   ob = ::find_object(path);
  
//   if (!ob)
//     ob = compile_object(path);

//   return clone_object(ob);  
// }

string file_name(object ob)
{
  if (!ob) 
    ob = this_object();
  return object_name(ob);
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

static nomask object load_object(string name) 
{
  object obj;
  
  // if name has a trailing .c, let's get rid of it.
  if (strlen(name) > 2 && name[strlen(name)-2..strlen(name)-1] == ".c") 
    name = name[0..strlen(name)-3];

  stderr(" - load_object: " + name + " from: "+previous_program()+"\n");

  obj = find_object(name);
  
  if (!obj)
    obj = compile_object(name);
  
  return obj;
}

// find_object - find an object by file name
// object find_object( string str );
// Find the object with the file name 'str'.  If the object is a
// cloned object, then it can be found using the file name which
// would by returned if file_name() was called with it as the
// argument.

static nomask object find_object(string name)
{
  object obj;

  // if name has a trailing .c, let's get rid of it.
  if (strlen(name) > 2 && name[strlen(name)-2..strlen(name)-1] == ".c") 
    name = name[0..strlen(name)-3];

  obj = ::find_object(name);

  if (!obj)
    stderr(" - find_object: <" + name + "> (not found)\n");
  
  return obj;
}

// as the driver object has its own destruct object, this
// cannot be part of the efuns general files. This function is included
// inside the auto object in /lib/core/auto.c

/*
int destruct(varargs object ob) 
{
  ...
}
*/
