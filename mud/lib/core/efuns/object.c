
// TODO tmp, borrar esto de aqui
static nomask void stderr(string str);


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

//      /secure/simul_efun/base_name.c
//      from the RotD Mudlib
//      returns the file name of the master copy of an object (no #???)
//      created by Descartes of Borg 930822

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


// static nomask object find_object(string path)
// {
//   return ::find_object(path);
// }

static nomask object load_object(string name) 
{
  object obj;
  
  // If name has a trailing .c, let's get rid of it.
  if (strlen(name) > 2 && name[strlen(name)-2..strlen(name)-1] == ".c") 
    name = name[0..strlen(name)-3];

  stderr(" - load_object: " + name + " from: "+previous_program()+"\n");

  obj = find_object(name);
  
  if (!obj)
    obj = compile_object(name);
  else
    stderr(" - find_object (found): <" + object_name(obj) + ">\n");
  
  return obj;
}

// int destruct(varargs object ob) 
// {
//   if (!ob)
//     ob = this_object();

//   destruct_object(ob);
  
//   return TRUE;
//   // return (this_object() == nil);
// }




