// guard.c -- the guard-role behaviour, as an NPC component.
//
// Watching an exit and stopping enemy citizenships is a role, not a race or a
// class, so it is a component an NPC can be given (a citizenship's guards get it
// when the area posts them) rather than a base class. This replaces the old
// /lib/npc/guardian.c mixin and its /lib/guard.c base: a guard is now a plain
// generic NPC carrying this component.
//
// The room side is unchanged -- the exit handler still calls
// guard_npc->guardian_check(mover) / guardian_message(); npc.c proxies those to
// this component. The composing NPC supplies query_city_ob() (its citizenship),
// which this component reads through query_entity().
//
// Who is stopped is a set of rules in the attributes; a mover goes through only
// if every rule the guard has lets them. Without attributes it stops the
// citizenships at war with its own, at the exit its post names.
//
// Attributes:
//   "direction"       - the exits it watches, comma-separated; without it,
//                       the ones the town_entrance POI of its post names
//   "enemies"         - 0 to stop nobody for their citizenship (default 1:
//                       those at war with the guard's)
//   "guilds"          - guild objects whose members pass; anybody else is
//                       stopped
//   "guildless"       - 1 to let those with no guild through the guild rules
//   "min_guild_level" - the guild level a member needs to pass
//   "min_class_level" - the class level anybody needs to pass
//   "min_align"       - the real alignment anybody needs to pass (0: not evil)
//   "barred_races"    - race ids (a lineage or its base race) never let through
//   "night_admit"     - "citizens": from nightfall to dawn only fellow citizens
//                       pass
//   "message"         - what whoever it stops is told
//
// The POI of its post adds one more: a town_entrance that admits only citizens
// (POI_FIELD_ADMIT) lets nobody else through. Defending people is the protector
// component's.
//
// The law: where the place has law, the guard attacks on sight whoever the
// crimes handler holds wanted or outlaw in its citizenship (when it or the
// culprit arrives, or when it is placed), and whoever commits a crime in front
// of it. A post that watches an exit also refuses that exit to the wanted. A
// guard sent to a crime elsewhere is driven by the responder component.

#include <areas/diplomacy.h>
#include <areas/poi.h>
#include <room/location.h>
#include <crimes.h>
#include <language.h>

inherit component "/lib/npc/component.c";

private string guard_direction, guard_message;
private int guard_enemies, guard_guildless, guard_min_guild_level,
            guard_min_class_level;
private string * guard_guilds;
// the least real alignment that passes, nil when the guard does not look at it
private mixed guard_min_align;
// race ids it never lets through; who it admits at night ("" for anybody)
private string * guard_barred_races;
private string guard_night_admit;
// what the last refused mover is told; scan_pending: a scan is already queued
static string refusal;
static int scan_pending;

void create()
{
  component::create();
  guard_direction = nil;
  guard_message = nil;
  guard_enemies = 1;
  guard_guildless = 0;
  guard_min_guild_level = 0;
  guard_min_class_level = 0;
  guard_guilds = ({ });
  guard_min_align = nil;
  guard_barred_races = ({ });
  guard_night_admit = "";
  refusal = nil;
  scan_pending = 0;
}

// The POI of the post this guard works at, or nil.
private mapping _post_poi()
{
  object me, post;
  mixed work;

  me = query_entity();
  work = me ? me->query_work() : nil;
  if (!stringp(work) || !strlen(work))
    return nil;

  post = (object)load_object(LOCATION_HANDLER)->load_location(work);
  if (!post || !post->query_area())
    return nil;

  return ((mapping)post->query_area()->query_pois())[work];
}

// The exits this guard watches: its own "direction" when it has one, otherwise
// read off the post it works at rather than stored: a town_entrance POI names
// the ways in, and a guard is posted to a place before it is posted to a
// direction. Empty at a square, where the watch is presence only and nobody is
// stopped. Several exits are written comma-separated.
private string * _watched_directions()
{
  mapping poi;
  string dirs;

  if (guard_direction && strlen(guard_direction))
    dirs = guard_direction;
  else
  {
    poi = _post_poi();
    if (!poi || poi[POI_FIELD_KIND] != POI_KIND_TOWN_ENTRANCE ||
        !stringp(poi[POI_FIELD_GUARD_DIR]))
      return ({ });
    dirs = poi[POI_FIELD_GUARD_DIR];
  }

  return explode(dirs, ",") - ({ "" });
}

