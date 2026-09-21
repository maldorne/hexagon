/* This is a pure ripoff from DiscWorld.  This room will make it far easier
 * for you to add new quests.  Otherwise using the quest_handler can be real
 * frustrating.
 */
/*	quest_room.c

	This room is used to add, list, and remove quests.

	This file was constructed by Godot (Marshall Buhl).  If you
	make any changes to it, please leave comments in the code
	detailing the changes and let him know about them.  This file
	is not the original, so it is important that he be informed.
	If not, he may unknowingly throw away your changes by
	uploading an new version.

    Change log:

	12/05/92 - Created by Godot.
	09/2026  - Rewritten for Hexagon, neverbot: quests are now code under
	           /games/<game>/quests/, so the room no longer creates them. It
	           inspects and validates what each game registers, and lets a
	           coder move a player's quests around while testing.

*/

#include <living/quests.h>
#include <user/quests.h>
#include "path.h"

inherit "/lib/room.c";

#define GAMES "/games/"
#define SOURCE "quest room"

void setup()
{
  set_short("Quest Room");
  set_long("The walls of this room are covered in parchments, one for every " +
    "quest a game offers, pinned in rows and annotated in a small, careful " +
    "hand. Type 'list' to begin.\n");
  set_light(100);
  add_exit(DIR_NORTH, ADMIN + "admin2", "standard");
}

void init()
{
  ::init();

  add_action("do_list",     "list");
  add_action("do_show",     "show");
  add_action("do_check",    "check");
  add_action("do_progress", "progress");
  add_action("do_assign",   "assign");
  add_action("do_fulfil",   "fulfil");
  add_action("do_forget",   "forget");
}

// The quests handler a game ships, or nil if it has none of its own.
private object quests_of(string game)
{
  object quests;

  if (!game || file_size(GAMES + game + "/handlers/quests.c") < 0)
    return nil;

  catch(quests = load_object(GAMES + game + "/handlers/quests"));

  return quests;
}

// The game a quest id belongs to: what comes before the colon.
private string game_of_id(string id)
{
  string game, rest;

  if (!id || sscanf(id, "%s:%s", game, rest) != 2)
    return nil;

  return game;
}

// A quest definition by its id, or nil with a failure message already set.
private object quest_by_id(string id)
{
  object quests, quest;

  quests = quests_of(game_of_id(id));

  if (!quests)
  {
    notify_fail("No game has a quests handler for '" + id + "'.\n");
    return nil;
  }

  quest = quests->query_quest(id);

  if (!quest)
    notify_fail("'" + id + "' is not registered in " + game_of_id(id) + ".\n");

  return quest;
}

// Whether an objective or hand-in target names something that exists: a
// template id under the game, or a file path.
private int target_exists(string game, string target)
{
  string base;

  if (!target || !strlen(target))
    return 0;

  base = GAMES + game + "/" + target;

  return file_size(base + ".c") >= 0 ||
         file_size(base + "." + mud_language() + ".json") >= 0 ||
         file_size(target) >= 0 ||
         file_size(target + ".c") >= 0 ||
         file_size(target + ".o") >= 0;
}

// A reward or any other declared value, as text.
private string describe(mixed value)
{
  string * parts;
  int i;

  if (arrayp(value))
  {
    parts = ({ });
    for (i = 0; i < sizeof(value); i++)
      parts += ({ describe(value[i]) });
    return "({ " + implode(parts, ", ") + " })";
  }

  if (stringp(value))
    return "\"" + value + "\"";

  return "" + value;
}

private void show_frame(string text, string title)
{
  tell_object(this_player(), handler("frames")->frame(text, title,
                                                      this_user()->query_cols()));
}

// The player a command is about, or nil with a failure message already set.
private object player_named(string name)
{
  object player;

  player = find_player(lower_case(name));

  if (!player)
    notify_fail("There is nobody called '" + name + "' playing right now.\n");

  return player;
}

/*
 * list            -> the games that register quests, and the commands
 * list <game>     -> every quest that game registers, and the files under its
 *                    quests directory that it does not
 */
