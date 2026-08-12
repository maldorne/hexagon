
#include <areas/area.h>

mapping loaded_areas;

private void _collect_census_uuids(string dir, mapping referenced);
private void _delete_npc_folder(string game, string uuid);

void create() {
  loaded_areas = ([ ]);
  // ::create();
}

mapping query_loaded_areas() {
  return loaded_areas;
}

// will create an area storage in the destination directory
// (and create such directory if it doesn't exist)
object create_area(string path)
{
  object area;

  // normalise to a single trailing slash so create_area(".../rooms") and
  // create_area(".../rooms/") key the same cached area (and never write a
  // stray "roomsarea.o" from a slashless path).
  if (strlen(path) && path[strlen(path) - 1] != '/')
    path += "/";

  if (file_size(path) != -2)
    mkdir(path);

  if (loaded_areas[path])
    return loaded_areas[path];

  area = clone_object(AREA_STORAGE_OBJECT);
  area->set_area_path(path);

  // will try to load the .o if it exists
  if (!area->restore_from_file_name(path + "area.o"))
  {
    // this will also save the area file
    area->set_file_name(path + "area.o");
  }

  loaded_areas[path] = area;

  return area;
}

// Look up an already-known area without creating anything. Returns the
// cached area if it is loaded, otherwise restores it from its area.o when
// that file exists on disk (and caches it), or nil when there is no such
// area. Unlike create_area this never mkdirs the path nor writes a fresh
// area.o -- use it for read-only lookups (death cleanup, queries) where a
// missing area must not be brought into existence.
object query_area(string path)
{
  object area;

  if (strlen(path) && path[strlen(path) - 1] != '/')
    path += "/";

  if (loaded_areas[path])
    return loaded_areas[path];

  if (file_size(path + "area.o") < 0)
    return nil;

  area = clone_object(AREA_STORAGE_OBJECT);
  area->set_area_path(path);
  area->restore_from_file_name(path + "area.o");
  loaded_areas[path] = area;

  return area;
}

// Delete an area whose locations are all gone: remove its area.o file,
// forget it, and drop the now-empty directory, so a wiped area leaves no
// dead area.o behind. Returns 1 if it was removed, 0 otherwise.
int remove_area_if_empty(object area)
{
  string area_file, path;

  if (!area || map_sizeof(area->query_locations()))
    return 0;

  area_file = area->query_file_name();
  path = area->query_area_path();

  if (area_file && file_size(area_file) >= 0)
    remove_file(area_file);

  if (path)
    map_delete(loaded_areas, path);
  destruct(area);

  if (path && file_size(path) == -2 && !sizeof(get_dir(path + "*")))
    rmdir(path);

  return 1;
}

// Delete an NPC's entire save folder (every file in it, then the folder). The
// NPC object is not loaded here, so this deletes the folder directly -- same
// shape as npc::delete_npc_save, kept scoped to the npcs save tree.
private void _delete_npc_folder(string game, string uuid)
{
  string udir;
  string * files;
  int i;

  udir = npc_save_dir(game, uuid);
  if (file_size(udir) != -2)
    return;

  files = (string *)get_dir(udir + "*");
  for (i = 0; i < sizeof(files); i++)
    catch(remove_file(udir + files[i]));
  catch(rmdir(udir));
}

// Delete the save folders of every NPC in this area's census. Called when the
// area is being removed (a clean) so its NPCs leave no orphan folder behind.
// Scoped to the area -- O(area's NPCs), unlike a game-wide verify. Returns the
// number of folders removed.
int prune_area_npc_saves(object area)
{
  string game;
  string * uuids;
  int i;

  if (!area)
    return 0;
  game = game_from_path(area->query_area_path());
  if (!game)
    return 0;

  uuids = map_indices(area->query_npc_census());
  for (i = 0; i < sizeof(uuids); i++)
    _delete_npc_folder(game, uuids[i]);

  return sizeof(uuids);
}

// Walk the area tree under `dir`, collecting every census NPC uuid into
// `referenced`. A directory holding an area.o is an area; recurse into
// subdirs so nested areas (e.g. a road area under a town) are covered too.
private void _collect_census_uuids(string dir, mapping referenced)
{
  mixed * entries;
  int i;

  if (file_size(dir + "area.o") >= 0)
  {
    object area;

    area = query_area(dir);
    if (area)
    {
      string * uuids;
      uuids = map_indices(area->query_npc_census());
      for (i = 0; i < sizeof(uuids); i++)
        referenced[uuids[i]] = 1;
    }
  }

  entries = get_dir(dir + "*", -1);
  for (i = 0; i < sizeof(entries); i++)
    if (entries[i][1] == -2)   // size -2 marks a directory
      _collect_census_uuids(dir + entries[i][0] + "/", referenced);
}

// Verify the NPC save folders of `game` against the census: every persisted
// NPC lives in /save/games/<game>/npcs/<letter>/<uuid>/, but only those whose
// uuid appears in some area's census are still real. The rest are orphans left
// when a census entry was dropped without the NPC dying (a reconversion, a
// census rebuild, a removed vacancy). Returns
//   ([ "orphans": ({ uuid, ... }),   folders with files but no census entry
//      "empty":   ({ uuid, ... }) ]) folders already emptied (save deleted)
// With `apply`, deletes each orphan's whole folder and every empty folder.
mapping verify_npc_saves(string game, int apply)
{
  mapping referenced;
  string npcbase;
  string * letters, * orphans, * empties;
  int i, j;

  referenced = ([ ]);
  _collect_census_uuids("/save/games/" + game + "/locations/areas/", referenced);

  npcbase = "/save/games/" + game + "/npcs/";
  orphans = ({ });
  empties = ({ });

  letters = get_dir(npcbase + "*");
  for (i = 0; i < sizeof(letters); i++)
  {
    string ldir;
    string * uuids;

    // the templates live under npcs/templates/, not a uuid shard -- skip them
    if (letters[i] == "templates")
      continue;
    ldir = npcbase + letters[i] + "/";
    if (file_size(ldir) != -2)
      continue;

    uuids = get_dir(ldir + "*");
    for (j = 0; j < sizeof(uuids); j++)
    {
      string udir;
      string * files;

      udir = ldir + uuids[j] + "/";
      if (file_size(udir) != -2 || referenced[uuids[j]])
        continue;

      files = get_dir(udir + "*");
      if (sizeof(files))
        orphans += ({ uuids[j] });
      else
        empties += ({ uuids[j] });

      if (apply)
        _delete_npc_folder(game, uuids[j]);
    }
  }

  return ([ "orphans": orphans, "empty": empties ]);
}

string add_location(object location)
{
  string file_name, dir_name;
  object area_storage;

  file_name = location->query_file_name();
  dir_name = "/" + implode(shift_right(explode(file_name, "/")), "/") + "/";

  // create the area storage if it doesn't exist, will return an already
  // created area if it is stored in the loaded_areas mapping
  area_storage = create_area(dir_name);

  // add the location to the area storage
  area_storage->add_location(location->query_file_name(), ([ ]));
  area_storage->add_loaded_location(location);

  // write a file in path + file_name with the location data
  remove_file(file_name);
  write_file(file_name, location->query_file_name());

  return dir_name + file_name;
}
