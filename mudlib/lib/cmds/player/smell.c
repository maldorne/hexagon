// smell -- what something here smells like, for whoever smells it.
//
// 10/2026 - Created for Hexagon, neverbot, replacing the sense actions every
//           room added by itself (see /lib/cmds/sense.c).

#include <language.h>

inherit "/lib/cmds/sense.c";

void setup()
{
  set_sense("smell");
  set_aliases(_LANG_CMD_SMELL_ALIAS);
  set_usage(_LANG_CMD_SMELL_SYNTAX);
  set_help(_LANG_CMD_SMELL_HELP);
}

string query_nothing_here() { return _LANG_CMD_SMELL_NOTHING_HERE; }
string query_nothing_in(object ob) { return _LANG_CMD_SMELL_NOTHING_IN; }