int do_list(string game)
{
  object quests, quest;
  string * dirs, * ids, * files, * registered_files;
  string text, path;
  int i;

  if (!this_player()->query_coder())
    return 0;

  if (!game || !strlen(game))
  {
    text = "";
    dirs = get_dir(GAMES);

    for (i = 0; i < sizeof(dirs); i++)
    {
      quests = quests_of(dirs[i]);
      if (quests)
        text += sprintf("  %-20s %d quests\n", dirs[i],
                        sizeof(quests->query_quest_ids()));
    }

    if (!strlen(text))
      text = "  No game registers any quest.\n";

    text = "Games with a quests handler:\n" + text + "\n" +
      "  list <game>                  quests a game registers\n" +
      "  show <id>                    everything a quest declares\n" +
      "  check <game>                 problems in a game's quests\n" +
      "  progress <player> [<game>]   a player's quests\n" +
      "  assign <player> <id>         start a quest for a player, no checks\n" +
      "  fulfil <player> <id>         meet every objective of an active quest\n" +
      "  forget <player> <id>         wipe a quest, active or handed in\n";

    show_frame(text, "Quests");
    return 1;
  }

  quests = quests_of(game);

  if (!quests)
  {
    notify_fail("'" + game + "' has no quests handler of its own.\n");
    return 0;
  }

  ids = quests->query_quest_ids();
  registered_files = ({ });
  text = "";

  for (i = 0; i < sizeof(ids); i++)
  {
    quest = quests->query_quest(ids[i]);

    if (!quest)
    {
      text += "  " + ids[i] + "  (cannot be loaded)\n";
      continue;
    }

    registered_files += ({ base_name(quest) });
    text += "  " + ids[i] + "\n    " + quest->query_title() +
      (quest->query_repeatable() ? "  [repeatable]" : "") + "\n";
  }

  if (!sizeof(ids))
    text += "  No quest registered.\n";

  // files in the quests directory the handler does not register
  files = get_dir(GAMES + game + "/quests/*.c");

  for (i = 0; i < sizeof(files); i++)
  {
    path = GAMES + game + "/quests/" + files[i][0 .. strlen(files[i]) - 3];
    if (member_array(path, registered_files) == -1)
      text += "  not registered: " + files[i] + "\n";
  }

  show_frame(text, "Quests in " + game);
  return 1;
}

// show <id> -> everything a quest declares
int do_show(string id)
{
  object quest;
  mapping * objectives, * rewards;
  string * needs;
  string text, description;
  int i;

  if (!this_player()->query_coder())
    return 0;

  if (!id || !strlen(id))
  {
    notify_fail("Syntax: show <quest id>\n");
    return 0;
  }

  quest = quest_by_id(id);

  if (!quest)
    return 0;

  text = quest->query_title() + "\n" +
    "  id:          " + quest->query_id() + "\n" +
    "  file:        " + base_name(quest) + ".c\n" +
    "  repeatable:  " + (quest->query_repeatable() ? "yes" : "no") + "\n";

  if (quest->query_needs_level())
    text += "  needs level: " + quest->query_needs_level() + "\n";

  needs = quest->query_needs_quests();
  if (sizeof(needs))
    text += "  needs:       " + implode(needs, ", ") + "\n";

  if (strlen(quest->query_hand_in()))
    text += "  hand in to:  " + quest->query_hand_in() + "\n";
  if (strlen(quest->query_hand_in_place()))
    text += "  hand in at:  " + quest->query_hand_in_place() + "\n";

  description = quest->query_description();
  if (strlen(description) && description[strlen(description) - 1] != '\n')
    description += "\n";
  text += "\n" + description;

  objectives = quest->query_objectives();
  text += "\nObjectives:\n";
  if (!sizeof(objectives))
    text += "  none\n";
  for (i = 0; i < sizeof(objectives); i++)
    text += "  " + (i + 1) + ". " + objectives[i][OBJ_KIND] + " " +
      objectives[i][OBJ_TARGET] + " x" + objectives[i][OBJ_COUNT] + "\n" +
      "     " + objectives[i][OBJ_TEXT] + "\n";

  rewards = quest->query_rewards();
  text += "\nRewards:\n";
  if (!sizeof(rewards))
    text += "  none\n";
  for (i = 0; i < sizeof(rewards); i++)
    text += "  " + rewards[i][REWARD_KIND] + " " +
      describe(rewards[i][REWARD_VALUE]) + "\n";

  show_frame(text, "Quest");
  return 1;
}

