/*
 * Riding handler.
 *
 * All the rules of mounts and vehicles live here: who may board a steed, what
 * a steed may cross, how a rider and the others in the seats move, what the
 * room reads, and what happens when something goes wrong. A steed is anything
 * that answers query_rideable() with its spec: an NPC through its rideable
 * component (/lib/npc/components/rideable.c), an item through /lib/obj/vehicle.c.
 * Both are plain data; nothing here is stored on the steed beyond that. The
 * riders of a steed are not kept anywhere: they are the livings in its place
 * whose query_mount() is the steed, so there is no list to fall out of step.
 *
 * The spec of a steed is the default row of the steeds table, then the row of
 * its kind, then its own keys (query_spec). Every text a ride says is a key of
 * that spec, so a game gives a horse and a motorbike different words without
 * touching this file. The meaning of every key: /lib/living/riding.md.
 *
 * A game may have its own copy: /games/<game>/handlers/riding.c inherits this
 * one and redefines what differs. Reach it with handler("riding", ob), never by
 * path.
 */

#include <living/riding.h>
#include <living/skills.h>
#include <common/properties.h>
#include <basic/move.h>
#include <room/room.h>

// who is moving by the rider's own doing right now: the steed and the others
// in the seats follow the driver, and nobody falls
static object * carrying;

// the value a skill the steed teaches is given at
#define RIDING_LEARN_VALUE 10

// prototype
int board_seat(object who, object steed, int seat);

void create()
{
  carrying = ({ });
}

/**
 * The spec a steed has when it carries `own`: the default row of the steeds
 * table, then the row of its kind, then `own`, the last one winning. For a
 * steed that is still being put together and does not answer for itself yet.
 */
mapping query_spec_of(object steed, mapping own)
{
  object kinds;

  if (!steed || !mappingp(own))
    return nil;

  kinds = table("steeds", steed);
  return ([ ]) + kinds->query_default_kind() + kinds->query_kind(own[RIDE_KIND]) + own;
}

/**
 * The spec of a steed: its own keys over its kind's row over the default row.
 * @return nil when the object is not a steed
 */
mapping query_spec(object steed)
{
  return steed ? query_spec_of(steed, steed->query_rideable()) : nil;
}

/**
 * Whether the object can be ridden.
 */
int query_steed(object ob)
{
  return ob && mappingp(ob->query_rideable());
}

/**
 * Whether what somebody typed after a verb names this steed: nothing at all
 * does, and so does a name it answers to, after the little words of the
 * language (the prepositions of its kind) are dropped.
 */
int query_addressed(object steed, string str)
{
  mapping spec;
  string * words, * little;
  int i;

  spec = query_spec(steed);
  if (!spec)
    return 0;

  if (!str || !strlen(trim(str)))
    return 1;

  words = explode(trim(str), " ");
  little = spec["prepositions"];
  while (sizeof(words) > 1 && member_array(words[0], little) != -1)
    words = words[1 ..];

  return steed->id(implode(words, " "));
}

/**
 * The livings riding a steed, driver first. They are in the steed's place.
 */
object * query_riders(object steed)
{
  object * obs, * riders;
  int i, j;

  if (!steed || !environment(steed))
    return ({ });

  obs = all_inventory(environment(steed));
  riders = ({ });

  for (i = 0; i < sizeof(obs); i++)
    if (living(obs[i]) && obs[i]->query_mount() == steed)
    {
      // keep the seats in order
      for (j = 0; j < sizeof(riders); j++)
        if ((int)obs[i]->query_seat() < (int)riders[j]->query_seat())
          break;

      if (j == 0)
        riders = ({ obs[i] }) + riders;
      else
        riders = riders[.. j - 1] + ({ obs[i] }) + riders[j ..];
    }

  return riders;
}

/**
 * Whether somebody sits in a seat of a steed.
 */
int query_seat_taken(object steed, int seat)
{
  object * riders;
  int i;

  riders = query_riders(steed);
  for (i = 0; i < sizeof(riders); i++)
    if ((int)riders[i]->query_seat() == seat)
      return 1;

  return 0;
}

/**
 * The first seat nobody sits in, or -1 when the steed is full.
 */
int query_free_seat(object steed)
{
  mapping spec;
  int i;

  spec = query_spec(steed);
  if (!spec)
    return -1;

  for (i = 0; i < (int)spec[RIDE_SEATS]; i++)
    if (!query_seat_taken(steed, i))
      return i;

  return -1;
}

/**
 * A text of the spec with the words of this ride put in. $N is the rider and
 * $S the steed's short in lower case; $T and $F are left for move_living.
 * @param keep_rider  1 to leave $N in, for the lines move_living writes
 */
private string text(mapping spec, string key, object who, object steed,
                    varargs int keep_rider)
{
  string str;

  str = spec[key];
  if (!stringp(str))
    return "";

  if (steed)
    str = replace_string(str, "$S", lower_case((string)steed->query_short()));
  if (who && !keep_rider)
    str = replace_string(str, "$N", (string)who->query_cap_name());

  return str;
}

