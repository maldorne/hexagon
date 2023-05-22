// cmds extracted from global.c, neverbot 4/2003

#include <mud/cmd.h>
#include <translations/common.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "invisible", "invis" }));
  set_usage("invisible");
  set_help("Makes you invisible.");
}

static int cmd(string str, object me, string verb)
{
  if (strlen(str) && str != "1")
  {
    if (!me->query_admin())
      notify_fail(_LANG_SYNTAX + ": " + query_usage() + "\n");
    else
      notify_fail(_LANG_SYNTAX + ": " + query_usage() + "\n" +
                  "          (accepts the optional parameter '1'\n" +
                  "           to leave invisibility 2).\n");
    return 0;
  }

  switch (me->query_invis())
  {
    case 2:
      if (strlen(str) && (str == "1"))
      {
        tell_object(me, "You make yourself a bit more visible.\n");
        me->set_invis(1);
        return 1;
      }

      notify_fail("You already are really invisible.\n");
      return 0;

    case 1:
      if (!me->query_admin()){
        notify_fail("With your current powers you can't make yourself "+ 
          "more invisible than you already are.\n");
        return 0;
      }

      if (!strlen(str))
      {
        tell_object(me, "Now you are really invisible.\n");
        me->set_invis(2);
        return 1;
      }

      notify_fail("You already are invisible.\n");
      return 0;

    default:
    case 0:
      tell_object(me, "You become invisible.\n");
      tell_room(environment(me), me->query_cap_name() + 
        " becomes invisible suddenly.\n", me);
      me->set_invis(1);
      return 1;
  }

  return 1;
}
