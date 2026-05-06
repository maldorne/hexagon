
#include <kernel.h>
#include <mud/cmd.h>
#include <mud/config.h>
#include "../../src/test.h"

inherit CMD_BASE;

#define HELP_TEXT "Run the Hexagon test suite.\n" + \
  "\n" + \
  "Usage:\n" + \
  "  test                       Run every test under /packages/test/src/.\n" + \
  "  test <file>                Run a single test file.\n" + \
  "  test <directory>           Run every test under the given directory.\n" + \
  "  test -h | --help           Show this help.\n" + \
  "  test -v | --verbose        Verbose output: show every IT() assertion\n" + \
  "                             individually (mocha style) instead of just a\n" + \
  "                             per-file PASS/FAIL line.\n" + \
  "  test -c | --show-caught    Show caught error traces inline. Useful when\n" + \
  "                             debugging an ASSERT(catch(...)) test or any\n" + \
  "                             test that legitimately raises errors. Off by\n" + \
  "                             default; the flag is scoped to this run.\n"

void setup()
{
  set_aliases(({ "test" }));
  set_usage("test [-v] [-c] [<file | directory>]");
  set_help(HELP_TEXT);
}

private void test_one(string file);
private void test_dir(string dir);

int remove_files(mixed file_info, varargs mixed args...)
{
  // only directories that don't start with a dot, and skip the
  // 'pending' subdir which holds tests not yet ready to run
  return (file_info[1] == -2)
      && (file_info[0][0] != '.')
      && (file_info[0] != "pending");
}

private void test_dir(string dir)
{
  string * files;
  mixed * subdirs;
  int i, j;

  files = get_dir(dir + "*.c");

  for (i = 0; i < sizeof(files); i++)
    test_one(dir + files[i]);

  subdirs = filter(get_dir(dir + "*", -1), "remove_files", this_object());

  for (i = 0; i < sizeof(subdirs); i++)
  {
    if (subdirs[i][0] == "fail")
    {
      // 'fail' subdir: every file in it must FAIL to compile/load.
      files = get_dir(dir + "fail/*.c");
      for (j = 0; j < sizeof(files); j++)
        ASSERT2(catch(load_object(dir + "fail/" + files[j])),
                "fail/" + files[j] + " loaded");
    }
    else
    {
      test_dir(dir + subdirs[i][0] + "/");
    }
  }
}

private void test_one(string file)
{
  string error;
  object tp, ob, hand;
  int * before, * after;
  int file_tests_attempted, file_tests_failed;
  int verbose, i, j;
  mixed * fails;
  mixed result;

  // the test handler itself is not a test
  if (file == TEST_H)
    return;

  tp = this_player();
  hand = find_object(TEST_H);
  verbose = hand->is_verbose();
  before = hand->query_stats();

  // in verbose mode, header before each file's per-assert output
  if (verbose)
    write("\n  " + file + "\n");

  BEGIN_FILE();

  // discard previous incarnation if any
  if (find_object(file))
    destruct(find_object(file));

  error = catch(ob = load_object(file));
  if (error || !ob)
  {
    write("  %^BOLD%^RED%^FAIL%^RESET%^  " + file + "\n");
    write("        load error: " +
          (error ? error : "load_object returned nil") + "\n");
    MARK_FILE_FAILED();
    return;
  }

  error = catch(result = ob->do_tests());

  if (tp != this_player())
    write("        %^BOLD%^YELLOW%^warning%^RESET%^: bad this_player() after " +
          file + "\n");

  after = hand->query_stats();
  // stats layout: ({ files_att, files_fail, tests_att, tests_fail,
  //                  asserts_att, asserts_fail })
  file_tests_attempted = after[2] - before[2];
  file_tests_failed    = after[3] - before[3];
  fails                = hand->query_file_failures();

  if (error)
  {
    if (!verbose)
      write("  %^BOLD%^RED%^FAIL%^RESET%^  " + file + "\n");
    write("        " + error);
    MARK_FILE_FAILED();
    return;
  }

  if (file_tests_failed > 0)
  {
    if (!verbose)
    {
      write("  %^BOLD%^RED%^FAIL%^RESET%^  " + file +
            " (" + file_tests_failed + "/" + file_tests_attempted +
            " tests failed)\n");

      // dump failed test titles + the lines of the asserts that failed
      for (i = 0; i < sizeof(fails); i++)
      {
        write("    %^BOLD%^RED%^x%^RESET%^ " + fails[i][0] + "\n");
        for (j = 0; j < sizeof(fails[i][1]); j++)
          write("        - assert failed at line " +
                fails[i][1][j] + "\n");
      }
    }
    MARK_FILE_FAILED();
    return;
  }

  if (!verbose)
    write("  %^BOLD%^GREEN%^PASS%^RESET%^  " + file +
          " (" + file_tests_attempted + " test" +
          (file_tests_attempted == 1 ? "" : "s") + ")\n");
}

