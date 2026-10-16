// Everything about the location the admin stands in: carving its neighbours,
// its title and prose, its sign, fixtures, exits, and the temple, guild and
// library roles it can play, and the job taught there.

#include <mud/cmd.h>
#include <room/location.h>
#include <room/room.h>

inherit LOCATION_CMD;

private int do_carve(object loc, string * args);
private int do_remove(object loc, string * args);
private int do_desc(object loc, string * args);
private int do_sign(object loc, string * args);
private int do_fixture(object loc, string * args);
private int do_exit(object loc, string * args);
private int do_temple(object loc, string * args);
private int do_guild(object loc, string * args);
private int do_job(object loc, string * args);
private int do_library(object loc, string * args);

void setup()
{
  set_aliases(({ "locations" }));
  set_usage(
    "locations < <dir> <name> [title] | remove <dir> >\n" +
    "locations desc [ long | short ] [ <text> | reset ]\n" +
    "locations sign < <text> | remove >\n" +
    "locations fixture [ list | add <file.c> [count] | remove <file.c> ]\n" +
    "locations exit <dir> <type> [material] [closed|locked|open]\n" +
    "locations temple < <deity path> | none >\n" +
    "locations guild < entrance | room > < <guild path> | none >\n" +
    "locations job < <job path> | none >\n" +
    "locations library [ add <text path> | remove <text path> ]");
  set_help(
    "Build the location you are standing in.\n" +
    "\n" +
    "  locations <dir> <name> [title]    carve a new location next to this one\n" +
    "  locations remove <dir>            delete the bare location next to this one\n" +
    "  locations desc [long] [<text>]    set its own prose (no text: read it back)\n" +
    "  locations desc short [<text>]     set its own title (no text: read it back)\n" +
    "  locations desc [short] reset      drop its own prose, or title\n" +
    "  locations sign <text>             post a sign (replaces the one posted)\n" +
    "  locations sign remove             take the sign down\n" +
    "  locations fixture [list]          the fixtures it carries\n" +
    "  locations fixture add <file.c> [count]   add a fixture\n" +
    "  locations fixture remove <file.c> remove one\n" +
    "  locations exit <dir> <type> [material] [closed|locked|open]\n" +
    "                                    re-type an exit, on both sides\n" +
    "  locations temple <deity path>     consecrate it to a deity\n" +
    "  locations temple none             unconsecrate it (it stays a temple)\n" +
    "  locations guild entrance|room <guild path>   make it the entrance or the\n" +
    "                                    main hall of a guild's house\n" +
    "  locations guild entrance|room none   stop it being one\n" +
    "  locations job <job path>          make it the place where a job is taught\n" +
    "  locations job none                stop it being one\n" +
    "  locations library                 list the books on its shelves\n" +
    "  locations library add <text path>    put a book on them (makes it a library)\n" +
    "  locations library remove <text path> take one off\n" +
    "\n" +
    "Carving takes a compass direction and a plain file name (letters, digits, " +
    "dash and underscore); the new location is joined to this one by open " +
    "exits both ways. Only a bare location can be removed: no components, not " +
    "converted from a room, nobody inside.\n" +
    "\n" +
    "A location with no prose of its own composes its body from its " +
    "components and, failing those, from the room it was converted from. " +
    "The title and the prose are the two things no component can work out.\n" +
    "\n" +
    "An exit's type is what the way through is made of (door, gate, ...); " +
    "both sides are re-typed so the pair reads the same. The door words ride " +
    "in the exit, so a door set closed comes back closed on every load; " +
    "locked implies closed. To read the exits of a location use exits.\n" +
    "\n" +
    "Paths may be relative to your current directory. Use room2loc to turn " +
    "rooms into locations and components to attach components.");
}