string query_direction() { return implode(_watched_directions(), ","); }

// The name of the guard's own citizenship in the diplomacy graph, "" when it
// has none.
private string _citizenship_name()
{
  object me, citizenship;
  string city_ob;

  me = query_entity();
  city_ob = me ? me->query_city_ob() : nil;
  if (!city_ob || !strlen(city_ob))
    return "";

  citizenship = load_object(city_ob);
  return citizenship ? citizenship->query_name() : "";
}

// Whether `who` is somebody the law of the guard's place wants: wanted or
// outlaw in the guard's citizenship, where the place has law at all.
private int _is_wanted(object who)
{
  object me, crimes;
  string citizenship, status;

  me = query_entity();
  if (!me || !who || who == me || !living(who) || who->query_dead())
    return 0;
  if (!environment(me))
    return 0;

  citizenship = _citizenship_name();
  if (!strlen(citizenship))
    return 0;

  crimes = handler("crimes", environment(me));
  if (!crimes || !strlen((string)crimes->query_jurisdiction(environment(me))))
    return 0;

  // a prisoner serving a sentence has already been dealt with
  if ((int)crimes->query_jail_left(who) > 0)
    return 0;

  status = (string)crimes->query_status(who, citizenship);
  return status == CRIME_STATUS_WANTED || status == CRIME_STATUS_OUTLAW;
}

// Attacks `who` if they are still here, unhidden, not already being fought
// and still wanted (or `known`, a culprit seen committing a crime just now).
private void _arrest(object who, int known)
{
  object me;

  me = query_entity();
  if (!me || !who || me->query_dead() || !environment(me) ||
      environment(who) != environment(me))
    return;
  if (who->query_hidden() || who->query_dead())
    return;
  if (member_array(who, me->query_attacker_list()) >= 0)
    return;
  if (!known && !_is_wanted(who))
    return;

  if (!me->query_timed_property("guard_halt"))
  {
    me->do_say(_LANG_GUARD_WANTED_SHOUT);
    me->add_timed_property("guard_halt", 1, 20);
  }
  me->attack_ob(who);
}

// Queued from the events, which never act inside themselves.
void react(object who)
{
  _arrest(who, 0);
}

// A culprit seen committing a crime: attacked wanted yet or not.
void react_known(object criminal)
{
  _arrest(criminal, 1);
}

// Looks over everybody in the guard's place and attacks the wanted.
void scan()
{
  object me;
  object * inv;
  int i;

  scan_pending = 0;
  me = query_entity();
  if (!me || !environment(me))
    return;

  inv = all_inventory(environment(me)) - ({ me });
  for (i = 0; i < sizeof(inv); i++)
    if (inv[i] && living(inv[i]) && _is_wanted(inv[i]))
      _arrest(inv[i], 0);
}

// Queues a scan unless one is already waiting.
private void _queue_scan()
{
  if (scan_pending)
    return;

  scan_pending = 1;
  call_out("scan", 1);
}

/**
 * Somebody has arrived: if the law wants them, the guard goes for them.
 */
void event_enter(object ob, varargs string mess, object from, mixed avoid)
{
  if (ob && living(ob) && _is_wanted(ob))
    call_out("react", 1, ob);
}

/**
 * The guard itself has moved: whoever it finds in the new place is looked over.
 */
void event_moved(object ob, object from, object dest)
{
  _queue_scan();
}

/**
 * A crime was committed in the guard's place. With a known culprit and points
 * under the law of the guard's own country, the guard attacks the culprit.
 */
void event_crime(object who, object criminal, string type, object victim,
                 int points)
{
  object me, diplomacy;
  string mine, law;

  me = query_entity();
  if (!me || !criminal || points <= 0 || criminal == me || !environment(me))
    return;

  mine = _citizenship_name();
  if (!strlen(mine))
    return;

  law = (string)handler("crimes", environment(me))->query_jurisdiction(
          environment(me));
  if (!strlen(law))
    return;

  // only the guards of the country whose law was broken
  diplomacy = handler("diplomacy", environment(me));
  if (diplomacy->query_root_citizenship(mine) !=
      diplomacy->query_root_citizenship(law))
    return;

  call_out("react_known", 1, criminal);
}

