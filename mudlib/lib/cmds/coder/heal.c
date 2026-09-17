// heal - a coder gets back their full life, energy and social points.
// From the CcMud 'vida' command.

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_HEAL_ALIASES);
  set_usage(_LANG_HEAL_ALIASES[0]);
  set_help("Restores your life, energy and social points to their maximum.");
}

static int cmd(string str, object me, string verb)
{
  me->set_hp(me->query_max_hp());
  me->set_gp(me->query_max_gp());
  me->set_social_points(me->query_max_social_points());

  tell_object(me, _LANG_HEAL_MESSAGE_MINE);
  tell_room(environment(me), _LANG_HEAL_MESSAGE_OTHERS, ({ me }));
  return 1;
}
