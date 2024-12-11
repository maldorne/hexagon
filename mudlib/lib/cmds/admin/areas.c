
#include <mud/cmd.h>
#include <areas/area.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "areas" }));
  set_usage("areas");
  set_help("List areas loaded in the location system.");
}

static int cmd(string str, object me, string verb) 
{
  mapping areas;
  string * keys;
  int i, j;

  areas = load_object(AREA_HANDLER)->query_loaded_areas();

  if (!areas || !map_sizeof(areas))
  {
    write("No areas loaded.\n");
    return 1;
  }

  write("Areas loaded:\n");
  keys = keys(areas);

  for (i = 0; i < sizeof(keys); i++)
  {
    object * locations;

    write(" - " + keys[i] + "\n");

    locations = areas[keys[i]]->query_loaded_locations();

    for (j = 0; j < sizeof(locations); j++)
    {
      write("   - " + locations[j]->query_file_name() + "\n");
    }
  }

  return 1;
}
