/*
 * Hearthstone for Ciudad Capital
 * Obviously based on World of Warcraft
 *
 * neverbot 17/04/09
 */

#include <language.h>

inherit "/lib/item.c";

#define NUM_SECONDS 10
#define LOCK_HEARTHSTONE "hearthstone_lock_property"
#define LOCK_HEARTHSTONE_INVOKE "hearthstone_invoke_lock_property"
// 2 seconds = 1 hb, 1 hour = 3600 seconds = 1800 hbs
#define LOCK_TIME 1800

static int acting;
string destination_path;
string destination_name;

void set_destination_path(string n) { destination_path = n; }
string query_destination_path() { return destination_path; }
void set_destination_name(string n) { destination_name = n; }
string query_destination_name() { return destination_name; }

void create()
{
  acting = 0;
  destination_path = "";
  destination_name = "";
  
  ::create();
  
  set_name(_LANG_HEARTHSTONE_NAME);
  set_short(_LANG_HEARTHSTONE_SHORT);
  add_alias(_LANG_HEARTHSTONE_ALIAS);

  set_main_plural(_LANG_HEARTHSTONE_PLURAL);
  add_plural(_LANG_HEARTHSTONE_PLURALS);
  set_long(_LANG_HEARTHSTONE_LONG);

  reset_drop();

  set_gender(2);
  set_weight(1);
}

string get_help(varargs string str) { return _LANG_HEARTHSTONE_HELP; }

string long(string s, int dark)
{
  return ::long(s, dark) + _LANG_HEARTHSTONE_MARK_INFO;
}

void init()
{
  add_action("do_travel", _LANG_HEARTHSTONE_TRANSPORT_VERBS);
  add_action("do_mark", _LANG_HEARTHSTONE_MARK_VERBS);
  add_action("do_invoke", _LANG_HEARTHSTONE_INVOKE_VERBS);

  ::init();
}

int do_travel(string str)
{
  if (acting)
  {
    notify_fail(_LANG_HEARTHSTONE_ACTING);
    return 0;
  }

  if (this_player()->query_dead())
  {
    notify_fail(_LANG_HEARTHSTONE_DEAD);
    return 0;
  }

  if (this_player()->query_fighting())
  {
    notify_fail(_LANG_HEARTHSTONE_COMBATS);
    return 0;
  }

  if (this_object()->query_timed_property(LOCK_HEARTHSTONE))
  {
    notify_fail(_LANG_HEARTHSTONE_LOCKED);
    return 0;
  }

  if (!stringp(destination_path) || !load_object(destination_path))
  {
    string race;
    race = this_player()->query_race_ob();

    if (!stringp(race))
    {
      notify_fail(_LANG_HEARTHSTONE_FORGOTTEN);
      return 0;
    }
    else
    {
      tell_object(this_player(), _LANG_HEARTHSTONE_ORIGIN);
      destination_path = load_object(race)->query_init_room();
    }
  }

  tell_object(this_player(), _LANG_HEARTHSTONE_MSG_ME);
  tell_room(environment(this_player()), _LANG_HEARTHSTONE_MSG_ROOM, this_player());

  acting = 1;

  call_out("continue_travel", 1, this_player(), NUM_SECONDS, environment(this_player()));
  return 1;
}

