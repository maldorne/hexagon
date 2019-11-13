Hexagon mudlib
==============

A project to make the original _ccmudlib_ and other MudOS-alike mud libraries (like the ones from _Final
Realms_ or _Discworld_) work with a modern [DGD](https://github.com/dworkin/dgd) mud driver.

**_What?_** Ok, a more detailed explanation. Text-based online games, usually called [MUDs](https://en.wikipedia.org/wiki/MUD), evolved from the [original game](https://en.wikipedia.org/wiki/MUD1) in _families_ of related games, mainly based in the technologies used to implement them. One of this big families groups the [LP-Muds](https://en.wikipedia.org/wiki/LPMud), coded in the [LPC programming language](https://en.wikipedia.org/wiki/LPC_(programming_language)). During the years, different virtual machines (usually called _drivers_ or _servers_ in the mud community) were developed to execute slightly similar _flavours_ of LPC, like LDMud, [MudOS](https://en.wikipedia.org/wiki/MudOS)/[FluffOS](https://en.wikipedia.org/wiki/FluffOS), or [DGD](https://en.wikipedia.org/wiki/Dworkin%27s_Game_Driver). After years of working in games developed over MudOS, this project started as a way of reuse all of their code using DGD.

**_Why?_** DGD has some modern approaches to the problems a _MUD driver_ tries to resolve, and it is published with a less restrictive open source [license](https://github.com/dworkin/dgd/blob/master/LICENSE). It offers less functionalities because it's not only oriented towards MUD games, so the programmer needs to code some of them by their own. This library acts as that layer.

### Current status

Next milestone *[v19.11 Anduin](docs/roadmap.md#current-milestone-v1911-anduin)*.

### More detailed information about the project
* Current project status and roadmap in the [roadmap document](docs/roadmap.md).
* Detailed compatibility information about every function in the [compatibility document](docs/compatibility.md).

### How to use or test the mudlib

Take a look to the [installation instructions](install/readme.md), it explains how to install,
configure and execute the Hexagon mudlib using different methods.
* a) [Compiling it yourself](install/readme.md): Compiling the DGD driver and
  executing it in a Unix environment (Unix, Linux, OS X, etc)
* b) [Using a virtual machine](install/vm/readme.md): Install a VirtualBox
  virtual machine already configured using vagrant.
* c) Using a Docker container (document yet to be written).


