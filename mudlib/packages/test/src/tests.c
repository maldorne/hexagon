/* 
 * This is the handler for the issue tracker. 
 */

inherit "/lib/core/object.c";

int num_tests_attempted;
int num_tests_failed;
int num_asserts_attempted;
int num_asserts_ok;


void create()
{
  ::create();

  num_tests_attempted = 0;
  num_tests_failed = 0;

  num_asserts_attempted = 0;
  num_asserts_ok = 0;
}

int add_test()
{
  return ++num_tests_attempted;
}

int fail_test()
{
  return ++num_tests_failed;
}

int start_assert()
{
  return ++num_asserts_attempted;
}

int end_assert()
{
  return ++num_asserts_ok;
}

int * query_stats()
{
  return ({ 
            num_tests_attempted, 
            num_tests_failed, 
            num_asserts_attempted, 
            num_asserts_ok 
          });
}
