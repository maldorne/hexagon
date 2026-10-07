// Recall item: carries its owner back to a place they marked. The mechanics
// live here and nothing of a setting does: every line the player reads is a
// message the item that inherits this one can replace (set_message), and so
// are the verbs (set_travel_verbs, set_mark_verbs). A fantasy game makes it a
// magic stone, a science fiction one a beacon.
//
//   mark    -- in a place that allows it (an inn, by default), remember it
//   travel  -- after a few seconds of standing still, go back to it
//
// The cooldown is kept on the owner, not on the item, so it survives the item
// being replaced and the character logging out.
//
// Based on the hearthstone for Ciudad Capital, neverbot 17/04/09.

#include <mud/mudos.h>
#include <room/location.h>
#include <language.h>

inherit "/lib/item.c";

// property on the owner while the item cannot be used again
#define RECALL_LOCK "recall_lock"

static int acting;
static int channel_time;
static int cooldown;
static string * travel_verbs;
static string * mark_verbs;
static mapping messages;

string destination_path;
string destination_name;

int query_recall() { return 1; }

void set_destination(string path, string name)
{
  destination_path = path;
  destination_name = name;
}
string query_destination_path() { return destination_path; }
string query_destination_name() { return destination_name; }

// seconds the owner has to stand still before travelling
void set_channel_time(int seconds) { channel_time = seconds; }
int query_channel_time() { return channel_time; }

// seconds of play before the item can be used again
void set_cooldown(int seconds) { cooldown = seconds; }
int query_cooldown() { return cooldown; }

void set_travel_verbs(string * verbs) { travel_verbs = verbs; }
void set_mark_verbs(string * verbs) { mark_verbs = verbs; }

// A message the player reads, by key. $mcname$ is the owner's name and $place$
// the marked place's.
void set_message(string key, string text) { messages[key] = text; }

string query_message(string key, varargs object who)
{
  string text;

  text = messages[key];
  if (!text)
    return "";

  if (who)
    text = replace_string(text, "$mcname$", who->query_cap_name());
  return replace_string(text, "$place$",
                        strlen(destination_name) ? destination_name : "");
}

void create()
{
  acting = 0;
  destination_path = "";
  destination_name = "";
  channel_time = 10;
  cooldown = 3600;
  travel_verbs = _LANG_RECALL_TRAVEL_VERBS;
  mark_verbs = _LANG_RECALL_MARK_VERBS;

  messages = ([
    "acting":        _LANG_RECALL_ACTING,
    "dead":          _LANG_RECALL_DEAD,
    "combat":        _LANG_RECALL_COMBAT,
    "cooldown":      _LANG_RECALL_COOLDOWN,
    "forgotten":     _LANG_RECALL_FORGOTTEN,
    "origin":        _LANG_RECALL_ORIGIN,
    "start_me":      _LANG_RECALL_START_ME,
    "start_room":    _LANG_RECALL_START_ROOM,
    "moved":         _LANG_RECALL_MOVED,
    "died":          _LANG_RECALL_DIED,
    "error":         _LANG_RECALL_ERROR,
    "arrive_me":     _LANG_RECALL_ARRIVE_ME,
    "arrive_room":   _LANG_RECALL_ARRIVE_ROOM,
    "leave_room":    _LANG_RECALL_LEAVE_ROOM,
    "mark_what":     _LANG_RECALL_MARK_WHAT,
    "mark_not_here": _LANG_RECALL_MARK_NOT_HERE,
    "marked":        _LANG_RECALL_MARKED,
    "marked_info":   _LANG_RECALL_MARKED_INFO,
    "unmarked_info": _LANG_RECALL_UNMARKED_INFO,
    "help":          _LANG_RECALL_HELP,
  ]);

  ::create();

  reset_drop();
  set_weight(1);
}

string query_help(varargs string str) { return query_message("help"); }

