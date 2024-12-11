/*
 * functions related to locations that make no sense 
 * including in the location object.
 * 
 * neverbot 06/24
 */

#include <areas/area.h>
#include <maps/maps.h>
#include <room/location.h>

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

  // load the location from the area system
  area = query_area_from_location_file_name(file_name);
  location = area->load_location(file_name);

  // sector = query_sector_from_location_file_name(file_name);

  // add the already loaded location into the sector system too
  load_object(MAPS_HANDLER)->add_location(location);

  return location;
}
