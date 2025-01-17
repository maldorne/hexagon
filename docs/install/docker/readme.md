
# Hexagon mudlib

## Developer how-to (using containers)

### Initial steps

Software required to be installed: just the docker engine in [Docker Desktop](https://www.docker.com/products/docker-desktop/).

And having access to some kind of command-line terminal.

  * In Windows, execute (`Windows Key + r`) the command `cmd`. 
  * In Mac OS, open the Terminal App (or use `Cmd + spacebar` and type `Terminal`).
  * In any kind of Unix/Linux, you know what we are talking about.

### How to use in a Docker container

- Use the image:

  - Download the image from the repository `docker pull ghcr.io/maldorne/hexagon:latest`
  - or generate it yourself `docker build --no-cache . -t ghcr.io/maldorne/hexagon:latest`

- Run the container in background:

  - `docker run --rm -d -p 23:5000 --name hexagon ghcr.io/maldorne/hexagon:latest`
  - `docker stop hexagon` will stop the container.

- Connect to the container if needed (it has to be running, see previous step):

  - `docker exec -it hexagon bash`

If you make changes to the image, push it after building it:

`docker push ghcr.io/maldorne/hexagon:latest`

To update the version you have in your computer, pull the image from the repository:

`docker pull ghcr.io/maldorne/hexagon:latest`
   
We are mapping the 23 port in the host machine to the 5000 port in the container, so if we execute `telnet localhost` in the host machine it will connect to the mud in the container.

### Connecting to the game

- Use in your local machine `telnet localhost` (it will use the default port 23) to connect to the mud inside the container and see if everything is working.

By default you will have an administrator account created, with username `admin`, email `admin@maldorne.org` and password `hexagon`.
