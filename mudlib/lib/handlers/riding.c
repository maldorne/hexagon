/*
 * Riding handler.
 *
 * All the rules of mounts and vehicles live here: who may board a mount, what
 * a mount may cross, how a rider and the others in the seats move, what the
 * room reads, and what happens when something goes wrong. A mount is anything
 * that answers query_rideable() with its spec: an NPC through its rideable
 * component (/lib/npc/components/rideable.c), an item through /lib/obj/vehicle.c.
 * Both are plain data; nothing here is stored on the mount beyond that. The
 * riders of a mount are not kept anywhere: they are the livings in its place
 * whose query_mount() is the mount, so there is no list to fall out of step.
 *
 * The spec of a mount is the default row of the mounts table, then the row of
 * its kind, then its own keys (query_spec). Every text a ride says is a key of
 * that spec, so a game gives a horse and a motorbike different words without
 * touching this file. The meaning of every key: /lib/living/riding.md.
 *
 * The mounts a player has bought are kept one file each, in the mounts folder of
 * the player's own save folder (query_mount_file): the file says what the mount
 * is made from, who owns it, where it stood and how hurt it was. The player
 * keeps the ids of its mounts (query_mount_ids) and the live ones, and the
 * mount only knows who owns it and under which id (RIDE_OWNER, RIDE_ID in its
 * spec). This handler keeps nothing of any player in memory, so reloading it
 * loses and duplicates nothing. A mount that is out is written when it goes
 * (its owner quits, its place unloads) and made again where it stood when the
 * owner logs in.
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
#include <room/location.h>
#include <living/persisted.h>

// who is moving by the rider's own doing right now: the mount and the others
// in the seats follow the driver, and nobody falls
static object * carrying;

// what save_object and restore_object go through to move one mount file; they
// hold something only for the instant of a save or a restore
string mount_template;  // bestiary template it is made from, or
string mount_file;      // the file it is cloned from
string mount_owner;     // name of its owner
string mount_id;        // its id
string mount_location;  // where it stood; empty when unknown
string mount_name;      // its short
int mount_hp;           // its hit points, 0 when unhurt

// the value a skill the mount teaches is given at
#define RIDING_LEARN_VALUE 10

// prototypes
int board_seat(object who, object mount, int seat);
string query_move_refusal(object who, object room, string dir);

void create()
{
  carrying = ({ });
}

/**
 * The spec a mount has when it carries `own`: the default row of the mounts
 * table, then the row of its kind, then `own`, the last one winning. For a
 * mount that is still being put together and does not answer for itself yet.
 */
mapping query_spec_of(object mount, mapping own)
{
  object kinds;

  if (!mount || !mappingp(own))
    return nil;

  kinds = table("mounts", mount);
  return ([ ]) + kinds->query_default_kind() + kinds->query_kind(own[RIDE_KIND]) + own;
}

/**
 * The spec of a mount: its own keys over its kind's row over the default row.
 * @return nil when the object is not a mount
 */
mapping query_spec(object mount)
{
  return mount ? query_spec_of(mount, mount->query_rideable()) : nil;
}

/**
 * Whether the object can be ridden.
 */
int query_mount(object ob)
{
  return ob && mappingp(ob->query_rideable());
}

/**
 * Whether what somebody typed after a verb names this mount: nothing at all
 * does, and so does a name it answers to, after the little words of the
 * language (the prepositions of its kind) are dropped.
 */
int query_addressed(object mount, string str)
{
  mapping spec;
  string * words, * little;
  int i;

  spec = query_spec(mount);
  if (!spec)
    return 0;

  if (!str || !strlen(trim(str)))
    return 1;

  words = explode(trim(str), " ");
  little = spec["prepositions"];
  while (sizeof(words) > 1 && member_array(words[0], little) != -1)
    words = words[1 ..];

  return mount->id(implode(words, " "));
}

/**
 * The livings riding a mount, driver first. They are in the mount's place.
 */
