# Dockerfile for running hexagon locally on Docker Desktop or any other
# docker host. Bakes the mudlib and the config into the image and just runs
# the DGD driver.
#
# Build:   docker build -t hexagon .
# Run:     docker run --rm -it -p 5000:5000 hexagon
# Connect: telnet localhost 5000  (or any mud client of your choice)

FROM --platform=linux/amd64 ghcr.io/maldorne/dgd:latest

LABEL org.opencontainers.image.source=https://github.com/maldorne/hexagon
LABEL org.opencontainers.image.description="Hexagon Mudlib"
LABEL org.opencontainers.image.licenses="Pending"

USER root
RUN apt-get update \
 && apt-get install -y --no-install-recommends procps telnet \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/mud
COPY --chown=mud:mud config.hexagon start.sh hexagon/
COPY --chown=mud:mud mudlib hexagon/mudlib/

USER mud
WORKDIR /opt/mud/hexagon
RUN mv config.hexagon ../bin/config.dgd \
 && chmod +x start.sh

ENTRYPOINT ["./start.sh"]

EXPOSE 5000/tcp