int continue_travel(object player, int count, object where)
{
  if (player->query_fighting())
  {
    tell_object(player, _LANG_HEARTHSTONE_COMBATS);
    acting = 0;
    return 1;
  }

  if (environment(player) != where)
  {
    tell_object(player, _LANG_HEARTHSTONE_MOVE);
    acting = 0;
    return 1;
  }

  if (player->query_dead())
  {
    tell_object(player, _LANG_HEARTHSTONE_DEAD2);
    return 1;
  }

  if (count <= 0)
  {
    object dest;
    dest = load_object(destination_path);

    if (!dest)
    {
      tell_object(player, _LANG_HEARTHSTONE_ERROR);
      acting = 0;
      return 1;
    }

    // do not transport mounts, if not you would appear with your horse in the
    // middle of a tavern...
    if (player->query_riding())
      player->destruct_ride_shadow();

    player->move(dest);
    add_timed_property(LOCK_HEARTHSTONE, 1, LOCK_TIME);
    
    // allow invoke after travel
    remove_timed_property(LOCK_HEARTHSTONE_INVOKE);
    
    tell_object(player, _LANG_HEARTHSTONE_TRANSPORT_ME);
    player->do_look();
    
    tell_room(dest, player->query_cap_name() + 
              _LANG_HEARTHSTONE_TRANSPORT_ROOM, player);
    
    acting = 0;
    return 1;
  }
  else
  {
    tell_object(player, query_short() + ": [%^MAGENTA%^" + 
      sprintf("%*' '-s", NUM_SECONDS, sprintf("%*'*'s", NUM_SECONDS - count, "")) +
      "%^RESET%^]\n");
  }

  call_out("continue_travel", 1, player, count-1, environment(player));
  return 1;
}

int do_mark(string str)
{
  if (!id(str))
  {
    notify_fail(_LANG_HEARTHSTONE_MARK_FAIL);
    return 0;
  }

  if (acting)
  {
    notify_fail(_LANG_HEARTHSTONE_ACTING);
    return 0;
  }

  if (this_player()->query_dead())
  {
    notify_fail(_LANG_HEARTHSTONE_DEAD);
    return 0;
  }

  if (this_player()->query_fighting())
  {
    notify_fail(_LANG_HEARTHSTONE_COMBATS);
    return 0;
  }

  if (!environment(this_player())->query_pub())
  {
    notify_fail(_LANG_HEARTHSTONE_NOT_INN_FAIL);
    return 0;
  }

  destination_path = base_name(environment(this_player()));
  destination_name = environment(this_player())->query_short();

  tell_object(this_player(), _LANG_HEARTHSTONE_DESTINATION +
    environment(this_player())->query_short() + ".\n");
  return 1;
}

int do_invoke(string str)
{
  object ob;

  // we are trying to invoke the mount
  if (member_array(str, _LANG_HEARTHSTONE_INVOKE_NAMES) == -1)
  {
    notify_fail(_LANG_HEARTHSTONE_INVOKE_WHAT);
    return 0;
  }

  if (!this_player()->query_mount())
  {
    notify_fail(_LANG_HEARTHSTONE_INVOKE_NO_MOUNT);
    return 0;
  }

  if (!environment(this_player())->query_outside())
  {
    notify_fail(_LANG_HEARTHSTONE_INVOKE_OUTSIDE);
    return 0;
  }

  if (environment(this_player())->query_water_environment())
  {
    notify_fail(_LANG_HEARTHSTONE_INVOKE_NO_WATER);
    return 0;
  }

  if (this_object()->query_timed_property(LOCK_HEARTHSTONE_INVOKE))
  {
    notify_fail(_LANG_HEARTHSTONE_LOCKED);
    return 0;
  }

  ob = this_player()->query_mount();

  if (environment(this_player()) == environment(ob))
  {
    notify_fail(_LANG_HEARTHSTONE_INVOKE_MOUNT_HERE);
    return 0;
  }

  // Por si la esta montando otra persona
  ob->unride();
  ob->move(environment(this_player()));

  tell_player(this_player(), _LANG_HEARTHSTONE_INVOKE_MSG_ME);
  tell_room(environment(this_player()), _LANG_HEARTHSTONE_INVOKE_MSG_ROOM, this_player())  ;

  // Con esto impedimos que invoque a la montura hasta la proxima vez que pueda transportarse
  //  (ya que lo logico es transportarse + invocar montura cuando encuentre un lugar apropiado)
  if (query_timed_property(LOCK_HEARTHSTONE))
    this_object()->add_timed_property(LOCK_HEARTHSTONE_INVOKE, 1,
      query_time_remaining(LOCK_HEARTHSTONE));
  else
    this_object()->add_timed_property(LOCK_HEARTHSTONE_INVOKE, 1, LOCK_TIME);

  return 1;
}

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