object * query_riders(object mount)
{
  object * obs, * riders;
  int i, j;

  if (!mount || !environment(mount))
    return ({ });

  obs = all_inventory(environment(mount));
  riders = ({ });

  for (i = 0; i < sizeof(obs); i++)
    if (living(obs[i]) && obs[i]->query_mount() == mount)
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
 * Whether somebody sits in a seat of a mount.
 */
int query_seat_taken(object mount, int seat)
{
  object * riders;
  int i;

  riders = query_riders(mount);
  for (i = 0; i < sizeof(riders); i++)
    if ((int)riders[i]->query_seat() == seat)
      return 1;

  return 0;
}

/**
 * The first seat nobody sits in, or -1 when the mount is full.
 */
int query_free_seat(object mount)
{
  mapping spec;
  int i;

  spec = query_spec(mount);
  if (!spec)
    return -1;

  for (i = 0; i < (int)spec[RIDE_SEATS]; i++)
    if (!query_seat_taken(mount, i))
      return i;

  return -1;
}

/**
 * A text of the spec with the words of this ride put in. $N is the rider and
 * $S the mount's short in lower case; $T and $F are left for move_living.
 * @param keep_rider  1 to leave $N in, for the lines move_living writes
 */
private string text(mapping spec, string key, object who, object mount,
                    varargs int keep_rider)
{
  string str;

  str = spec[key];
  if (!stringp(str))
    return "";

  if (mount)
    str = replace_string(str, "$S", lower_case((string)mount->query_short()));
  if (who && !keep_rider)
    str = replace_string(str, "$N", (string)who->query_cap_name());

  return str;
}

string query_owner(object mount);
string query_mount_id(object mount);

/**
 * The reason somebody cannot take a seat of a mount, written for them, or nil
 * when they can. A driver who does not know the skill the mount teaches counts
 * as knowing it at the value it would be given.
 * @param seat  the seat wanted, or -1 for the first free one
 */
string query_board_refusal(object who, object mount, int seat)
{
  mapping spec, req;
  mixed * need;
  object race;
  int value, free_seat, * sizes;
  string refusal;

  spec = query_spec(mount);
  if (!spec || !who)
    return nil;

  if (who->query_dead())
    return text(spec, "refuse_dead", who, mount);

  if (who->query_property(PASSED_OUT_PROP))
    return text(spec, "refuse_passed_out", who, mount);

  if (who->query_mount())
    return text(spec, "refuse_already", who, mount);

  free_seat = query_free_seat(mount);
  if (free_seat == -1 ||
      (seat != -1 && (seat < 0 || seat >= (int)spec[RIDE_SEATS] ||
                      query_seat_taken(mount, seat))))
    return text(spec, "refuse_full", who, mount);

  if (seat == -1)
    seat = free_seat;

  // an owned mount is driven only by its owner; others may still be passengers
  if (seat == RIDING_DRIVER_SEAT && query_owner(mount) &&
      query_owner(mount) != (string)who->query_name())
    return text(spec, "refuse_not_owner", who, mount);

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
        return text(spec, "refuse_skill", who, mount);
    }

    sizes = req[RIDE_REQ_BODY_SIZE];
    if (pointerp(sizes) && sizeof(sizes) == 2 && who->query_race_ob())
    {
      race = load_object((string)who->query_race_ob());
      if (race && ((int)race->query_body_size() < sizes[0] ||
                   (int)race->query_body_size() > sizes[1]))
        return text(spec, "refuse_body_size", who, mount);
    }
  }

  // whatever else the mount itself wants to ask
  refusal = mount->mount_can_board(who, seat);
  if (stringp(refusal) && strlen(refusal))
    return refusal;

  return nil;
}

/**
 * Seats somebody on a mount, in the first free seat, and tells them and the
 * place. The refusal, if any, is told to the rider.
 * @return 1 when they are on it
 */
int board(object who, object mount)
{
  return board_seat(who, mount, -1);
}

