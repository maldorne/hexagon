
// wimpy cmd from player.c, now external. Morgoth 7/Mar/95
// Changed: if you type "wimpy" without a number, or with an out of range
//    number, it will now show you the actual setting, without changing
//    it, after warning you. The old version would not show it in either
//    case, and would set it to 0 in the latter case.

#include <mud/cmd.h>
#include <translations/common.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_WIMPY_ALIAS);
  set_usage(_LANG_CMD_WIMPY_SYNTAX);
  set_help(_LANG_CMD_WIMPY_HELP);
}

static int cmd(string str, object me, string verb)
{
  int wimpy;

  if (!strlen(str))
  {
    write(_LANG_SYNTAX + ": " +_LANG_CMD_WIMPY_SYNTAX + "\n" + _LANG_CMD_WIMPY_NO_VALID);
    return 1;
  }

  if (sscanf(str, "%d", wimpy) != 1)
  {
    notify_fail(_LANG_CMD_WIMPY_NO_NUMBER);
    return 0;
  }

  if (wimpy < 0 || wimpy > 100)
  {
    notify_fail(_LANG_CMD_WIMPY_NO_VALID);
    return 0;
  }
  
  wimpy = me->set_wimpy(wimpy);

  if (wimpy == 0)
    tell_object(me, _LANG_CMD_WIMPY_BRAVE);
  else
    tell_object(me, _LANG_CMD_WIMPY_MSG);

  return 1;
}
