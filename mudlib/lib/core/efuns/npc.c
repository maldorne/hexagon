// NPC-related simulated efuns.

// Save directory for a persisted NPC: one folder per NPC, keyed by its uuid
// and bucketed by the uuid's first character, under the owning game. Callers
// append the file name (npc.o). Mirrors player_save_dir.
static string npc_save_dir(string game, string id)
{
  return "/save/games/" + game + "/npcs/" + id[0..0] + "/" + id + "/";
}