/**
 * The reason somebody cannot take a seat of a steed, written for them, or nil
 * when they can. A driver who does not know the skill the steed teaches counts
 * as knowing it at the value it would be given.
 * @param seat  the seat wanted, or -1 for the first free one
 */
string query_board_refusal(object who, object steed, int seat)
{
  mapping spec, req;
  mixed * need;
  object race;
  int value, free_seat, * sizes;
  string refusal;

  spec = query_spec(steed);
  if (!spec || !who)
    return nil;

  if (who->query_dead())
    return text(spec, "refuse_dead", who, steed);

  if (who->query_property(PASSED_OUT_PROP))
    return text(spec, "refuse_passed_out", who, steed);

  if (who->query_mount())
    return text(spec, "refuse_already", who, steed);

  free_seat = query_free_seat(steed);
  if (free_seat == -1 ||
      (seat != -1 && (seat < 0 || seat >= (int)spec[RIDE_SEATS] ||
                      query_seat_taken(steed, seat))))
    return text(spec, "refuse_full", who, steed);

  if (seat == -1)
    seat = free_seat;

  // only whoever drives has to know how
  if (seat == RIDING_DRIVER_SEAT && mappingp(spec[RIDE_REQUIRES]))
  {
    req = spec[RIDE_REQUIRES];

    need = req[RIDE_REQ_SKILL];
    if (pointerp(need) && sizeof(need) == 2)
    {
      value = who->query_skill_value(need[0]);
      if (!who->query_known_skill(need[0]) && spec[RIDE_LEARN] == need[0])
        value = RIDING_LEARN_VALUE;
      if (value < need[1])
        return text(spec, "refuse_skill", who, steed);
    }

    sizes = req[RIDE_REQ_BODY_SIZE];
    if (pointerp(sizes) && sizeof(sizes) == 2 && who->query_race_ob())
    {
      race = load_object((string)who->query_race_ob());
      if (race && ((int)race->query_body_size() < sizes[0] ||
                   (int)race->query_body_size() > sizes[1]))
        return text(spec, "refuse_body_size", who, steed);
    }
  }

  // whatever else the steed itself wants to ask
  refusal = steed->steed_can_board(who, seat);
  if (stringp(refusal) && strlen(refusal))
    return refusal;

  return nil;
}

/**
 * Seats somebody on a steed, in the first free seat, and tells them and the
 * place. The refusal, if any, is told to the rider.
 * @return 1 when they are on it
 */
int board(object who, object steed)
{
  return board_seat(who, steed, -1);
}

/**
 * Seats somebody on a steed in the seat they want (-1: the first free one).
 * Whoever takes the driver's seat of a steed that teaches a skill and does not
 * know it learns it, at RIDING_LEARN_VALUE.
 * @return 1 when they are on it
 */
int board_seat(object who, object steed, int seat)
{
  mapping spec;
  string refusal;

  spec = query_spec(steed);
  if (!spec || !who || environment(who) != environment(steed))
    return 0;

  refusal = query_board_refusal(who, steed, seat);
  if (refusal)
  {
    tell_object(who, refusal);
    return 0;
  }

  if (seat == -1)
    seat = query_free_seat(steed);

  if (seat == RIDING_DRIVER_SEAT && stringp(spec[RIDE_LEARN]) &&
      !who->query_known_skill(spec[RIDE_LEARN]))
    who->set_skill_value(spec[RIDE_LEARN], RIDING_LEARN_VALUE);

  who->set_mount(steed, seat);
  who->add_extra_look(who);

  tell_object(who, text(spec, "board_me", who, steed));
  tell_room(environment(who), text(spec, "board_room", who, steed), ({ who }));

  steed->steed_boarded(who);
  return 1;
}

/**
 * Takes somebody off the steed they ride, with nothing but words: a fall does
 * no damage.
 * @param fall  1 to tell the fall instead of the dismount
 * @return 1 when they were riding
 */
int dismount(object who, varargs int fall)
{
  mapping spec;
  object steed;
  string mine, others;

  if (!who)
    return 0;

  steed = who->query_mount();
  if (!steed)
    return 0;

  spec = query_spec(steed);
  if (!spec)
    spec = table("steeds", who)->query_default_kind();

  who->clear_mount();
  who->remove_extra_look(who);

  mine = text(spec, fall ? "fall_me" : "dismount_me", who, steed);
  others = text(spec, fall ? "fall_room" : "dismount_room", who, steed);

  tell_object(who, mine);
  if (environment(who))
    tell_room(environment(who), others, ({ who }));

  steed->steed_dismounted(who);
  return 1;
}

/**
 * Everybody on a steed falls: it died, was destructed, or was taken somewhere
 * they were not.
 * @param where  the place they are in, when the steed no longer is
 */
