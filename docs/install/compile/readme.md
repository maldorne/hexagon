Installation instructions
=========================

These instructions assume you have some knowledge about compiling code under a Unix enviroment (Unix, Linux, OS X, etc). **If it's not the case**, ignore all these instructions below, and just go to the [Hexagon Virtual Machine how-to document](/docs/install/vm/readme.md), the instructions are much shorter and, hopefully, easy to follow for a non-developer.

DGD should be able to [compile under Windows](https://github.com/dworkin/dgd/tree/master/src/host/win32) as noticed in its own documentation, but I've never tried. Any feedback or documentation about this would be appreciated.

## Instructions

### Download both the driver and the mudlib

In your directory of choice:

```sh
git clone https://github.com/maldorne/hexagon.git
git clone https://github.com/maldorne/dgd.git
```

This will create both the `dgd` and `hexagon` directories.

As you can see, the DGD project is a fork from [the original](https://github.com/dworkin/dgd), but the differences are minimal:

  * Allow `//` comments by default, uncommenting the `-DSLASHSLASH` flag
    in `dgd/src/Makefile`.
  * Change the inner working of the `restore_object` kfun, so it
    does not reset all non-static variables that do not hold object
    values when restoring the object. This is the default mode in MudOS. To change this, 
    in the file `src/kfun/file.cpp` there are some lines commented in the function `kf_restore_object`.

### Compile the driver

```sh
cd dgd/src
make
make install
cd ../..
```

This will create the `dgd` executable in the `dgd/bin` directory. Now we have to copy the configuration files to the root directory.

### Edit the configuration files

```sh
cp hexagon/docs/install/config.example.dgd config.dgd
cp hexagon/docs/install/mud.sh .
chmod a+x mud.sh
```

Now edit the `./config.dgd` file, changing the directory field to the absolute path of the `hexagon/mudlib` directory (must end in `/whatever/hexagon/mudlib` without an ending slash).

### Executing the game

- Run `./mud.sh` to execute the game.
- Use `telnet localhost 6047` to connect and see if everything is working.

By default you will have an administrator account created, with username `admin`, email `admin@maldorne.org` and password `hexagon`.
