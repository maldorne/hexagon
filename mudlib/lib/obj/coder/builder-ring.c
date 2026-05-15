/*
 * One ring to build them all
 *
 * neverbot 06/06/24
 */

inherit "/lib/armour.c";

#include <room/location.h>
#include <areas/area.h>
#include <maps/maps.h>
#include <translations/armour.h>

#define BUILDER_RING_BUILD_VERB ({ "build" })
#define BUILDER_RING_OPTIONS ({ "selection", "convert" })
#define BUILDER_RING_SELECTION_SYNTAX "build selection < add | remove | list >"
#define BUILDER_RING_CONVERT_SYNTAX "build convert [< selection | filename | dirname | here >]"
#define BUILDER_RING_HELP "This ring can be used by coders to help them building areas.\n\n" + \
                "Available commands:\n" + \
                "\t" + BUILDER_RING_SELECTION_SYNTAX + "\n" + \
                "\t" + BUILDER_RING_CONVERT_SYNTAX

static string * selection;
static mapping objects;

void create()
{
  selection = ({ });
  objects = ([ ]);

  ::create();
  
  set_name("builder ring");
  set_short("Builder ring");
  add_alias(({ "builder", "ring" }));

  set_main_plural("Builder rings");
  add_plural(({ "builder rings", "rings" }));
  set_long("This ring can be used by coders to help them building areas.\n");

  set_base_armour(RING);

  reset_drop();
  set_weight(1);
}

string query_help(varargs string str) { return BUILDER_RING_HELP; }

void init()
{
  add_action("do_build", BUILDER_RING_BUILD_VERB);
  ::init();
}

int do_selection(string str);
int do_convert(string str);

// Resolve a target string into the list of files it represents, for the
// three cases shared by `do_selection add` and `do_convert`:
//   * absolute directory  -> its contents
//   * absolute file       -> the file itself
//   * file relative to the caller's coder path
// Returns ({ }) if `target` matches none of these — the caller decides
// what to do with the remaining cases ("here", "*", glob patterns).
static string * _resolve_targets(string target)
{
  string coder_path;

  if (file_size(target) == -2)
    return get_files(target + "/*");

  if (file_size(target) > 0)
    return ({ target });

  coder_path = this_user()->query_role()->query_path();

  if (file_size(coder_path + "/" + target) > 0)
    return ({ coder_path + "/" + target });

  return ({ });
}

static int _filter_loadable(string file)
{
  object what;

  // if file ends in .c, it should be a room
  if (file[strlen(file) - 2..] == ".c")
  {
    catch(what = load_object(file));
    if (what && 
        what->query_room())
    {
      objects[file] = what;
      return 1;
    }

    return 0;
  }
  else if (file[strlen(file) - 2..] == ".o")
  {
    what = load_object(LOCATION_HANDLER)->load_location(file);
    if (what && what->query_location())
    {
      objects[file] = what;
      return 1;
    }

    return 0;
  }

  return 0;
}

static int _filter_dot_c(string file)
{
  return file[strlen(file) - 2..] == ".c";
}

string pretty_selection()
{
  string ret;
  int i;

  ret = "Current selection:\n";

  for (i = 0; i < sizeof(selection); i++)
  ret += "  - " + selection[i] + 
         " " + trim(to_string(objects[selection[i]])) + 
         " (" + (objects[selection[i]]->query_location() ? "location" : "room") + ") \n";

  return ret + "\n";
}

int do_build(string str)
{
  string * args;
  string verb;

  args = explode(str, " ");

  if (!query_in_use())
  {
    notify_fail("You must equip the ring before using it.\n");
    return 0;
  }

  if (!this_player()->query_coder())
  {
    notify_fail("You won't be able to handle the power of this ring.\n");
    return 0;
  }

  if (!str || !strlen(str) || sizeof(args) < 1)
  {
    notify_fail("Build what?\n");
    return 0;
  }

  verb = args[0];

  if (member_array(verb, BUILDER_RING_OPTIONS) == -1)
  {
    notify_fail("Unknown build command.\n\n" + BUILDER_RING_HELP + "\n");
    return 0;
  }

  if (sizeof(args) < 2)
  {
    notify_fail("Build " + verb + " what?\n");
    return 0;
  }

  // write(to_string(verb));
  // write(to_string(implode(args[1..], " ")));

  if (verb == "selection")
    return do_selection(implode(args[1..], " "));
  else if (verb == "convert")
    return do_convert(implode(args[1..], " "));
  else
  {
    notify_fail("Unknown build command.\n\n" + BUILDER_RING_HELP + "\n");
    return 0;
  }

  return 1;
}

