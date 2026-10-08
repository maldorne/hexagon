// moved-files.c -- where coders declare which files were renamed or moved.
//
// Every pair old path -> new path written here lets an IOU for an item saved
// under the old path be reclaimed as the new one. The table itself lives in
// /lib/handlers/moved-files.c.
//
// 10/2026 - Created for Hexagon, neverbot, after the lost and found office
//           of the original muds (Raskolnikov, 1996).

#include "path.h"

inherit "/lib/room.c";

#define MOVED_FILES "moved-files"

void setup()
{
  set_short("Moved Files");
  set_long("A cramped office stacked with crates, each one labelled with the " +
    "name a thing used to have and, underneath, the name it has now. Whoever " +
    "turns up with an IOU for something lost is sent to look here. " +
    "Type 'list', 'add <old path> <new path>' or 'remove <old path>'.\n");
  set_light(100);
  add_exit(DIR_EAST, ADMIN + "admin1", "standard");
}

void init()
{
  ::init();

  add_action("do_list",   "list");
  add_action("do_add",    "add");
  add_action("do_remove", "remove");
}

int do_list(string str)
{
  mapping moved;
  string * olds;
  string text, mark;
  int i;

  if (!this_player()->query_coder())
    return 0;

  moved = handler(MOVED_FILES)->query_moved_files();
  olds = sort_array(map_indices(moved));

  if (!sizeof(olds))
  {
    write("No moved files declared.\n");
    return 1;
  }

  text = "";

  // a new path whose file is gone would only hand out another IOU
  for (i = 0; i < sizeof(olds); i++)
  {
    mark = (file_size(moved[olds[i]] + ".c") > 0) ? "" : "   (missing)";
    text += "  " + olds[i] + "\n    -> " + moved[olds[i]] + mark + "\n";
  }

  write(text);
  return 1;
}

int do_add(string str)
{
  string old_path, new_path;
  object moved;

  if (!this_player()->query_coder())
    return 0;

  if (!str || sscanf(str, "%s %s", old_path, new_path) != 2)
  {
    notify_fail("Usage: add <old path> <new path>\n");
    return 0;
  }

  moved = handler(MOVED_FILES);
  old_path = moved->normalize_path(old_path);
  new_path = moved->normalize_path(new_path);

  if (file_size(new_path + ".c") <= 0)
  {
    notify_fail("There is no file " + new_path + ".c\n");
    return 0;
  }

  if (!moved->add_moved_file(old_path, new_path))
  {
    notify_fail("The old and the new path have to be different.\n");
    return 0;
  }

  write("Added: " + old_path + " -> " + new_path + "\n");
  return 1;
}

int do_remove(string str)
{
  object moved;

  if (!this_player()->query_coder())
    return 0;

  if (!str || !strlen(str))
  {
    notify_fail("Usage: remove <old path>\n");
    return 0;
  }

  moved = handler(MOVED_FILES);

  if (!moved->remove_moved_file(str))
  {
    notify_fail("'" + moved->normalize_path(str) + "' is not in the list.\n");
    return 0;
  }

  write("Removed: " + moved->normalize_path(str) + "\n");
  return 1;
}