private string format_summary(int duration)
{
  string res;
  int * stats;

  stats = find_object(TEST_H)->query_stats();
  // stats: ({ files_att, files_fail, tests_att, tests_fail,
  //           asserts_att, asserts_fail })

  res = "\n";

  res += "  Test files   ";
  if (stats[1] > 0)
    res += "%^BOLD%^RED%^" + stats[1] + " failed%^RESET%^ | ";
  res += "%^BOLD%^GREEN%^" + (stats[0] - stats[1]) +
         " passed%^RESET%^ (" + stats[0] + ")\n";

  res += "       Tests   ";
  if (stats[3] > 0)
    res += "%^BOLD%^RED%^" + stats[3] + " failed%^RESET%^ | ";
  res += "%^BOLD%^GREEN%^" + (stats[2] - stats[3]) +
         " passed%^RESET%^ (" + stats[2] + ")\n";

  res += "     Asserts   ";
  if (stats[5] > 0)
    res += "%^BOLD%^RED%^" + stats[5] + " failed%^RESET%^ | ";
  res += "%^BOLD%^GREEN%^" + (stats[4] - stats[5]) +
         " passed%^RESET%^ (" + stats[4] + ")\n";

  res += "        Time   " + duration + "s\n";

  return res;
}

static int cmd(string str, object me, string verb)
{
  string * argv;
  string target, arg;
  object hand, drv;
  int target_kind, start, i;
  int flag_help, flag_show_caught, flag_verbose;
  int previous_show_caught;

  // parse arguments
  argv = (str && strlen(str)) ? (explode(str, " ") - ({ "" })) : ({ });
  target = nil;
  flag_help = 0;
  flag_show_caught = 0;
  flag_verbose = 0;

  for (i = 0; i < sizeof(argv); i++)
  {
    arg = argv[i];

    if (arg == "-h" || arg == "--help")
      flag_help = 1;
    else if (arg == "-v" || arg == "--verbose")
      flag_verbose = 1;
    else if (arg == "-c" || arg == "--show-caught")
      flag_show_caught = 1;
    else if (strlen(arg) > 1 && arg[0] == '-')
    {
      write("Unknown flag: " + arg + "\n\n");
      write(query_help());
      return 1;
    }
    else if (target)
    {
      write("Too many arguments: " + arg + "\n\n");
      write(query_help());
      return 1;
    }
    else
      target = arg;
  }

  if (flag_help)
  {
    write(query_help());
    return 1;
  }

  hand = load_object(TEST_H);
  if (!hand)
  {
    write("Cannot load test handler.\n");
    return 1;
  }
  hand->set_verbose(flag_verbose);

  // by default, suppress caught-error inform_user()s while the suite runs
  // so the output stays clean. -c forces them on. either way we restore
  // the previous value when we finish.
  drv = find_object(DRIVER);
  previous_show_caught = drv->query_show_caught_errors();
  drv->set_show_caught_errors(flag_show_caught ? 1 : 0);

  start = time();

  if (target)
  {
    target_kind = file_exists(target);

    if (target_kind == 1)
      test_one(target);
    else if (target_kind == -1)
      test_dir(target);
    else
    {
      write("No such file or directory: " + target + "\n\n");
      write(query_help());
      drv->set_show_caught_errors(previous_show_caught);
      destruct(hand);
      return 1;
    }
  }
  else
  {
    test_dir("/packages/test/src/");
  }

  write(format_summary(time() - start));

  // restore previous caught-error state
  drv->set_show_caught_errors(previous_show_caught);

  if (hand)
    destruct(hand);

  return 1;
}
