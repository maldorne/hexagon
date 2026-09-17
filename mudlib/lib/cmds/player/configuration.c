/* ********************************************************************
 *
 *   Command to show and change the player configuration
 *   neverbot 04
 *
 * ********************************************************************
 */
// Reviewed for Hexagon, neverbot 09/2026: every value is checked against
// what its setting accepts, a wrong one is refused with the values that
// would do, and each category says whether it belongs to the account or to
// the character.

#include <mud/cmd.h>
#include <user/configuration.h>
#include <translations/common.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_CONFIG_ALIAS);
  set_usage(_LANG_CMD_CONFIG_SYNTAX);
  set_help(_LANG_CMD_CONFIG_HELP);
}

// A word as typed, compared without case and without accents, so that both
// spellings a player may use name the same thing.
private string plain(string word)
{
  string * from, * to;
  int i;

  word = lower_case(word);
  from = ({ "á", "é", "í", "ó", "ú", "ü", "Á", "É", "Í", "Ó", "Ú" });
  to   = ({ "a", "e", "i", "o", "u", "u", "a", "e", "i", "o", "u" });

  for (i = 0; i < sizeof(from); i++)
    word = replace_string(word, from[i], to[i]);

  return word;
}

// The name of a topic of the category that a typed word stands for.
private string topic_named(mapping topics, string word)
{
  string * names;
  int i;

  names = map_indices(topics);

  for (i = 0; i < sizeof(names); i++)
    if (plain(names[i]) == plain(word))
      return names[i];

  return nil;
}

private object target_of(mapping setting, object me)
{
  return setting[CONFIG_OWNER] == CONFIG_ON_USER ? me->user() : me;
}

private mixed query_value(mapping setting, object me)
{
  object target;

  target = target_of(setting, me);

  if (!target || !function_exists(setting[CONFIG_QUERY], target))
    return nil;

  if (!undefinedp(setting[CONFIG_PARAM]))
    return call_other(target, setting[CONFIG_QUERY], setting[CONFIG_PARAM]);

  return call_other(target, setting[CONFIG_QUERY]);
}

private string show_value(mapping setting, object me)
{
  mixed value;

  value = query_value(setting, me);

  if (setting[CONFIG_TYPE] == CONFIG_BOOL)
    return value ? _LANG_YES : _LANG_NO;

  if (value == nil || (stringp(value) && !strlen(value)))
    return "-";

  return "" + value;
}

// What a setting accepts, for the player to read.
private string accepted(mapping setting)
{
  switch (setting[CONFIG_TYPE])
  {
    case CONFIG_BOOL:
      return _LANG_CMD_CONFIG_ACCEPTS_BOOL;
    case CONFIG_INT:
      return _LANG_CMD_CONFIG_ACCEPTS_INT;
    case CONFIG_CHOICE:
      return _LANG_CMD_CONFIG_ACCEPTS_CHOICE;
  }

  return _LANG_CMD_CONFIG_ACCEPTS_STRING;
}

// Turn what the player typed into the value to store, or nil if the setting
// does not accept it.
private mixed parse_value(mapping setting, string value)
{
  int number, i;

  switch (setting[CONFIG_TYPE])
  {
    case CONFIG_BOOL:
      if (affirmative(value))
        return 1;
      if (negative(value))
        return 0;
      return nil;

    case CONFIG_INT:
      if (sscanf(value, "%d", number) != 1 || ("" + number) != value ||
          number < setting[CONFIG_MIN] || number > setting[CONFIG_MAX])
        return nil;
      return number;

    case CONFIG_CHOICE:
      for (i = 0; i < sizeof(setting[CONFIG_OPTIONS]); i++)
        if (plain(setting[CONFIG_OPTIONS][i]) == plain(value))
          return setting[CONFIG_OPTIONS][i];
      return nil;
  }

  // a free text: one of the words for nothing empties it
  if (member_array(lower_case(value), _LANG_NONE_OPTIONS) != -1)
    return "";

  return value;
}

private string owner_of(mapping topics)
{
  string * names;

  names = map_indices(topics);

  if (!sizeof(names))
    return "";

  return topics[names[0]][CONFIG_OWNER] == CONFIG_ON_USER ?
         _LANG_CMD_CONFIG_OWNER_USER : _LANG_CMD_CONFIG_OWNER_PLAYER;
}

