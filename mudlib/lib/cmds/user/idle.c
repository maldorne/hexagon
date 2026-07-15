
// idle - set the linkdead grace period for this session
//
// int idle();          // show current grace period, plus bounds
// int idle(int secs);  // set grace period (bounded to [min, max])
//
// While a connection is dropped the player object survives for this many
// seconds so combat cannot be dodged by pulling the plug. The window is
// bounded per-mudlib in /lib/user.c (LINKDEAD_MIN / LINKDEAD_MAX). A
// reconnect within the window silently takes over the same player object
// with all its runtime state (combat, timed props, callouts). After the
// window expires the object is destructed and the next login loads a
// fresh copy from disk.

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

string query_usage()
{
  return _LANG_IDLE_SYNTAX;
}

string query_help()
{
  return _LANG_IDLE_HELP;
}

int cmd(string str, object me, string verb)
{
  object user;
  int value;

  user = me->user();
  if (!user)
    return 0;

  if (!str || str == "")
  {
    tell_object(me, sprintf(_LANG_IDLE_SHOW,
      user->query_linkdead_grace(),
      user->query_linkdead_min(),
      user->query_linkdead_max()));
    return 1;
  }

  value = to_int(str);
  if (value <= 0)
  {
    tell_object(me, _LANG_IDLE_SYNTAX);
    return 0;
  }

  value = user->set_linkdead_grace(value);
  tell_object(me, sprintf(_LANG_IDLE_SET, value));
  return 1;
}