static int cmd(string str, object me, string verb)
{
  string * args;
  object loc;

  args = words(str);
  if (!sizeof(args))
  {
    notify_fail("Usage:\n" + query_usage() + "\n");
    return 0;
  }

  loc = location_of(me);
  if (!loc)
  {
    notify_fail("Stand in a location (not a plain room) to work on it.\n");
    return 0;
  }

  switch (args[0])
  {
    case "remove":
    case "delete":
      return do_remove(loc, args);
    case "desc":
      return do_desc(loc, args);
    case "sign":
      return do_sign(loc, args);
    case "fixture":
      return do_fixture(loc, args);
    case "exit":
      return do_exit(loc, args);
    case "temple":
      return do_temple(loc, args);
    case "guild":
      return do_guild(loc, args);
    case "job":
      return do_job(loc, args);
    case "library":
      return do_library(loc, args);
  }

  return do_carve(loc, args);
}

/**
 * locations <dir> <name> [title]: carves a location next to this one.
 */
private int do_carve(object loc, string * args)
{
  mixed made;

  if (sizeof(args) < 2)
  {
    notify_fail("Usage: locations < <dir> <name> [title] | remove <dir> >\n");
    return 0;
  }

  made = LOCATION_HANDLER->carve_location(loc, args[0], args[1],
                                          rest_of(args, 2), nil);
  if (stringp(made))
  {
    notify_fail(made + "\n");
    return 0;
  }

  write("Carved a location to the " +
        ROOM_HAND->localize_dir(ROOM_HAND->canonical_dir(args[0])) + ", " +
        ((object)made)->query_file_name() + ".\n");
  return 1;
}

/**
 * locations remove <dir>: deletes the bare location next to this one.
 */
private int do_remove(object loc, string * args)
{
  mixed gone;

  if (sizeof(args) < 2)
  {
    notify_fail("Usage: locations remove <dir>\n");
    return 0;
  }

  gone = LOCATION_HANDLER->uncarve_location(loc, args[1], nil, nil);

  // it answers ({ the file it deleted }), or the reason it would not
  if (stringp(gone) || !pointerp(gone))
  {
    notify_fail(stringp(gone) ? gone + "\n" : "Could not remove it.\n");
    return 0;
  }

  write("Removed the location to the " +
        ROOM_HAND->localize_dir(ROOM_HAND->canonical_dir(args[1])) +
        ", " + gone[0] + ".\n");
  return 1;
}

/**
 * locations desc: the title and the prose the location has of its own, the two
 * things no component can work out. `short` is the title read in one line,
 * anything else is the prose; both fall back to what the room it was converted
 * from said, if it came from one.
 */
private int do_desc(object loc, string * args)
{
  mixed current, original;
  string text;
  int title;

  args = args[1 ..];
  title = sizeof(args) && args[0] == "short";
  if (title || (sizeof(args) && args[0] == "long"))
    args = args[1 ..];
  text = rest_of(args, 0);

  if (!strlen(text))
  {
    current  = title ? loc->query_specific_short() : loc->query_specific_long();
    original = title ? loc->query_original_short() : loc->query_original_long();

    write("Its own " + (title ? "title" : "prose") + ":\n  " +
          (stringp(current) && strlen(current) ? "\"" + current + "\""
                                               : "(none)") + "\n");
    write("The room it came from:\n  " +
          (stringp(original) && strlen(original) ? "\"" + original + "\""
                                                 : "(none)") + "\n");
    return 1;
  }

  if (text == "reset")
  {
    if (title)
      loc->set_specific_short("");
    else
      loc->set_specific_long("");
    loc->save_me();
    write("Dropped its own " + (title ? "title" : "prose") + ".\n");
    return 1;
  }

  if (title)
    loc->set_specific_short(text);
  else
    loc->set_specific_long(text);

  loc->save_me();
  write(title ? "Titled.\n" : "Described.\n");
  return 1;
}

/**
 * locations sign <text> | remove: one sign per location.
 */
