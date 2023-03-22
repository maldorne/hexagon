
// encumbrance cmd from player.c, now external. Morgoth 2/Mar/95

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_ENCUMBRANCE_ALIAS);
  set_usage(_LANG_CMD_ENCUMBRANCE_SYNTAX);
  set_help(_LANG_CMD_ENCUMBRANCE_HELP);
}

static int cmd(string arg, object me, string verb)
{
  int now, max;
  
  now = me->query_contents_weight();
  max = me->query_max_weight();
  
  write(_LANG_CMD_ENCUMBRANCE_MSG);
  return 1;
}
