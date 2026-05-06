
#include <living/living.h>

// do not remove the .c, will be compared with the results of get_dir
#define TEST_H "/packages/test/src/tests.c"

// File-level markers driven by the runner (cmds/admin/test.c) — not for
// use inside individual test files.
#define BEGIN_FILE()        find_object(TEST_H)->begin_file()
#define MARK_FILE_FAILED()  find_object(TEST_H)->mark_file_failed()

// Group a related batch of ASSERTs under a single titled test (mocha's
// `it("...", () => { ... })`). The test passes only if every ASSERT
// inside passes; the group is what counts towards the "Tests" total.
//
//   TEST("allocate(N) returns size N for 0 <= N < 10");
//     for (i = 0; i < 10; i++) ASSERT(sizeof(allocate(i)) == i);
//   END_TEST();
#define TEST(title)  find_object(TEST_H)->begin_test(title)
#define END_TEST()   find_object(TEST_H)->end_test()

// One-shot titled test: TEST + single ASSERT + END_TEST in one line.
//   IT("allocate(-1) raises an error", catch(allocate(-1)));
#define IT(title, x) do { TEST(title); ASSERT(x); END_TEST(); } while (0)

// Boolean check. When called inside a TEST block, a failure marks the
// enclosing test as failed (and remembers the line for reporting).
// When called outside any TEST block, it still counts toward the
// "Asserts" total but does not affect the test count.
//
// Wrapped in do {...} while (0) so the macro is a single statement and
// can be used as the body of a brace-less for/if without splitting.
#define ASSERT(x) do { \
    object _h; \
    _h = find_object(TEST_H); \
    if (x) { \
      _h->note_assert(1); \
    } else { \
      mixed _trace; \
      _trace = call_trace(); \
      _h->note_assert(0, _trace[sizeof(_trace) - 1][3]); \
    } \
  } while (0)

// ASSERT2(x, reason) — used by the runner for the special fail/ subdir
// (files that must fail to load). Untitled, prints the reason on
// failure. Untouched by the new TEST/IT semantics on purpose.
#define ASSERT2(x, r) do { \
    if (!(x)) { \
      write("%^BOLD%^RED%^" + file_name(this_object()) + ":" + r + \
            ", Check failed.%^RESET%^\n"); \
    } \
  } while (0)

#define ADD_LIVING(x)    set_living_name(x)
#define REMOVE_LIVING()  LIVING_HANDLER->remove_living(this_object())
