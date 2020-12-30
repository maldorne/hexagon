

#ifndef __MUD_CONFIG
#define __MUD_CONFIG

#include <status.h>

// new defines for hexagon mudlib

#define CONFIG_LOG_HEART_BEATS FALSE
#define CONFIG_LOG_CALL_OUTS FALSE


// old defines

/*
 * Common save options
 *   (if defined, every important object will be saved in
 *    /save/{ shops, pubs, vaults, etc })
 */
#define COMMON_SAVE

// define to forbid player logons
#undef PLAYER_LOCKOUT

/*
 * Multimud options
 */

// Undefine one of these two options depending the mud we are on
// _PRIMARY_MUD for the game port
// _DEVELOPMENT_MUD for the development/testing port
// (obviously, let one of them defined)

#define MULTIMUD
#define _PRIMARY_MUD
#undef _DEVELOPMENT_MUD

// for /net/remote_cre.c (shared channel)
#define ALLOWED_MUDS ({"CC","CC_DEV"})

#ifdef _PRIMARY_MUD
#define THIS_MUD "CC"
#else
#define THIS_MUD "CC_DEV"
#endif

// define to print every debugger log in the console
#define USE_STANDARD_LOG
// prepend debug type before standard log
// only used if USE_STANDARD_LOG is defined
#define USE_STANDARD_LOG_TYPE

// Definir para utilizar funciones de alineamiento
#define USE_ALIGNMENT


// old defines

// for /include/ftp.h (ftp server port)
// TELNET_PORT will only be used if the driver is compiled
// with -DNETWORK_EXTENSIONS
#ifdef _PRIMARY_MUD
#define TELNET_PORT status()[ST_TELNETPORTS][0]
#define FTP_PORT 21
#define FTP_VERSION "Ciudad Capital Mud (Juego) - FTPD"
#else
#define TELNET_PORT status()[ST_TELNETPORTS][0]
#define FTP_PORT 3007
#define FTP_VERSION "Ciudad Capital Mud (Desarrollo) - FTPD"
#endif


#endif
