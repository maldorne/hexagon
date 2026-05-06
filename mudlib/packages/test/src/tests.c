/*
 * This is the handler for the test suite.
 *
 * Three counter levels (mocha-inspired):
 *
 *  - files   : per .c file the runner processes.
 *  - tests   : logical units, opened with TEST(title) / closed with
 *              END_TEST(). A test fails if any ASSERT inside it fails.
 *              IT(title, expr) is sugar for TEST + ASSERT + END_TEST.
 *  - asserts : raw boolean checks. A failed assert always marks the
 *              enclosing test as failed (if any).
 */

inherit "/lib/core/object.c";

int num_files_attempted;
int num_files_failed;
int num_tests_attempted;
int num_tests_failed;
int num_asserts_attempted;
int num_asserts_failed;

// runtime presentation flag set by the test command (-v / --verbose)
int verbose_mode;

// in-flight test state (set by begin_test, cleared by end_test)
string _test_title;
int    _test_failed;
int *  _test_failed_lines;

// per-file failure log of completed-but-failed tests. Each entry is
// ({ title, ({ line, line, ... }) }). Cleared by begin_file().
mixed * file_failures;


void create()
{
  ::create();

  num_files_attempted = 0;
  num_files_failed = 0;
  num_tests_attempted = 0;
  num_tests_failed = 0;
  num_asserts_attempted = 0;
  num_asserts_failed = 0;

  verbose_mode = 0;
  _test_title = nil;
  _test_failed = 0;
  _test_failed_lines = ({ });
  file_failures = ({ });
}

// ----- file-level ---------------------------------------------------

void begin_file()
{
  num_files_attempted++;
  // any in-flight test from a previous (crashed) file is dropped
  _test_title = nil;
  _test_failed = 0;
  _test_failed_lines = ({ });
  file_failures = ({ });
}

void mark_file_failed()
{
  num_files_failed++;
}

mixed * query_file_failures()
{
  return file_failures;
}

// ----- test-level ---------------------------------------------------

void begin_test(string title)
{
  num_tests_attempted++;
  _test_title = title;
  _test_failed = 0;
  _test_failed_lines = ({ });
}

void end_test()
{
  if (_test_failed)
  {
    num_tests_failed++;
    file_failures += ({ ({ _test_title, _test_failed_lines }) });

    if (verbose_mode)
    {
      int i;
      write("    %^BOLD%^RED%^x%^RESET%^ " + _test_title + "\n");
      for (i = 0; i < sizeof(_test_failed_lines); i++)
        write("        - assert failed at line " +
              _test_failed_lines[i] + "\n");
    }
  }
  else if (verbose_mode)
  {
    write("    %^BOLD%^GREEN%^+%^RESET%^ " + _test_title + "\n");
  }

  _test_title = nil;
  _test_failed = 0;
  _test_failed_lines = ({ });
}

// ----- assert-level -------------------------------------------------

void note_assert(int passed, varargs int line)
{
  num_asserts_attempted++;
  if (passed)
    return;

  num_asserts_failed++;

  // if we're inside a TEST block, attribute the failure to it
  if (_test_title)
  {
    _test_failed = 1;
    _test_failed_lines += ({ line });
  }
}

// ----- queries ------------------------------------------------------

int * query_stats()
{
  return ({
            num_files_attempted,
            num_files_failed,
            num_tests_attempted,
            num_tests_failed,
            num_asserts_attempted,
            num_asserts_failed
          });
}

int  is_verbose()           { return verbose_mode; }
void set_verbose(int flag)  { verbose_mode = flag; }