// Called once the NPC is standing in the world. Register on the exit so the
// exit handler consults us; it checks we are present before asking, so this
// holds across the walk to the barracks and back. A guard with its own
// direction watches it where it stands; one posted to a town entrance watches
// it at its post.
void placed()
{
  object me, post;
  string * dirs;
  int i;

  me = query_entity();
  if (!me)
    return;

  _queue_scan();

  if (guard_direction && strlen(guard_direction))
  {
    post = environment(me);
    dirs = _watched_directions();
    for (i = 0; post && i < sizeof(dirs); i++)
      post->register_guard(me, dirs[i]);
    return;
  }

  // The direction of a guard posted to a town entrance is read off its post,
  // which may not be loaded, and loading it places more people: even asking
  // for it is left to an execution of its own.
  call_out("register_at_post", 0);
}

/**
 * Registers the guard on the exits it watches at its town entrance post,
 * loading the post if needed.
 */
void register_at_post()
{
  object me, post;
  string * dirs;
  int i;

  me = query_entity();
  dirs = _watched_directions();
  if (!me || !sizeof(dirs))
    return;

  post = (object)load_object(LOCATION_HANDLER)->load_location(
           (string)me->query_work());
  for (i = 0; post && i < sizeof(dirs); i++)
    post->register_guard(me, dirs[i]);
}

// The guild rules: the guilds whose members pass, and the level a member needs.
// Somebody with no guild passes them only when the guard lets the guildless
// through.
private int _passes_guild(object mover)
{
  string guild;
  int i, member;

  if (!sizeof(guard_guilds) && !guard_min_guild_level)
    return 1;

  guild = mover->query_guild_ob();
  if (!guild || !strlen(guild))
    return guard_guildless;

  if (sizeof(guard_guilds) && member_array(guild, guard_guilds) == -1)
    return 0;

  return (int)mover->query_guild_level() >= guard_min_guild_level;
}

// Blocks citizenships this guard's citizenship is at war with, so a diplomacy
// change flips who is stopped without touching the guard. Either side lacking a
// citizenship, the mover passes.
private int _passes_citizenship(object mover)
{
  string my_city_ob, mover_city_ob;
  object me, my_citizenship, mover_citizenship;

  me = query_entity();
  if (!guard_enemies)
    return 1;

  // a mover with no citizenship is neutral and passes
  my_city_ob = me->query_city_ob();
  mover_city_ob = mover->query_city_ob();
  if (!my_city_ob || !mover_city_ob)
    return 1;

  // city_ob is the citizenship object's path; load it and read its name and
  // game -- diplomacy works in names, and resolves the parent cascade
  my_citizenship = load_object(my_city_ob);
  mover_citizenship = load_object(mover_city_ob);
  if (!my_citizenship || !mover_citizenship)
    return 1;

  return !handler("diplomacy", my_citizenship)->is_enemy(
             my_citizenship->query_name(),
             mover_citizenship->query_name());
}

// Is `who` of the guard's citizenship or of one under it? Somebody with no
// citizenship is not.
private int _is_fellow_citizen(object who)
{
  object me, diplomacy, citizenship;
  string my_city_ob, their_city_ob, mine, name;

  me = query_entity();
  my_city_ob = me ? me->query_city_ob() : nil;
  their_city_ob = who->query_city_ob();
  if (!my_city_ob || !their_city_ob)
    return 0;

  citizenship = load_object(my_city_ob);
  if (!citizenship || !load_object(their_city_ob))
    return 0;
  mine = citizenship->query_name();
  diplomacy = handler("diplomacy", citizenship);
  name = load_object(their_city_ob)->query_name();

  // walk up from their citizenship to the top
  while (name && strlen(name))
  {
    if (name == mine)
      return 1;
    name = diplomacy->query_parent(name);
  }

  return 0;
}

// A post that admits only citizens lets through the guard's citizenship and
// those under it; a mover with no citizenship is a stranger there.
private int _passes_admission(object mover)
{
  mapping poi;

  poi = _post_poi();
  if (!poi || poi[POI_FIELD_ADMIT] != POI_ADMIT_CITIZENS)
    return 1;
  if (!query_entity()->query_city_ob())
    return 1;

  return _is_fellow_citizen(mover);
}

