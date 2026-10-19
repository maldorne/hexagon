// Reads a location or area save as data: restore_object puts the saved
// variables here, so no location is ever loaded or materialized (loading one
// clones its NPCs). Only the fields the map needs are declared; the rest of
// the save is ignored by the restore.

// location save
string file_name;
int * coordinates;
mapping _exit_map;
mapping component_info;
string _specific_short;
string _original_short;
string * room_zones;

// area save
string exploration_name;
string parent_area;

/**
 * The map fields of a save, nil when the file cannot be read.
 */
mapping read(string path)
{
  int result;

  result = -1;

  if (catch(result = restore_object(path)) || result < 0)
    return nil;

  return ([
    "file_name": file_name,
    "coordinates": coordinates,
    "exits": _exit_map,
    "components": component_info,
    "specific_short": _specific_short,
    "original_short": _original_short,
    "zones": room_zones,
    "exploration_name": exploration_name,
    "parent_area": parent_area,
  ]);
}
