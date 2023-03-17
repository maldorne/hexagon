

#ifndef __MUD_CONFIG
#define __MUD_CONFIG

#include <status.h>

#define MUD_NAME "Hexagon"

// do not use the cron handler on start
// usually this will be set to TRUE,
// set to FALSE just for easier debugging purposes
// (less objects loaded, less messages in stderr, etc)
#define CONFIG_USE_CRON FALSE

// ----------------------------------------------------------------------
//  log features
// ----------------------------------------------------------------------

// define to print every debugger log in the terminal
#define DEBUGGER_USE_STANDARD_LOG
// prepend debug type before standard log
// only used if USE_STANDARD_LOG is defined
#define DEBUGGER_USE_STANDARD_LOG_TYPE

// specific logs that will be printed only in the terminal
// these are useful only for mudlib debugging
#define CONFIG_LOG_HEART_BEATS FALSE
#define CONFIG_LOG_CALL_OUTS FALSE
#define CONFIG_LOG_INITIATOR_OBJECTS FALSE
#define CONFIG_LOG_CRONTAB FALSE

// ----------------------------------------------------------------------
//  error features
// ----------------------------------------------------------------------

// true: messages from caught errors will have more information
// false: to avoid printing in stderr and showing to coders the full 
// trace when errors are caught
#define LOG_CAUGHT_ERRORS TRUE

// when loggin info of an error (stderr or to a coder)
// show the full trace or only the line where the error occurred
#define LOG_FULL_TRACE TRUE










// ----------------------------------------------------------------------
//  old defines
// ----------------------------------------------------------------------

/*
 * Common save options
 *   (if defined, every important object will be saved in
 *    /save/{ shops, pubs, vaults, etc })
 */
#define COMMON_SAVE

// define to forbid player logons
#undef PLAYER_LOCKOUT

// Definir para utilizar funciones de alineamiento
#define USE_ALIGNMENT

#endif
