/* The Pub Globe. A chatter between rooms with the Globe in them.
 * Version 2, with globewho
 * Baldrick, '93, fix jan '96.
 *
 * Traducido por Folken@CcMud, Julio 2003
 * Generic for every pub of every game, neverbot 09/2026
 */

// Every pub puts one up: whatever is said into a globe is heard in every
// other one, whatever game the pub belongs to. The blueprint keeps the list of
// the globes that exist, since the driver has no way to list the clones of a
// program.

#include <room/ventures.h>
#include <language.h>

inherit "/lib/item.c";

// Only meaningful on the blueprint: every globe in the world.
static object * globes;

void setup()
{
  set_name(_LANG_GLOBE_NAME);
  set_short(_LANG_GLOBE_SHORT);
  set_long(_LANG_GLOBE_LONG);
  set_main_plural(_LANG_GLOBE_PLURAL);

  add_alias(_LANG_GLOBE_ALIASES);
  add_plural(_LANG_GLOBE_PLURALS);

  reset_get();
  set_weight(10000);
  set_value(0);
  set_read_mess(_LANG_GLOBE_READ);
}

void create()
{
  globes = ({ });

  ::create();

  if (clonep(this_object()))
    load_object(GLOBE_OB)->register_globe(this_object());
}

void dest_me()
{
  object blueprint;

  if (clonep(this_object()) && (blueprint = find_object(GLOBE_OB)))
    blueprint->unregister_globe(this_object());

  ::dest_me();
}

void register_globe(object ob)
{
  globes = (globes - ({ nil, ob })) + ({ ob });
}

void unregister_globe(object ob)
{
  globes -= ({ nil, ob });
}

object * query_globes() { return globes - ({ nil }); }

void init()
{
  // register again, so a reloaded blueprint learns of the globes still around
  if (clonep(this_object()))
    load_object(GLOBE_OB)->register_globe(this_object());

  add_action("do_globe", _LANG_GLOBE_VERB);
  ::init();
}

// How somebody who has had too much sounds.
private string drunk_speech(string str)
{
  return replace(str, _LANG_GLOBE_DRUNK_REPLACEMENTS);
}

// Say it in this globe's room, to everybody but the one who spoke.
void globe_tell(string message, object speaker)
{
  if (environment())
    tell_room(environment(), message, speaker);
}

int do_chat(string str)
{
  object * all;
  int i;

  if (!strlen(str))
  {
    notify_fail(_LANG_GLOBE_NO_MESSAGE);
    return 0;
  }

  if (this_player()->query_drunk())
    str = drunk_speech(str);

  all = load_object(GLOBE_OB)->query_globes();

  for (i = 0; i < sizeof(all); i++)
    all[i]->globe_tell(_LANG_GLOBE_HEARD, this_player());

  write(_LANG_GLOBE_SAID);
  return 1;
}

// Who is standing by this globe, for somebody asking from any other one.
void show_listeners(object curious)
{
  object * here;
  object env;
  string place, game;
  int i;

  if (!(env = environment()))
    return;

  place = env->short();
  game = game_master_object(env) ? game_master_object(env)->query_game_name() : "";

  tell_object(curious, _LANG_GLOBE_WHO_PLACE);

  here = all_inventory(env);

  for (i = 0; i < sizeof(here); i++)
    if (living(here[i]) && !here[i]->query_hide_shadow() && !here[i]->query_invis())
      tell_object(curious, "  " + here[i]->query_cap_name() + "\n");
}

int do_globe_who()
{
  object * all;
  int i;

  all = load_object(GLOBE_OB)->query_globes();

  for (i = 0; i < sizeof(all); i++)
    all[i]->show_listeners(this_player());

  return 1;
}

// One verb for the globe, what to do with it given as its first word.
int do_globe(string str)
{
  string option, rest;

  if (!str || !strlen(str))
  {
    notify_fail(_LANG_GLOBE_USAGE);
    return 0;
  }

  if (sscanf(str, "%s %s", option, rest) != 2)
  {
    option = str;
    rest = "";
  }

  if (member_array(option, _LANG_GLOBE_CHAT_OPTIONS) != -1)
    return do_chat(rest);

  if (member_array(option, _LANG_GLOBE_WHO_OPTIONS) != -1)
    return do_globe_who();

  notify_fail(_LANG_GLOBE_USAGE);
  return 0;
}
