
#include <mud/cmd.h>
#include <living/riding.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_STEEDS_ALIAS);
  set_usage(_LANG_CMD_STEEDS_SYNTAX);
  set_help(_LANG_CMD_STEEDS_HELP);
}

/**
 * steeds: the steeds the player owns and where each one is.
 */
static int cmd(string str, object me, string verb)
{
  mapping owned;
  string * ids, text, place;
  object steed;
  int i;

  owned = handler("riding", me)->query_owned((string)me->query_name(), me);
  if (!map_sizeof(owned))
  {
    tell_object(me, _LANG_CMD_STEEDS_NONE);
    return 1;
  }

  text = _LANG_CMD_STEEDS_HEADER;
  ids = map_indices(owned);
  for (i = 0; i < sizeof(ids); i++)
  {
    steed = owned[ids[i]]["steed"];

    // a steed that is out stands in a place; one that is not is not anywhere
    place = steed && environment(steed) ?
        (string)environment(steed)->short() : _LANG_CMD_STEEDS_NOWHERE;
    if (!stringp(place) || !strlen(place))
      place = _LANG_CMD_STEEDS_UNKNOWN;

    text += _LANG_CMD_STEEDS_LINE;
  }

  tell_object(me, text);
  return 1;
}
