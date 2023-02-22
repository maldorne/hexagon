
#ifndef _USER_H
#define _USER_H

#define USER_OB        "/lib/user"
#define USERS_SAVE_DIR "/save/users/"

// rlimits of every command issued by the user
#define MAX_USER_DEPTH 100
// #define MAX_USER_TICKS 100000
#define MAX_USER_TICKS 10000000

// taken from old player.c, neverbot 4/2003
// ------------

// with an idle time > TELL_WARN_TIME, show a warning to other users
// when trying to communicate
#define TELL_WARN_TIME 120

#endif
