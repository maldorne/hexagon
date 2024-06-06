
FROM --platform=linux/amd64 ghcr.io/maldorne/dgd:latest

LABEL org.opencontainers.image.source=https://github.com/maldorne/hexagon
LABEL org.opencontainers.image.description="Hexagon Mudlib"
LABEL org.opencontainers.image.licenses="Pending"

USER root

RUN apt-get update
# temporary, will be removed from final image
RUN apt-get install -y --force-yes procps telnet

# clone the full contents of the mudlib
WORKDIR /opt/mud
COPY --chown=mud:mud .config.dgd start.sh hexagon/
COPY --chown=mud:mud mudlib hexagon/mudlib/

# reuse the same user created by the dgd:latest image
USER mud

# finishing touches
WORKDIR /opt/mud/hexagon
RUN mv .config.dgd ../bin/config.dgd
RUN chmod +x start.sh

ENTRYPOINT ./start.sh

# expose telnet mudos port
EXPOSE 5000/tcp
