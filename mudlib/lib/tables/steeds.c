// Steeds table: the kinds of steed (horse, dragon, motorbike, car...) and, for
// each, the defaults of its spec and every text a ride says. Read by the
// riding handler (/lib/handlers/riding.c); the shape of a row and the meaning
// of every key is in /lib/living/riding.md.
//
// The mudlib ships only the neutral kind "mount", which every missing key of a
// kind falls back to. A game's own table (/games/<game>/tables/steeds.c)
// inherits this one and adds its kinds with add_kind:
//
//     inherit "/lib/tables/steeds";
//
//     void create()
//     {
//       ::create();
//       add_kind("horse", ([ "seats" : 1, "speed" : 2, "arrive" : "@$N ..." ]));
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
      RIDE_SPEED        : 1,
      RIDE_MEDIUM       : ({ RIDE_MEDIUM_GROUND }),
      RIDE_TERRAIN_DENY : ({ }),
      RIDE_TERRAIN_ONLY : ({ }),
      RIDE_EXITS_DENY   : ({ }),
      RIDE_EXIT_SIZE    : 0,

      // the verbs the steed answers to
      "verbs_board"    : _LANG_STEEDS_VERBS_BOARD,
      "verbs_dismount" : _LANG_STEEDS_VERBS_DISMOUNT,
      // words dropped from what follows the verb: "mount onto the horse"
      "prepositions"   : _LANG_STEEDS_PREPOSITIONS,

      // $N the rider, $S the steed's short in lower case, $T the direction,
      // $F where the rider comes from; the lines of a step start with the
      // marker move_living strips
      "board_me"     : _LANG_STEEDS_BOARD_ME,
      "board_room"   : _LANG_STEEDS_BOARD_ROOM,
      "dismount_me"  : _LANG_STEEDS_DISMOUNT_ME,
      "dismount_room": _LANG_STEEDS_DISMOUNT_ROOM,
      "leave"        : _LANG_STEEDS_LEAVE,
      "arrive"       : _LANG_STEEDS_ARRIVE,
      "listing"      : _LANG_STEEDS_LISTING,
      "look"         : _LANG_STEEDS_LOOK,
      "fall_me"      : _LANG_STEEDS_FALL_ME,
      "fall_room"    : _LANG_STEEDS_FALL_ROOM,

      // why a ride is refused
      "refuse_dead"       : _LANG_STEEDS_REFUSE_DEAD,
      "refuse_passed_out" : _LANG_STEEDS_REFUSE_PASSED_OUT,
      "refuse_already"    : _LANG_STEEDS_REFUSE_ALREADY,
      "refuse_full"       : _LANG_STEEDS_REFUSE_FULL,
      "refuse_skill"      : _LANG_STEEDS_REFUSE_SKILL,
      "refuse_body_size"  : _LANG_STEEDS_REFUSE_BODY_SIZE,
      "refuse_passenger"  : _LANG_STEEDS_REFUSE_PASSENGER,
      "refuse_exit"       : _LANG_STEEDS_REFUSE_EXIT,
      "refuse_exit_size"  : _LANG_STEEDS_REFUSE_EXIT_SIZE,
      "refuse_terrain"    : _LANG_STEEDS_REFUSE_TERRAIN,
      "refuse_indoors"    : _LANG_STEEDS_REFUSE_INDOORS,
      "refuse_underground": _LANG_STEEDS_REFUSE_UNDERGROUND,
      "refuse_water"      : _LANG_STEEDS_REFUSE_WATER,
      "refuse_ground"     : _LANG_STEEDS_REFUSE_GROUND,
    ]),
  ]);
}

/**
 * Adds a kind of steed; a game's own table calls it from create.
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
