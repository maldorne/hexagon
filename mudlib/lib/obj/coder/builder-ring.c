/*
 * One ring to build them all
 *
 * neverbot 06/06/24
 */

inherit "/lib/armour.c";

#include <room/location.h>
#include <areas/area.h>
#include <maps/maps.h>
#include <translations/exits.h>
#include <translations/armour.h>

#define BUILDER_RING_CONVERT_SYNTAX "convert < filename | dirname | here >"
#define BUILDER_RING_HELP "This ring can be used by coders to help them building areas.\n\n" + \
                "Available commands:\n" + \
                "\t" + BUILDER_RING_CONVERT_SYNTAX 

void create()
{
  ::create();
  
  set_name("builder ring");
  set_short("Builder ring");
  add_alias(({ "builder", "ring" }));

  set_main_plural("Builder rings");
  add_plural(({ "builder rings", "rings" }));
  set_long("This ring can be used by coders to help them building areas.\n");

  set_base_armour(RING);

  reset_drop();

  // only for spanish language
  // set_gender(1);
  set_weight(1);
}

string query_help(varargs string str) { return BUILDER_RING_HELP; }

void init()
{
  add_action("do_convert", "convert");
  ::init();
}

object convert_room_to_location(object room);

int do_convert(string str)
{
  // objects (rooms) to be converted to locations
  object * obs; 
  string * files;
  int i;

  files = ({ });

  if (!str || !strlen(str))
  {
    notify_fail(capitalize(query_verb()) + " what?\n\n" + BUILDER_RING_CONVERT_SYNTAX + "\n");
    return 0;
  }

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

  // converting our environment
  if (str == "here")
  {
    obs = ({ environment(this_player()) });
  }
  // is a directory
  else if (file_size(str) == -2)
  {
    files = get_files(str + "/*");
  }
  // is a file
  else if (file_size(str) > 0)
  {
    files = ({ str });
  }
  // is a filename in the current directory
  else if (file_size(this_user()->query_role()->query_path() + "/" + str) > 0)
  {
    files = ({ this_user()->query_role()->query_path() + "/" + str });
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

  if (sizeof(files))
    call_out("do_convert_files", 0, files);
 
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
        write("   %^GREEN%^Coordinates%^RESET%^: " + location->query_coordinates()[0] + " " + location->query_coordinates()[1] + " " + location->query_coordinates()[2] + "\n");
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
