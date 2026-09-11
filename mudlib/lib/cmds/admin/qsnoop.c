
// Watch another session without telling anybody. Same link as 'snoop': only
// the notice to the watched person and the inform to the channel are left
// out. The log entry is not -- every snoop is logged.

#include <std.h>
#include <mud/cmd.h>
#include <mud/secure.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "qsnoop" }));
  set_usage("qsnoop [<name>]");
  set_help(
    "Watch somebody's session quietly.\n" +
    "\n" +
    "  qsnoop <name>  start watching them\n" +
    "  qsnoop         stop watching whoever you are watching\n" +
    "\n" +
    "The same as 'snoop', except that neither they nor the inform channel " +
    "are told. It still goes to the snoop log, and 'snoopers' still lists " +
    "it. The limits are the same: staff cannot be watched, and no session " +
    "can be at both ends of a link.");
}

static int cmd(string str, object me, string verb)
{
  object user, target, body;
  string name;

  user = me->user();

  if (!str || !strlen(str))
  {
    target = user->query_snooping();
    if (!target)
    {
      notify_fail("You are not watching anybody.\n");
      return 0;
    }

    body = target->player();
    name = body ? body->query_cap_name() : "a session";
    user->set_snooping(nil);
    write("You stop watching " + name + ".\n");
    log_file("snoop", "[" + ctime(time(), 4) + "] " + me->query_cap_name() +
             " stops qsnooping " + name + ".\n");
    return 1;
  }

  target = find_player(lower_case(str));
  target = target ? target->user() : nil;
  if (!target)
  {
    notify_fail("Nobody called " + capitalize(str) + " is connected.\n");
    return 0;
  }

  if (!user->set_snooping(target))
  {
    notify_fail("You may not watch " + capitalize(str) + ".\n");
    return 0;
  }

  body = target->player();
  write("You start watching " + body->query_cap_name() + ", quietly.\n");
  log_file("snoop", "[" + ctime(time(), 4) + "] " + me->query_cap_name() +
           " qsnoops " + body->query_cap_name() + ".\n");
  return 1;
}