private int do_sign(object loc, string * args)
{
  string text;

  text = rest_of(args, 1);

  if (text == "remove")
  {
    if (!loc->has_component(LOCATION_COMPONENT_SIGN))
    {
      notify_fail("There is no sign here.\n");
      return 0;
    }
    loc->query_component_by_type(LOCATION_COMPONENT_SIGN)->take_down();
    loc->remove_component(LOCATION_COMPONENT_SIGN);
    loc->save_me();
    write("Sign taken down.\n");
    return 1;
  }

  if (!strlen(text))
  {
    notify_fail("Usage: locations sign < <text> | remove >\n");
    return 0;
  }

  // a new sign replaces whatever was posted
  if (loc->has_component(LOCATION_COMPONENT_SIGN))
  {
    loc->query_component_by_type(LOCATION_COMPONENT_SIGN)->take_down();
    loc->remove_component(LOCATION_COMPONENT_SIGN);
  }

  loc->add_component(LOCATION_COMPONENT_SIGN, ([ "sign_text": text ]));
  loc->save_me();
  write("Posted a sign reading: " + text + "\n");
  return 1;
}

/**
 * locations fixture [list] | add <file.c> [count] | remove <file.c>
 */
private int do_fixture(object loc, string * args)
{
  object bp;
  string * paths, path, out;
  mapping fixtures;
  int count, i;

  args = args[1 ..];

  if (!sizeof(args) || args[0] == "list")
  {
    fixtures = loc->query_fixtures();
    paths = map_indices(fixtures);
    if (!sizeof(paths))
    {
      write("This location has no fixtures.\n");
      return 1;
    }
    out = "Fixtures of this location:\n";
    for (i = 0; i < sizeof(paths); i++)
      out += "  " + paths[i] + " x" + fixtures[paths[i]] + "\n";
    write(out);
    return 1;
  }

  if (sizeof(args) < 2 || (args[0] != "add" && args[0] != "remove"))
  {
    notify_fail("Usage: locations fixture [list] | add <file.c> [count] | " +
                "remove <file.c>\n");
    return 0;
  }

  path = get_path(args[1]);

  if (args[0] == "remove")
  {
    if (!loc->remove_fixture(path))
    {
      notify_fail("'" + path + "' is not a fixture of this location.\n");
      return 0;
    }
    loc->save_me();
    write("Removed fixture " + path + ".\n");
    return 1;
  }

  bp = nil;
  catch(bp = load_object(path));
  if (!bp || !bp->query_fixture())
  {
    notify_fail("'" + path + "' does not load or is not a fixture " +
                "(it has to inherit /lib/obj/fixture.c).\n");
    return 0;
  }

  count = 1;
  if (sizeof(args) > 2)
    sscanf(args[2], "%d", count);

  loc->add_fixture(path, count);
  loc->save_me();
  write("Added fixture " + base_name(bp) + " x" + count + ".\n");
  return 1;
}

/**
 * locations exit <dir> <type> [material] [closed|locked|open]: what a way
 * through is made of, on both sides at once. A pair of locations carved by hand
 * starts with plain open passages; a building wants a door, a town gate a gate,
 * and both sides have to agree or one half of the pair opens onto a doorway the
 * other calls a door. The door options ride in the exit map, so a door set
 * closed comes back closed on every load. `locked` implies `closed`.
 */
