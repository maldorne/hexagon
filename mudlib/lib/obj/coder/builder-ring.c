/*
 * One ring to build them all
 *
 * neverbot 06/06/24
 */

inherit "/lib/armour.c";

#include <room/location.h>
#include <translations/armour.h>

#define BUILDER_RING_CONVERT_SYNTAX "convert < filename | dirname | here >"
#define BUILDER_RING_HELP "This ring can be used by coders to help them building areas.\n\n" + \
                 "Available commands:\n" + \
                 "\t" + BUILDER_RING_CONVERT_SYNTAX + "\n"

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

int convert_room_to_location(object room);

int do_convert(string str)
{
  // objects (rooms) to be converted to locations
  object * obs; 
  object tmp;
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

  // load the objects in the file list
  if (sizeof(files) > 0)
  {
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
    notify_fail("No valid objects loaded.\n");
    return 0;
  }

  // convert the rooms to locations
  for (i = 0; i < sizeof(obs); i++)
  {
    write("Converting " + file_name(obs[i]) + " ...\n");

    if (!convert_room_to_location(obs[i]))
    {
      write("Error converting " + file_name(obs[i]) + ".\n");
    }
  }

  write("Conversion finished.\n");
  return 1;
}

int convert_room_to_location(object room)
{
  object location;
  string file_name, * exits;
  mapping exit_map;;
  int i;

  if (!room)
    return 0;

  if (!room->query_room())
    return 0;

  if (room->query_location())
    return 0;

  // what file name would a location based on this room?
  file_name = load_object(LOCATION_HANDLER)->generate_location_file_name(room);

  // new location object
  location = clone_object(BASE_LOCATION_OBJ);

  // will try to load the .o if it exists
  location->set_file_name(file_name);

  location->set_original_short(room->short());
  location->set_original_long(room->long());

  exit_map = room->query_exit_map();
  exits = keys(exit_map);

  for (i = 0; i < sizeof(exits); i++)
  {
    location->add_exit(exits[i], exit_map[exits[i]]);
  }



  location->save_me();

  return 1;
}

