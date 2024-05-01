
#include <living/living.h>

// do not remove the .c, will be compared with the results of get_dir
#define TEST_H "/packages/test/src/tests.c"

#define START_TEST() find_object(TEST_H)->add_test()
#define FAIL_TEST() find_object(TEST_H)->fail_test()
#define START_ASSERT() find_object(TEST_H)->start_assert()
#define END_ASSERT() find_object(TEST_H)->end_assert()

#define ASSERT(x) START_ASSERT();if (!(x)) { mixed t;t=call_trace();write("%^BOLD%^RED%^"+file_name(this_object()) + ": Check failed on line "+t[sizeof(t)-1][3]+".%^RESET%^\n"); }END_ASSERT();
#define ASSERT2(x, r) if (!(x)) { write("%^BOLD%^RED%^"+file_name(this_object()) + ":" + r + ", Check failed.%^RESET%^\n"); }

#define ADD_LIVING(x) set_living_name(x)
#define REMOVE_LIVING() LIVING_HANDLER->remove_living(this_object())
