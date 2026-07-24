
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

  if (file_size(path) != -2)
    mkdir(path);

  if (loaded_areas[path])
    return loaded_areas[path];

  area = clone_object(AREA_STORAGE_OBJECT);
  area->set_area_path(path);

  // will try to load the .o if it exists
  if (!area->restore_from_file_name(path + "area.o"))
  {
    // this will also save the area file
    area->set_file_name(path + "area.o");
  }

  loaded_areas[path] = area;

  return area;
}

// Delete an area whose locations are all gone: remove its area.o file,
// forget it, and drop the now-empty directory, so a wiped area leaves no
// dead area.o behind. Returns 1 if it was removed, 0 otherwise.
int remove_area_if_empty(object area)
{
  string area_file, path;

  if (!area || map_sizeof(area->query_locations()))
    return 0;

  area_file = area->query_file_name();
  path = area->query_area_path();

  if (area_file && file_size(area_file) >= 0)
    remove_file(area_file);

  if (path)
    map_delete(loaded_areas, path);
  destruct(area);

  if (path && file_size(path) == -2 && !sizeof(get_dir(path + "*")))
    rmdir(path);

  return 1;
}

string add_location(object location)
{
  string file_name, dir_name;
  object area_storage;

  file_name = location->query_file_name();
  dir_name = "/" + implode(shift_right(explode(file_name, "/")), "/") + "/";

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
