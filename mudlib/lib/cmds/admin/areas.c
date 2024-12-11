
#include <mud/cmd.h>
#include <areas/area.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "areas" }));
  set_usage("areas [locations] [connections]");
  set_help("List areas loaded in the location system.\n" + 
           "If 'locations' is specified, it will also list the locations of every area.\n" + 
           "If 'connections' is specified, it will also list their connections to other areas.\n");
}

static int cmd(string str, object me, string verb) 
{
  mapping areas;
  string * keys;
  int i, j;
  string * pieces;
  int show_locations, show_connections;
  string ret;

  areas = load_object(AREA_HANDLER)->query_loaded_areas();

  if (!areas || !map_sizeof(areas))
  {
    write("No areas loaded.\n");
    return 1;
  }

  show_locations = FALSE;
  show_connections = FALSE;
  pieces = explode(str, " ");

  if (member_array("locations", pieces) != -1)
    show_locations = TRUE;
  if (member_array("connections", pieces) != -1)
    show_connections = TRUE;

  ret = "Areas loaded:\n";
  keys = keys(areas);

  for (i = 0; i < sizeof(keys); i++)
  {
    object * locations;
    mapping connections;
    string * connections_keys;

    ret += " - %^GREEN%^" + keys[i] + "%^RESET%^\n";

    if (show_locations)
    {
      ret += "\n  Locations:\n\n";
      locations = areas[keys[i]]->query_loaded_locations();

      for (j = 0; j < sizeof(locations); j++)
      {
        ret += "   - " + locations[j]->query_file_name() + "\n";
      }

      ret += "\n";
    }

    if (show_connections)
    {
      ret += "\n  Connections:\n\n";

      connections = areas[keys[i]]->query_connections();
      connections_keys = keys(connections);

      for (j = 0; j < sizeof(connections_keys); j++)
      {
        ret += "   - " + connections_keys[j] + "\n     %^CYAN%^" + 
              connections[connections_keys[j]][0] + "%^RESET%^ : " + 
              connections[connections_keys[j]][1] + "\n";
      }

      ret += "\n";
    }
  }

  write(ret);
  return 1;
}
