
#include <mud/cmd.h>
#include <maps/maps.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "sectors" }));
  set_usage("sectors");
  set_help("List sectors loaded in the map system.");
}

static int cmd(string str, object me, string verb) 
{
  mapping sectors;
  string * keys;
  int i, j;

  sectors = load_object(MAPS_HANDLER)->query_loaded_sectors();

  if (!sectors || !map_sizeof(sectors))
  {
    write("No sectors loaded.\n");
    return 1;
  }

  write("Sectors loaded:\n");
  keys = keys(sectors);

  for (i = 0; i < sizeof(keys); i++)
  {
    object * locations;

    write(" - " + keys[i] + "\n");

    locations = sectors[keys[i]]->query_loaded_locations();

    for (j = 0; j < sizeof(locations); j++)
    {
      write("   - " + locations[j]->query_file_name() + "\n");
    }
  }

  return 1;
}