/**
 * Seats somebody on a mount in the seat they want (-1: the first free one).
 * Whoever takes the driver's seat of a mount that teaches a skill and does not
 * know it learns it, at RIDING_LEARN_VALUE.
 * @return 1 when they are on it
 */
int board_seat(object who, object mount, int seat)
{
  mapping spec;
  string refusal;

  spec = query_spec(mount);
  if (!spec || !who || environment(who) != environment(mount))
    return 0;

  refusal = query_board_refusal(who, mount, seat);
  if (refusal)
  {
    tell_object(who, refusal);
    return 0;
  }

  if (seat == -1)
    seat = query_free_seat(mount);

  if (seat == RIDING_DRIVER_SEAT && stringp(spec[RIDE_LEARN]) &&
      !who->query_known_skill(spec[RIDE_LEARN]))
    who->set_skill_value(spec[RIDE_LEARN], RIDING_LEARN_VALUE);

  who->set_mount(mount, seat);
  who->add_extra_look(who);

  tell_object(who, text(spec, "board_me", who, mount));
  tell_room(environment(who), text(spec, "board_room", who, mount), ({ who }));

  mount->mount_boarded(who);
  return 1;
}

/**
 * Takes somebody off the mount they ride, with nothing but words: a fall does
 * no damage.
 * @param fall   1 to tell the fall instead of the dismount
 * @param quiet  1 to say nothing at all (a rider that died)
 * @return 1 when they were riding
 */
int dismount(object who, varargs int fall, int quiet)
{
  mapping spec;
  object mount;
  string mine, others;

  if (!who)
    return 0;

  mount = who->query_mount();
  if (!mount)
    return 0;

  spec = query_spec(mount);
  if (!spec)
    spec = table("mounts", who)->query_default_kind();

  who->clear_mount();
  who->remove_extra_look(who);

  if (!quiet)
  {
    mine = text(spec, fall ? "fall_me" : "dismount_me", who, mount);
    others = text(spec, fall ? "fall_room" : "dismount_room", who, mount);

    tell_object(who, mine);
    if (environment(who))
      tell_room(environment(who), others, ({ who }));
  }

  mount->mount_dismounted(who);
  return 1;
}

/**
 * Everybody on a mount falls: it died, was destructed, or was taken somewhere
 * they were not.
 * @param where  the place they are in, when the mount no longer is
 */
void unseat_riders(object mount, varargs object where)
{
  object * riders, env;
  int i;

  env = where ? where : environment(mount);
  if (!env)
    return;

  riders = all_inventory(env);
  for (i = 0; i < sizeof(riders); i++)
    if (living(riders[i]) && riders[i]->query_mount() == mount)
      dismount(riders[i], 1);
}

// --- Combat ---------------------------------------------------------------------

/**
 * Whether `who` owns `mount`: the owner of a bought mount, ridden or not.
 */
private int owns_mount(object who, object mount)
{
  string owner;

  if (!who || !mount || !mappingp(mount->query_rideable()))
    return 0;

  owner = query_owner(mount);
  return stringp(owner) && owner == (string)who->query_name();
}

/**
 * Whether two livings are a rider and its own mount: the one sits on the other
 * or owns it. They never attack each other.
 */
int query_mates(object a, object b)
{
  if (!a || !b)
    return 0;

  if (a->query_mount() == b || b->query_mount() == a)
    return 1;

  return owns_mount(a, b) || owns_mount(b, a);
}

/**
 * The refusal for `who` when it tries to attack `other`: they are a rider and
 * its own mount. Nil when they may fight.
 */
string query_attack_refusal(object who, object other)
{
  object mount;

  if (!query_mates(who, other))
    return nil;

  mount = mappingp(other->query_rideable()) ? other : who;
  return text(query_spec(mount), "refuse_attack", who, mount);
}

/**
 * Whether a mount takes part in the fights of its rider: it is a living, alive,
 * and its kind is linked to the fights.
 */