// check <game> -> what in a game's quests points at nothing or is left unset
int do_check(string game)
{
  object quests, quest;
  mapping * objectives, * rewards;
  string * ids, * needs, * problems, * kinds, * reward_kinds;
  string text;
  int i, j;

  if (!this_player()->query_coder())
    return 0;

  quests = quests_of(game);

  if (!quests)
  {
    notify_fail("Syntax: check <game>, for a game with a quests handler.\n");
    return 0;
  }

  kinds = ({ OBJECTIVE_KILL, OBJECTIVE_BEAT, OBJECTIVE_GET, OBJECTIVE_REACH,
             OBJECTIVE_TALK });
  reward_kinds = ({ REWARD_XP, REWARD_JOB_XP, REWARD_MONEY, REWARD_ITEM,
                    REWARD_TITLE, REWARD_SKILL });

  ids = quests->query_quest_ids();
  text = "";

  for (i = 0; i < sizeof(ids); i++)
  {
    problems = ({ });
    quest = quests->query_quest(ids[i]);

    if (!quest)
    {
      text += ids[i] + "\n  cannot be loaded\n";
      continue;
    }

    if (game_of_id(ids[i]) != game)
      problems += ({ "the id does not start with '" + game + ":'" });
    if (!strlen(quest->query_title()))
      problems += ({ "no title" });
    if (!strlen(quest->query_description()))
      problems += ({ "no description" });

    objectives = quest->query_objectives();

    if (!sizeof(objectives) && !strlen(quest->query_hand_in()) &&
        !strlen(quest->query_hand_in_place()))
      problems += ({ "no objectives and nowhere to hand it in" });

    for (j = 0; j < sizeof(objectives); j++)
    {
      if (member_array(objectives[j][OBJ_KIND], kinds) == -1)
        problems += ({ "objective " + (j + 1) + ": unknown kind '" +
                       objectives[j][OBJ_KIND] + "'" });
      if (!target_exists(game, objectives[j][OBJ_TARGET]))
        problems += ({ "objective " + (j + 1) + ": nothing found for '" +
                       objectives[j][OBJ_TARGET] + "'" });
      if (!objectives[j][OBJ_TEXT] || !strlen(objectives[j][OBJ_TEXT]))
        problems += ({ "objective " + (j + 1) + ": no text" });
    }

    if (strlen(quest->query_hand_in()) &&
        !target_exists(game, quest->query_hand_in()))
      problems += ({ "hand in: nothing found for '" +
                     quest->query_hand_in() + "'" });

    if (strlen(quest->query_hand_in_place()) &&
        !target_exists(game, quest->query_hand_in_place()))
      problems += ({ "hand in at: nothing found for '" +
                     quest->query_hand_in_place() + "'" });

    needs = quest->query_needs_quests();
    for (j = 0; j < sizeof(needs); j++)
      if (!quests->query_quest(needs[j]))
        problems += ({ "needs '" + needs[j] + "', which is not registered" });

    rewards = quest->query_rewards();
    for (j = 0; j < sizeof(rewards); j++)
    {
      if (member_array(rewards[j][REWARD_KIND], reward_kinds) == -1)
        problems += ({ "reward " + (j + 1) + ": unknown kind '" +
                       rewards[j][REWARD_KIND] + "'" });
      if (rewards[j][REWARD_KIND] == REWARD_ITEM &&
          !target_exists(game, rewards[j][REWARD_VALUE]))
        problems += ({ "reward " + (j + 1) + ": no item at " +
                       describe(rewards[j][REWARD_VALUE]) });
    }

    text += ids[i] + "\n";
    if (!sizeof(problems))
      text += "  ok\n";
    for (j = 0; j < sizeof(problems); j++)
      text += "  - " + problems[j] + "\n";
  }

  if (!sizeof(ids))
    text = "No quest registered.\n";

  show_frame(text, "Check " + game);
  return 1;
}

