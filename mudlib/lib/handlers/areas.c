
#include <areas/area.h>

mapping loaded_areas;

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

  stderr("🎃 create_area: " + path + "\n");

  if (loaded_areas[path])
    return loaded_areas[path];

  if (file_size(path) != -2) 
    mkdir(path);

  area = clone_object(AREA_STORAGE_OBJECT);

  // will try to load the .o if it exists
  if (!area->restore_from_file_name(path + "area.o"))
  {
    area->set_file_name(path + "area.o");
  }

  loaded_areas[path] = area;

  return area;
}

string add_location(object location) 
{
  int x, y, z;
  int area_x, area_y, area_z;
  string path, file_name;
  object area_storage;

  if (location->query_coordinates() == nil)
    return "";

  x = location->query_coordinates()[0];
  y = location->query_coordinates()[1];
  z = location->query_coordinates()[2];

  // every area could store 10x10x10 locations
  area_x = x / 10;
  area_y = y / 10;
  area_z = z / 10;

  stderr("🎃 add_location: " + location->query_file_name() + "\n");

  path = "/save/games/" + game_from_path(location->query_file_name()) + 
         "/maps/" + location->query_map_name() + "/" + 
         area_x + "/" + area_y + "/" + area_z + "/";
  file_name = "" + x + "_" + y + "_" + z + ".o";
  
  // create the area storage if it doesn't exist
  area_storage = create_area(path);

  // add the location to the area storage
  area_storage->add_location(location->query_file_name(), x, y, z, ([ ]));

  // write a file in path + file_name with the location data
  remove_file(path + file_name);
  write_file(path + file_name, location->query_file_name());

  return path + file_name;
}
