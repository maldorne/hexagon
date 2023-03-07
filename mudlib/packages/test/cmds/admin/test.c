
#include <mud/cmd.h>
#include <mud/config.h>
#include "../../src/test.h"

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "test" }));
  set_usage("test [ <file | directory> ]");
  set_help("Execute a test suite.");
}

private int main(string fun);

int remove_files(mixed file_info, varargs mixed args...)
{
  // only directories that don't start with a dot
  return (file_info[1] == -2) && (file_info[0][0] != '.');
}

private int recurse(string dir) 
{
  mixed leaks;
  string * files;
  mixed * subdirs;
  int i, j;

  write("Testing directory: " + dir + "\n");

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

private int main(string file)
{
  string leaks, error;
  object tp, ob;
  mixed result;

  tp = this_player();

  // there are no tests in the test handler
  if (file == TEST_H)
    return 1;

  write("%^BOLD%^Testing file: " + file + " ...%^RESET%^\n");
  
  // increment test counters
  START_TEST();

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
  if (error || !ob) 
  {
    write("%^BOLD%^RED%^File do not load.%^RESET%^\n");
    FAIL_TEST();
    return 1;
  }
  
  error = catch(result = ob->do_tests());
  
  if (error) 
  {
    write("%^BOLD%^Warning: error in test catched, won't continue with this file tests.%^RESET%^\n");
    FAIL_TEST();
  }

  if (tp != this_player())
    write("%^BOLD%^RED%^Bad this_player() after file: " + file + "%^RESET%^\n");

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

private string show_stats()
{
  string res;
  int * stats;

  res = "\n";
  stats = find_object(TEST_H)->query_stats();

  res += "Total test files: " + stats[0] + "\n";
  if (stats[1] > 0)
    res += "%^BOLD%^RED%^Total test files failed: " + stats[1] + "%^RESET%^\n";

  res += "Total asserts attempted: " + stats[2] + "\n";
  res += "Total asserts passed: " + stats[3] + "\n";
  
  if (stats[2] != stats[3])
    res += "%^BOLD%^RED%^Total asserts failed: " + (stats[2]-stats[3]) + "%^RESET%^\n";

  return handler("frames")->frame(res, "Test results");
}

static int cmd(string str, object me, string verb)
{
  int result;
  object hand;

  hand = load_object(TEST_H);

  if (!hand)
  {
    write("Cannot load test handler.\n");
    return 1;
  }

  if (LOG_CAUGHT_ERRORS)
    write("%^BOLD%^Warning: LOG_CAUGHT_ERRORS is set to TRUE, test results will be full of catch traces.%^RESET%^\n");

  if (strlen(str))
  {
    result = file_exists(str);

    // it is a file
    if (result == 1)
      main(str);
    // it is a directory
    else if (result == -1)
      recurse(str);
    else 
      write("Syntax: test [ <file | directory> ]\n");
  }
  else
  {
    recurse("/packages/test/src/");
  }

  write("\n" + show_stats());
  
  if (hand)
    destruct(hand);
  
  return 1;
}
