/*
 * functions related to locations that make no sense 
 * including in the location object.
 * 
 * neverbot 06/24
 */

#include <areas/area.h>
#include <maps/maps.h>
#include <room/location.h>
#include <room/room.h>
#include <translations/exits.h>

// Resolve a target string into the list of files it represents for
// room-to-location conversion. Handles the three cases that are common
// to every conversion entry point:
//   * absolute directory  -> its contents
//   * absolute file       -> the file itself
//   * file relative to the caller's coder path
// Returns ({ }) if `target` matches none of these; the caller decides
// what to do with the remaining cases ("here", "*", glob patterns).
string * resolve_targets(string target)
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

// what would be the file name for a location generated from object ob?
string get_location_file_name_from_room(object ob)
{
  string base_dir;
  string * pieces;

  base_dir = game_save_dir(ob);

  pieces = explode(base_name(ob), "/");

  // don't know where this object came from, 
  // but we are not going to save it
  // if (sizeof(pieces) < 2)
  //   return "";

  return base_dir + "locations/" + implode(pieces[2..], "/") + ".o";  
}

object query_area_from_location_file_name(string file_name)
{
  string * pieces;
  string area_name;

  pieces = explode(file_name, "/");

  // /save/games/x/locations/...
  if (sizeof(pieces) < 3)
    return nil;

  area_name = "/" + implode(pieces[0..sizeof(pieces)-2], "/") + "/";

  return load_object(AREA_HANDLER)->create_area(area_name);
}

object query_sector_from_location_file_name(string file_name)
{
  object location;

  // we need to clone the locations instead of build the path, because 
  // we don't have the location coordinates until it is loaded

  location = clone_object(BASE_LOCATION_OBJ);
  location->restore_from_file_name(file_name);

  return load_object(MAPS_HANDLER)->query_sector_from_location(location);
}

object load_location(string file_name)
{
  object area, sector, location;

  if (!strlen(file_name))
    return nil;

  if (file_size(file_name) < 0)
    return nil;

  // if the filename does not end in .o, this is not a location
  if (file_name[strlen(file_name) - 2..strlen(file_name) - 1] != ".o")
    return nil;

  // load the location from the area system
  area = query_area_from_location_file_name(file_name);
  location = area->load_location(file_name);

  if (!location)
    return nil;

  // sector = query_sector_from_location_file_name(file_name);

  // add the already loaded location into the sector system too
  load_object(MAPS_HANDLER)->add_location(location);

  return location;
}

