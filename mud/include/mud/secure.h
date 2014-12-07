// Defines for /secure objects, and possible dom and cre master.c's
// 1 june 93 Chrisy

// #ifdef  __SECURE_INCLUDE
// #define __SECURE_INCLUDE

#define ROOT "root"
#define BACKBONE "root"

// Objects
// old /secure/master
#define MASTER "/lib/core/secure"
#define SECURE "/lib/core/secure"
// old /secure/bastards
#define ACCESS "/lib/core/access"
// old /secure/ftp_access
#define FTPACC "/lib/net/ftp_access"

/* Positions */
#define POS_ADMIN 1
#define ADMINISTRATORS (["god":1, ROOT:1, ])

/* Masks */
#define READ_MASK 1
#define WRITE_MASK 2
#define GRANT_MASK 4

// Command positions
#define PLAYER_CMD 1
#define CODER_CMD 2
#define COORDINATOR_CMD 3
#define ADMINISTRATOR_CMD 4

// #endif

