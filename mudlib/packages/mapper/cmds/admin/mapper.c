// mapper -- admin tool that writes a player-safe JSON map of an area. The work
// is done by the mapper package (/packages/mapper/).
//
//   mapper update <area dir> [all]
//   mapper list [<game>]

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_MAPPER_ALIAS);
  set_usage(_LANG_CMD_MAPPER_SYNTAX);
  set_help(_LANG_CMD_MAPPER_HELP);
}

static int cmd(string str, object me, string verb)
{
  string * words;
  string dir, error;
  object mapper;

  if (this_player(1) != this_user())
    return 0;

  if (!strlen(str))
  {
    tell_object(me, _LANG_CMD_MAPPER_USAGE);
    return 1;
  }

  mapper = package("mapper");

  if (!mapper)
  {
    tell_object(me, "The mapper package is not available.\n");
    return 1;
  }

  words = explode(trim(str), " ") - ({ "" });

  if (words[0] == "list" && sizeof(words) <= 2)
  {
    tell_object(me, mapper->list_maps(sizeof(words) == 2 ? words[1] : ""));
    return 1;
  }

  if (words[0] == "update" && (sizeof(words) == 2 ||
      (sizeof(words) == 3 && words[2] == "all")))
  {
    // relative to the current folder, with or without a trailing slash
    dir = resolve_path(words[1]);

    error = mapper->update_map(me, dir, sizeof(words) == 3);

    if (strlen(error))
      tell_object(me, error);

    return 1;
  }

  tell_object(me, _LANG_CMD_MAPPER_USAGE);
  return 1;
}
