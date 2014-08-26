ccmudlib
========

A project to make the original [ccmudlib](http://www.ciudadcapital.net) 
and other MudOS-alike mud libraries (like the ones from _Final 
Realms_ or _Discworld_) work with a modern DGD mud driver.

### How to use
1. Download. Be sure the `driver/dgd` submodule is downloaded too.
2. Compile DGD (see below).
3. Copy the file `driver/config.example.dgd` to `driver/config.dgd`
4. Edit it, changing the directory field to the absolute path of the 
   mud directory (must end in `/whatever/mud` without an ending slash)
5. `sh mud.sh`
6. `telnet localhost 6047` to connect and see if everything is working.

### How to compile the DGD driver
1. Copy the contents of `driver/dgd.Makefile` to `driver/dgd/src/Makefile`
   (the only change is to include the `SLASHSLASH` flag to allow this 
   kind of `// comments` in the mud code).
2. `cd driver/dgd/src`
3. `make`
4. `make install`
5. It's done!