private int fights_beside_rider(object mount)
{
  mapping spec;

  if (!mount || !living(mount) || mount->query_dead())
    return 0;

  spec = query_spec(mount);
  return spec && spec[RIDE_COMBAT] == RIDE_COMBAT_LINKED;
}

/**
 * A rider has taken on `foe`, or `foe` has taken on the rider: the mount joins
 * the fight as one more combatant.
 */
void rider_fights(object rider, object foe)
{
  object mount;

  mount = rider ? rider->query_mount() : nil;
  if (!mount || !foe || query_mates(mount, foe) || !fights_beside_rider(mount) ||
      environment(mount) != environment(foe))
    return;

  mount->attack_ob(foe);
}

/**
 * `foe` has started on a rider. The mount joins the fight and, with the chance
 * of its kind (RIDE_PROTECT), takes the foe's blows in the rider's place.
 */
void rider_attacked(object rider, object foe)
{
  mapping spec;
  object mount;

  mount = rider ? rider->query_mount() : nil;
  if (!mount || !foe || query_mates(mount, foe) || !fights_beside_rider(mount) ||
      environment(mount) != environment(foe))
    return;

  spec = query_spec(mount);
  if (member_array(mount, foe->query_attacker_list()) == -1 &&
      random(100) < (int)spec[RIDE_PROTECT])
  {
    tell_room(environment(mount), text(spec, "protect_room", rider, mount));
    foe->stop_fight(rider, 1);
    foe->attack_ob(mount);
    return;
  }

  mount->attack_ob(foe);
}

/**
 * `foe` has started on a mount with somebody on it: those riding take it on.
 */
void mount_attacked(object mount, object foe)
{
  object * riders;
  int i;

  if (!foe || query_mates(mount, foe) || !fights_beside_rider(mount))
    return;

  riders = query_riders(mount);
  for (i = 0; i < sizeof(riders); i++)
    if (riders[i] != foe && environment(riders[i]) == environment(foe))
      riders[i]->attack_ob(foe);
}

/**
 * Whether `attacker` shares in the experience of a kill. A mount does not when
 * whoever rides it or owns it is among the `attackers`.
 */
int query_shares_xp(object attacker, object * attackers)
{
  int i;

  if (!attacker || !mappingp(attacker->query_rideable()))
    return 1;

  for (i = 0; i < sizeof(attackers); i++)
    if (attackers[i] != attacker && query_mates(attacker, attackers[i]))
      return 0;

  return 1;
}

/**
 * Whether `who` is something the mount fears: it answers to one of the names,
 * is of one of the race ids or carries one of the properties in the RIDE_FEARS
 * of the mount.
 */
int query_feared(object mount, object who)
{
  mapping spec;
  mixed * fears;
  int i;

  spec = query_spec(mount);
  if (!spec || !who || who == mount || query_mates(mount, who))
    return 0;

  fears = spec[RIDE_FEARS];
  if (!pointerp(fears))
    return 0;

  for (i = 0; i < sizeof(fears); i++)
    if (stringp(fears[i]) &&
        (who->id(fears[i]) || who->query_race_id() == fears[i] ||
         who->query_property(fears[i])))
      return 1;

  return 0;
}

/**
 * Whether a mount with somebody on it wants to run: it is hurt under its
 * wimpy (RIDE_WIMPY) or it is fighting what it fears. Only while ridden.
 */
int query_wants_to_flee(object mount)
{
  mapping spec;
  object * foes;
  int i, wimpy;

  if (!mount || !living(mount) || !sizeof(query_riders(mount)))
    return 0;

  spec = query_spec(mount);
  wimpy = (int)spec[RIDE_WIMPY];
  if (wimpy && (int)mount->query_hp() < (int)mount->query_max_hp() * wimpy / 100)
    return 1;

  foes = mount->query_attacker_list();
  for (i = 0; i < sizeof(foes); i++)
    if (query_feared(mount, foes[i]))
      return 1;

  return 0;
}

