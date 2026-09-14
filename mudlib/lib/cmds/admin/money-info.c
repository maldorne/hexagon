
// What the economy is doing: how much money the game hands out and takes back,
// per place, from the money tracker.

#include <mud/cmd.h>
#include <mud/secure.h>
#include <basic/money.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "money-info" }));
  set_usage("money-info [<game>]");
  set_help(
    "Report what the money tracker has seen, place by place.\n" +
    "\n" +
    "  money-info                 every place that has moved money\n" +
    "  money-info <game>          only the places of that game\n" +
    "\n" +
    "A place is the directory a transaction happened in, so it reads as " +
    "game and area. Every figure is in copper per player hour, which is " +
    "what makes two areas comparable however busy each has been:\n" +
    "\n" +
    "  given / taken    what reached players, and what came back\n" +
    "  per level        the same, divided by the level of who earned it\n" +
    "  under 15         only what players below level 15 earned\n" +
    "\n" +
    "The first table counts what was given and taken separately; the " +
    "second nets them off, which is the number that says whether a place " +
    "leaks money into the game.");
}

static int cmd(string str, object me, string verb)
{
  string ret, game;

  game = (str && strlen(str)) ? str : nil;

  ret = "%^BOLD%^GREEN%^Given and taken%^RESET%^:\n\n";
  ret += game ? MONEY_TRACKER->full_domain_stats(game)
              : MONEY_TRACKER->full_domain_stats();

  ret += "\n%^BOLD%^GREEN%^Netted off%^RESET%^:\n\n";
  ret += game ? MONEY_TRACKER->select_domain_stats(game)
              : MONEY_TRACKER->select_domain_stats();

  me->user()->more_string(ret);
  return 1;
}
