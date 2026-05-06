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

As you can see, the DGD project is a fork from [the original](https://github.com/dworkin/dgd), but the differences are minimal. The
fork adds three compile-time flags, all enabled by default in
`dgd/src/Makefile`:

  * **`SLASHSLASH`** — allows `//` line comments in LPC.
  * **`PRESERVE_DEFAULTS_ON_RESTORE`** — when `restore_object()` reads a
    `.o` file, variables that are not present in the file keep their
    current in-memory value instead of being zeroed. This matters when
    a savefile is older than the LPC code and a new variable was added:
    with this flag the new variable retains the default assigned by
    `create()` instead of becoming `0` / `0.0` / `nil`.
  * **`SUPPORT_PROXY_PROTOCOL`** — parses [HAProxy PROXY protocol
    v1](https://www.haproxy.org/download/1.8/doc/proxy-protocol.txt)
    headers on accepted TCP connections, so the driver reports real
    client IPs when running behind a TCP reverse proxy (Traefik,
    HAProxy, ...). Connections without a header keep working unchanged.

To build vanilla upstream DGD instead, run `make DEFINES=""`.

### Compile the driver

```sh
cd dgd/src
make
make install
cd ../..
```

This will create the `dgd` executable in the `dgd/bin` directory.

### Compile the extensions

The mudlib depends on the C `sprintf` kfun shipped as a `dgd-extensions` module; the legacy LPC implementation under `mudlib/packages/sprintf/` is kept as reference but not loaded. Without the kfun the auto-object will fail to compile and the mud will not boot.

```sh
cd dgd/dgd-extensions/src
make sprintf
cd ../../..
```

This produces `dgd/dgd-extensions/sprintf.1.5`. Take note of the absolute path to that file; you'll wire it into the config in the next step.

The `dgd-extensions` repo bundles other optional kfuns too (`lower_case`, `regexp`, `dbase`, `jit`, `zlib`, `crypto`, `tls`); Hexagon does not use them, so we only build `sprintf`. If you want to experiment with any of the others, build them with their own target (e.g. `make regexp`).

### Edit the configuration files

```sh
cp hexagon/config.hexagon hexagon/config.dgd
cp hexagon/docs/install/mud.sh hexagon/
chmod a+x hexagon/mud.sh
```

Now edit `hexagon/config.dgd`:

  * Set the `directory` field to the absolute path of `hexagon/mudlib` (must end in `/whatever/hexagon/mudlib` without a trailing slash).
  * In the `modules =` block, replace `<absolute path to dgd-extensions>` with the absolute path you noted in the previous step. The line should end up looking like:

    ```
    modules = ([
      "/your/path/to/dgd/dgd-extensions/sprintf.1.5" : ""
    ]);
    ```

### Executing the game

- Run `hexagon/mud.sh` to execute the game.
- Use `telnet localhost 5000` to connect and see if everything is working.

By default you will have an administrator account created, with username `admin`, email `admin@maldorne.org` and password `hexagon`.

### Executing the game using snapshots

Some advanced information you will probably never need: DGD allows generating snapshops of its current internal database, and restart the system later using the exact database state.

Inside the game, use:

 - `exec dump_state()` and a new `snapshot` file will be generated in `hexagon/mudlib/tmp/snapshot` (if you have an older one, it will be moved to `hexagon/mudlib/tmp/snapshot.old`).
 - If you have a snapshot file, you can start the game using `dgd <path of config file> <path of snapshot file>`, and the game will be restored using the database dump provided, and calling `restored` in the `driver` object.
 - If you have started the game using a snapshot, you can `exec shutdown(1)` and a new `snapshot` file will be generated in `hexagon/mudlib/tmp/snapshot`, _AND_ the mud will be restored using the commands provided in the `hotboot` parameters in your `config.dgd` file.
 - FYI: you can also use `exec swapout()` to swap out all the objects to `hexagon/mudlib/tmp/swap` and clean up memory.
