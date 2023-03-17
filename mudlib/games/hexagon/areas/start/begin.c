/*
 * new start room for hexagon
 * here the new players have to choose a game before starting to play
 *
 */

inherit "/lib/room";

#include <living/races.h>
#include <language.h>

void setup()
{
  set_light((LIGHT_STD_HIGH - LIGHT_STD_LOW)/2);
  set_short(mud_name() + ": " + _LANG_START_SHORT);
  set_long(_LANG_START_LONG + " " + implode(_LANG_START_HINTS, " ") + "\n");
}

int show_hints(object who)
{
  tell_object(who, "\n%^BOLD%^" + implode(_LANG_START_HINTS, "\n") + "%^RESET%^\n");
  return 1;
}

void event_enter(object who, varargs string msg, object from, mixed avoid)
{
  if (living(who))
    call_out("show_hints", 1, who);

  ::event_enter(who, msg, from, avoid);
}

void init()
{
  ::init();
  add_action("do_play", _LANG_PLAY_VERBS);
}

int do_play(string str)
{
  int i;
  object * games;
  string path;

  if (!strlen(str))
  {
    notify_fail(_LANG_PLAY_FAIL);
    return 0;
  }

  if (sscanf(str, "%d", i) != 1)
  {
    notify_fail(_LANG_PLAY_FAIL);
    return 0;
  }

  i -= 1;

  games = handler("games")->query_game_objects();

  if (i < 0 || i >= sizeof(games))
  {
    notify_fail(_LANG_PLAY_FAIL);
    return 0;
  }

  path = game_root(games[i]) + "areas/start/begin.c";

  if (!load_object(path))
  {
    notify_fail(_LANG_PLAY_GAME_DO_NOT_LOAD);
    return 0;
  }

  write("\n" + _LANG_PLAY_START + "\n");
  this_player()->move(path);
  this_player()->do_look();
  return 1;
}
