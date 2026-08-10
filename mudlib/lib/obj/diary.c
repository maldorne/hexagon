// Player diary. Part of every player's mandatory inventory (MUST_HAVE), a
// read-only front-end over the exploration component:
//
//   diario explorador  - the places the player has discovered (this game)
//
// Travel points (diario viajes) and the encyclopedia are follow-ups.
// Ported from the CcMud diary, Folken 04/2009.

#include <areas/area.h>
#include <language.h>

inherit "/lib/item.c";

int query_player_diary() { return 1; }

void create()
{
  ::create();

  set_name(_LANG_DIARY_NAME);
  set_short(_LANG_DIARY_SHORT);
  add_alias(_LANG_DIARY_ALIAS);
  set_main_plural(_LANG_DIARY_PLURAL);
  add_plural(_LANG_DIARY_PLURALS);
  set_long(_LANG_DIARY_LONG);

  reset_drop();
  set_weight(1);
}

void init()
{
  ::init();
  add_action("do_diary", _LANG_DIARY_VERBS);
}

string query_help(varargs string str) { return _LANG_DIARY_HELP; }

// diario explorador: list the areas the player has discovered in the game
// they are currently in.
static int do_explorer()
{
  string game, ret, name;
  string * paths, * names;
  object area;
  int i, n;

  game = game_name(this_player());
  paths = this_player()->query_explorations(game);
  names = ({ });

  // Resolve each stored area path to its display name in this instance's
  // language. Skip areas that no longer exist or no longer grant a name.
  for (i = 0; paths && i < sizeof(paths); i++)
  {
    // query_area returns nil for a path with no area.o, so a stored area
    // that no longer exists is skipped without materialising an empty one.
    area = load_object(AREA_HANDLER)->query_area(paths[i]);
    if (!area)
      continue;

    name = area->query_exploration_name();
    if (name && name != "")
      names += ({ name });
  }

  n = sizeof(names);
  if (!n)
  {
    tell_player(this_player(), _LANG_DIARY_EXPLORER_EMPTY);
    return 1;
  }

  ret = _LANG_DIARY_EXPLORER_HEADER;
  for (i = 0; i < n; i++)
    ret += "\t" + names[i] + "\n";
  ret += _LANG_DIARY_EXPLORER_COUNT_PRE + number_as_string(n) + " " +
         (n == 1 ? _LANG_DIARY_PLACE_SG : _LANG_DIARY_PLACE_PL) + ".\n";

  tell_player(this_player(), ret);
  return 1;
}

int do_diary(string str)
{
  if (this_player()->query_dead())
  {
    notify_fail(_LANG_DIARY_DEAD);
    return 0;
  }

  if (!str || str == "")
  {
    notify_fail(_LANG_DIARY_WHAT);
    return 0;
  }

  if (member_array(str, _LANG_DIARY_EXPLORER_SUBVERBS) != -1)
    return do_explorer();

  // Sections not implemented yet (travel anchors, encyclopedia).
  if (member_array(str, _LANG_DIARY_PENDING_SUBVERBS) != -1)
  {
    tell_player(this_player(), _LANG_DIARY_NOT_YET);
    return 1;
  }

  notify_fail(_LANG_DIARY_WHAT);
  return 0;
}
