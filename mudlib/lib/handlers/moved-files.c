/*
 * Moved files handler.
 *
 * When a file is renamed or moved, whatever was saved under its old path can no
 * longer be cloned, and the player gets an IOU for it. This handler keeps the
 * pairs old path -> new path that coders declare, so an IOU can find the item
 * under its new name when it is reclaimed. Nothing else reads it: saved
 * auto_loads are never rewritten on the fly.
 *
 * Paths are full mudlib paths without the .c, the way an auto_load names them
 * (/games/<game>/baseobs/weapons/<item>). The pairs are managed from the moved
 * files room of the admin area.
 */

inherit "/lib/core/object.c";

#define MOVED_FILES_SAVE "/save/moved-files"

// how many renames are followed from one path before giving up
#define MAX_HOPS 10

// moved_files[old_path] == new_path
mapping moved_files;

void create()
{
  moved_files = ([ ]);
  ::create();
  restore_object(MOVED_FILES_SAVE, 1);

  if (!mappingp(moved_files))
    moved_files = ([ ]);
}

void setup()
{
  string name;
  int cnum;

  // only the master copy keeps the table
  if (sscanf(file_name(this_object()), "%s#%d", name, cnum) == 2)
    dest_me();
}

void save_handler()
{
  save_object(MOVED_FILES_SAVE, 1);
}

// A path as the table keeps it: rooted, and without the .c.
string normalize_path(string path)
{
  if (!path)
    return nil;

  path = trim(path);

  if (!strlen(path))
    return nil;

  if (path[0] != '/')
    path = "/" + path;

  if (strlen(path) > 2 && path[strlen(path) - 2..] == ".c")
    path = path[0..strlen(path) - 3];

  return path;
}

mapping query_moved_files() { return ([ ]) + moved_files; }

// Where a path lives now, following every rename declared after it, or nil
// when it was never moved. A cycle stops after MAX_HOPS.
string query_new_path(string path)
{
  string current;
  int hops;

  current = normalize_path(path);

  if (!current || undefinedp(moved_files[current]))
    return nil;

  while (!undefinedp(moved_files[current]) && hops < MAX_HOPS)
  {
    current = moved_files[current];
    hops++;
  }

  return current;
}

// Declare that old_path is now new_path. Returns 0 when either is empty or
// both are the same.
int add_moved_file(string old_path, string new_path)
{
  old_path = normalize_path(old_path);
  new_path = normalize_path(new_path);

  if (!old_path || !new_path || old_path == new_path)
    return 0;

  moved_files[old_path] = new_path;
  save_handler();
  return 1;
}

// Forget a pair. Returns 0 when old_path was not in the table.
int remove_moved_file(string old_path)
{
  old_path = normalize_path(old_path);

  if (!old_path || undefinedp(moved_files[old_path]))
    return 0;

  moved_files[old_path] = nil;
  save_handler();
  return 1;
}

mixed * stats()
{
  return ::stats() + ({
    ({ "Moved files", sizeof(map_indices(moved_files)), }),
  });
}