/**
 * Whether something the mount fears stands in its place.
 */
int query_fear_present(object mount)
{
  object * here;
  int i;

  if (!mount || !environment(mount))
    return 0;

  here = all_inventory(environment(mount));
  for (i = 0; i < sizeof(here); i++)
    if (living(here[i]) && !here[i]->query_dead() && query_feared(mount, here[i]))
      return 1;

  return 0;
}

/**
 * A mount runs from where it is through an exit its driver may take, chosen at
 * random, carrying whoever rides it. The driver does the walking, so the mount
 * and the passengers follow as in any ride.
 * @return 1 when it ran, 0 when there is nobody on it or nowhere it may go
 */
int run_away(object mount)
{
  mapping spec;
  object * riders;
  object driver, env;
  mixed * dirs;
  int i, n, start;

  riders = query_riders(mount);
  env = mount ? environment(mount) : nil;
  if (!sizeof(riders) || !env)
    return 0;

  driver = riders[0];
  dirs = env->query_dest_dir();
  n = sizeof(dirs) / 2;
  if (!n)
    return 0;

  spec = query_spec(mount);
  start = random(n);
  for (i = 0; i < n; i++)
  {
    string dir;

    dir = dirs[((start + i) % n) * 2];
    if (query_move_refusal(driver, env, dir))
      continue;

    tell_object(driver, text(spec, "flee_me", driver, mount));
    tell_room(env, text(spec, "flee_room", driver, mount), ({ driver }));

    if (env->do_exit_command("", dir, driver))
      return 1;
  }

  return 0;
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
  object mount, dest, kinds;
  string type, reason;
  mixed size;
  string * medium;

  mount = who ? who->query_mount() : nil;
  spec = query_spec(mount);
  if (!spec || !room)
    return nil;

  if (who->query_seat() != RIDING_DRIVER_SEAT)
    return text(spec, "refuse_passenger", who, mount);

  type = room->query_ex_type(dir);
  if (type && member_array(type, spec[RIDE_EXITS_DENY]) != -1)
    return text(spec, spec["refuse_" + type] ? "refuse_" + type : "refuse_exit",
                who, mount);

  if (type && pointerp(spec[RIDE_TERRAIN_ONLY]) && sizeof(spec[RIDE_TERRAIN_ONLY]) &&
      member_array(type, spec[RIDE_TERRAIN_ONLY]) == -1)
    return text(spec, "refuse_terrain", who, mount);

  // how big the exit is; an exit with no size does not hold it back
  size = room->query_size(dir);
  if (intp(size) && size > 0 && size < (int)spec[RIDE_EXIT_SIZE])
    return text(spec, "refuse_exit_size", who, mount);

  dest = room->query_dest_object(dir);
  if (!dest)
    return nil;

  if (member_array(RIDE_TERRAIN_INDOORS, spec[RIDE_TERRAIN_DENY]) != -1 &&
      !dest->query_outside())
    return text(spec, "refuse_indoors", who, mount);

  if (member_array(RIDE_TERRAIN_UNDERGROUND, spec[RIDE_TERRAIN_DENY]) != -1 &&
      dest->query_underground())
    return text(spec, "refuse_underground", who, mount);

  // the medium of the place beyond: a place on water needs a mount that crosses it
  medium = spec[RIDE_MEDIUM];
  if (dest->query_water_environment())
  {
    if (member_array(RIDE_MEDIUM_WATER, medium) == -1)
      return text(spec, "refuse_water", who, mount);
  }
  else if (member_array(RIDE_MEDIUM_GROUND, medium) == -1 &&
           member_array(RIDE_MEDIUM_AIR, medium) == -1)
    return text(spec, "refuse_ground", who, mount);

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
  object mount;

  mount = who ? who->query_mount() : nil;
  spec = query_spec(mount);
  if (!spec)
    return nil;

  return text(spec, arriving ? "arrive" : "leave", who, mount, 1);
}

