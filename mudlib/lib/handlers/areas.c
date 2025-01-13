
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

  stderr("🎃 areas create_area: " + path + "\n");

  if (file_size(path) != -2) 
    mkdir(path);

  if (loaded_areas[path])
    return loaded_areas[path];

  area = clone_object(AREA_STORAGE_OBJECT);
  area->set_area_name(path);

  // will try to load the .o if it exists
  if (!area->restore_from_file_name(path + "area.o"))
  {
    // this will also save the area file
    area->set_file_name(path + "area.o");
  }

  loaded_areas[path] = area;

  return area;
}

string add_location(object location) 
{
  string file_name, dir_name;
  object area_storage;

  stderr("🎃 areas add_location: " + location->query_file_name() + "\n");

  file_name = location->query_file_name();
  dir_name = "/" + implode(shift_right(explode(file_name, "/")), "/") + "/";
  
  stderr("🎃 areas dir_name: " + dir_name + "\n");

  // create the area storage if it doesn't exist, will return an already
  // created area if it is stored in the loaded_areas mapping
  area_storage = create_area(dir_name);

  // add the location to the area storage
  area_storage->add_location(location->query_file_name(), ([ ]));
  area_storage->add_loaded_location(location);

  // write a file in path + file_name with the location data
  remove_file(file_name);
  write_file(file_name, location->query_file_name());

  return dir_name + file_name;
}
