// balance -- admin tool to dump the numbers that balance a game: its races,
// classes, guilds, jobs, damaging and healing spells and skills, evaluated by the
// game's own code on a test monster, written as CSV files under
// /games/<game>/docs/balance/ (one folder per game). The work is done by the
// balance package (/packages/balance/).
//
//   balance [<game>]    the game of the room you are in by default

#include <mud/cmd.h>
#include "/packages/balance/src/balance.h"

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "balance" }));
  set_usage("balance [<game>]");
  set_help(
    "Dumps the numbers that balance a game as CSV files under\n" +
    "/games/<game>/docs/balance/: its races, classes, guilds, jobs, the spells and\n" +
    "skills that damage or heal, and the baselines (hit points, regeneration)\n" +
    "they are measured against. Everything is evaluated by the game's own code\n" +
    "on a test monster with every stat at " + BALANCE_STAT + ", a spell for each\n" +
    "level 1 to " + BALANCE_LEVELS + " and power 1 to " + BALANCE_POWERS + ".\n" +
    "\n" +
    "  balance            the game of the room you are in\n" +
    "  balance <game>     another game, by its folder name\n" +
    "\n" +
    "Files: races.csv, classes.csv, guilds.csv, jobs.csv, effects.csv, skills.csv,\n" +
    "baselines.csv, stat_bonuses.csv. The work is split in steps and takes a\n" +
    "few seconds; the command says when it is done. A game adds columns to\n" +
    "baselines.csv with query_balance_baselines(level) in its master.\n" +
    "The mud's web server shows them at /balance/<game>/.\n");
}

static int cmd(string str, object me, string verb)
{
  object balance;
  string error;

  if (this_player(1) != this_user())
    return 0;

  balance = package("balance");

  if (!balance)
  {
    tell_object(me, "The balance package is not available.\n");
    return 1;
  }

  error = balance->start_dump(me, strlen(str) ? trim(str) : game_name(me));

  if (strlen(error))
    tell_object(me, error);

  return 1;
}
