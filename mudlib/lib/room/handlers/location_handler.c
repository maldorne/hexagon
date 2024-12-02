/*
 * functions related to locations that make no sense 
 * including in the location object.
 * 
 * neverbot 06/24
 */

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
