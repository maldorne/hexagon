

#ifndef __MUD_CONFIG
#define __MUD_CONFIG

/* 
 * Opciones de salvado común 
 *   (si esta activado, todos los objetos importantes se
 *    guardan en /save/{shops, pubs, vaults, etc})
 */
#define COMMON_SAVE

// definir para impedir la entrada a players
#undef PLAYER_LOCKOUT

/* 
 * Opciones Multimud
 */

// Descomentar una de estas dos opciones segun el mud en el que nos encontremos
// _PRIMARY_MUD para el puerto de juego
// _DEVELOPMENT_MUD para el puerto de desarrollo/testing
// (obviamente, dejad descomentada solo una de las opciones)

#define MULTIMUD
#define _PRIMARY_MUD
#undef _DEVELOPMENT_MUD

// Para /net/remote_cre.c (canal cre comunicado)
#define ALLOWED_MUDS ({"CC","CC_DEV"})

#ifdef _PRIMARY_MUD
#define THIS_MUD "CC"
#else
#define THIS_MUD "CC_DEV"
#endif

// Para /include/ftp.h (puerto del servidor ftp)
#ifdef _PRIMARY_MUD
#define FTP_PORT 21
#define FTP_VERSION "Ciudad Capital Mud (Juego) - FTPD"
#else
#define FTP_PORT 3007
#define FTP_VERSION "Ciudad Capital Mud (Desarrollo) - FTPD"
#endif

// Definir para utilizar funciones de alineamiento
#define USE_ALIGNMENT

#endif