/**
 * What follows a rider's short in a room: where they sit.
 */
string query_listing(object who)
{
  mapping spec;
  object mount;

  mount = who ? who->query_mount() : nil;
  spec = query_spec(mount);
  return spec ? text(spec, "listing", who, mount) : "";
}

/**
 * The line `look` adds to a rider.
 */
string query_look(object who)
{
  mapping spec;
  object mount;

  mount = who ? who->query_mount() : nil;
  spec = query_spec(mount);
  return spec ? text(spec, "look", who, mount) : "";
}

/**
 * The driver has just walked through an exit: the mount and the others in the
 * seats come after, in silence. If the mount cannot follow, the driver falls.
 * @param from  the place the driver left
 */
void carry(object who, object from)
{
  object mount, dest, * riders;
  int i;

  mount = who->query_mount();
  dest = environment(who);
  if (!mount || !dest || environment(mount) == dest)
    return;

  riders = query_riders(mount) - ({ who });

  carrying = ({ mount }) + riders;

  if (mount->move(dest) != MOVE_OK)
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
 * push): they fall where they land, and the mount stays where it was. A move
 * to the place they already are in (the login of a link-dead player is one)
 * throws nobody off.
 */
void rider_moved(object who, object from, object dest)
{
  if (member_array(who, carrying) != -1)
    return;

  if (from == dest)
    return;

  dismount(who, 1);
}

/**
 * A mount has been moved: unless it is following its driver, those on it were
 * left behind and fall.
 */
void mount_moved(object mount, object from, object dest)
{
  if (member_array(mount, carrying) != -1)
    return;

  unseat_riders(mount, from);
}

// --- The mounts a player owns -------------------------------------------------

/**
 * How many mounts a player may own at once; a game's handler may change it.
 */
int query_max_owned_mounts()
{
  return MAX_OWNED_MOUNTS;
}

/**
 * The file a mount is saved in (without the .o): one per mount, in the mounts
 * folder of its owner's save folder.
 */
string query_mount_file(string owner, string id)
{
  return player_save_dir(owner) + MOUNTS_SAVE_DIR + id;
}

/**
 * Who owns a mount, or nil.
 */
string query_owner(object mount)
{
  mapping own;

  own = mount ? mount->query_rideable() : nil;
  return mappingp(own) && stringp(own[RIDE_OWNER]) ? own[RIDE_OWNER] : nil;
}

/**
 * The id of a mount among its owner's, which is the name of its file, or nil.
 */
string query_mount_id(object mount)
{
  mapping own;

  own = mount ? mount->query_rideable() : nil;
  return mappingp(own) && stringp(own[RIDE_ID]) ? own[RIDE_ID] : nil;
}

/**
 * How many mounts a player owns.
 */
int query_owned_count(object owner)
{
  return owner ? sizeof(owner->query_mount_ids()) : 0;
}

// What names a place in a mount file: the file of a location, the path of a room.
private string place_of(object env)
{
  if (!env)
    return "";

  if (env->query_location())
    return (string)env->query_file_name();

  return file_name(env);
}

// The place a path names, or nil when it does not load.
private object load_place(string path)
{
  object place;

  if (!stringp(path) || !strlen(path))
    return nil;

  place = nil;
  if (strlen(path) > 2 && path[strlen(path) - 2 ..] == ".o")
    catch(place = LOCATION_HANDLER->load_location(path));
  else
    catch(place = load_object(path));

  return place;
}

// Empties the variables a mount file goes through.
private void clear_mount_vars()
{
  mount_template = nil;
  mount_file = nil;
  mount_owner = nil;
  mount_id = nil;
  mount_location = nil;
  mount_name = nil;
  mount_hp = 0;
}

/**
 * Writes the file of a mount: what it is made from, its owner and id, where it
 * stands, its short and how hurt it is.
 * @param own  its spec, for a provider that can no longer answer query_rideable
 *             (an NPC's components are let go before they clean up)
 */
