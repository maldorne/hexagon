/*
 * functions related to locations that make no sense 
 * including in the location object.
 * 
 * neverbot 06/24
 */

#include <areas/area.h>
#include <maps/maps.h>
#include <room/location.h>
#include <translations/exits.h>

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

private string short_file_name(string path)
{
  string * parts;

  parts = explode(path, "/");

  return parts[sizeof(parts) - 1];
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
    ret += "  Checking " + short_file_name(locations[i]->query_file_name()) + "\n";

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
    ret += "  " + short_file_name(aux[i]) + ":\n";
    for (j = 0; j < sizeof(connected[aux[i]]); j+=2)
      ret += "    " + connected[aux[i]][j] + " -> " + short_file_name(connected[aux[i]][j+1]) + "\n";
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

    ret += "  Checking " +short_file_name(current->query_file_name()) + "\n";
    
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

      ret += "   - exit " + exits[i] + " to " + short_file_name(exits[i+1]);
      
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

  if (sizeof(room->query_room_zones()))
    location->set_zones(room->query_room_zones());

  exit_map = room->query_exit_map();

  // write("Exits:\n" + to_string(exit_map) + "\n");

  // add exits to the location object, will be saved on next save_me()
  location->add_exits_from_exit_map(exit_map);

  ret = "";

  // add components to the location object
  if (room->query_shop())
  {
    location->add_component(LOCATION_COMPONENT_SHOP, ([ 
      "permanent_goods" : room->query_permanent_goods(),
        ]));
    ret += "   Adding component shop.\n";
  }

  if (room->query_pub())
  {
    location->add_component(LOCATION_COMPONENT_PUB, ([ 
      "menu_items" : room->query_menu_items(),
        ]));
    ret += "   Adding component pub.\n";
  }



  // TO DO
  // add shop as point of interest in area
  // add attender as vacancy in area

  write(ret);

  location->save_me();

  return location;
}

