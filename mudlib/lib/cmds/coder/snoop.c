
// Watch another session: everything it is told, and everything it types.
// The link itself lives on the two user objects; the secure object decides
// who may make it. The person being watched is told, and the staff channel
// hears about it.

#include <std.h>
#include <mud/cmd.h>
#include <mud/secure.h>
#include <translations/inform.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "snoop" }));
  set_usage("snoop [<name>]");
  set_help(
    "Watch somebody's session.\n" +
    "\n" +
    "  snoop <name>  start watching them\n" +
    "  snoop         stop watching whoever you are watching\n" +
    "\n" +
    "You see what the game tells them and, marked with their name in " +
    "brackets, what they type. They are told they are being watched, and " +
    "so is anybody listening to the snoop inform channel. Staff cannot be " +
    "watched, nobody can be watched by two people at once, and a session " +
    "that is already watching somebody cannot itself be watched.\n" +
    "\n" +
    "To watch without telling them, admins have 'qsnoop'.");
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
             " stops snooping " + name + ".\n");
    event(users(), "inform", me->query_cap_name() + " stops snooping " +
          name, INFORM_EVENT_SNOOP, me);
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
  write("You start watching " + body->query_cap_name() + ".\n");
  tell_object(body, _LANG_SNOOP_WATCHED(me->query_cap_name()));
  log_file("snoop", "[" + ctime(time(), 4) + "] " + me->query_cap_name() +
           " snoops " + body->query_cap_name() + ".\n");
  event(users(), "inform", me->query_cap_name() + " starts snooping " +
        body->query_cap_name(), INFORM_EVENT_SNOOP, me);
  return 1;
}
