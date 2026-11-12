// Mounts table: the kinds of mount (horse, dragon, motorbike, car...) and, for
// each, the defaults of its spec and every text a ride says. Read by the
// riding handler (/lib/handlers/riding.c); the shape of a row and the meaning
// of every key is in /lib/living/riding.md.
//
// The mudlib ships only the neutral kind "mount", which every missing key of a
// kind falls back to. A game's own table (/games/<game>/tables/mounts.c)
// inherits this one and adds its kinds with add_kind:
//
//     inherit "/lib/tables/mounts";
//
//     void create()
//     {
//       ::create();
//       add_kind("horse", ([ "seats" : 1, "arrive" : "@$N ..." ]));
//     }

#include <living/riding.h>
#include <language.h>

private mapping kinds;

void create()
{
  kinds = ([
    RIDING_DEFAULT_KIND : ([
      // spec defaults
      RIDE_SEATS        : 1,
      RIDE_MEDIUM       : ({ RIDE_MEDIUM_GROUND }),
      RIDE_TERRAIN_DENY : ({ }),
      RIDE_TERRAIN_ONLY : ({ }),
      RIDE_EXITS_DENY   : ({ }),
      RIDE_EXIT_SIZE    : 0,
      RIDE_COMBAT       : RIDE_COMBAT_LINKED,
      RIDE_PROTECT      : 0,
      RIDE_WIMPY        : 0,
      RIDE_FEARS        : ({ }),
      RIDE_SPEED        : 100,
      RIDE_SUMMON       : RIDE_SUMMON_OUTDOORS,

      // the verbs the mount answers to
      "verbs_board"    : _LANG_MOUNTS_VERBS_BOARD,
      "verbs_dismount" : _LANG_MOUNTS_VERBS_DISMOUNT,
      // words dropped from what follows the verb: "mount onto the horse"
      "prepositions"   : _LANG_MOUNTS_PREPOSITIONS,

      // $N the rider, $S the mount's short in lower case, $T the direction,
      // $F where the rider comes from; the lines of a step start with the
      // marker move_living strips
      "board_me"     : _LANG_MOUNTS_BOARD_ME,
      "board_room"   : _LANG_MOUNTS_BOARD_ROOM,
      "dismount_me"  : _LANG_MOUNTS_DISMOUNT_ME,
      "dismount_room": _LANG_MOUNTS_DISMOUNT_ROOM,
      "leave"        : _LANG_MOUNTS_LEAVE,
      "arrive"       : _LANG_MOUNTS_ARRIVE,
      "listing"      : _LANG_MOUNTS_LISTING,
      "look"         : _LANG_MOUNTS_LOOK,
      "fall_me"      : _LANG_MOUNTS_FALL_ME,
      "fall_room"    : _LANG_MOUNTS_FALL_ROOM,
      // the mount takes the blows aimed at its rider; $N the rider
      "protect_room" : _LANG_MOUNTS_PROTECT_ROOM,
      // the mount runs from a fight or from what it fears, carrying its rider
      "flee_me"      : _LANG_MOUNTS_FLEE_ME,
      "flee_room"    : _LANG_MOUNTS_FLEE_ROOM,
      // its owner calls it ($N the owner) and sends it away
      "summon_me"    : _LANG_MOUNTS_SUMMON_ME,
      "summon_room"  : _LANG_MOUNTS_SUMMON_ROOM,
      "dismiss_me"   : _LANG_MOUNTS_DISMISS_ME,
      "dismiss_room" : _LANG_MOUNTS_DISMISS_ROOM,
      // travelling on its own; $T is where it heads
      "travel_start"   : _LANG_MOUNTS_TRAVEL_START,
      "travel_follow"  : _LANG_MOUNTS_TRAVEL_FOLLOW,
      "travel_stop"    : _LANG_MOUNTS_TRAVEL_STOP,
      "travel_lost"    : _LANG_MOUNTS_TRAVEL_LOST,
      "travel_blocked" : _LANG_MOUNTS_TRAVEL_BLOCKED,
      "travel_fight"   : _LANG_MOUNTS_TRAVEL_FIGHT,

      // why a ride is refused
      "refuse_dead"       : _LANG_MOUNTS_REFUSE_DEAD,
      "refuse_passed_out" : _LANG_MOUNTS_REFUSE_PASSED_OUT,
      "refuse_already"    : _LANG_MOUNTS_REFUSE_ALREADY,
      "refuse_full"       : _LANG_MOUNTS_REFUSE_FULL,
      "refuse_skill"      : _LANG_MOUNTS_REFUSE_SKILL,
      "refuse_body_size"  : _LANG_MOUNTS_REFUSE_BODY_SIZE,
      "refuse_passenger"  : _LANG_MOUNTS_REFUSE_PASSENGER,
      "refuse_not_owner"  : _LANG_MOUNTS_REFUSE_NOT_OWNER,
      "refuse_exit"       : _LANG_MOUNTS_REFUSE_EXIT,
      "refuse_exit_size"  : _LANG_MOUNTS_REFUSE_EXIT_SIZE,
      "refuse_terrain"    : _LANG_MOUNTS_REFUSE_TERRAIN,
      "refuse_indoors"    : _LANG_MOUNTS_REFUSE_INDOORS,
      "refuse_underground": _LANG_MOUNTS_REFUSE_UNDERGROUND,
      "refuse_water"      : _LANG_MOUNTS_REFUSE_WATER,
      "refuse_ground"     : _LANG_MOUNTS_REFUSE_GROUND,
      "refuse_attack"     : _LANG_MOUNTS_REFUSE_ATTACK,
      "refuse_summon"        : _LANG_MOUNTS_REFUSE_SUMMON,
      "refuse_summon_indoors": _LANG_MOUNTS_REFUSE_SUMMON_INDOORS,
      "refuse_summon_water"  : _LANG_MOUNTS_REFUSE_SUMMON_WATER,
    ]),
  ]);
}

/**
 * Adds a kind of mount; a game's own table calls it from create.
 * @param id   the kind, an English word never shown to a player
 * @param row  the spec defaults and texts of the kind; whatever it leaves out
 *             is read from the default row
 */
void add_kind(string id, mapping row)
{
  kinds[id] = row;
}

/**
 * The row of a kind, or the default one when the table does not know it.
 */
mapping query_kind(string id)
{
  if (id && kinds[id])
    return kinds[id];

  return kinds[RIDING_DEFAULT_KIND];
}

mapping query_default_kind()
{
  return kinds[RIDING_DEFAULT_KIND];
}

string * query_kinds()
{
  return map_indices(kinds);
}
