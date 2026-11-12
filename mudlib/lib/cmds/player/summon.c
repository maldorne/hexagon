#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_SUMMON_ALIAS);
  set_usage(_LANG_CMD_SUMMON_SYNTAX);
  set_help(_LANG_CMD_SUMMON_HELP);
}

/**
 * summon: calls one of the mounts the player owns to their side; the one that
 * was out is stored. The riding handler decides and tells what the kind of
 * mount refuses; the rest of the refusals are told here.
 */
static int cmd(string str, object me, string verb)
{
  object riding;
  string result;

  if (me->query_dead())
  {
    tell_object(me, _LANG_CMD_SUMMON_DEAD);
    return 1;
  }

  riding = handler("riding", me);

  // with a single mount there is nothing to choose
  if ((!str || !strlen(trim(str))) && (int)riding->query_owned_count(me) == 1)
    str = "1";

  if (!str || !strlen(trim(str)))
  {
    tell_object(me, (int)riding->query_owned_count(me) ?
                _LANG_CMD_SUMMON_WHAT : _LANG_CMD_SUMMON_NONE);
    return 1;
  }

  result = (string)riding->summon(me, trim(str));

  switch (result)
  {
    case "none":      tell_object(me, _LANG_CMD_SUMMON_NONE); break;
    case "unknown":   tell_object(me, _LANG_CMD_SUMMON_UNKNOWN); break;
    case "ambiguous": tell_object(me, _LANG_CMD_SUMMON_AMBIGUOUS); break;
    case "here":      tell_object(me, _LANG_CMD_SUMMON_HERE); break;
    case "cooldown":  tell_object(me, _LANG_CMD_SUMMON_COOLDOWN); break;
    case "failed":    tell_object(me, _LANG_CMD_SUMMON_FAILED); break;
  }

  return 1;
}
