
#ifndef __NETWORK_H 
#define __NETWORK_H 
 
#include <net/socket_errors.h> 
 
#define MUD_NETWORK               0 
#define STREAM_NETWORK            1 
#define DATAGRAM_NETWORK          2 
#define STREAM_BINARY             3
#define DATAGRAM_BINARY           4

#define PORT_MUD                  __PORT__ 
#define PORT_FTP                  4444
#define PORT_HTTP                 5678
#undef  PORT_OOB
#undef  PORT_RCP
#undef  PORT_UDP

#define CLIENT                    "/net/intermud3/client"
#define INTERMUD_D                "/net/intermud3/intermud"
#define SERVICES_D                "/net/intermud3/services"
 
#endif /* __NETWORK_H */ 