int do_selection(string str)
{
  string * args;
  string * resolved;
  string verb;
  string target;

  args = explode(str, " ");
  verb = args[0];

  if (sizeof(args) < 1)
  {
    notify_fail("What?\n");
    return 0;
  }

  if (verb == "add")
  {
    if (sizeof(args) < 2)
    {
      notify_fail(capitalize(verb) + " what?\n");
      return 0;
    }

    target = args[1];

    if (member_array(target, selection) != -1)
    {
      notify_fail("Already in the selection.\n");
      return 0;
    }

    // converting our environment
    if (target == "here")
    {
      selection += ({ base_name(environment(this_player())) });
    }
    else if (target == "*")
    {
      selection += get_files(this_user()->query_role()->query_current_path() + "/*");
    }
    else if (sizeof(resolved = _resolve_targets(target)))
    {
      selection += resolved;
    }
    // is a pattern in the current directory
    else if (sizeof(get_files(this_user()->query_role()->query_path() + "/" + target)) > 0)
    {
      selection += get_files(this_user()->query_role()->query_path() + "/" + target);
    }

    selection = unique_array(selection);
    selection = filter_array(selection, "_filter_loadable");

    if (!sizeof(selection))
    {
      notify_fail("No loadable files found.\n");
      return 0;
    }

    write(pretty_selection());
  }
  else if (verb == "remove")
  {
    if (sizeof(args) < 2)
    {
      notify_fail(capitalize(verb) + " what?\n");
      return 0;
    }

    target = args[1];

    // TO DO: search in the array using wildcards (*, filenam*, etc)

    if (member_array(target, selection) == -1)
    {
      notify_fail("Not in the selection.\n");
      return 0;
    }

    selection -= ({ target });
    objects = m_delete(objects, target);

    write("Removed " + target + " from the selection.\n");
  }
  else if (verb == "clean")
  {
    selection = ({ });
    objects = ([ ]);
    write("Selection cleaned.\n");
  }
  else if (verb == "list")
  {
    if (!sizeof(selection))
    {
      write("No selection.\n");
      return 1;
    }

    write(pretty_selection());
  }
  else
  {
    notify_fail("Unknown selection command.\n\n" + BUILDER_RING_SELECTION_SYNTAX + "\n");
    return 0;
  }

  return 1;
}

int do_convert(string str)
{
  // objects (rooms) to be converted to locations
  object * obs; 
  string * files;
  int i;

  files = ({ });

  // without arguments, use the current selection
  if (!str || !strlen(str) || str == "selection")
  {
    files = selection;
  }
  else
  {
    // converting our environment
    if (str == "here")
    {
      obs = ({ environment(this_player()) });
    }
    else if (sizeof(files = _resolve_targets(str)))
    {
      // resolved by the shared helper
    }
    // is a file pattern (maybe using "*")
    else
    {
      files = get_cfiles(str);

      if (!sizeof(files))
      {
        notify_fail("No such file or directory.\n");
        return 0;
      }
    }
  }

  // for conversion, filter just .c files (rooms), so 
  // we do not try to reconvert locations to locations
  files = filter_array(files, "_filter_dot_c");

  if (sizeof(files))
    call_out("do_convert_files", 0, files);
  else
    write("No files to convert.\n");
 
  return 1;
} 

int do_convert_files(string * files)
{
  object location;
  object * locations;
  object * rooms; 
  int num_coordinates;
  int i;

  write("Converting files to locations\n\n");
  write(" * Loading " + sizeof(files) + " objects ...\n");

  // load the objects in the file list
  if (sizeof(files) > 0)
  {
    object tmp;
    rooms = ({ });

    for (i = 0; i < sizeof(files); i++)
    {
      // catch(tmp = load_object(files[i]));
      tmp = load_object(files[i]);

      if (tmp)
        rooms += ({ tmp });
      else
      {
        write("Error loading " + files[i] + ".\n");
        continue;
      }
    }
  }

  if (sizeof(rooms) == 0)
  {
    write("No valid objects loaded.\n");
    return 1;
  }

  write(" * " + sizeof(rooms) + " objects loaded.\n\n");

  num_coordinates = 0;
  locations = ({ });

  // convert the rooms to locations
  for (i = 0; i < sizeof(rooms); i++)
  {
    write(" * Converting " + file_name(rooms[i]) + ".c ...\n");

    if (!(location = load_object(LOCATION_HANDLER)->convert_room_to_location(rooms[i])))
    {
      write("   Error converting " + file_name(rooms[i]) + ".c.\n");
    }
    else
    {
      locations += ({ location });

      write("   " + file_name(rooms[i]) + ".c to\n     " + location->query_file_name() + "\n");
      if (location->query_coordinates())
      {
        num_coordinates++;
        write("   %^GREEN%^Coordinates%^RESET%^: " + location->query_coordinates()[0] + 
              " " + location->query_coordinates()[1] + " " + location->query_coordinates()[2] + "\n");
      }
      else
        write("   %^RED%^Coordinates%^RESET%^: none\n");
    }
  }

  write("\nConversion finished.\n");
  write("  Rooms converted: " + sizeof(locations) + "\n");
  write("  Locations with coordinates: " + num_coordinates + "\n");

  if (sizeof(locations))
    call_out("do_guess_coordinates", 0, locations);

  return 1;
}


