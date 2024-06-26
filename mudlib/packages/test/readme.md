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