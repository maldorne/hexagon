
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

#define IDENTD "/net/identd"
#define NUM_MAX_OBJ 50 // Numero maximo de objetos
#define MIN_AGE 1800
// -------------

#define NO_LINE_PROP "no new line"
#define RACE_STD "/obj/races/desconocida"
#define MAX_IDLE 900
#define TIME_BETWEEN_SAVES 450

#define MAX_TELL_HIS 10
#define TELL_WARN_TIME 120
#define ROUND_TIME 40
#define DEFAULT_TIME 10

// En language.h
// #define LANGUAGE_HAND "/obj/handlers/languages"

#endif