string long(string s, int dark)
{
  return ::long(s, dark) +
    query_message(strlen(destination_name) ? "marked_info" : "unmarked_info");
}

void init()
{
  add_action("do_travel", travel_verbs);
  add_action("do_mark", mark_verbs);
  ::init();
}

// Whether the owner may mark `place`: an inn, by default -- a legacy room that
// is a pub, or a location with the pub component.
int can_mark_here(object place)
{
  if (!place)
    return 0;
  if (place->query_location())
    return place->query_component_by_type(LOCATION_COMPONENT_PUB) != nil;
  return place->query_pub();
}

// The file a place is saved under: a location's .o, a room's source.
private string place_file(object place)
{
  if (place->query_location())
    return place->query_file_name();
  return base_name(place);
}

// Load a remembered place. A room that has been converted since it was marked
// is replaced by its location.
object load_destination(string path)
{
  object room;
  string location_file;

  if (!stringp(path) || !strlen(path))
    return nil;

  if (path[strlen(path) - 2..] == ".o")
    return load_object(LOCATION_HANDLER)->load_location(path);

  catch(room = load_object(path));
  if (!room)
    return nil;

  location_file = load_object(LOCATION_HANDLER)->get_location_file_name_from_room(room);
  if (location_file && file_size(location_file) > 0)
    return load_object(LOCATION_HANDLER)->load_location(location_file);

  return room;
}

// Where an owner with nothing marked goes back to: their citizenship's first
// room, else their race's -- and only if it belongs to the game they play.
private string origin_of(object who)
{
  string * paths;
  string path;
  int i;

  paths = ({ });
  if (who->query_city_ob())
    catch(paths += ({ load_object(who->query_city_ob())->query_init_room() }));
  if (who->query_race_ob())
    catch(paths += ({ load_object(who->query_race_ob())->query_init_room() }));

  for (i = 0; i < sizeof(paths); i++)
  {
    path = paths[i];
    if (stringp(path) && strlen(path) && game_from_path(path) == game_name(who))
      return path;
  }

  return nil;
}

// Common refusals for both verbs. 1 if the owner may go on.
private int owner_can_act(object who)
{
  if (acting)
    return notify_fail(query_message("acting")), 0;
  if (who->query_dead())
    return notify_fail(query_message("dead")), 0;
  if (who->query_fighting())
    return notify_fail(query_message("combat")), 0;
  return 1;
}

int do_travel(string str)
{
  object who, dest;
  string origin;

  who = this_player();

  // a bare verb, or the verb on this item
  if (str && strlen(str) && !id(str))
    return 0;

  if (!owner_can_act(who))
    return 0;

  if (who->query_timed_property(RECALL_LOCK))
  {
    notify_fail(query_message("cooldown"));
    return 0;
  }

  dest = load_destination(destination_path);
  if (!dest)
  {
    origin = origin_of(who);
    if (!origin || !load_destination(origin))
    {
      notify_fail(query_message("forgotten"));
      return 0;
    }
    tell_object(who, query_message("origin"));
    destination_path = origin;
    destination_name = "";
  }

  tell_object(who, query_message("start_me", who));
  tell_room(environment(who), query_message("start_room", who), who);

  acting = 1;
  call_out("continue_travel", 1, who, channel_time, environment(who));
  return 1;
}