private void write_mount(object mount, mapping own)
{
  string path, source, * pieces;

  path = query_mount_file(own[RIDE_OWNER], own[RIDE_ID]);
  pieces = explode(path, "/");
  mkdir(implode(pieces[.. sizeof(pieces) - 2], "/") + "/");

  clear_mount_vars();
  source = mount->query_npc_source();
  if (stringp(source) && strlen(source))
    mount_template = source;
  else
  {
    mount_file = base_name(mount);
    if (mount_file[strlen(mount_file) - 2 ..] != ".c")
      mount_file += ".c";
  }

  mount_owner = own[RIDE_OWNER];
  mount_id = own[RIDE_ID];
  mount_location = place_of(environment(mount));
  mount_name = (string)mount->query_short();
  mount_hp = (int)mount->query_hp();

  save_object(path, 1);
  clear_mount_vars();
}

/**
 * What a mount file holds, or nil when it has none.
 * @return ([ "template", "file", "location", "name", "hp" ])
 */
private mapping read_mount(string owner, string id)
{
  mapping data;
  string path;

  path = query_mount_file(owner, id);
  if (!file_exists(path + ".o"))
    return nil;

  clear_mount_vars();
  restore_object(path, 1);
  data = ([ "template" : mount_template, "file" : mount_file,
            "location" : mount_location, "name" : mount_name,
            "hp" : mount_hp ]);
  clear_mount_vars();
  return data;
}

// Ties a mount to its owner and to its file: both go in the spec the mount
// carries (the rideable component of an NPC, the spec of a vehicle).
private void mark_owner(object mount, string owner, string id)
{
  mapping own;

  own = mount->query_rideable();
  own = ([ ]) + (mappingp(own) ? own : ([ ])) + ([ RIDE_OWNER : owner, RIDE_ID : id ]);

  if (mount->query_component_by_type("rideable"))
    mount->add_component("rideable", own);
  else
    mount->set_rideable(own);
}

// Makes the mount a file describes, its owner's, and puts it in a place, ready
// to be ridden. Nil when it cannot be made or moved.
private object make_mount(object owner, string id, mapping data, object dest)
{
  object mount;
  int hp;

  mount = nil;
  if (stringp(data["template"]))
    mount = BESTIARY_HANDLER->spawn_from_template(game_name(dest), data["template"]);
  else if (stringp(data["file"]))
    catch(mount = clone_object(data["file"]));

  if (!mount)
    return nil;

  mark_owner(mount, (string)owner->query_name(), id);

  hp = data["hp"];
  if (hp > 0)
  {
    if (hp > (int)mount->query_max_hp())
      hp = (int)mount->query_max_hp();
    mount->set_hp(hp);
  }

  if (mount->move(dest) != MOVE_OK)
  {
    mount->dest_me();
    return nil;
  }

  // its components may need to see the world it now stands in
  mount->components_placed();
  owner->add_owned_mount(mount);
  return mount;
}

/**
 * The mounts a player owns, in the order they were bought, each
 * ([ "name" : its short, "mount" : the live one or nil ]). One whose file is
 * gone and is not out is left out.
 */
mapping * query_owned(object owner)
{
  mapping data;
  mapping * out;
  object * live;
  string * ids;
  int i, j;

  out = ({ });
  live = owner->query_owned_mounts();
  ids = owner->query_mount_ids();
  for (i = 0; i < sizeof(ids); i++)
  {
    for (j = 0; j < sizeof(live); j++)
      if (query_mount_id(live[j]) == ids[i])
        break;

    if (j < sizeof(live))
    {
      out += ({ ([ "name" : (string)live[j]->query_short(), "mount" : live[j] ]) });
      continue;
    }

    data = read_mount((string)owner->query_name(), ids[i]);
    if (data)
      out += ({ ([ "name" : data["name"], "mount" : nil ]) });
  }

  return out;
}

