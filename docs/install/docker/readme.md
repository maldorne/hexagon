
# Hexagon mudlib

## Developer how-to (using containers)

This is the recommended way to run Hexagon on your own machine for testing
or development. The repository ships with a stand-alone `Dockerfile` at the
project root that bakes the mudlib and the DGD driver into a single image,
so you only need Docker installed and nothing else.

### Initial steps

Software required: just the docker engine in [Docker Desktop](https://www.docker.com/products/docker-desktop/) (Mac/Windows) or your distribution's docker package (Linux).

You also need a command-line terminal:

  * In Windows, run (`Windows Key + r`) the command `cmd`.
  * In macOS, open the Terminal app (or use `Cmd + spacebar` and type `Terminal`).
  * In any kind of Unix/Linux, you know what we are talking about.

### Build and run

Clone the repository and from its root:

```sh
docker build -t hexagon .
docker run --rm -it -p 5000:5000 --name hexagon hexagon
```

`docker stop hexagon` (in another terminal) stops the container. The `--rm` flag removes it on exit so you always start clean.

If you prefer to expose the mud on the standard telnet port 23 instead of 5000, change the mapping:

```sh
docker run --rm -it -p 23:5000 --name hexagon hexagon
```

### Open a shell inside a running container

```sh
docker exec -it hexagon bash
```

### Connecting to the game

From the host machine, use any mud client (or plain telnet) pointing at the mapped port:

```sh
telnet localhost 5000
```

The repository ships with a seed administrator account so you can log in immediately:

| field    | value               |
| -------- | ------------------- |
| username | `admin`             |
| email    | `admin@maldorne.org`|
| password | `hexagon`           |
