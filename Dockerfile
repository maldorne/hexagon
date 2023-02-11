
FROM neverbot/maldorne-dgd:v1.6 as base

USER root

RUN apt-get update && apt-get install -y --force-yes git
# temporary, will be removed from final image
RUN apt-get install -y procps telnet

# clone the full contents of the mudlib
WORKDIR /opt/mud
RUN git clone --depth 1 -b master --single-branch https://github.com/maldorne/hexagon.git hexagon
RUN chown -R mud:mud hexagon

# reuse the same user created by the maldorne-mudos:v22.2b13 image
USER mud

# finishing touches
WORKDIR /opt/mud/hexagon
RUN mv .config.dgd ../bin/config.dgd
RUN chmod +x start.sh

ENTRYPOINT ./start.sh && /bin/bash

# expose telnet mudos port
EXPOSE 5000/tcp
