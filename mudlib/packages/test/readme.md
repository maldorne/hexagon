# Test Suite 

Based on a test suite for the MudOS driver, this package provides a set of tests for the Hexagon mudlib working on the DGD driver. The tests are intended to be run from within the mudlib, and are written in LPC. 

## Links

Can also be found packed with a minimal mudlib called Lil in [the downloads section of lpmuds.net](http://www.lpmuds.net/downloads.html).

An updated version for FluffOS 2019 can be found at [MudRen](https://github.com/MudRen/testsuite).

## Original Readme

This directory contains a version of the Lil mudlib with additional code
added to provide a testsuite for MudOS.  Currently the testsuite is small
but hopefully it will grow with time (assuming the MudOS developers add
to it).  The efun tests are in the ./single/efuns directory with one efun
tested per file (named after the efun).  Each efun test should attempt to tell
all (each case/branch) of the driver code that implements that particular
efun.  In addition, the efun tests should attempt to fully test the
functionality of the efun (including boundary conditions).  Eventually,
tests will be added that ensure all of the eoperators are tested as well.
Currently the only way setup to invoke the tests is to login to Lil
and type 'efun efun_name' e.g. 'efun capitalize'.  Eventually there will
be a way to invoke the tests in batch mode via the driver's -f flag.

--

This directory contains a very minimal mudlib named Lil intended for use in
bootstrapping a mudlib to be built from scratch.  Anyone who has attempted
to build a mudlib from the ground up knows that the most painful part is
getting the mudlib to the point where the driver will boot up and accept
logins.  After this point, its much easier to debug objects because the
driver is actually available to compile the objects and provide feedback
about what might be wrong!  Lil is provided to allow that first painful
step to be avoided (useful learning experience though it may be).  Also,
we felt that providing a driver that won't run out of the box (and it won't
run without a mudlib) is a bit silly.  Yet we didn't want to provide a full
mudlib because we'd rather see many different mudlibs rather than just
variations on a single mudlib (also, there are several complete
MudOS mudlibs available and there isn't any good way to choose which
one we would include).

## Updates

Conversion to Hexagon being done by neverbot (when I have time), 2023/2024...

The original Lil-style runner has been substantially rewritten for
Hexagon. Below is a running log of what's been changed; expect this
section to keep growing as the package keeps diverging from its origin.

### Test runner (`cmds/admin/test.c`)

- **Mocha-style output.** Each file emits a single `PASS` or `FAIL`
  line with the count of *tests* (not asserts) it contains; failed
  tests are listed indented underneath.
- **CLI flags**:
  - `-h` / `--help` — show the usage text (same as `help test`).
  - `-v` / `--verbose` — switch to per-test live output: every
    `IT`/`TEST` is shown as it runs, with a green `+` for pass and a
    red `x` for fail, plus the line numbers of any failed asserts.
  - `-c` / `--show-caught` — re-enable caught-error traces inline.
    By default the runner suppresses them so the output stays clean
    even for tests that legitimately use `ASSERT(catch(...))`. The
    flag is scoped to a single run; the previous global state of
    `SHOW_CAUGHT_ERRORS` is restored on exit.
- **Pending tests skipped.** Anything under
  `src/efuns/pending/` is excluded from the run via the `remove_files`
  filter. Files there are tracked in git but won't run until you move
  them up to `src/efuns/`.
- **Better failure detection.** The runner now diffs the test/assert
  counters before and after each file, so a `do_tests()` that
  *silently* records a failed `ASSERT` (no thrown error) is correctly
  reported as `FAIL`. The legacy implementation only marked a file as
  failed if `do_tests()` actually threw.

### Test handler (`src/tests.c`)

Three counter levels, mirroring mocha's mental model:

| level     | what it counts                                                                    |
|:----------|:----------------------------------------------------------------------------------|
| `files`   | each `.c` file the runner processes                                               |
| `tests`   | logical units (one per `TEST(...)` block, one per `IT(...)`)                      |
| `asserts` | raw boolean checks (every `ASSERT(...)`, including the ones inside `TEST` blocks) |

A test fails if any of the asserts inside it fails; a file fails if
any of its tests fails (or if `do_tests()` throws, or the file fails
to load). The handler also exposes a verbose-mode flag and a
per-file failure log (cleared at the start of each file) used by the
runner to dump failed test titles + line numbers.

### Macros (`src/test.h`)

- `TEST("title");` ... asserts ... `END_TEST();` — group multiple
  asserts under a single titled test (the `it("...", () => {...})`
  shape from mocha). The test passes only if every assert inside
  passes.
- `IT("title", expr);` — sugar for one-liner tests; expands to
  `TEST + ASSERT + END_TEST`. Use this for the common case of a single
  boolean check per test.
- `ASSERT(x);` — boolean check. Inside a `TEST` block, a failure
  marks the enclosing test as failed and remembers the line number
  for reporting; outside any `TEST`, it still bumps the assert
  counter but is otherwise inert.
- `ASSERT2(x, reason);` — kept for the `fail/` subdir (files that
  must fail to load); writes the reason inline on failure.
- `EQUALS(a, b);` — deep value equality for aggregates. LPC `==` on
  arrays and mappings compares by reference, so use `EQUALS` when
  you need to compare two distinct aggregates by structure: it
  recursively walks arrays (same size, same elements in order) and
  mappings (same keys, same values), and falls back to `==` on
  scalars. Typical use: `ASSERT(EQUALS(json_decode(s), ({ 1, 2 })))`.
- All macros are wrapped in `do { ... } while (0)` so they expand to
  a single statement and can safely be used as the body of a
  brace-less `for`/`if`.

### Conventions for new tests

- Wrap each logical assertion in `IT` or in a `TEST`/`END_TEST`
  block. A loop that performs many sub-checks should live inside one
  `TEST` rather than producing N independent titled assertions.
- Title each test from the user/caller's perspective ("does X when
  Y"), not in terms of the implementation.
- Files that are not yet ready live under `src/efuns/pending/` and
  are excluded from the runner. Promote a file by moving it up one
  directory, no other action required.