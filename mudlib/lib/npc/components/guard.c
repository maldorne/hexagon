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
// which this component reads through query_owner().
//
// Who is stopped is a set of rules in the attributes; a mover goes through only
// if every rule the guard has lets them. Without attributes it stops the
// citizenships at war with its own, at the exit its post names.
//
// Attributes:
//   "direction"       - the exit it watches; without it, the one the
//                       town_entrance POI of its post names
//   "enemies"         - 0 to stop nobody for their citizenship (default 1:
//                       those at war with the guard's)
//   "guilds"          - guild objects whose members pass; anybody else is
//                       stopped
//   "guildless"       - 1 to let those with no guild through the guild rules
//   "min_guild_level" - the guild level a member needs to pass
//   "min_class_level" - the class level anybody needs to pass
//   "message"         - what whoever it stops is told

#include <areas/diplomacy.h>
#include <areas/poi.h>
#include <room/location.h>

inherit component "/lib/npc/component.c";

private string guard_direction, guard_message;
private int guard_enemies, guard_guildless, guard_min_guild_level,
            guard_min_class_level;
private string * guard_guilds;

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
}

// A guild object named with or without its extension is the same guild.
private string _guild_path(string path)
{
  if (!path || !strlen(path))
    return "";
  if (path[0] != '/')
    path = "/" + path;
  if (strlen(path) > 2 && path[strlen(path) - 2 ..] == ".c")
    path = path[0 .. strlen(path) - 3];
  return path;
}

// The exit this guard watches: its own "direction" when it has one, otherwise
// read off the post it works at rather than stored: a town_entrance POI names
// the way in, and a guard is posted to a place before it is posted to a
// direction. Empty at a square, where the watch is presence only and nobody is
// stopped.
private string _watched_direction()
{
  object me, post;
  mapping poi;
  mixed work;

  if (guard_direction && strlen(guard_direction))
    return guard_direction;

  me = query_owner();
  work = me ? me->query_work() : nil;
  if (!stringp(work) || !strlen(work))
    return "";

  post = (object)load_object(LOCATION_HANDLER)->load_location(work);
  if (!post || !post->query_area())
    return "";

  poi = ((mapping)post->query_area()->query_pois())[work];
  if (!poi || poi[POI_FIELD_KIND] != POI_KIND_TOWN_ENTRANCE)
    return "";

  return poi[POI_FIELD_GUARD_DIR] ? poi[POI_FIELD_GUARD_DIR] : "";
}

string query_direction() { return _watched_direction(); }

// Called once the NPC is standing in the world. Register on the exit so the
// exit handler consults us; it checks we are present before asking, so this
// holds across the walk to the barracks and back. A guard with its own
// direction watches it where it stands; one posted to a town entrance watches
// it at its post.
void placed()
{
  object me, post;
  string dir;

  dir = _watched_direction();
  me = query_owner();
  if (!strlen(dir) || !me)
    return;

  if (guard_direction && strlen(guard_direction))
    post = environment(me);
  else
    post = (object)load_object(LOCATION_HANDLER)->load_location(
             (string)me->query_work());
  if (post)
    post->register_guard(me, dir);
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

  guild = _guild_path(mover->query_guild_ob());
  if (!strlen(guild))
    return guard_guildless;

  if (sizeof(guard_guilds))
  {
    member = 0;
    for (i = 0; i < sizeof(guard_guilds); i++)
      if (_guild_path(guard_guilds[i]) == guild)
        member = 1;
    if (!member)
      return 0;
  }

  return (int)mover->query_guild_level() >= guard_min_guild_level;
}

// Blocks citizenships this guard's citizenship is at war with, so a diplomacy
// change flips who is stopped without touching the guard. Either side lacking a
// citizenship, the mover passes.
private int _passes_citizenship(object mover)
{
  string my_city_ob, mover_city_ob;
  object me, my_citizenship, mover_citizenship;

  me = query_owner();
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

// Let `mover` through the watched exit? Only if every rule lets them. With no
// direction posted, the mover passes.
int check(object mover)
{
  if (!strlen(_watched_direction()) || !mover || !query_owner())
    return 1;

  if (guard_min_class_level &&
      (int)mover->query_class_level() < guard_min_class_level)
    return 0;

  return _passes_guild(mover) && _passes_citizenship(mover);
}

// Message shown when the guard stops someone, as written: a guard that speaks
// says so in it. nil falls back to the exit handler's default.
string message()
{
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
  if (stringp(args["message"]))
    guard_message = args["message"];
}

mixed * stats()
{
  return component::stats() + ({
    ({ "Direction", _watched_direction() }),
    ({ "Enemies", guard_enemies }),
    ({ "Guilds", implode(guard_guilds, ", ") }),
    ({ "Guildless", guard_guildless }),
    ({ "Min Guild Level", guard_min_guild_level }),
    ({ "Min Class Level", guard_min_class_level }),
  });
}
