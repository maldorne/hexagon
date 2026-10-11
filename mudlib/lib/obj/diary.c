// Player diary. Part of every player's mandatory inventory (MUST_HAVE), a
// read-only front-end over the exploration component:
//
//   diario explorador  - the places the player has discovered (this game)
//   diario enciclopedia [<n>] - the books the player has read, and reading one again
//
// Travel points (diario viajes) are a follow-up.
// Ported from the CcMud diary, neverbot 04/2009.

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
    tell_object(this_player(), _LANG_DIARY_EXPLORER_EMPTY);
    return 1;
  }

  // in alphabetical order, not in the order they were found
  names = sort_array(names);

  ret = _LANG_DIARY_EXPLORER_HEADER;
  for (i = 0; i < n; i++)
    ret += "\t" + names[i] + "\n";
  ret += _LANG_DIARY_EXPLORER_COUNT_PRE + number_as_string(n) + " " +
         (n == 1 ? _LANG_DIARY_PLACE_SG : _LANG_DIARY_PLACE_PL) + ".\n";

  tell_object(this_player(), ret);
  return 1;
}

// diario enciclopedia: the books the player has read in this game, in the
// order they read them; diario enciclopedia <n> opens one of them again.
int do_encyclopedia(string str)
{
  string * paths;
  string game, ret, title;
  int i, num;

  if (this_player()->query_dead())
  {
    notify_fail(_LANG_DIARY_DEAD);
    return 0;
  }

  game = game_name(this_player());
  paths = this_player()->query_encyclopedia_articles(game);

  if (!sizeof(paths))
  {
    tell_object(this_player(), _LANG_DIARY_ENCYCLOPEDIA_EMPTY);
    return 1;
  }

  if (str && strlen(str))
  {
    if (sscanf(str, "%d", num) != 1 || num < 1 || num > sizeof(paths))
    {
      notify_fail(_LANG_DIARY_ENCYCLOPEDIA_NO_SUCH);
      return 0;
    }
    handler("books")->start_reading(this_player(), paths[num - 1]);
    return 1;
  }

  ret = _LANG_DIARY_ENCYCLOPEDIA_HEADER;
  for (i = 0; i < sizeof(paths); i++)
  {
    title = handler("books")->query_title(paths[i]);
    ret += "\t" + (i + 1) + ") " +
           ((title && strlen(title)) ? title : _LANG_DIARY_ENCYCLOPEDIA_LOST) + "\n";
  }
  ret += _LANG_DIARY_ENCYCLOPEDIA_FOOTER;

  tell_object(this_player(), ret);
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

  // the section, or the section and the number of a book in it
  if (member_array(explode(str, " ")[0], _LANG_DIARY_ENCYCLOPEDIA_VERBS) != -1)
    return do_encyclopedia(implode(explode(str, " ")[1..], " "));

  // Sections not implemented yet (travel anchors).
  if (member_array(str, _LANG_DIARY_PENDING_SUBVERBS) != -1)
  {
    tell_object(this_player(), _LANG_DIARY_NOT_YET);
    return 1;
  }

  notify_fail(_LANG_DIARY_WHAT);
  return 0;
}