void continue_travel(object who, int count, object where)
{
  object dest;

  // the owner quit, or the item left them
  if (!who || environment(this_object()) != who)
  {
    acting = 0;
    return;
  }

  if (who->query_dead())
  {
    tell_object(who, query_message("died"));
    acting = 0;
    return;
  }

  if (who->query_fighting())
  {
    tell_object(who, query_message("combat"));
    acting = 0;
    return;
  }

  if (environment(who) != where)
  {
    tell_object(who, query_message("moved"));
    acting = 0;
    return;
  }

  if (count > 0)
  {
    tell_object(who, query_short() + ": [" +
      sprintf("%p%-*s", ' ', channel_time,
              sprintf("%p%*s", '*', channel_time - count, "")) + "]\n");
    call_out("continue_travel", 1, who, count - 1, where);
    return;
  }

  acting = 0;
  dest = load_destination(destination_path);
  if (!dest)
  {
    tell_object(who, query_message("error"));
    return;
  }

  // Mounts do not travel: nobody should arrive at an inn on horseback. Kept
  // until mounts work again.
  // if (who->query_riding())
  //   who->destruct_ride_shadow();

  tell_room(where, query_message("leave_room", who), who);
  who->move(dest);
  who->add_timed_property(RECALL_LOCK, 1, cooldown / HEART_BEAT_TIME);

  tell_object(who, query_message("arrive_me", who));
  who->do_look();
  tell_room(dest, query_message("arrive_room", who), who);
}

int do_mark(string str)
{
  object who, place;

  who = this_player();

  if (!str || !id(str))
  {
    notify_fail(query_message("mark_what"));
    return 0;
  }

  if (!owner_can_act(who))
    return 0;

  place = environment(who);
  if (!can_mark_here(place))
  {
    notify_fail(query_message("mark_not_here"));
    return 0;
  }

  // a location composes its name in short(); query_short() is only its own
  set_destination(place_file(place), place->short());
  tell_object(who, query_message("marked", who));
  return 1;
}

// Summoning the owner's mount, kept until mounts work again. It shared the
// cooldown with travelling, worked only outdoors and out of water, and took
// the mount from whoever was riding it.
//
// int do_invoke(string str)
// {
//   object ob;
//
//   if (member_array(str, _LANG_HEARTHSTONE_INVOKE_NAMES) == -1)
//     return notify_fail(_LANG_HEARTHSTONE_INVOKE_WHAT), 0;
//   if (!this_player()->query_mount())
//     return notify_fail(_LANG_HEARTHSTONE_INVOKE_NO_MOUNT), 0;
//   if (!environment(this_player())->query_outside())
//     return notify_fail(_LANG_HEARTHSTONE_INVOKE_OUTSIDE), 0;
//   if (environment(this_player())->query_water_environment())
//     return notify_fail(_LANG_HEARTHSTONE_INVOKE_NO_WATER), 0;
//   if (this_player()->query_timed_property(RECALL_LOCK))
//     return notify_fail(query_message("cooldown")), 0;
//
//   ob = this_player()->query_mount();
//   if (environment(this_player()) == environment(ob))
//     return notify_fail(_LANG_HEARTHSTONE_INVOKE_MOUNT_HERE), 0;
//
//   // in case somebody else is riding it
//   ob->unride();
//   ob->move(environment(this_player()));
//
//   tell_player(this_player(), _LANG_HEARTHSTONE_INVOKE_MSG_ME);
//   tell_room(environment(this_player()), _LANG_HEARTHSTONE_INVOKE_MSG_ROOM, this_player());
//
//   this_player()->add_timed_property(RECALL_LOCK, 1, cooldown / HEART_BEAT_TIME);
//   return 1;
// }

mixed * stats()
{
  return ::stats() + ({
      ({ "Destination Path", destination_path, }),
      ({ "Destination Name", destination_name, }),
    });
}

mapping query_auto_load_attributes()
{
  return ([ "::" : ::query_auto_load_attributes() ]) +
      ((strlen(destination_path)) ? ([ "destination path" : destination_path ]) : ([ ])) +
      ((strlen(destination_name)) ? ([ "destination name" : destination_name ]) : ([ ]));
}

void init_auto_load_attributes(mapping attribute_map)
{
  if (!undefinedp(attribute_map["destination path"]))
    destination_path = attribute_map["destination path"];
  if (!undefinedp(attribute_map["destination name"]))
    destination_name = attribute_map["destination name"];
  if (!undefinedp(attribute_map["::"]))
    ::init_auto_load_attributes(attribute_map["::"]);
}
