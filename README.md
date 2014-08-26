ccmudlib
========

A project to make MudOS-alike mud libraries (like the ones from Final 
Realms/Discworld) to work with a modern DGD mud driver.


## How to compile the DGD Driver
1. Copy the contents of `driver/dgd.Makefile` to `driver/dgd/src/Makefile`
2. `cd driver/dgd/src`
3. `make`
4. `make install`
5. It's done!


## How to use
1. Download. Be sure the driver/DGD submodule is download too.
2. Compile DGD (see below).
3. Copy the file `driver/config.example.dgd` to `driver/config.dgd`
4. Edit it changing the directory field to the absolute path of the 
   mud directory (must end in `/whatever/mud` without the slash)
5. `sh mud.sh`
6. `telnet localhost 6047` to connect and see if everything is working.
