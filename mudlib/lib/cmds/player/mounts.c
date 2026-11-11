
#include <mud/cmd.h>
#include <living/riding.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_MOUNTS_ALIAS);
  set_usage(_LANG_CMD_MOUNTS_SYNTAX);
  set_help(_LANG_CMD_MOUNTS_HELP);
}

/**
 * mounts: the mounts the player owns and where each one is.
 */
static int cmd(string str, object me, string verb)
{
  mapping * owned;
  string text, place;
  object mount;
  int i;

  owned = handler("riding", me)->query_owned(me);
  if (!sizeof(owned))
  {
    tell_object(me, _LANG_CMD_MOUNTS_NONE);
    return 1;
  }

  text = _LANG_CMD_MOUNTS_HEADER;
  for (i = 0; i < sizeof(owned); i++)
  {
    mount = owned[i]["mount"];

    // a mount that is out stands in a place; one that is not is not anywhere
    place = mount && environment(mount) ?
        (string)environment(mount)->short() : _LANG_CMD_MOUNTS_NOWHERE;
    if (!stringp(place) || !strlen(place))
      place = _LANG_CMD_MOUNTS_UNKNOWN;

    text += _LANG_CMD_MOUNTS_LINE;
  }

  tell_object(me, text);
  return 1;
}