private string show_category(string category, mapping topics, object me)
{
  string * names;
  string ret, title, cmd, value, owner;
  mapping setting;
  int i;

  title = table("configurations")->query_category_name(category);
  owner = owner_of(topics);
  names = sort_array(map_indices(topics));

  ret = "\n" + _LANG_CMD_CONFIG_OPTIONS_IN_CATEGORY + "\n";

  for (i = 0; i < sizeof(names); i++)
  {
    setting = topics[names[i]];
    cmd = setting[CONFIG_COMMAND];

    value = show_value(setting, me);
    if (strlen(value) > 18)
      value = value[0..16] + "~";

    ret += sprintf("  %22s : %-18s %s\n", names[i], value,
                   strlen(cmd) ? "[ %^GREEN%^" + cmd + "%^RESET%^ ]" : "");
  }

  return ret;
}

private string frame(string text, object me)
{
  return handler("frames")->frame(text, _LANG_CMD_CONFIG_FOR_USER,
                                  me->user()->query_cols());
}

static int cmd(string str, object me, string verb)
{
  mapping data, setting;
  string * categories, * words;
  string category, topic, value, title, ret, shown;
  mixed parsed, done;
  object target;
  int i;

  data = table("configurations")->query_config_data(me);
  words = explode(trim(str ? str : ""), " ") - ({ "" });

  // everything
  if (!sizeof(words))
  {
    categories = table("configurations")->query_categories();
    ret = "";

    for (i = 0; i < sizeof(categories); i++)
      ret += show_category(categories[i], data[categories[i]], me);

    ret += "\n" + _LANG_CMD_CONFIG_HINT;

    if (me->user()->query_verbose())
      ret += "\n" + _LANG_CMD_CONFIG_USE_COMMANDS;

    write(frame(ret, me));
    return 1;
  }

  category = table("configurations")->query_category_from_name(plain(words[0]));

  if (!strlen(category))
  {
    notify_fail(_LANG_CMD_CONFIG_NOT_VALID_CATEGORY);
    return 0;
  }

  title = table("configurations")->query_category_name(category);

  // one category
  if (sizeof(words) == 1)
  {
    write(frame(show_category(category, data[category], me), me));
    return 1;
  }

  topic = topic_named(data[category], words[1]);
  setting = topic ? data[category][topic] : nil;

  if (!setting)
  {
    notify_fail(_LANG_CMD_CONFIG_NOT_VALID_TOPIC);
    return 0;
  }

  // one setting: its value and what it accepts
  if (sizeof(words) == 2)
  {
    shown = show_value(setting, me);
    write(_LANG_CMD_CONFIG_CURRENT + accepted(setting));
    return 1;
  }

  value = implode(words[2..], " ");
  parsed = parse_value(setting, value);

  if (parsed == nil)
  {
    notify_fail(_LANG_CMD_CONFIG_NOT_VALID_VALUE + accepted(setting));
    return 0;
  }

  target = target_of(setting, me);

  if (!target || !function_exists(setting[CONFIG_SET], target))
  {
    notify_fail(_LANG_CMD_CONFIG_CANNOT_SET);
    return 0;
  }

  if (!undefinedp(setting[CONFIG_PARAM]))
    done = call_other(target, setting[CONFIG_SET], setting[CONFIG_PARAM], parsed);
  else
    done = call_other(target, setting[CONFIG_SET], parsed);

  // some settings can say no for a reason of their own
  if (setting[CONFIG_REFUSES] && !done)
  {
    notify_fail(_LANG_CMD_CONFIG_REFUSED);
    return 0;
  }

  // the setting has the last word: it may round a value or refuse it
  shown = show_value(setting, me);

  if (setting[CONFIG_TYPE] != CONFIG_BOOL && setting[CONFIG_TYPE] != CONFIG_STRING &&
      lower_case("" + query_value(setting, me)) != lower_case("" + parsed))
    write(_LANG_CMD_CONFIG_ADJUSTED);
  else
    write(_LANG_CMD_CONFIG_CURRENT);

  return 1;
}