// Whether `mover` is of a people the guard never lets through, by its lineage
// or by the race the lineage belongs to.
private int _is_barred_race(object mover)
{
  if (!sizeof(guard_barred_races))
    return 0;

  return member_array((string)mover->query_race_id(), guard_barred_races) != -1 ||
         member_array((string)mover->query_base_race_id(), guard_barred_races) != -1;
}

// At night a post that admits only citizens then lets nobody else through.
private int _passes_night(object mover)
{
  object weather;

  if (guard_night_admit != POI_ADMIT_CITIZENS)
    return 1;

  weather = handler("weather", query_entity());
  if (!weather || weather->query_day())
    return 1;

  return _is_fellow_citizen(mover);
}

// Let `mover` through a watched exit? Only if every rule lets them. With no
// direction posted, the mover passes.
int check(object mover)
{
  refusal = nil;

  if (!sizeof(_watched_directions()) || !mover || !query_entity())
    return 1;

  // the wanted do not pass the gate
  if (_is_wanted(mover))
  {
    refusal = query_entity()->query_cap_name() + ": " +
              _LANG_GUARD_WANTED_BLOCKED + "\n";
    return 0;
  }

  if (guard_min_class_level &&
      (int)mover->query_class_level() < guard_min_class_level)
    return 0;

  if (intp(guard_min_align) &&
      (int)mover->query_real_align() < guard_min_align)
    return 0;

  if (_is_barred_race(mover))
  {
    refusal = query_entity()->query_cap_name() + ": " +
              _LANG_GUARD_RACE_BLOCKED + "\n";
    return 0;
  }

  if (!_passes_night(mover))
  {
    refusal = query_entity()->query_cap_name() + ": " +
              _LANG_GUARD_NIGHT_BLOCKED + "\n";
    return 0;
  }

  return _passes_guild(mover) && _passes_citizenship(mover) &&
         _passes_admission(mover);
}

// Message shown when the guard stops someone, as written: a guard that speaks
// says so in it. nil falls back to the exit handler's default.
string message()
{
  if (refusal)
    return refusal;

  if (!guard_message || !strlen(guard_message))
    return nil;
  return guard_message + "\n";
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() + ([
    "direction"       : guard_direction,
    "enemies"         : guard_enemies,
    "guilds"          : guard_guilds,
    "guildless"       : guard_guildless,
    "min_guild_level" : guard_min_guild_level,
    "min_class_level" : guard_min_class_level,
    "min_align"       : guard_min_align,
    "barred_races"    : guard_barred_races,
    "night_admit"     : guard_night_admit,
    "message"         : guard_message,
  ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;

  if (stringp(args["direction"]))
    guard_direction = args["direction"];
  if (intp(args["enemies"]) && !undefinedp(args["enemies"]))
    guard_enemies = args["enemies"];
  if (pointerp(args["guilds"]))
    guard_guilds = ({ }) + args["guilds"];
  if (intp(args["guildless"]))
    guard_guildless = args["guildless"];
  if (intp(args["min_guild_level"]))
    guard_min_guild_level = args["min_guild_level"];
  if (intp(args["min_class_level"]))
    guard_min_class_level = args["min_class_level"];
  if (intp(args["min_align"]))
    guard_min_align = args["min_align"];
  if (pointerp(args["barred_races"]))
    guard_barred_races = ({ }) + args["barred_races"];
  if (stringp(args["night_admit"]))
    guard_night_admit = args["night_admit"];
  if (stringp(args["message"]))
    guard_message = args["message"];
}

mixed * stats()
{
  return component::stats() + ({
    ({ "Direction", query_direction() }),
    ({ "Enemies", guard_enemies }),
    ({ "Guilds", implode(guard_guilds, ", ") }),
    ({ "Guildless", guard_guildless }),
    ({ "Min Guild Level", guard_min_guild_level }),
    ({ "Min Class Level", guard_min_class_level }),
    ({ "Min Align", intp(guard_min_align) ? "" + guard_min_align : "-" }),
    ({ "Barred Races", implode(guard_barred_races, ", ") }),
    ({ "Night Admit", guard_night_admit }),
  });
}
