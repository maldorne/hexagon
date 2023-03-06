
#include <mud/cmd.h>
#include "../../src/test.h"

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "test" }));
  set_usage("test [ <file | directory> ]");
  set_help("Execute a test suite.");
}

int main(string fun);

int remove_files(mixed file_info, varargs mixed args...)
{
  // only directories that don't start with a dot
  return (file_info[1] == -2) && (file_info[0][0] != '.');
}

int recurse(string dir) 
{
  mixed leaks;
  string * files;
  mixed * subdirs;
  int i, j;

  files = get_dir(dir + "*.c");
  
  for (i = 0; i < sizeof(files); i++)
  main(dir + files[i]);
  
  subdirs = filter(get_dir(dir + "*", -1), "remove_files", this_object());

  for (i = 0; i < sizeof(subdirs); i++)
  {
    if (subdirs[i][0] == "fail") 
    {
      files = get_dir(dir + "fail/*.c");

      for (j = 0; j < sizeof(files); j++)
      {
        ASSERT2(catch(load_object(dir + "fail/" + files[j])), "fail/" + files[j] + " loaded");

#if defined(__DEBUGMALLOC_EXTENSIONS__) && defined(__CHECK_MEMORY__)
        leaks = check_memory();
        if (sizeof(filter(explode(leaks, "\n"), (: $1 && $1[0] :))) != 1) 
        {
          write("After trying to compile: " + dir + "fail/" + fn + "\n");
          write(leaks);
          error("LEAK\n");
        }
#endif
      }
      // rm("/log/compile");
    } 
    else
    {
      recurse(dir + subdirs[i][0] + "/");
    }
  }

  return 1;
}

int main(string file)
{
  string leaks, error;
  object tp, ob;
  mixed result;

  tp = this_player();
  
  write("Testing: " + file + "...\n");

  // if (!file || !strlen(file) || file == "") 
  // {
  //   recurse("/single/tests/");
  //   write("Checks succeeded.\n");
  //   return 1;
  // }

  // if the test file is already loaded, we need to discard it first
  if (find_object(file))
    destruct(find_object(file));

  error = catch(ob = load_object(file));
  if (error) return 0;
  
  error = catch(result = ob->do_tests());
  if (error) 
  {
    write("Errors catched.\n");
    return 0;
  }

  if (tp != this_player())
    error("Bad this_player() after calling " + file + "\n");

#if defined(__DEBUGMALLOC_EXTENSIONS__) && defined(__CHECK_MEMORY__)

  leaks = check_memory();
  
  if (sizeof(filter(explode(leaks, "\n"), (: $1 && $1[0] :))) != 1) 
  {
    write("After calling: " + file + "\n");
    write(leaks);
    error("LEAK\n");
  }

#endif

  return 1;
}

static int cmd(string str, object me, string verb)
{
  int result;

  if (strlen(str))
  {
    result = file_exists(str);

    // it is a file
    if (result == 1)
      return main(str);
    // it is a directory
    if (result == -1)
      return recurse(str);

    write("Syntax: test [ <file | directory> ]\n");
    return 1;
  }

  return recurse("/packages/test/src/");
}