int do_guess_coordinates(object * locations)
{
  string * exits;
  object current;       // location currently checking
  object * pending;     // locations pending to guess coordinates
  object * done;        // locations already checked
  mapping connected;    // locations connected to a different area, ([ file_name : ({ "direction", destination }) ])
  string * file_names;  // array with the file names of every location in this area
  object area;
  string initial_area_name;
  int i, j;
  string * aux;
  string ret;

  pending = ({ });
  done = ({ });
  connected = ([ ]);
  file_names = ({ });
  ret = "";
  
  // find locations in the list with coordinates as starting points,
  // and update every exit to the locations instead of the rooms
  for (i = 0; i < sizeof(locations); i++)
  {
    // if some exit destination is to a .c file whose location
    // already exists, we change the destination to the .o file
    locations[i]->update_exits_to_locations();

    // write("Checking " + file_name(locations[i]) + " (" + locations[i]->query_file_name() + ")\n");

    file_names += ({ locations[i]->query_file_name() });

    // write("Updated exits for " + file_name(locations[i]) + " " +
    //  + to_string(locations[i]->query_dest_dir()) + "\n");

    if (locations[i]->query_coordinates())
      pending += ({ locations[i] });
  }

  // get the area (the same for every location, use the first one as an example)
  area = load_object(LOCATION_HANDLER)->query_area_from_location_file_name(locations[0]->query_file_name());
  initial_area_name = area->query_area_name();

  ret += "\nCheck connections to different areas ...\n";

  // after updating every exit, check locations connected to different areas
  for (i = 0; i < sizeof(locations); i++)
  {
    ret += "  Checking " + get_path_file_name(locations[i]->query_file_name()) + "\n";

    exits = locations[i]->query_dest_dir();

    for (j = 0; j < sizeof(exits); j+=2)
    {
      object destination;
      destination = load_object(LOCATION_HANDLER)->load_location(exits[j+1]);

      // maybe this is not a location, but a room
      if (!destination)
        continue;

      // if any of our locations has an exit connecting with somewhere
      // outside our area
      if (member_array(exits[j+1], file_names) == -1 && 
          (locations[i]->query_area_name() != destination->query_area_name()))
      {
        if (connected[locations[i]->query_file_name()])
          connected[locations[i]->query_file_name()] += ({ exits[j], exits[j+1] });
        else
          connected[locations[i]->query_file_name()] = ({ exits[j], exits[j+1] });

        area->add_connection(locations[i]->query_file_name(), exits[j], exits[j+1]);

        // if we can guess the coordinates from outside the area, 
        // add this location as one of the initial ones
        if (locations[i]->guess_coordinates() && 
            member_array(locations[i], pending) == -1)
          pending += ({ locations[i] });
      }
    }
  }

  // log the connections
  aux = keys(connected);

  for (i = 0; i < sizeof(aux); i++)
  {
    ret += "  " + get_path_file_name(aux[i]) + ":\n";
    for (j = 0; j < sizeof(connected[aux[i]]); j+=2)
      ret += "    " + connected[aux[i]][j] + " -> " + get_path_file_name(connected[aux[i]][j+1]) + "\n";
  }

  write(ret);
  ret = "";
  // write("Connections (" + map_sizeof(connected) + "): " + to_string(connected));

  if (!sizeof(pending))
  {
    write("\n  No locations with coordinates found.\n");
    return 1;
  }

  ret += "\nTry guessing missing coordinates ...\n";
  ret += "Initially we have " + sizeof(pending) + " locations to check coordinates:\n";

  for (i = 0; i < sizeof(pending); i++)
    ret += "   - " + pending[i]->query_file_name() + "\n";

  write(ret);

  while (sizeof(pending))
  {
    ret = "\n";
    // ret += "Locations (" + sizeof(locations) + "): " + to_string(locations);
    ret += "  Yet to check " + sizeof(pending) + ", "; // "): " + to_string(pending));
    ret += "already done " + sizeof(done) + + "\n"; // "): " + to_string(done));

    current = pending[0];

    ret += "  Checking " +get_path_file_name(current->query_file_name()) + "\n";
    
    // already checked location, remove from pending
    // and continue
    if (member_array(current, done) != -1)
    {
      pending -= ({ current });
      continue;
    }

    exits = current->query_dest_dir();
    // write("Exits: " + to_string(exits) + "\n");

    for (i = 0; i < sizeof(exits); i+=2)
    {
      object dest;

      // if it is not a valid exit for guessing coordinates
      if (member_array(exits[i], ({ DIR_NORTH, DIR_SOUTH, DIR_EAST, DIR_WEST, 
                                  DIR_NORTHWEST, DIR_NORTHEAST, DIR_SOUTHWEST, DIR_SOUTHEAST, 
                                  DIR_UP, DIR_DOWN })) == -1)
      {
        continue;
      }

      ret += "   - exit " + exits[i] + " to " + get_path_file_name(exits[i+1]);
      
      dest = current->query_dest_object(exits[i]);

      if (dest)
      {
        int found;

        if (!dest->query_location())
        {
          ret += " (not a location)\n";
          continue;
        }

        // write("Destination: " + to_string(dest) + " " + dest->query_file_name() + "\n");
        found = dest->guess_coordinates();

        if (found)
          ret += " (%^GREEN%^found%^RESET%^)";

        // enqueue the destination for guessing coordinates, only with locations from
        // our original list, in other case we will remap the full game
        if (member_array(dest, locations) == -1)
        {
          ret += " (outside area)\n";
          continue;
        }

        // not peding to process and not already processed
        if ((member_array(dest, pending) == -1) && (member_array(dest, done) == -1))
          pending += ({ dest });
        // write(" Exits: " + to_string(dest->query_dest_dir()) + "\n");
      }

      ret += "\n";
    }

    // location done
    pending -= ({ current });
    done += ({ current });
    write(ret);
  }

  return 1;
}

