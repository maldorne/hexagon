// crafts -- what the crafters standing here make to order, and commissioning it.
//
// 10/2026 - Created for Hexagon, neverbot. The crafters in the room are found
//           by this command, as the quests command finds the quest givers, and
//           every decision is asked of the crafter itself.

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_CRAFTS_ALIAS);
  set_usage(_LANG_CMD_CRAFTS_SYNTAX);
  set_help(_LANG_CMD_CRAFTS_HELP);
}

// Every recipe on offer here to `me`, numbered by its place in this list:
// ({ ({ crafter carrier, crafter code, recipe }), ... })
private mixed * entries(object me)
{
  object * here;
  object crafts;
  mixed * out;
  int i, j;

  out = ({ });
  if (!environment(me))
    return out;

  crafts = handler("crafts");
  here = all_inventory(environment(me));

  for (i = 0; i < sizeof(here); i++)
  {
    object code;
    mapping * recipes;

    code = crafts->crafter_of(here[i]);
    if (!code)
      continue;

    recipes = code->recipes_for(me);
    for (j = 0; j < sizeof(recipes); j++)
      out += ({ ({ here[i], code, recipes[j] }) });
  }

  return out;
}

// The short of an item blueprint, as a player reads it.
private string item_name(string path)
{
  object ob;

  ob = nil;
  catch(ob = load_object(path));
  return ob ? (string)ob->query_short() : path;
}

private string price_text(int price)
{
  if (price <= 0)
    return _LANG_CMD_CRAFTS_FREE;
  return handler("money")->money_value_string(price);
}

private string materials_text(mapping materials)
{
  string * paths, * parts;
  int i;

  parts = ({ });
  paths = map_indices(materials);
  for (i = 0; i < sizeof(paths); i++)
    parts += ({ (materials[paths[i]] > 1 ? materials[paths[i]] + " x " : "") +
                item_name(paths[i]) });

  return implode(parts, ", ");
}

private int list_all(object me, mixed * list)
{
  string out;
  object last;
  int i;

  if (!sizeof(list))
  {
    write(_LANG_CMD_CRAFTS_NONE);
    return 1;
  }

  out = "";
  last = nil;
  for (i = 0; i < sizeof(list); i++)
  {
    if (list[i][0] != last)
    {
      last = list[i][0];
      out += _LANG_CMD_CRAFTS_FROM;
    }
    out += "  [%^BOLD%^" + (i + 1) + "%^RESET%^] " +
           item_name(list[i][2]["result"]) + "\n";
  }

  write(out + _LANG_CMD_CRAFTS_LIST_FOOTER);
  return 1;
}

private int show_one(object me, mixed * entry)
{
  mapping recipe, missing;
  object made;
  string out;

  recipe = entry[2];
  made = nil;
  catch(made = load_object(recipe["result"]));

  out = "%^BOLD%^" + item_name(recipe["result"]) + "%^RESET%^\n";
  if (made)
    out += wrap(made->query_long(), me->user()->query_cols());
  out += _LANG_CMD_CRAFTS_NEEDS + materials_text(recipe["materials"]) + "\n";
  out += _LANG_CMD_CRAFTS_COSTS + price_text(recipe["price"]) + "\n";

  missing = entry[1]->missing_for(me, recipe);
  if (map_sizeof(missing))
    out += _LANG_CMD_CRAFTS_YOU_LACK + materials_text(missing) + "\n";
  else if (!entry[1]->can_afford(me, recipe))
    out += _LANG_CMD_CRAFTS_CANNOT_PAY;

  write(out);
  return 1;
}

private int commission(object me, mixed * entry)
{
  mapping recipe, missing;
  object made, who;

  recipe = entry[2];
  who = entry[0];

  missing = entry[1]->missing_for(me, recipe);
  if (map_sizeof(missing))
  {
    write(_LANG_CMD_CRAFTS_YOU_LACK + materials_text(missing) + "\n");
    return 1;
  }

  if (!entry[1]->can_afford(me, recipe))
  {
    write(_LANG_CMD_CRAFTS_CANNOT_PAY);
    return 1;
  }

  made = entry[1]->craft(me, recipe);
  if (!made)
  {
    write(_LANG_CMD_CRAFTS_FAILED);
    return 1;
  }

  write(_LANG_CMD_CRAFTS_DONE_ME);
  tell_room(environment(me), _LANG_CMD_CRAFTS_DONE_ROOM, ({ me }));
  return 1;
}

static int cmd(string str, object me, string verb)
{
  mixed * list;
  string * words;
  int n;

  list = entries(me);
  words = explode(str ? str : "", " ") - ({ "" });

  if (!sizeof(words))
    return list_all(me, list);

  n = 0;
  if (sizeof(words) > 1)
    sscanf(words[1], "%d", n);
  // with only one recipe here, the number may be left out
  if (!n && sizeof(list) == 1)
    n = 1;

  if (n < 1 || n > sizeof(list))
  {
    notify_fail(_LANG_CMD_CRAFTS_NO_SUCH);
    return 0;
  }

  if (member_array(words[0], _LANG_CMD_CRAFTS_INFO_WORDS) != -1)
    return show_one(me, list[n - 1]);

  if (member_array(words[0], _LANG_CMD_CRAFTS_ORDER_WORDS) != -1)
    return commission(me, list[n - 1]);

  notify_fail(_LANG_CMD_CRAFTS_SYNTAX + "\n");
  return 0;
}
