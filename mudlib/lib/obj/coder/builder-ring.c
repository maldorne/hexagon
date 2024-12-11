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

string get_help(varargs string str) { return BUILDER_RING_HELP; }

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
  object * obs; 
  int num_coordinates;
  int i;

  write("Converting files to locations\n\n");
  write(" * Loading " + sizeof(files) + " objects ...\n");

  // load the objects in the file list
  if (sizeof(files) > 0)
  {
    object tmp;
    obs = ({ });

    for (i = 0; i < sizeof(files); i++)
    {
      // catch(tmp = load_object(files[i]));
      tmp = load_object(files[i]);

      if (tmp)
        obs += ({ tmp });
      else
      {
        write("Error loading " + files[i] + ".\n");
        continue;
      }
    }
  }

  if (sizeof(obs) == 0)
  {
    write("No valid objects loaded.\n");
    return 1;
  }

  write(" * " + sizeof(obs) + " objects loaded.\n\n");

  num_coordinates = 0;
  locations = ({ });

  // convert the rooms to locations
  for (i = 0; i < sizeof(obs); i++)
  {
    write(" * Converting " + file_name(obs[i]) + ".c ...\n");

    if (!(location = convert_room_to_location(obs[i])))
    {
      write("   Error converting " + file_name(obs[i]) + ".c.\n");
    }
    else
    {
      locations += ({ location });

      write("   " + file_name(obs[i]) + ".c to\n     " + location->query_file_name() + "\n");
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

int do_guess_coordinates(object * locations)
{
  string * exits;
  object * pending;
  object * done;
  object current;
  int i;

  pending = ({ });
  done = ({ });
  
  write("\nTry guessing missing coordinates ...\n");

  // find locations in the list with coordinates as starting points,
  // and update every exit to the locations instead of the rooms
  for (i = 0; i < sizeof(locations); i++)
  {
    // if some exit destination is to a .c file whose location
    // already exists, we change the destination to the .o file
    locations[i]->update_exits_to_locations();

    // write("Updated exits for " + file_name(locations[i]) + " " +
    //  + to_string(locations[i]->query_dest_dir()) + "\n");

    if (locations[i]->query_coordinates())
      pending += ({ locations[i] });
  }

  // TODO
  // if we do not have any location with coordinates, we should check connections with
  // other locations outside our list
  
  if (!sizeof(pending))
  {
    write("No locations with coordinates found.\n");
    return 1;
  }

  while (sizeof(pending))
  {
    // write("Locations (" + sizeof(locations) + "): " + to_string(locations));
    write("Pending (" + sizeof(pending) + ")\n"); // "): " + to_string(pending));
    write("Done (" + sizeof(done) + + ")\n"); // "): " + to_string(done));

    current = pending[0];

    write("Checking " + file_name(current) + " (" +current->query_file_name() + ")\n");
    
    // already checked location, remove from pending
    // and continue
    if (member_array(current, done) != -1)
    {
      pending -= ({ current });
      continue;
    }

    exits = current->query_dest_dir();
    write("Exits: " + to_string(exits) + "\n");

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

      write("Checking exit " + exits[i] + " to " + exits[i+1] + "\n");
      
      dest = current->query_dest_object(exits[i]);

      if (dest && dest->query_location())
      {
        write("Destination: " + to_string(dest) + " " + dest->query_file_name() + "\n");
       
        // enqueue the destination for guessing coordinates, only with locations from
        // our original list, in other case we will remap the full game
        if (member_array(dest, locations) != -1)
          continue;
          
        pending += ({ dest });
        write("🎃 Exits: " + to_string(dest->query_dest_dir()) + "\n");

        dest->guess_coordinates();
      }
    }

    // location done
    pending -= ({ current });
    done += ({ current });
  }

  return 1;
}

object convert_room_to_location(object room)
{
  object location;
  string file_name, * exits;
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

  exit_map = room->query_exit_map();

  // debug: show the files to the user
  // write("Exits:\n" + to_string(exit_map) + "\n");

  // add exits to the location object, will be saved on next save_me()
  location->add_exits_from_exit_map(exit_map);

  // for (i = 0; i < sizeof(exits); i++)
  // {
  //   // prototype
  //   // mixed add_exit(string direc, mixed dest, string type,
  //   //           varargs string material)
  //   location->add_exit(exits[i], 
  //                      exit_map[exits[i]][0], 
  //                      exit_map[exits[i]][1],
  //                      exit_map[exits[i]][2]);
  // }

  location->save_me();

  return location;
}