/**
 * Buys a mount: makes the one the stock entry names in `where`, saves its file
 * and adds it to the buyer's. Whoever sells has already asked for the price and
 * the cap (query_owned_count, query_max_owned_mounts).
 * @param stock  ([ "template" : id ]) or ([ "file" : path ])
 * @return the mount, or nil when the owner has too many or it cannot be made
 */
object add_mount(object owner, mapping stock, object where)
{
  mapping data;
  string name, id;
  object mount;

  if (!owner || !mappingp(stock) || !where)
    return nil;

  name = (string)owner->query_name();
  if (query_owned_count(owner) >= query_max_owned_mounts())
    return nil;

  if (!stringp(stock[MOUNT_TEMPLATE]) && !stringp(stock[MOUNT_FILE]))
    return nil;

  do
    id = "" + time() + "-" + random(1000000);
  while (file_exists(query_mount_file(name, id) + ".o") ||
         member_array(id, owner->query_mount_ids()) != -1);

  data = ([ "template" : stock[MOUNT_TEMPLATE], "file" : stock[MOUNT_FILE],
            "hp" : 0 ]);
  mount = make_mount(owner, id, data, where);
  if (!mount)
    return nil;

  write_mount(mount, mount->query_rideable());
  owner->add_mount_id(id);
  owner->save_me();
  return mount;
}

/**
 * An owned mount is going away (its owner quits, its place unloads): its file
 * is written where it stands, so its owner finds it at the next login. A mount
 * that died has no file left and is not written again.
 *
 * @param mount  the mount going
 * @param own    optional: its spec, for a provider that can no longer answer
 *               query_rideable (an NPC's components are let go before they
 *               clean up)
 */
void mount_gone(object mount, varargs mapping own)
{
  string owner, id;

  owner = own ? own[RIDE_OWNER] : query_owner(mount);
  id = own ? own[RIDE_ID] : query_mount_id(mount);
  if (!stringp(owner) || !stringp(id) || !environment(mount) ||
      !file_exists(query_mount_file(owner, id) + ".o"))
    return;

  write_mount(mount, own ? own : mount->query_rideable());
}

/**
 * An owned mount has died: its file goes and it is not the owner's any more.
 */
void mount_died(object mount)
{
  object owner;
  string name, id;

  name = query_owner(mount);
  id = query_mount_id(mount);
  if (!name || !id)
    return;

  rm(query_mount_file(name, id) + ".o");

  owner = find_player(name);
  if (owner)
  {
    owner->remove_mount_id(id);
    owner->save_me();
  }
}

/**
 * A player quits: they get off whatever they ride, and every mount of theirs
 * that is out is written where it stands and taken away.
 */
void owner_quit(object owner)
{
  object * live;
  int i;

  if (!owner)
    return;

  dismount(owner);

  // mount_gone writes each file
  live = owner->query_owned_mounts();
  for (i = 0; i < sizeof(live); i++)
    live[i]->dest_me();
}

/**
 * A player logs in: each mount of theirs is made again where it stood, or
 * beside them when that place does not load. A file that is gone leaves their
 * list.
 */
void owner_enter(object owner)
{
  mapping data;
  object * live, dest;
  string name, * ids;
  int i, j, changed;

  if (!owner || !environment(owner) || !strlen((string)owner->query_name()))
    return;

  name = (string)owner->query_name();
  live = owner->query_owned_mounts();
  ids = owner->query_mount_ids();
  for (i = 0; i < sizeof(ids); i++)
  {
    for (j = 0; j < sizeof(live); j++)
      if (query_mount_id(live[j]) == ids[i])
        break;

    if (j < sizeof(live))
      continue;

    data = read_mount(name, ids[i]);
    if (!data)
    {
      owner->remove_mount_id(ids[i]);
      changed = 1;
      continue;
    }

    dest = load_place(data["location"]);
    if (!dest)
      dest = environment(owner);

    make_mount(owner, ids[i], data, dest);
  }

  if (changed)
    owner->save_me();
}
