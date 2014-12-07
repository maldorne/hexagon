#ifndef _PLAYER_H
#define _PLAYER_H

// Tabla de actualizacion de jugadores
#define SHEET_TABLE "/table/sheet_table.c"

// Objetos que un player siempre debe tener
#define MUST_HAVE ({"/obj/misc/diary","/obj/misc/hearthstone",})

// Definiciones extraidas del player.c, Folken 4/2003
// ------------
// Taniwha 14/09/1995 how long shall we disable guild commands 
//  and spells after login
#define LOGINLOCK 20
#define START_POS "/room/inicio.c"
#define ENTRY_POS "/d/ciudad_capital/barrio_antiguo/rooms/calzada02.c"

#define IDENTD "/net/identd"
#define NUM_MAX_OBJ 50 // Numero maximo de objetos
#define G_CHAR (query_gender()==2?"a":"o")
#define MIN_AGE 1800
// -------------

#define NO_POWER "Puntos sociales insuficientes.\n"
#define NOT_ALLOWED "Aún no posees esa habilidad.\n"
#define SHOUT_COST 20
#define TELL_COST 1
#define EMOTE_COST 1
#define ECHO_COST 1
#define ECHOTO_COST 4
#define EMOTEALL_COST 40
#define NO_LINE_PROP "no new line"
#define RACE_STD "/obj/races/desconocida"
#define MAX_IDLE 900
// #define DEATH_CHAR clone_object("/obj/death")
#define DEATH_SHADOW "/obj/shadows/death_shadow"
#define TIME_BETWEEN_SAVES 450
// #define LOADING "loading" // Ahora en properties.h
// #define AUTOEQUIP_PROP "autoequip"
#define GUEST_PROP "guest"

#define MAX_TELL_HIS 10
#define TELL_WARN_TIME 120
#define ROUND_TIME 40
#define DEFAULT_TIME 10

// En language.h
// #define LANGUAGE_HAND "/obj/handlers/languages"

#endif