void unseat_riders(object steed, varargs object where)
{
  object * riders, env;
  int i;

  env = where ? where : environment(steed);
  if (!env)
    return;

  riders = all_inventory(env);
  for (i = 0; i < sizeof(riders); i++)
    if (living(riders[i]) && riders[i]->query_mount() == steed)
      dismount(riders[i], 1);
}

/**
 * Why the rider cannot take an exit, or nil when the ride may go on: the
 * seat, the kind of exit, its size, and what lies beyond it.
 * @param room  the place they leave
 * @param dir   the exit
 */
string query_move_refusal(object who, object room, string dir)
{
  mapping spec;
  object steed, dest, kinds;
  string type, reason;
  mixed size;
  string * medium;

  steed = who ? who->query_mount() : nil;
  spec = query_spec(steed);
  if (!spec || !room)
    return nil;

  if (who->query_seat() != RIDING_DRIVER_SEAT)
    return text(spec, "refuse_passenger", who, steed);

  type = room->query_ex_type(dir);
  if (type && member_array(type, spec[RIDE_EXITS_DENY]) != -1)
    return text(spec, spec["refuse_" + type] ? "refuse_" + type : "refuse_exit",
                who, steed);

  if (type && pointerp(spec[RIDE_TERRAIN_ONLY]) && sizeof(spec[RIDE_TERRAIN_ONLY]) &&
      member_array(type, spec[RIDE_TERRAIN_ONLY]) == -1)
    return text(spec, "refuse_terrain", who, steed);

  // how big the exit is; an exit with no size does not hold it back
  size = room->query_size(dir);
  if (intp(size) && size > 0 && size < (int)spec[RIDE_EXIT_SIZE])
    return text(spec, "refuse_exit_size", who, steed);

  dest = room->query_dest_object(dir);
  if (!dest)
    return nil;

  if (member_array(RIDE_TERRAIN_INDOORS, spec[RIDE_TERRAIN_DENY]) != -1 &&
      !dest->query_outside())
    return text(spec, "refuse_indoors", who, steed);

  if (member_array(RIDE_TERRAIN_UNDERGROUND, spec[RIDE_TERRAIN_DENY]) != -1 &&
      dest->query_underground())
    return text(spec, "refuse_underground", who, steed);

  // the medium of the place beyond: a place on water needs a steed that crosses it
  medium = spec[RIDE_MEDIUM];
  if (dest->query_water_environment())
  {
    if (member_array(RIDE_MEDIUM_WATER, medium) == -1)
      return text(spec, "refuse_water", who, steed);
  }
  else if (member_array(RIDE_MEDIUM_GROUND, medium) == -1 &&
           member_array(RIDE_MEDIUM_AIR, medium) == -1)
    return text(spec, "refuse_ground", who, steed);

  return nil;
}

/**
 * The line a rider leaves (0) or arrives (1) with, for move_living: the
 * leaving line takes $N and $T, the arriving one $N and $F. Nil when the
 * rider rides nothing.
 */
string query_travel_line(object who, int arriving)
{
  mapping spec;
  object steed;

  steed = who ? who->query_mount() : nil;
  spec = query_spec(steed);
  if (!spec)
    return nil;

  return text(spec, arriving ? "arrive" : "leave", who, steed, 1);
}

/**
 * What follows a rider's short in a room: where they sit.
 */
string query_listing(object who)
{
  mapping spec;
  object steed;

  steed = who ? who->query_mount() : nil;
  spec = query_spec(steed);
  return spec ? text(spec, "listing", who, steed) : "";
}

/**
 * The line `look` adds to a rider.
 */
string query_look(object who)
{
  mapping spec;
  object steed;

  steed = who ? who->query_mount() : nil;
  spec = query_spec(steed);
  return spec ? text(spec, "look", who, steed) : "";
}

/**
 * The driver has just walked through an exit: the steed and the others in the
 * seats come after, in silence. If the steed cannot follow, the driver falls.
 * @param from  the place the driver left
 */
void carry(object who, object from)
{
  object steed, dest, * riders;
  int i;

  steed = who->query_mount();
  dest = environment(who);
  if (!steed || !dest || environment(steed) == dest)
    return;

  riders = query_riders(steed) - ({ who });

  carrying = ({ steed }) + riders;

  if (steed->move(dest) != MOVE_OK)
  {
    carrying = ({ });
    dismount(who, 1);
    return;
  }

  for (i = 0; i < sizeof(riders); i++)
  {
    riders[i]->move(dest);
    if (interactive(riders[i]))
      riders[i]->do_look();
  }

  carrying = ({ });
}

/**
 * A rider has been moved by something that is not their ride (a teleport, a
 * push): they fall where they land, and the steed stays where it was.
 */
void rider_moved(object who, object from, object dest)
{
  if (member_array(who, carrying) != -1)
    return;

  dismount(who, 1);
}

/**
 * A steed has been moved: unless it is following its driver, those on it were
 * left behind and fall.
 */
void steed_moved(object steed, object from, object dest)
{
  if (member_array(steed, carrying) != -1)
    return;

  unseat_riders(steed, from);
}
