#ifndef _PLAYER_H
#define _PLAYER_H

#define RANKING_OB  "/lib/obj/handlers/ranking"

// objects a player must have
#define MUST_HAVE ({ "/lib/obj/diary", "/lib/obj/hearthstone", })

// taken from old player.c, neverbot 4/2003
// ------------
// Taniwha 14/09/1995 how long shall we disable guild commands 
//  and spells after login
#define LOGINLOCK 20
#define START_POS "/game/areas/start/start"
#define ENTRY_POS "/d/ciudad_capital/barrio_antiguo/rooms/calzada02"

#define IDENTD "/net/identd"
#define NUM_MAX_OBJ 50 // Numero maximo de objetos
#define G_CHAR (this_object()->query_gender()==2?"a":"o")
#define MIN_AGE 1800
// -------------

#define NO_LINE_PROP "no new line"
#define RACE_STD "/obj/races/desconocida"
#define MAX_IDLE 900
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