private int do_exit(object loc, string * args)
{
  string canon, ldir, type, material, dest, file;
  mapping exits, nex, options;
  object neighbour;
  string * dirs;
  int i;

  args = args[1 ..];

  if (sizeof(args) < 2)
  {
    notify_fail("Usage: locations exit <dir> <type> [material] " +
                "[closed|locked|open]\n");
    return 0;
  }

  type = args[1];

  canon = ROOM_HAND->canonical_dir(args[0]);
  if (!canon)
  {
    notify_fail("'" + args[0] + "' is not a compass direction.\n");
    return 0;
  }
  ldir = ROOM_HAND->localize_dir(canon);

  exits = loc->query_exit_map();
  if (!exits || !exits[ldir])
  {
    notify_fail("There is no exit to the " + ldir + " here.\n");
    return 0;
  }

  // anything after the type is either the material or one of the door words
  options = ([ ]);
  material = nil;

  for (i = 2; i < sizeof(args); i++)
  {
    if (args[i] == "closed")
      options["closed"] = 1;
    else if (args[i] == "locked")
    {
      options["closed"] = 1;
      options["locked"] = 1;
    }
    else if (args[i] == "open")
    {
      options["closed"] = 0;
      map_delete(options, "locked");
    }
    else
      material = args[i];
  }

  if (!map_sizeof(options))
    options = nil;

  dest = exits[ldir][0];
  if (!stringp(dest))
  {
    notify_fail("That exit has no destination to re-type.\n");
    return 0;
  }

  loc->add_exit(ldir, dest, type, material, options);
  loc->save_me();

  // and the way back, so the pair reads the same from either side
  file = loc->query_file_name();
  neighbour = LOCATION_HANDLER->load_location(dest);

  if (neighbour)
  {
    nex = neighbour->query_exit_map();
    dirs = nex ? map_indices(nex) : ({ });

    for (i = 0; i < sizeof(dirs); i++)
      if (nex[dirs[i]][0] == file)
        neighbour->add_exit(dirs[i], file, type, material, options);

    neighbour->save_me();
  }

  write("The way " + ldir + " is now of type '" + type + "'" +
        (material ? " of " + material : "") +
        (options ? " (" + (options["locked"] ? "locked"
                           : (options["closed"] ? "closed" : "open")) + ")"
                 : "") +
        ", on both sides.\n");
  return 1;
}

/**
 * locations temple <deity path> | none: consecrates the location to a deity,
 * attaching the temple component if it is not there yet. `none` unconsecrates
 * it and leaves the component in place: the building is still a temple, it
 * just has no god.
 */
private int do_temple(object loc, string * args)
{
  object temple;
  string deity;

  deity = rest_of(args, 1);
  if (!strlen(deity))
  {
    notify_fail("Usage: locations temple < <deity path> | none >\n");
    return 0;
  }

  if (deity == "none")
    deity = "";
  else if (!load_object(deity))
  {
    notify_fail("No deity loads at '" + deity + "'.\n");
    return 0;
  }

  temple = (object)loc->query_component_by_type(LOCATION_COMPONENT_TEMPLE);
  if (!temple)
  {
    loc->add_component(LOCATION_COMPONENT_TEMPLE, ([ ]));
    temple = (object)loc->query_component_by_type(LOCATION_COMPONENT_TEMPLE);
  }

  if (!temple)
  {
    notify_fail("The temple component would not attach here.\n");
    return 0;
  }

  temple->set_deity(deity);
  loc->save_me();

  write(strlen(deity)
          ? "Consecrated to " + deity + ".\n"
          : "Unconsecrated; the temple keeps no god.\n");
  return 1;
}

/**
 * locations guild entrance|room <guild path> | none: makes the location the
 * entrance or the main hall of a guild's house, or stops it being one.
 */
private int do_guild(object loc, string * args)
{
  object comp;
  string kind, path, type;

  if (sizeof(args) < 3 || (args[1] != "entrance" && args[1] != "room"))
  {
    notify_fail("Usage: locations guild < entrance | room > " +
                "< <guild path> | none >\n");
    return 0;
  }

  kind = args[1];
  path = rest_of(args, 2);

  type = (kind == "entrance") ? LOCATION_COMPONENT_GUILD_ENTRANCE
                              : LOCATION_COMPONENT_GUILD_ROOM;
  comp = loc->query_component_by_type(type);

  if (path == "none")
  {
    if (!comp)
    {
      notify_fail("This location is not a guild " + kind + ".\n");
      return 0;
    }
    comp->take_down();
    loc->remove_component(type);
    loc->save_me();
    write("This location is no longer a guild " + kind + ".\n");
    return 1;
  }

  path = get_path(path);
  if (strlen(path) > 2 && path[strlen(path) - 2 ..] == ".c")
    path = path[0 .. strlen(path) - 3];
  if (!load_object(path) || !load_object(path)->query_is_guild())
  {
    notify_fail("No guild loads at " + path + ".\n");
    return 0;
  }

  if (!comp)
  {
    loc->add_component(type, ([ ]));
    comp = loc->query_component_by_type(type);
  }
  if (!comp)
  {
    notify_fail("The guild " + kind + " component would not attach here.\n");
    return 0;
  }

  if (kind == "entrance")
    comp->set_entrance_guild(path);
  else
    comp->set_room_guild(path);
  loc->save_me();

  write("This location is now the guild " + kind + " of " + path + ".\n");
  return 1;
}

