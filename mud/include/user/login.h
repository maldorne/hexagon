

#ifndef _LOGIN_INCLUDE
#define _LOGIN_INCLUDE

#define LOGIN_TIMEOUT 300
#define MIN_UPTIME_LOGIN_TIME 20
#define MAX_PLAYERS 50

// From /lib/core/user/login
#define MAX_RETRIES 3
#define MIN_LEN 3
#define MAX_LEN 11
#define CODER_SLOTS 5

#ifndef _PRIMARY_MUD
#define WELCOME_MESSAGE "/doc/welcome_devel.txt"
#else
#define WELCOME_MESSAGE "/doc/welcome.txt"
#endif

#define OPTIONS_MESSAGE "/doc/login_options.txt"

#endif

