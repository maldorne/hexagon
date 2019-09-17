
#include <user/migration.h>
#include <mud/secure.h>

// Version de la ficha que utiliza el player, si es menor 
// que la actual del juego, se actualizara cuando haga login
int last_migration;  

void create()
{
  last_migration = 0; 
}

nomask int query_last_migration() { return last_migration; }
private int update_last_migration(int v) 
{ 
  last_migration = v; 
  return last_migration; 
}

nomask int update_player()
{
  int v; 
  string * files;
  int i;

  v = query_last_migration();

  // for safety reasons, we allow set_role only to be called from /lib/core/login
  if (!SECURE->valid_progname("/lib/core/login"))
    return 0;

  // Aun no tiene nivel (es la primera conexion y esta creando el personaje)
  if (this_object()->query_level() < 1)
  {
    update_last_migration(LAST_MIGRATION); 
    return 0;
  }

  // Si ya esta actualizado
  if (v >= LAST_MIGRATION)
    return 0;

  files = get_dir(MIGRATIONS_PATH);

  for (i = 0; i < sizeof(files); i++)
  {
    string * pieces;
    object migration;
    string error, migration_name, player_version;
    int result;

    // if (get_file_extension(files[i]) != "c")
    //   continue;

    pieces = explode(files[i], ".");

    // only .c files
    if ((sizeof(pieces) < 2) || (pieces[sizeof(pieces) - 1] != "c"))
      continue;

    migration_name = implode(pieces[0..sizeof(pieces)-2], ".");

    // ignore the base migration, is just for inherits
    if (MIGRATIONS_PATH + migration_name == BASE_MIGRATION)
      continue;

    player_version = sprintf("%-0.4d", last_migration);

    // only newer migrations
    if (strcmp(migration_name, player_version) >= 0)
      continue;

    error = catch(migration = load_object(MIGRATIONS_PATH + files[i]));

    if (error || !migration)
    {
      stderr(" *** migration error in migration file " + files[i] + "\n");
      return 0;
    }

    if (catch(result = migration->update_player(this_object())))
    {
      stderr(" *** migration error in user " + object_name(this_object()) + 
             ", file " + files[i] + "\n");
      return 0;
    }

    if (result <= 0)
    {
      log_file(LOG_MIGRATIONS, "["+ctime(time(), 4)+"] " + 
        this_object()->query_cap_name() + " failed to migrate to version " + 
        query_last_migration() + ". Aborted.\n");

      return 0;
    }

    stderr(" *** migration " + migration_name + " applied to user " + object_name(this_object()) + "\n");

    update_last_migration(query_last_migration() + 1);

    log_file(LOG_MIGRATIONS, "["+ctime(time(), 4)+"] " + 
      this_object()->query_cap_name() + " migrated to version " + 
      query_last_migration() + ".\n");

    // DO NOT SAVE!! the process of loading the player has not finished yet
    // this_object()->save_me();
    call_out("save_me", 10);
  }

  return 1;
}

mixed stats()
{
  return ({
    ({ "Sheet Version", last_migration, }),
      });
}
