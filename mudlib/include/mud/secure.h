// Defines for /secure objects, and possible dom and cre master.c's
// 1 june 93 Chrisy

// #ifdef  __SECURE_INCLUDE
// #define __SECURE_INCLUDE

#include <kernel.h>

// euids
#define DRIVER_EIUD  "driver"
#define ROOT         "root"
#define BACKBONE     "root"
// #define SECURE_EUID  "secure"
#define ROOM_EUID    "room"
#define CMD_EUID     "cmd"
#define TMP_EUID     "tmp"
#define NETWORK_EUID "network"
#define MAIL_EUID    "mailer"
#define LOGIN_EUID   "login"
#define PLAYER_EUID  "player"
#define NOBODY_EUID  "nobody"

#define SECURE find_object(DRIVER)->secure()

// Objects
// old /secure/master
#define MASTER "/lib/core/secure"
#define SECURE_OB "/lib/core/secure/secure"
#define DEBUGGER_OB "/lib/core/secure/debugger"
#define SECURE_SAVE_PATH "/save/secure/secure.o"
// old /secure/ftp_access
#define FTPACC "/lib/net/ftp_access"

/* Positions */
#define POS_ADMIN 1
#define ADMINISTRATORS (["admin":1, ROOT:1, ])

/* Masks */
#define READ_MASK 1
#define WRITE_MASK 2
#define GRANT_MASK 4

// #endif
