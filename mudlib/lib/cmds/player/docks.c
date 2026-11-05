
#include <mud/cmd.h>
#include <sector/sector.h>
#include <language.h>

inherit CMD_BASE;

// player `docks` cmd: the places of the game where a ship can land, one per
// port, as the world map draws them (see query_dock_state of the world map
// handler). Both the dock and the place are shown by name only.

void setup()
{
  set_aliases(_LANG_CMD_DOCKS_ALIAS);
  set_usage(_LANG_CMD_DOCKS_SYNTAX);
  set_help(_LANG_CMD_DOCKS_HELP);
}

// A text without its colour tokens (%^NAME%^) nor ANSI colour sequences.
private string _plain(string text)
{
  string ret;
  int i, len;

  ret = "";
  len = strlen(text);
  for (i = 0; i < len; i++)
  {
    if (text[i] == 27 && i + 1 < len && text[i + 1] == '[')
    {
      // skip up to the final byte of the sequence
      for (i += 2; i < len && (text[i] < 0x40 || text[i] > 0x7E); i++);
      continue;
    }
    if (text[i] == '%' && i + 1 < len && text[i + 1] == '^')
    {
      // skip up to the closing "%^" of the token
      for (i += 2; i + 1 < len && !(text[i] == '%' && text[i + 1] == '^'); i++);
      i++;
      continue;
    }
    ret += text[i .. i];
  }
  return ret;
}

// A table cell: the text padded with spaces to `width` visible characters.
private string _cell(string text, int width)
{
  return text + pad("", width - strlen(text, 1));
}

/**
 * Called back by the sectors handler (list_docks) with the sectors holding a
 * dock: tells the player the docks in a frame, as two columns with the dock
 * name and the place it belongs to, sorted by dock name.
 */
void show_docks(object who, string game, string map_name, mixed * docks)
{
  string ** list, * keys, * entry, key, text, head;
  int i, j, cols, dock_w, place_w;

  if (!who)
    return;

  list = load_object(WORLDMAP_HANDLER)->dock_list(docks);
  if (!sizeof(list))
  {
    tell_object(who, _LANG_DOCKS_NONE);
    return;
  }

  // the names as they read, with no colour sequences (a sober table, and a
  // name that leaves its colour open cannot bleed into the next column)
  keys = allocate(sizeof(list));
  for (i = 0; i < sizeof(list); i++)
  {
    list[i][0] = _plain(list[i][0]);
    list[i][1] = _plain(list[i][1]);
    keys[i] = lower_case(list[i][0]);
  }

  // insertion sort by name

  for (i = 1; i < sizeof(list); i++)
    for (j = i; j > 0 && keys[j] < keys[j - 1]; j--)
    {
      entry = list[j];
      list[j] = list[j - 1];
      list[j - 1] = entry;
      key = keys[j];
      keys[j] = keys[j - 1];
      keys[j - 1] = key;
    }

  // each column is as wide as its heading or its widest entry
  dock_w = strlen(_LANG_DOCKS_COL_DOCK, 1);
  place_w = strlen(_LANG_DOCKS_COL_PLACE, 1);
  for (i = 0; i < sizeof(list); i++)
  {
    if (strlen(list[i][0], 1) > dock_w)
      dock_w = strlen(list[i][0], 1);
    if (strlen(list[i][1], 1) > place_w)
      place_w = strlen(list[i][1], 1);
  }

  head = _cell(_LANG_DOCKS_COL_DOCK, dock_w) + "  " + _LANG_DOCKS_COL_PLACE;
  text = head + "\n" + pad("", dock_w + 2 + place_w, "-") + "\n";
  for (i = 0; i < sizeof(list); i++)
    text += _cell(list[i][0], dock_w) + "  " + list[i][1] + "\n";

  cols = who->user() ? who->user()->query_cols() : 0;
  if (cols <= 0)
    cols = 80;
  tell_object(who, handler("frames")->frame(text, _LANG_DOCKS_TITLE, cols));
}

static int cmd(string str, object me, string verb)
{
  string game, map_name;
  object env;

  game = game_name(me);
  if (!strlen(game))
  {
    notify_fail(_LANG_DOCKS_NO_GAME);
    return 0;
  }

  env = environment(me);
  map_name = (env && env->query_location()) ? env->query_map_name() : "default";

  write(_LANG_DOCKS_SEARCHING);
  load_object(SECTORS_HANDLER)->list_docks(game, map_name, me, this_object(),
                                           "show_docks");
  return 1;
}
