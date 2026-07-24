
// idle - set the personal idle grace period, in MINUTES
//
// int idle();           // show current grace + allowed range
// int idle(int mins);   // set grace (clamped to [min, max])
//
// The idle grace period drives two behaviours:
//   * AFK auto-kick: if the interactive player sends no input for this
//     many minutes, the session is force-quit (with save).
//   * Linkdead grace: if the TCP connection drops, the player object
//     survives this long so a reconnect can silently resume the same
//     runtime state (combat, timed props, callouts). Once the grace
//     expires, the player is destructed for good and the next login
//     loads a fresh copy from disk.
// Range and default live in /lib/user.c (IDLE_GRACE_MIN/MAX/DEFAULT).

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
      user->query_idle_grace(),
      user->query_idle_grace_min(),
      user->query_idle_grace_max()));
    return 1;
  }

  value = to_int(str);
  if (value <= 0)
  {
    tell_object(me, _LANG_IDLE_SYNTAX);
    return 0;
  }

  value = user->set_idle_grace(value);
  tell_object(me, sprintf(_LANG_IDLE_SET, value));
  return 1;
}