// Aggregate the legacy room's add_clone calls into a path → count
// mapping. The source storage (room.c::room_clones) encodes each
// add_clone(path, count, [flag]) as `count` placeholders followed by an
// optional flag int and the path string at the end. After reset() the
// placeholders are replaced by cloned objects; this walk handles both
// pre- and post-reset shapes by scanning the array backwards.
static mapping _extract_original_add_clones(object room)
{
  mixed * raw;
  mapping ret;
  int i, count;
  string current_path;

  ret = ([ ]);

  if (!function_exists("query_room_clones", room)) return ret;
  raw = room->query_room_clones();
  if (!raw || !sizeof(raw)) return ret;

  current_path = nil;
  count = 0;

  for (i = sizeof(raw) - 1; i >= 0; i--)
  {
    if (stringp(raw[i]))
    {
      // commit the previous batch (if any)
      if (current_path && count > 0)
        ret[current_path] = (ret[current_path] ? ret[current_path] : 0) + count;
      current_path = raw[i];
      count = 0;
    }
    else if (intp(raw[i]) && raw[i] != 0)
    {
      // flag int — ignore for the count purpose
    }
    else
    {
      // nil (pre-reset placeholder) or cloned object (post-reset)
      count++;
    }
  }
  if (current_path && count > 0)
    ret[current_path] = (ret[current_path] ? ret[current_path] : 0) + count;

  return ret;
}

// Read the legacy room's add_item declarations back as an ordered
// list of ({ id_or_id_array, desc }) pairs. The source room funnels
// every add_item call into a singleton holder cloned from
// /lib/room/items/item.c, accumulating parallel arrays for shorts
// and longs (see lib/room/items/item.c::setup_item).
static mixed * _extract_original_items(object room)
{
  mapping room_items;
  object holder;
  string * shorts;
  string * longs;
  mixed * ret;
  int i;

  ret = ({ });

  if (!function_exists("query_items", room)) return ret;
  room_items = room->query_items();
  if (!room_items) return ret;

  holder = room_items["The object"];
  if (!holder) return ret;

  shorts = holder->query_shrt();
  longs = holder->query_lng();
  if (!shorts || !longs) return ret;

  // item.c::create() seeds shrt and lng with a leading empty
  // placeholder; skip pairs where both fields are empty.
  for (i = 0; i < sizeof(shorts) && i < sizeof(longs); i++)
  {
    if ((!shorts[i] || !strlen(shorts[i])) &&
        (!longs[i]  || !strlen(longs[i])))
      continue;
    ret += ({ ({ shorts[i], longs[i] }) });
  }

  return ret;
}

object convert_room_to_location(object room)
{
  object location;
  string file_name, * exits, ret;
  mapping exit_map;
  int i;

  if (!room)
    return nil;

  if (!room->query_room())
    return nil;

  if (room->query_location())
    return nil;

  // what file name would a location based on this room?
  file_name = load_object(LOCATION_HANDLER)->get_location_file_name_from_room(room);

  // new location object
  location = clone_object(BASE_LOCATION_OBJ);

  // will try to load the .o if it exists
  if (!location->restore_from_file_name(file_name))
  {
    location->set_file_name(file_name);
  }

  location->set_original_room_file_name(base_name(room) + ".c");
  location->set_original_short(room->query_short());
  location->set_original_long(room->query_long());
  location->set_original_add_clones(_extract_original_add_clones(room));
  location->set_original_items(_extract_original_items(room));
  location->stamp_last_imported_at();

