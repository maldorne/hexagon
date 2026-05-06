
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
  "  test -c | --show-caught    Show caught error traces inline. Useful when\n" + \
  "                             debugging an ASSERT(catch(...)) test or any\n" + \
  "                             test that legitimately raises errors. Off by\n" + \
  "                             default; the flag is scoped to this run.\n"

void setup()
{
  set_aliases(({ "test" }));
  set_usage("test [-c] [<file | directory>]");
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
  int file_attempted, file_failed;
  mixed result;

  // the test handler itself is not a test
  if (file == TEST_H)
    return;

  tp = this_player();
  hand = find_object(TEST_H);
  before = hand->query_stats();

  START_TEST();

  // discard previous incarnation if any
  if (find_object(file))
    destruct(find_object(file));

  error = catch(ob = load_object(file));
  if (error || !ob)
  {
    write("  %^BOLD%^RED%^FAIL%^RESET%^  " + file + "\n");
    write("        load error: " +
          (error ? error : "load_object returned nil") + "\n");
    FAIL_TEST();
    return;
  }

  error = catch(result = ob->do_tests());

  if (tp != this_player())
    write("        %^BOLD%^YELLOW%^warning%^RESET%^: bad this_player() after " +
          file + "\n");

  after = hand->query_stats();
  file_attempted = after[2] - before[2];
  file_failed    = file_attempted - (after[3] - before[3]);

  if (error)
  {
    write("  %^BOLD%^RED%^FAIL%^RESET%^  " + file + "\n");
    write("        " + error);
    FAIL_TEST();
    return;
  }

  if (file_failed > 0)
  {
    write("  %^BOLD%^RED%^FAIL%^RESET%^  " + file +
          " (" + file_failed + "/" + file_attempted + " asserts failed)\n");
    FAIL_TEST();
    return;
  }

  write("  %^BOLD%^GREEN%^PASS%^RESET%^  " + file +
        " (" + file_attempted + " assert" +
        (file_attempted == 1 ? "" : "s") + ")\n");
}

private string format_summary(int duration)
{
  string res;
  int * stats;
  int files_total, files_passed, files_failed;
  int asserts_attempted, asserts_passed, asserts_failed;

  stats = find_object(TEST_H)->query_stats();
  files_total       = stats[0];
  files_failed      = stats[1];
  files_passed      = files_total - files_failed;
  asserts_attempted = stats[2];
  asserts_passed    = stats[3];
  asserts_failed    = asserts_attempted - asserts_passed;

  res = "\n";
  res += "  Test files   ";
  if (files_failed > 0)
    res += "%^BOLD%^RED%^" + files_failed + " failed%^RESET%^ | ";
  res += "%^BOLD%^GREEN%^" + files_passed + " passed%^RESET%^ (" +
         files_total + ")\n";

  res += "       Tests   ";
  if (asserts_failed > 0)
    res += "%^BOLD%^RED%^" + asserts_failed + " failed%^RESET%^ | ";
  res += "%^BOLD%^GREEN%^" + asserts_passed + " passed%^RESET%^ (" +
         asserts_attempted + ")\n";

  res += "        Time   " + duration + "s\n";

  return res;
}

static int cmd(string str, object me, string verb)
{
  string * argv;
  string target, arg;
  object hand, drv;
  int target_kind, start, i;
  int flag_help, flag_show_caught;
  int previous_show_caught;

  // parse arguments
  argv = (str && strlen(str)) ? (explode(str, " ") - ({ "" })) : ({ });
  target = nil;
  flag_help = 0;
  flag_show_caught = 0;

  for (i = 0; i < sizeof(argv); i++)
  {
    arg = argv[i];

    if (arg == "-h" || arg == "--help")
      flag_help = 1;
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
