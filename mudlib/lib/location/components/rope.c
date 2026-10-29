// Rope component. Attached during conversion when the source room answers
// query_rope_anchor() with the direction of one of its exits. That exit is
// sealed until somebody ties a rope: "tie rope" takes one from what they
// carry (anything answering query_rope()) and opens the exit, "untie rope"
// gives it back and seals the exit again.
//
// The tied rope's file is saved with the location, so the exit is open again
// every time the location loads while a rope is tied.
//
// Plugs into the location component hook system (see <room/location.h>).

#include <room/location.h>
#include <language.h>
#include <translations/exits.h>

inherit component "/lib/location/component.c";

// the exit the rope opens
private string direction;
// what the rope is tied to, as the room names it ("a tree"); may be empty
private string anchor;
// file of the rope that is tied; empty when there is none
private string rope_file;

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_ROPE);
  direction = "";
  anchor = "";
  rope_file = "";

  // Hidden-inventory-item discipline, as the props component: the component
  // stands in the location's inventory so the location's init reaches it and
  // looking at the rope finds it.
  set_name("");
  reset_get();
  reset_drop();
}

void set_direction(string dir) { direction = dir ? dir : ""; }
string query_direction() { return direction; }

void set_anchor(string text) { anchor = text ? text : ""; }
string query_anchor() { return anchor; }

int query_tied() { return strlen(rope_file) > 0; }

/**
 * Opens the exit when a rope is tied and seals it when not. The location
 * forgets which exits were revealed every time it loads, so this runs again
 * on each load.
 */
private void sync_exit()
{
  object place;

  place = query_entity();
  if (!place || !strlen(direction))
    return;

  if (query_tied())
    place->reveal_exit(direction);
  else
    place->seal_exit(direction);
}

void initialize(object place)
{
  component::initialize(place);
  if (place && environment(this_object()) != place)
    move(place);
  sync_exit();
}

void placed() { sync_exit(); }

/**
 * The phrase for what the rope is tied to, "to a tree"; empty when the room
 * names nothing.
 */
private string anchor_phrase()
{
  string article;

  if (!strlen(anchor))
    return "";

  article = _LANG_ROPE_ARTICLE;
  if (strlen(article) && strlen(anchor) > strlen(article) &&
      anchor[0..strlen(article) - 1] == article)
    return _LANG_ROPE_ARTICLE_CONTRACTED + anchor[strlen(article)..];

  return _LANG_ROPE_TO + anchor;
}

/**
 * The line the look of the location gets while a rope is tied.
 */
private string rope_line()
{
  string line;

  line = _LANG_ROPE_LINE;
  if (strlen(anchor))
    line += " " + anchor_phrase();

  if (direction == DIR_UP || direction == DIR_DOWN)
    line += _LANG_ROPE_HANGS;
  else
    line += _LANG_ROPE_STRETCHES;

  return line + direction + ".\n";
}

// ************************************************************
//  Looking at the rope
// ************************************************************

// find_match asks these of what stands in the location: the rope answers to
// its words only while it is tied
string * query_alias() { return query_tied() ? _LANG_ROPE_WORDS : ({ }); }
string * query_plurals() { return ({ }); }

int id(string str)
{
  return query_tied() && str && member_array(str, _LANG_ROPE_WORDS) != -1;
}

int id_plural(string str) { return 0; }

string long(varargs string str, int dark)
{
  if (!str || !strlen(str) || !query_tied())
    return "";

  return rope_line();
}

mapping query_hooks()
{
  return ([
    "extra_look":    HOOK_PRIORITY_STRUCTURE,
    "glance_string": HOOK_PRIORITY_STRUCTURE,
  ]);
}

string hook_extra_look(mixed * args)
{
  return query_tied() ? rope_line() : "";
}

string hook_glance_string(mixed * args)
{
  return query_tied() ? rope_line() : "";
}

// ************************************************************
//  Verbs
// ************************************************************

/**
 * Registers the verbs on whoever enters; the component stands in the
 * location's inventory, so the move/init pass reaches it.
 */
void init()
{
  string * verbs;
  int i;

  verbs = _LANG_ROPE_TIE_VERBS;
  for (i = 0; i < sizeof(verbs); i++)
    add_action("do_tie", verbs[i]);

  verbs = _LANG_ROPE_UNTIE_VERBS;
  for (i = 0; i < sizeof(verbs); i++)
    add_action("do_untie", verbs[i]);
}

/**
 * Whether the argument of a verb names the rope: "rope", "the rope", "rope to
 * the tree".
 */
private int names_rope(string str)
{
  string * words;

  if (!str || !strlen(str))
    return 0;

  words = explode(str, " ");
  if (sizeof(words) > 1 && member_array(words[0], _LANG_ROPE_ARTICLES) != -1)
    words = words[1..];

  return member_array(words[0], _LANG_ROPE_WORDS) != -1;
}

/**
 * The first thing the living carries that is a rope.
 */
private object find_rope(object who)
{
  object * carried;
  int i;

  carried = all_inventory(who);
  for (i = 0; i < sizeof(carried); i++)
    if (carried[i]->query_rope())
      return carried[i];

  return nil;
}

int do_tie(string str)
{
  object who, place, rope;
  string phrase, file;

  who = this_player();
  place = query_entity();
  if (!who || !place || !names_rope(str))
    return 0;

  if (query_tied())
  {
    notify_fail(_LANG_ROPE_ALREADY_TIED);
    return 0;
  }

  rope = find_rope(who);
  if (!rope)
  {
    notify_fail(_LANG_ROPE_NO_ROPE);
    return 0;
  }

  file = base_name(rope);
  if (file[strlen(file) - 2..] != ".c")
    file += ".c";
  rope_file = file;
  rope->dest_me();

  sync_exit();
  place->save_me();

  phrase = strlen(anchor) ? " " + anchor_phrase() : "";
  tell_object(who, _LANG_ROPE_TIE_SELF + phrase + ".\n");
  tell_room(place, who->query_cap_name() + _LANG_ROPE_TIE_OTHERS + phrase +
            ".\n", ({ who }));
  return 1;
}

int do_untie(string str)
{
  object who, place, rope;

  who = this_player();
  place = query_entity();
  if (!who || !place || !names_rope(str))
    return 0;

  if (!query_tied())
  {
    notify_fail(_LANG_ROPE_NOTHING_TIED);
    return 0;
  }

  rope = clone_object(rope_file);
  rope_file = "";

  sync_exit();
  place->save_me();

  // a living that cannot carry the rope leaves it on the floor
  if (rope->move(who))
  {
    rope->move(place);
    tell_object(who, _LANG_ROPE_UNTIE_SELF_FLOOR);
  }
  else
    tell_object(who, _LANG_ROPE_UNTIE_SELF);

  tell_room(place, who->query_cap_name() + _LANG_ROPE_UNTIE_OTHERS, ({ who }));
  return 1;
}

string query_info()
{
  return direction + (query_tied() ? " (tied)" : " (untied)");
}

// ************************************************************
//  Persistence
// ************************************************************

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "direction": direction, "anchor": anchor,
            "rope_file": rope_file ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);

  if (stringp(args["direction"]))
    direction = args["direction"];
  if (stringp(args["anchor"]))
    anchor = args["anchor"];
  if (stringp(args["rope_file"]))
    rope_file = args["rope_file"];
}