// progress <player> [<game>] -> a player's quests, active and handed in
int do_progress(string str)
{
  object player, quests, quest;
  mapping active, done;
  mapping * objectives;
  string * ids;
  string name, game, text;
  int * progress;
  int i, j;

  if (!this_player()->query_coder())
    return 0;

  if (!str || sscanf(str, "%s %s", name, game) != 2)
    name = str;

  if (!name || !strlen(name))
  {
    notify_fail("Syntax: progress <player> [<game>]\n");
    return 0;
  }

  player = player_named(name);

  if (!player)
    return 0;

  if (!game)
    game = game_name(player);

  quests = quests_of(game);
  active = player->query_active_quests(game);
  done = player->query_done_quests(game);
  text = "Doing:\n";

  ids = map_indices(active);
  if (!sizeof(ids))
    text += "  nothing\n";

  for (i = 0; i < sizeof(ids); i++)
  {
    quest = quests ? quests->query_quest(ids[i]) : nil;
    progress = active[ids[i]][QUEST_PROGRESS];

    text += "  " + ids[i] + "\n" +
      "    from " + active[ids[i]][QUEST_SOURCE] + ", " +
      ctime(active[ids[i]][QUEST_STARTED]) + "\n";

    if (!quest)
    {
      text += "    (not registered in " + game + ")\n";
      continue;
    }

    objectives = quest->query_objectives();
    for (j = 0; j < sizeof(objectives); j++)
      text += "    " + (j + 1) + ". " + objectives[j][OBJ_KIND] + " " +
        objectives[j][OBJ_TARGET] + ": " +
        (j < sizeof(progress) ? progress[j] : 0) + "/" +
        objectives[j][OBJ_COUNT] + "\n";
  }

  text += "\nHanded in:\n";

  ids = map_indices(done);
  if (!sizeof(ids))
    text += "  nothing\n";

  for (i = 0; i < sizeof(ids); i++)
    text += "  " + ids[i] + "\n    " + done[ids[i]][QUEST_TIMES] +
      " time(s), last " + ctime(done[ids[i]][QUEST_LAST]) + "\n";

  show_frame(text, capitalize(player->query_name()) + " in " + game);
  return 1;
}

// The player and the quest of a "<player> <id>" argument, as ({ player, quest }),
// or nil with a failure message already set.
private mixed * player_and_quest(string str, string verb)
{
  object player, quest;
  string name, id;

  if (!str || sscanf(str, "%s %s", name, id) != 2)
  {
    notify_fail("Syntax: " + verb + " <player> <quest id>\n");
    return nil;
  }

  player = player_named(name);

  if (!player)
    return nil;

  quest = quest_by_id(id);

  if (!quest)
    return nil;

  return ({ player, quest });
}

// assign <player> <id> -> start a quest without asking whether it may be taken
int do_assign(string str)
{
  object player, quest;
  mixed * found;
  string game;

  if (!this_player()->query_coder())
    return 0;

  found = player_and_quest(str, "assign");

  if (!found)
    return 0;

  player = found[0];
  quest = found[1];

  game = game_of_id(quest->query_id());

  player->start_quest(game, quest->query_id(),
                         quest->query_objective_count(), SOURCE);

  write("Started " + quest->query_id() + " for " +
        player->query_cap_name() + ".\n");
  return 1;
}

// fulfil <player> <id> -> meet every objective of a quest being done
int do_fulfil(string str)
{
  object player, quest;
  mixed * found;
  mapping * objectives;
  string game, id;
  int i;

  if (!this_player()->query_coder())
    return 0;

  found = player_and_quest(str, "fulfil");

  if (!found)
    return 0;

  player = found[0];
  quest = found[1];

  id = quest->query_id();
  game = game_of_id(id);

  if (!player->is_doing_quest(game, id))
  {
    notify_fail(player->query_cap_name() + " is not doing " + id + ".\n");
    return 0;
  }

  objectives = quest->query_objectives();

  for (i = 0; i < sizeof(objectives); i++)
    player->adjust_progress(game, id, i, objectives[i][OBJ_COUNT],
                               objectives[i][OBJ_COUNT]);

  write("Every objective of " + id + " met for " +
        player->query_cap_name() + ".\n");
  return 1;
}

// forget <player> <id> -> wipe a quest, as if it had never been taken
int do_forget(string str)
{
  object player, quest;
  mixed * found;
  string id;

  if (!this_player()->query_coder())
    return 0;

  found = player_and_quest(str, "forget");

  if (!found)
    return 0;

  player = found[0];
  quest = found[1];

  id = quest->query_id();
  player->forget_quest(game_of_id(id), id);

  write(player->query_cap_name() + " no longer remembers " + id + ".\n");
  return 1;
}
