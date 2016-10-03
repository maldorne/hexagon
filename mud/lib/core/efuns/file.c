
#include <files/file.h>

// Non-dgd efuns
// neverbot, 03/2014

// implementation from http://phantasmal.sourceforge.net/DGD/external/file_size.c

static nomask int file_size(string path)
{
    int i;
    mixed **dir;
    string *comps, base;

    if (path == "/")
    {
      return -2;
    }

    comps = explode(path, "/");
    base = comps[sizeof(comps) - 1];
    dir = get_dir(path);
    i = sizeof(dir[0]);

    while (i--) 
    {
      if (dir[0][i] == base) 
      {
        return dir[1][i];
      }
    }
    return -1;
}

// TODO file_length

// This is a very temporary hack.  file_length() seems to crash and burn
//   with directories.  So let's wrap it...   -- Hamlet 
static nomask int file_length(string file_name) 
{
  // int tmp;

  // tmp = file_size(file_name);
  // if(tmp < 1)
  //   return tmp;

  // return ::file_length(file_name);

  return file_size(file_name);
}



//  /secure/simul_efun/file_exists.c
//  from the RotD Mudlib
//  returns true if the file named exists
//  created by Descartes of Borg 930822

// int file_exists(string str) {
//     int ret;
//     // seteuid(geteuid(previous_object()));
//     if (file_size(str) > -1) 
//       ret = 1;
//     else 
//       ret = 0;
//     // seteuid(0);
//     return ret;
// }

static nomask int file_exists(string str) 
{
  mixed * content;
  int * sizes;

  content = get_dir(str);
  sizes = content[1];
  
  // no files
  if (!sizes || (sizeof(sizes) == 0)) 
    return 0;

  if (sizes[0] == -2) 
    return -1;

  return 1;
}

// Stuff required as a result of MudOS upgrades..
// Will proliferate into other simul_efuns later
// 6 Oct 1993 Chrisy

// int cat(string file, int start, varargs int num)
// {
//   string s;
//   object me;
//   // Fix by wonderflug.
//   if ( this_player() ) 
//     me = this_player();
//   else
//     me = previous_object();
//   // if(!"/lib/core/master.c"->valid_read(file, geteuid(me), "read_file") || 
//   //   file_length(file) <= 0)
//   //   return 0;
//   if (!num)  
//    num = file_length(file);
//   s = read_file(file, start, num);
//   if (me->query_player())
//     s = me->fix_string(s);
//   else
//     s = "/lib/core/events.c"->fix_string(s);
//   if(!s)
//     return 0;
//   write(s+"\n");
//   return 1;
// }

static nomask int cat(string file) 
{
  int i;
  string * lines;

  i = file_exists(file);
  if (i == 0) 
  {
    write ("No such file.\n");
    return 0;
  }

  if (i == -1) 
  {
    write ("That file is a directory.\n");
    return 0;
  }

  lines = explode(read_file(file), "\n");

  if (sizeof(lines) > MAX_CAT_LINES) 
    lines = lines[0..MAX_CAT_LINES-1];
  
  for (i = 0; i < sizeof(lines); i++) 
      write (lines[i]+"\n");

  return 1;
}

static nomask mixed creator_file(string file, varargs int author) 
{
  string *str;

  if (!file || !stringp(file)) 
    return "noone";
  
  str = explode(file, "/") - ({ "" });
  if (sizeof(str) < 2) 
    return get_root_uid();
  
  switch (str[0]) 
  {
    case "lib" :
      return get_root_uid();
    case "net" :
      // return "Network stuff";
      return "network";

    case "game" :
      if (sizeof(str) < 3)
        return get_root_uid();

      //if (!author)
        return capitalize(str[2]);
     
     // return ("/d/"+str[1]+"/master")->author_file(str);

    case "tmp" :
     return "tmp_files";

    case "home" :
      if (sizeof(str) < 3)
        return get_root_uid();
      return str[1];

    default:
      return get_root_uid();    
  }
}

static nomask mixed author_file(mixed bing) 
{
  return creator_file(bing, 1);
}


static nomask int rename(string from, string to)
{
  return rename_file(from, to);
}

static nomask int rm(string name)
{
  return remove_file(name);
}

static nomask int mkdir(string dir)
{
  return make_dir(dir);
}

static nomask int rmdir(string dir)
{
  return remove_dir(dir);
}

// int cp(string src, string dst);
// Copies the file 'src' to the file 'dst'.
// Returns 1 for success, returns -1 if the first src is unreadable, -2 if
// dst is unreadable, and -3 if an i/o error occurs.

// TODO cp check if this is working

static nomask int cp(string src, string dst)
{
  mixed chunk;
  int offset, sz, n;
  mixed *info;

  // get_file returns:
  // ({ ({ file names }), ({ file sizes }), ({ file mod times }) })

  info = get_dir(src);

  // file does not exist
  if (sizeof(info[0]) == 0)
    return -1;

  // is a directory
  if (sizeof(info[2]) && (info[2][0] < 0))
    return -1;

  offset = 0;
  sz = info[2][0];

  do {
    chunk = read_file(src, offset, 57344);

    // some error, should not happen
    if (typeof(chunk) != T_STRING) 
      return -1;

    n = write_file(dst, chunk);

    if (n <= 0) 
    {
      // should not happen
      return -1;
    }

    offset += strlen(chunk);
    sz -= strlen(chunk);

  } while (sz > 0 && strlen(chunk) != 0);

  return 1;
}
