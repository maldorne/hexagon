
#include <files/file.h>
#include <mud/secure.h>

#include "files/write_file.c"
#include "files/read_file.c"

// file_size - get the size of a file
// int file_size( string file );
// file_size() returns the size of file 'file' in bytes.  Size -1
// indicates that 'file' either does not exist, or that it is not
// readable. Size -2 indicates that 'file' is a directory.

// implementation from http://phantasmal.sourceforge.net/DGD/external/file_size.c

static nomask int file_size(string path)
{
    int i;
    mixed **dir;
    string *comps, base;

    if (path == "/")
      return -2;

    comps = explode(path, "/");
    base = comps[sizeof(comps) - 1];
    dir = get_dir(path);
    i = sizeof(dir[0]);

    while (i--)
    {
      if (dir[0][i] == base)
      {

stderr(to_string(dir));
        return dir[1][i];
      }
    }

    return -1;
}

static nomask int file_length(string file_name)
{
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
    write("No such file.\n");
    return 0;
  }

  if (i == -1)
  {
    write("That file is a directory.\n");
    return 0;
  }

  lines = full_explode(read_file(file), "\n");

  if (sizeof(lines) > MAX_CAT_LINES)
    lines = lines[0..MAX_CAT_LINES-1];

  for (i = 0; i < sizeof(lines); i++)
      write(lines[i]+"\n");

  return 1;
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

// mixed array get_dir(string dir);
// mixed array get_dir(string dir, int flag);

// If `dir' is a filename ('*' and '?' wildcards are supported), an array of
// strings is returned containing all filenames that match the specification.
// If `dir' is a directory name (ending with a slash--ie: "/u/", "/adm/", etc),
// all filenames in that directory are returned.

// If called with a second argument equal to -1, get_dir will return an array
// of subarrays, where the format of each subarray is:

//   ({ filename, size_of_file, last_time_file_touched })

// Where filename is a string and last_time_file_touched is an integer being
// number of seconds since January 1, 1970 (same format as time()).  The
// size_of_file element is the same value that is returned by file_size(); the
// size of the file in bytes, or -2 if it's a directory.

static nomask mixed * get_dir(string dir, varargs int flag)
{
  mixed ** obs;

  if (!SECURE->valid_read(dir, geteuid(), "get_dir"))
    return ({ });

  obs = ::get_dir(dir);

  // is a directory
  if (sizeof(obs[GDIR_SIZES]) && (obs[GDIR_SIZES][0] == -2))
    obs = ::get_dir(dir + "*");

  if (flag && (flag == -1))
  {
    mixed * result;
    int i;

    result = ({ });

    for (i = 0; i < sizeof(obs[GDIR_FILES]); i++)
      result += ({ ({ obs[GDIR_FILES][i], obs[GDIR_SIZES][i], obs[GDIR_TIMES][i] }) });

    return result;
  }

  return obs[GDIR_FILES];
}