  if (sizeof(room->query_room_zones()))
    location->set_zones(room->query_room_zones());

  exit_map = room->query_exit_map();

  // write("Exits:\n" + to_string(exit_map) + "\n");

  location->set_exits_from_exit_map(exit_map);

  ret = "";

  // add components to the location object. Keys use the mixin-namespaced
  // names defined in /lib/ventures/*.c::query_auto_load_attributes(). The
  // location's save_me will refresh the mapping from the live components
  // on every subsequent save (pull-on-save model).
  if (room->query_shop())
  {
    location->add_component(LOCATION_COMPONENT_SHOP, ([
      "shop_permanent_goods" : room->query_permanent_goods(),
        ]));
    ret += "   Adding component shop.\n";
  }

  if (room->query_pub())
  {
    location->add_component(LOCATION_COMPONENT_PUB, ([
      "pub_menu_items" : room->query_menu_items(),
        ]));
    ret += "   Adding component pub.\n";
  }

  if (room->query_property(MAZE_PROP))
  {
    location->add_component(LOCATION_COMPONENT_MAZE, ([ ]));
    ret += "   Adding component maze.\n";
  }

  // TO DO
  // add shop as point of interest in area
  // add attender as vacancy in area

  write(ret);

  location->save_me();

  return location;
}

