// resistances, neverbot 3/2002
//
// 11/2026 - Moved to the player commands, neverbot: a player sees their own
//           resistances, a coder anybody's; names are the translated names of
//           the damage types.

#include <mud/cmd.h>
#include <living/spells.h>
#include <living/damage-types.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_RESISTANCES_ALIAS);
  set_usage(_LANG_CMD_RESISTANCES_SYNTAX);
  set_help(_LANG_CMD_RESISTANCES_HELP);
}

/**
 * Pads a text with spaces to a width counted in characters (the pad efun
 * counts bytes), so accented names line up.
 */
private string pad_chars(string text, int width)
{
  int len;

  len = strlen(text, 1);
  while (len < width)
  {
    text += " ";
    len++;
  }
  return text;
}

static int cmd(string name, object me, string verb)
{
  string info, * names;
  object who, * tmp;
  object table_ob;
  int i, width, half;

  who = me;

  // only a coder looks at somebody else
  if (strlen(name) && me->query_coder())
  {
    name = me->expand_nickname(name);
    who = find_living(name);
    if (!objectp(who))
    {
      tmp = find_match(name, environment(me));
      if (sizeof(tmp))
        who = tmp[0];
    }
    if (!objectp(who))
      who = find_player(name);
    if (!objectp(who))
      who = find_object(name);
    if (!objectp(who))
    {
      notify_fail(_LANG_CMD_RESISTANCES_NOT_FOUND);
      return 0;
    }
  }

  table_ob = table("damage-types");

  names = allocate(sizeof(DAMAGE_ELEMENTAL_TYPES));
  width = 0;
  for (i = 0; i < sizeof(names); i++)
  {
    names[i] = capitalize(table_ob->query_damage_type_name(DAMAGE_ELEMENTAL_TYPES[i]));
    if (strlen(names[i], 1) > width)
      width = strlen(names[i], 1);
  }

  // two columns, the first one the longer when the count is odd
  half = (sizeof(names) + 1) / 2;
  info = "";
  for (i = 0; i < half; i++)
  {
    info += "  " + pad_chars(names[i] + ":", width + 1) + " " +
            sprintf("%4d%%", who->query_resistance(DAMAGE_ELEMENTAL_TYPES[i]));
    if (i + half < sizeof(names))
      info += "      " + pad_chars(names[i + half] + ":", width + 1) + " " +
              sprintf("%4d%%", who->query_resistance(DAMAGE_ELEMENTAL_TYPES[i + half]));
    info += "\n";
  }

  if (who->query_property(PROPERTY_SPELL_INVUL))
    info += "\n  " + _LANG_CMD_RESISTANCES_INVULNERABLE + ": " +
            who->query_property(PROPERTY_SPELL_INVUL) + "\n";

  info = handler("frames")->frame(info, _LANG_CMD_RESISTANCES_TITLE,
                                  this_user()->query_cols());
  tell_object(me, info);
  return 1;
}
