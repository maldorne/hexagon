
#include <maps/maps.h>

mapping loaded_sectors;

void create() {
  loaded_sectors = ([ ]);
  // ::create();
}

mapping query_loaded_sectors() {
  return loaded_sectors;
}

// will create an sector storage in the destination directory
// (and create such directory if it doesn't exist)
object create_sector(string path) 
{
  object sector;

  stderr("🎃 create_sector: " + path + "\n");

  if (file_size(path) != -2) 
    mkdir(path);

  if (loaded_sectors[path])
    return loaded_sectors[path];

  sector = clone_object(MAP_SECTOR_STORAGE_OBJECT);

  // will try to load the .o if it exists
  if (!sector->restore_from_file_name(path + "sector.o"))
  {
    sector->set_file_name(path + "sector.o");
  }

  loaded_sectors[path] = sector;

  return sector;
}

string add_location(object location) 
{
  int x, y, z;
  int sector_x, sector_y, sector_z;
  string path, file_name;
  object sector_storage;

  if (location->query_coordinates() == nil)
    return "";

  x = location->query_coordinates()[0];
  y = location->query_coordinates()[1];
  z = location->query_coordinates()[2];

  // every sector could store 10x10x10 locations
  sector_x = x / 10;
  sector_y = y / 10;
  sector_z = z / 10;

  stderr("🎃 add_location: " + location->query_file_name() + "\n");

  path = "/save/games/" + game_from_path(location->query_file_name()) + 
         "/maps/" + location->query_map_name() + "/" + 
         sector_x + "/" + sector_y + "/" + sector_z + "/";
  file_name = "" + x + "_" + y + "_" + z + ".o";
  
  // create the sector storage if it doesn't exist
  sector_storage = create_sector(path);

  // add the location to the sector storage
  sector_storage->add_location(location->query_file_name(), x, y, z, ([ ]));

  // write a file in path + file_name with the location data
  remove_file(path + file_name);
  write_file(path + file_name, location->query_file_name());

  return path + file_name;
}