// Convert a batch of room files to locations, with progress output to
// the current player. Filters out files that fail to load and reports
// per-file outcomes. After the batch, schedules a coordinate-inference
// pass on every successfully converted location.
int batch_convert(string * files, varargs int reload)
{
  object location;
  object * locations;
  object * rooms;
  object tmp;
  int num_coordinates;
  int i;

  write("Converting files to locations\n\n");
  if (reload)
    write(" * Loading " + sizeof(files) + " objects (reload mode) ...\n");
  else
    write(" * Loading " + sizeof(files) + " objects ...\n");

  rooms = ({ });

  for (i = 0; i < sizeof(files); i++)
  {
    // reload: destruct existing blueprint so load_object recompiles
    // from disk. Caller (room2loc reload) has already accepted the
    // risk of nuking any player still standing in the room.
    if (reload)
    {
      tmp = find_object(files[i]);
      if (tmp)
        destruct_object(tmp);
    }

    tmp = load_object(files[i]);

    if (tmp)
      rooms += ({ tmp });
    else
      write("Error loading " + files[i] + ".\n");
  }

  if (sizeof(rooms) == 0)
  {
    write("No valid objects loaded.\n");
    return 1;
  }

  write(" * " + sizeof(rooms) + " objects loaded.\n\n");

  num_coordinates = 0;
  locations = ({ });

  for (i = 0; i < sizeof(rooms); i++)
  {
    write(" * Converting " + file_name(rooms[i]) + ".c ...\n");

    if (!(location = convert_room_to_location(rooms[i])))
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


/**
 * Resolve a clean scope into its save-tree directory and the matching
 * source-tree prefix. The save-tree directory mirrors the source tree
 * one-for-one (see `get_location_file_name_from_room`), so wherever a
 * location's `.c` used to live in `/games/<game>/areas/<rest>/`, its
 * `.o` is at `/save/games/<game>/locations/areas/<rest>/`.
 *
 * Accepts either form on input. Returns ({ save_dir, source_prefix })
 * or ({ "", "" }) if the input does not look like an area path.
 */
string * resolve_clean_scope(string scope)
{
  string game, rest;

  if (!scope || !strlen(scope))
    return ({ "", "" });

  if (scope[strlen(scope) - 1] == '/')
    scope = scope[0..strlen(scope) - 2];

  if (sscanf(scope, "/save/games/%s/locations/areas/%s", game, rest) == 2)
    return ({ scope,
              "/games/" + game + "/areas/" + rest });

  if (sscanf(scope, "/games/%s/areas/%s", game, rest) == 2)
    return ({ "/save/games/" + game + "/locations/areas/" + rest,
              scope });

  return ({ "", "" });
}

/**
 * List the location `.o` files in `<save_dir>` that are real orphans:
 * conversion-derived locations whose source `.c` no longer exists.
 *
 * A location is in scope when `_original_room_file_name` starts with
 * `source_prefix + "/"` (it was converted from a `.c` inside the scope
 * directory) AND that source no longer exists on disk. Programmatically
 * created locations (clones saved at runtime, no source `.c`) are left
 * alone — their `_original_room_file_name` is empty.
 *
 * The walk is scoped to the single directory mirroring the source — we
 * never look elsewhere.
 */
private string * _list_scope_orphans(string save_dir, string source_prefix)
{
  string * files;
  string * orphans;
  int i;

  files = get_files(save_dir + "/*.o");
  files -= ({ save_dir + "/area.o" });

  orphans = ({ });

  for (i = 0; i < sizeof(files); i++)
  {
    object loc;
    string orig;

    loc = load_location(files[i]);
    if (!loc)
      continue;

    orig = loc->query_original_room_file_name();

    // programmatic locations (no source .c): never orphans, never touch
    if (!orig || !strlen(orig))
      continue;

    // only consider locations whose source belongs to this scope
    if (!starts_with(orig, source_prefix + "/"))
      continue;

    // a real orphan: the source .c file no longer exists on disk
    if (file_size(orig) < 0)
      orphans += ({ files[i] });
  }

  return orphans;
}

private mapping _find_inbound_exits(mapping orphan_set);

/**
 * Preview the impact of cleaning `scope`. `scope` is a source-tree
 * path (e.g. `/games/rl-aeternum/areas/forest/rooms`) or the matching
 * save-tree path. Only the corresponding save directory is consulted
 * for orphan discovery; each `.o` there is checked against the source
 * prefix via `_original_room_file_name`.
 *
 * Inbound-exit discovery is bounded to the destinations the orphans
 * themselves point at (see `_find_inbound_exits`), so this never
 * walks the rest of the world even when there are many locations.
 *
 * Returns ({ string * orphans, mapping adjacent_trim }).
 *   orphans       — `.o` paths inside the scope that match.
 *   adjacent_trim — destinations-with-exits-back into the orphan set.
 */
mixed * clean_preview(string scope)
{
  string * resolved;
  string save_dir, source_prefix;
  string * orphans;
  mapping orphan_set;
  mapping adjacent_trim;
  int i;

  resolved = resolve_clean_scope(scope);
  save_dir = resolved[0];
  source_prefix = resolved[1];

  if (!strlen(save_dir))
    return ({ ({ }), ([ ]) });

  orphans = _list_scope_orphans(save_dir, source_prefix);

  adjacent_trim = ([ ]);

  if (sizeof(orphans))
  {
    orphan_set = ([ ]);
    for (i = 0; i < sizeof(orphans); i++)
      orphan_set[orphans[i]] = 1;

    adjacent_trim = _find_inbound_exits(orphan_set);
  }

  return ({ orphans, adjacent_trim });
}

/**
 * Discover the locations whose exits would point at a deleted orphan
 * once the cleanup runs. The orphans themselves know their outgoing
 * exits; the unique destinations of those exits are exactly the
 * locations we need to inspect, plus loading each destination once
 * gives us its own exit list to look for the inbound back-pointer.
 *
 * This is bounded by `sum(outgoing_exits_per_orphan)` destinations to
 * load — never the whole world. The two assumptions are: (1) the save
 * tree mirrors the source tree (so an orphan's destinations live where
 * its exit string says they live); (2) the world's exit graph is at
 * least mostly reciprocal (if A→B exists, B→A usually does too — the
 * standard MUD convention). Non-reciprocal exits FROM an external
 * location to an orphan that the orphan does not know about are not
 * trimmed; that is the explicit cost of not scanning the world.
 *
 * Returns a mapping ([ adjacent_path : ({ "dir1", "dir2", ... }) ])
 * of exits to drop on each affected location.
 */
private mapping _find_inbound_exits(mapping orphan_set)
{
  string * orphans;
  mapping affected;
  mapping result;
  string * affected_keys;
  int i;

  orphans = map_indices(orphan_set);
  affected = ([ ]);

  // step 1: collect unique non-orphan destinations of orphan exits
  for (i = 0; i < sizeof(orphans); i++)
  {
    object orphan;
    mixed * exits;
    int e;

    orphan = load_location(orphans[i]);
    if (!orphan)
      continue;

    exits = orphan->query_dest_dir();

    for (e = 0; e < sizeof(exits); e += 2)
    {
      if (orphan_set[exits[e + 1]])
        continue;
      affected[exits[e + 1]] = 1;
    }
  }

  // step 2: load each affected destination and look for exits back
  // into the orphan set
  result = ([ ]);
  affected_keys = map_indices(affected);

  for (i = 0; i < sizeof(affected_keys); i++)
  {
    object loc;
    mixed * exits;
    string * to_drop;
    int e;

    loc = load_location(affected_keys[i]);
    if (!loc)
      continue;

    exits = loc->query_dest_dir();
    to_drop = ({ });

    for (e = 0; e < sizeof(exits); e += 2)
    {
      if (orphan_set[exits[e + 1]])
        to_drop += ({ exits[e] });
    }

    if (sizeof(to_drop))
      result[affected_keys[i]] = to_drop;
  }

  return result;
}

/**
 * Execute the cleanup the preview described. Two phases:
 *  1. Trim any inbound exits in the locations adjacent to the orphans
 *     (discovered via the orphans' own outgoing exits — see
 *     `_find_inbound_exits`).
 *  2. Remove the orphans themselves: drop their map index entries,
 *     their area index entries, and the `.o` files.
 *
 * Returns ({ orphan_count, adjacent_count, exit_count }) — counts of
 * the three things that happened.
 */
mixed * clean_apply(string scope)
{
  mixed * preview;
  string * orphans;
  mapping orphan_set;
  mapping adjacent_trim;
  string * adj_keys;
  int adjacent_count, exit_count;
  int i, j;

  preview = clean_preview(scope);
  orphans = preview[0];
  adjacent_trim = preview[1];

  if (!sizeof(orphans))
    return ({ 0, 0, 0 });

  orphan_set = ([ ]);
  for (i = 0; i < sizeof(orphans); i++)
    orphan_set[orphans[i]] = 1;

  // phase 1: trim inbound exits on adjacent locations
  adj_keys = map_indices(adjacent_trim);
  adjacent_count = 0;
  exit_count = 0;

  for (i = 0; i < sizeof(adj_keys); i++)
  {
    object loc;
    string * dirs;

    loc = load_location(adj_keys[i]);
    if (!loc)
      continue;

    dirs = adjacent_trim[adj_keys[i]];

    for (j = 0; j < sizeof(dirs); j++)
      loc->remove_exit(dirs[j]);

    loc->save_me();

    adjacent_count++;
    exit_count += sizeof(dirs);
  }

  // phase 2: remove the orphans
  for (i = 0; i < sizeof(orphans); i++)
  {
    object loc, area;

    loc = load_location(orphans[i]);

    if (loc)
    {
      if (loc->query_coordinates())
      {
        int * coords;
        coords = loc->query_coordinates();
        load_object(MAPS_HANDLER)->remove_location_from_map(
            orphans[i], loc->query_map_name(),
            coords[0], coords[1], coords[2]);
      }

      area = query_area_from_location_file_name(orphans[i]);
      if (area)
        area->remove_location(orphans[i]);

      destruct(loc);
    }

    if (file_size(orphans[i]) >= 0)
      remove_file(orphans[i]);
  }

  return ({ sizeof(orphans), adjacent_count, exit_count });
}

// ============================================================
//  Map reindexing
//
//  Loading a location re-registers it into its sector and area with the
//  current lift logic (terrain component tallies + path/road way exits;
//  see maps.c::add_location). Reindexing therefore just means loading
//  every persisted location once so stale sector.o / area.o files
//  rebuild against the code as it stands now.
//
//  Each load is cheap — a single restore plus the sector write — so an
//  area-sized batch fits within one execution's rlimits (the `exits
//  area` coder command already scans a whole area synchronously, and it
//  does strictly more work per file). A whole game spans many areas, so
//  reindex_game() spreads the areas across call_outs: each fires on the
//  next tick with a fresh rlimits budget, the standard DGD idiom for a
//  job too large for a single execution.
// ============================================================

// Reindex a single area synchronously. Returns the number of locations
// reloaded, or -1 if the area directory does not exist.
//
// With do_save set, each loaded location is also saved back. Loading
// alone rebuilds the in-memory sector/area indexes; saving additionally
// rewrites the location's own .o — used to migrate persisted data to the
// current on-disk format (e.g. canonical English exit direction keys).
int reindex_area(string game, string area, varargs int do_save)
{
  object area_ob;
  string area_path;
  string * files;
  int i, count;

  // Accept the area either bare ("naduk", as `exits area` takes it) or
  // as query_area_name() reports it ("naduk/rooms"): strip a trailing
  // "/rooms" or "/" so the path is rebuilt exactly once.
  while (strlen(area) && area[strlen(area) - 1] == '/')
    area = area[0 .. strlen(area) - 2];
  if (strlen(area) >= 6 && area[strlen(area) - 6 ..] == "/rooms")
    area = area[0 .. strlen(area) - 7];

  area_path = "/save/games/" + game + "/locations/areas/" + area + "/rooms/";

  if (file_size(area_path) != -2)
    return -1;

  // the area object is the authority on which files belong to it — the
  // same source `exits area` uses. load_location gates on it anyway.
  area_ob = load_object(AREA_HANDLER)->create_area(area_path);
  files = map_indices(area_ob->query_locations());

  count = 0;
  for (i = 0; i < sizeof(files); i++)
  {
    object loc;
    if (loc = load_location(files[i]))
    {
      if (do_save)
        loc->save_me();
      count++;
    }
  }

  return count;
}

// One step of a game-wide reindex: reindex areas[idx], then schedule the
// next on the following tick. Public because the call_out dispatcher
// reaches it through call_other. initiator is notified per area so a
// long run reports progress instead of going silent.
void reindex_game_step(string game, string * areas, int idx, object initiator,
                       int do_save)
{
  int count;

  if (idx >= sizeof(areas))
  {
    if (initiator)
      tell_object(initiator, "Map reindex of '" + game + "' complete: " +
                  sizeof(areas) + " areas.\n");
    return;
  }

  count = reindex_area(game, areas[idx], do_save);

  if (initiator)
    tell_object(initiator, "  reindexed area '" + areas[idx] + "': " +
                (count < 0 ? "not found" : count + " locations") +
                (do_save ? " (saved)" : "") + "\n");

  call_out("reindex_game_step", 0, game, areas, idx + 1, initiator, do_save);
}

// Kick off a game-wide reindex, one area per tick. Returns the number of
// areas scheduled, or -1 if the game has no areas directory. With do_save
// set every location is also re-saved (data migration).
int reindex_game(string game, object initiator, varargs int do_save)
{
  string areas_root;
  string * entries, * areas;
  int i;

  areas_root = "/save/games/" + game + "/locations/areas/";

  if (file_size(areas_root) != -2)
    return -1;

  entries = get_dir(areas_root + "*");
  entries = entries ? entries : ({ });

  // keep only sub-directories (each area is a dir)
  areas = ({ });
  for (i = 0; i < sizeof(entries); i++)
    if (file_size(areas_root + entries[i]) == -2)
      areas += ({ entries[i] });

  if (sizeof(areas))
    call_out("reindex_game_step", 0, game, areas, 0, initiator, do_save);

  return sizeof(areas);
}
