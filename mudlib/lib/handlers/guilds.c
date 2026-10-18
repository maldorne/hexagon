// Guilds handler: which guilds a game has, and what they teach. The effects
// ask it whether a spell or a skill belongs to a guild at all, to tell the
// ones a guild teaches (usable only by its members) from the generic ones.
//
// A game keeps its guilds under <game root>obj/guilds/ (one level of folders).

inherit "/lib/core/object.c";

// game root -> the guild files found under it
private mapping guild_files;

void create()
{
  ::create();
  guild_files = ([ ]);
}

/**
 * The .c files of a folder and of the folders under it (one level).
 * @param  dir  the folder, ending in /
 * @return full paths
 */
private string * files_under(string dir)
{
  string * names, * more, * files;
  int i, j;

  files = ({ });
  names = get_dir(dir + "*");
  if (!names)
    return files;

  for (i = 0; i < sizeof(names); i++)
  {
    if (file_size(dir + names[i]) == -2)
    {
      more = get_dir(dir + names[i] + "/*.c");
      for (j = 0; more && j < sizeof(more); j++)
        files += ({ dir + names[i] + "/" + more[j] });
    }
    else if (strlen(names[i]) > 2 && names[i][strlen(names[i]) - 2..] == ".c")
      files += ({ dir + names[i] });
  }

  return files;
}

/**
 * The guilds of the game an object is in, loaded. The folder is read once;
 * updating this handler reads it again.
 * @param  who  any object of the game
 * @return the guild objects, none for an object outside any game
 */
object * query_guilds(object who)
{
  string root;
  object * guilds;
  object guild;
  int i;

  root = game_root(who);
  if (!root || !strlen(root))
    return ({ });

  if (!guild_files[root])
    guild_files[root] = files_under(root + "obj/guilds/");

  guilds = ({ });
  for (i = 0; i < sizeof(guild_files[root]); i++)
  {
    guild = nil;
    catch(guild = load_object(guild_files[root][i]));
    if (guild && guild->query_is_guild())
      guilds += ({ guild });
  }

  return guilds;
}

/**
 * Whether any guild of the game an object is in teaches an effect.
 * @param  key  the spell's path (with .c) or the skill's id
 * @param  who  any object of the game
 */
int query_taught(string key, object who)
{
  object * guilds;
  int i;

  guilds = query_guilds(who);
  for (i = 0; i < sizeof(guilds); i++)
    if (guilds[i]->query_teaches(key))
      return 1;

  return 0;
}