/**
 * locations job <job path> | none: makes the location the place where a job is
 * taught (joined and left), or stops it being one.
 */
private int do_job(object loc, string * args)
{
  object comp;
  string path;

  if (sizeof(args) < 2)
  {
    notify_fail("Usage: locations job < <job path> | none >\n");
    return 0;
  }

  path = rest_of(args, 1);
  comp = loc->query_component_by_type(LOCATION_COMPONENT_JOB_ENTRANCE);

  if (path == "none")
  {
    if (!comp)
    {
      notify_fail("This location is not a job entrance.\n");
      return 0;
    }
    comp->take_down();
    loc->remove_component(LOCATION_COMPONENT_JOB_ENTRANCE);
    loc->save_me();
    write("This location is no longer a job entrance.\n");
    return 1;
  }

  path = get_path(path);
  if (strlen(path) < 3 || path[strlen(path) - 2 ..] != ".c")
    path += ".c";
  if (!load_object(path) || !load_object(path)->query_is_job())
  {
    notify_fail("No job loads at " + path + ".\n");
    return 0;
  }

  if (!comp)
  {
    loc->add_component(LOCATION_COMPONENT_JOB_ENTRANCE, ([ ]));
    comp = loc->query_component_by_type(LOCATION_COMPONENT_JOB_ENTRANCE);
  }
  if (!comp)
  {
    notify_fail("The job entrance component would not attach here.\n");
    return 0;
  }

  comp->set_entrance_job(path);
  loc->save_me();

  write("This location is now the job entrance of " + path + ".\n");
  return 1;
}

/**
 * locations library [add|remove <text path>]: lists the books on the shelves,
 * puts one on them (making the location a library) or takes one off.
 */
private int do_library(object loc, string * args)
{
  object library;
  string verb, path, * books;
  int i;

  library = loc->query_component_by_type(LOCATION_COMPONENT_LIBRARY);

  if (sizeof(args) == 1)
  {
    if (!library)
    {
      notify_fail("This location is not a library.\n");
      return 0;
    }
    books = library->query_library_books();
    write("Books on the shelves (" + sizeof(books) + "):\n");
    for (i = 0; i < sizeof(books); i++)
      write("  " + (i + 1) + ") " + books[i] + "  [" +
            handler("books")->query_title(books[i]) + "]\n");
    return 1;
  }

  verb = args[1];
  if (sizeof(args) < 3 || (verb != "add" && verb != "remove"))
  {
    notify_fail("Usage: locations library [add|remove <text path>]\n");
    return 0;
  }

  path = get_path(rest_of(args, 2));

  if (verb == "add")
  {
    if (!handler("books")->query_page_count(path))
    {
      notify_fail("No readable book text at " + path + ".\n");
      return 0;
    }
    if (!library)
    {
      loc->add_component(LOCATION_COMPONENT_LIBRARY, ([ ]));
      library = loc->query_component_by_type(LOCATION_COMPONENT_LIBRARY);
    }
    if (!library->add_library_book(path))
    {
      notify_fail("That book is already on the shelves.\n");
      return 0;
    }
    loc->save_me();
    write("Added " + path + " [" + handler("books")->query_title(path) +
          "] to the shelves.\n");
    return 1;
  }

  if (!library || !library->remove_library_book(path))
  {
    notify_fail("That book is not on these shelves.\n");
    return 0;
  }
  loc->save_me();
  write("Removed " + path + " from the shelves.\n");
  return 1;
}
