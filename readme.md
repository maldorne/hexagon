<p align="center">
  <img width="300" alt="hexagon logo" src="/assets/hexagon_logo.png">
</p>

Hexagon mudlib
==============

[![Hexagon](https://img.shields.io/badge/Hexagon-v20.09%20Anduin-yellow)](https://github.com/houseofmaldorne/hexagon)
[![DGD](https://img.shields.io/badge/DGD-1.6.6-green.svg)](https://github.com/dworkin/dgd)
[![Deploy to hosting](https://github.com/houseofmaldorne/hexagon/workflows/Deploy%20to%20hosting/badge.svg)](https://github.com/houseofmaldorne/hexagon/actions)

A project to make the original _ccmudlib_ and other MudOS-alike mud libraries (like the ones from _Final
Realms_ or _Discworld_) work with a modern [DGD](https://github.com/dworkin/dgd) mud driver.

**_What?_** Ok, a more detailed explanation. Text-based online games, usually called [MUDs](https://en.wikipedia.org/wiki/MUD), evolved from the [original game](https://en.wikipedia.org/wiki/MUD1) in _families_ of related games, mainly based in the technologies used to implement them. One of these big families groups the [LP-Muds](https://en.wikipedia.org/wiki/LPMud), coded in the [LPC programming language](https://en.wikipedia.org/wiki/LPC_(programming_language)). During the years, different virtual machines (usually called _drivers_ or _servers_ in the mud community) were developed to execute slightly similar _flavours_ of LPC, like LDMud, [MudOS](https://en.wikipedia.org/wiki/MudOS)/[FluffOS](https://en.wikipedia.org/wiki/FluffOS), or [DGD](https://en.wikipedia.org/wiki/Dworkin%27s_Game_Driver). After years of working in games developed over MudOS, this project started as a way of reuse all of that code using DGD.

**_Why?_** DGD has some modern approaches to the problems a _MUD driver_ tries to solve, and it is published with a less restrictive open source [license](https://github.com/dworkin/dgd/blob/master/LICENSE). It offers less functionalities because it's not only oriented towards MUD games, so the programmer needs to code some of them by their own. This library acts as that layer.

### Current status

Current version *[v21.09 Beregost](https://github.com/houseofmaldorne/hexagon/releases/tag/21.09)*.

Next milestone *[v21.?? Castle Black](docs/roadmap.md#current-milestone-v21-castle-black)*.

### More detailed information about the project
* Current project status and roadmap in the [roadmap document](docs/roadmap.md).
* Detailed compatibility information with every MudOS function (its _efuns_) in the [compatibility document](docs/compatibility.md).
* Compatibility about every command from old mudlibs in the [commands document](docs/commands.md).

### How to use or test the mudlib

Take a look to the [installation instructions](install/readme.md), it explains how to install,
configure and execute the Hexagon mudlib using different methods.
* a) [Compiling it yourself](install/readme.md): Compiling the DGD driver and
  executing it in a Unix environment (Unix, Linux, OS X, etc)
* b) [Using a virtual machine](install/vm/readme.md): Install a VirtualBox
  virtual machine already configured using vagrant.
* c) ~~Using a Docker container~~ (document yet to be written).


