Installation instructions
=========================

These instructions assume you have some knowledge about compiling code under a Unix enviroment (Unix, Linux, OS X, etc). **If it's not the case**, ignore all these instructions below, and just go to the [Hexagon Virtual Machine how-to document](vm/readme.md), the instructions are much shorter and, hopefully, easy to follow for a non-developer.

DGD should be able to [compile under Windows](https://github.com/dworkin/dgd/tree/master/src/host/win32) as noticed in its own documentation, but I've never tried. Any feedback or documentation about this would be appreciated.

#### Instructions

1. **Download**. Be sure the `driver/dgd` submodule is downloaded too.
   In command line, you can do both things with:

   `git clone --recursive git://github.com/houseofmaldorne/hexagon.git`
2. `cd hexagon`
3. **Compile and install the DGD driver**:
   1. `./install/install.sh`

      This script will edit certain files of the driver, and launch
      the compilation process. If you want to know, just in case, it does:
      * Allow `//` comments, uncommenting the `-DSLASHSLASH` flag
        in `dgd/src/Makefile`.
      * Change the inner working of the `restore_object` kfun, so it
        does not reset all non-static variables that do not hold object
        values when restoring the object.
        This is the default mode in MudOS.
      * Compiling and installing the executable (`make` and `make install`).
      * Creating a default `config.dgd` file.
      * Creating an empty `tmp` directory, if needed.
      * Nothing more. As a side note: if you are using a version of DGD minor
        than 1.6 (i.e. 1.5.x), you can also activate the flag
        `NETWORK_EXTENSIONS` in `dgd/src/Makefile`, to allow connections
        from the mud to the outer world. From version 1.6 this package does
        not exist anymore.
        If you use an older driver but you are not going to use things like ftp
        servers, remote channels, etc, you don't need to use the
        `NETWORK_EXTENSIONS` flag, the mudlib should work fine without it.
4. Edit the `./config.dgd` file, changing the directory field to the absolute path of the
   `mudlib` directory (must end in `/whatever/mudlib` without an ending slash)
5. **Execute** `./mud.sh`
6. `telnet localhost 6047` to connect and see if everything is working.
7. By default you will have an administrator account created, with username `admin` and password `administrator`.
