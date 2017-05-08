Hexagon mudlib
==============

A project to make the original [ccmudlib](http://www.ciudadcapital.net) 
and other MudOS-alike mud libraries (like the ones from _Final 
Realms_ or _Discworld_) work with a modern [DGD](https://github.com/dworkin/dgd) mud driver.

### More detailed information about the project
* Current project status and roadmap in the [status document](status.md).
* Detailed compatibility information about every function in the [compatibility document](status_compat.md).

### How to use or test the mudlib

These instructions assume you have some knowledge about compiling code under a Unix enviroment (Unix, Linux, OS X, etc). If that is not the case, I recommend to follow the instructions about [How to install the environment in a virtual machine](install_in_vm.md) instead, which are suitable for any operating system (including Windows).

DGD should be able to [compile under Windows](https://github.com/dworkin/dgd/tree/master/src/host/win32) as noticed in its own documentation, but I've never tried. Any feedback or documentation about this would be appreciated.

#### Instructions

1. Download. Be sure the `driver/dgd` submodule is downloaded too.
   In command line, you can do both things with:

   `git clone --recursive git://github.com/houseofmaldorne/hexagon.git`
2. `cd hexagon`   
2. **Compile the DGD driver**:
   1. `cd driver/dgd/src`
   2. Edit the file `Makefile` and change the line

      `DEFINES=-D$(HOST)       # -DSLASHSLASH -DNETWORK_EXTENSIONS -DNOFLOAT -DCLOSURES -DCO_THROTTLE=50`

      with

      `DEFINES=-D$(HOST) -DSLASHSLASH # -DNETWORK_EXTENSIONS -DNOFLOAT -DCLOSURES -DCO_THROTTLE=50`

      The only change is to include the `SLASHSLASH` flag to allow this 
      kind of `// comments` in the mud code.

   3. `make`
   4. `make install`
   5. `cd ../../..`
   6. Nothing more. As a side note: if you are using a version of DGD minor 
      than 1.6 (i.e. 1.5.x), you can also activate the flag `NETWORK_EXTENSIONS`
      in step number two, to allow connections from the mud to the outer world. 
      From version 1.6 this package does not exist anymore.

      If you use an older driver but you are not going to use things like ftp 
      servers, remote channels, etc, you don't need to use the `NETWORK_EXTENSIONS` 
      flag, the mudlib should work fine without it.

3. Copy the file `driver/config.example.dgd` to `driver/config.dgd`

   `cp driver/config.example.dgd driver/config.dgd`
4. Edit it, changing the directory field to the absolute path of the 
   `mud` directory (must end in `/whatever/mud` without an ending slash)
5. Create the `tmp` directory to store the swap file if it does not exist:

   `mkdir driver/dgd/tmp`
5. Execute `./mud.sh`
6. `telnet localhost 6047` to connect and see if everything is working.
7. By default you will have an administrator account created, with username `admin` and password `admin`.

