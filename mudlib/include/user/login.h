

#ifndef _LOGIN_INCLUDE
#define _LOGIN_INCLUDE

#define LOGIN_TIMEOUT 300
// #define MIN_UPTIME_LOGIN_TIME 20
#define MIN_UPTIME_LOGIN_TIME 1
#define MAX_PLAYERS 50
#define CODER_SLOTS 5

// destination room to leave there the /lib/link objects
#define LOGIN_LOUNGE "/games/hexagon/areas/login_lounge.c"
#define LINK_OB "/lib/link.c"

// from /lib/core/user/login
#define MAX_RETRIES 3
#define MIN_LEN 3
#define MAX_LEN 11

#ifdef _PRIMARY_MUD
#define WELCOME_MESSAGE "welcome.txt"
#else
#define WELCOME_MESSAGE "welcome_devel.txt"
#endif

#define LOGIN_OPTIONS_MESSAGE "login_options.txt"

#endif
