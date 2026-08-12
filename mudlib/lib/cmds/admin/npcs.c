
#include <mud/cmd.h>
#include <areas/area.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "npcs" }));
  set_usage("npcs verify [apply]");
  set_help("Verify the current game's NPC save folders against the census. " +
           "Every persisted NPC lives in /save/games/<game>/npcs/<uuid>/, but " +
           "only those still in an area's census are real; the rest are orphans " +
           "left when a census entry was dropped without the NPC dying (a " +
           "reconversion, a census rebuild, a removed vacancy). Lists orphan " +
           "folders and already-empty folders; 'apply' deletes them. Run it " +
           "standing in the game you want to check.");
}

static int cmd(string str, object me, string verb)
{
  string * args;
  string game;
  object env;
  int apply;

  args = (str && strlen(str)) ? explode(str, " ") : ({ });
  if (!sizeof(args) || args[0] != "verify")
  {
    notify_fail("Usage: npcs verify [apply]\n");
    return 0;
  }
  apply = (sizeof(args) > 1 && args[1] == "apply");

  env = environment(me);
  if (!env || !env->query_file_name())
  {
    notify_fail("Stand in a game location to pick the game to verify.\n");
    return 0;
  }
  game = game_from_path(env->query_file_name());
  if (!game)
  {
    notify_fail("Cannot tell which game you are in.\n");
    return 0;
  }

  // Runs in the background (it may span thousands of areas / folders); the
  // summary is sent when it finishes.
  load_object(AREA_HANDLER)->verify_npc_saves(game, apply, me);
  write("Verifying NPC saves for '" + game + "'" +
        (apply ? " (deleting orphans)" : "") +
        " in the background; the summary will follow.\n");

  return 1;
}
