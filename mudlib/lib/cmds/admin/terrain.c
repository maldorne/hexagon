
#include <mud/cmd.h>
#include <room/location.h>
#include <maps/sector.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "terrain" }));
  set_usage("terrain\n" +
            "terrain <type> [k=v ...]\n" +
            "terrain remove <type>\n" +
            "terrain area <name> <type> [k=v ...]\n" +
            "terrain area <name> remove <type>");
  set_help(
    "Attach or detach a cartography component on locations so the world\n" +
    "map can classify their sector. Types: city, road, forest, coast,\n" +
    "underground.\n" +
    "\n" +
    "  terrain                        list the current location's terrain\n" +
    "  terrain <type> [k=v ...]       tag the current location\n" +
    "  terrain remove <type>          untag the current location\n" +
    "  terrain area <name> <type> ..  tag every location of the area\n" +
    "  terrain area <name> remove <t> untag every location of the area\n" +
    "\n" +
    "Optional k=v pairs seed the component's classifier, e.g.\n" +
    "  terrain city size=village\n" +
    "  terrain forest depth=jungle\n" +
    "\n" +
    "The area form loads and re-saves every location in the area, so\n" +
    "its sector.o reclassifies immediately. Game is inferred from where\n" +
    "you stand.\n");
}

private int _is_terrain_type(string type)
{
  return member_array(type, SECTOR_CONTRIB_COMPONENTS) != -1;
}

// parse "k=v" tokens into a properties mapping
private mapping _parse_props(string * tokens)
{
  mapping props;
  int i;

  props = ([ ]);
  for (i = 0; i < sizeof(tokens); i++)
  {
    string k, v;
    if (sscanf(tokens[i], "%s=%s", k, v) == 2 && strlen(k))
      props[k] = v;
  }
  return props;
}

// list the terrain components attached to a location
private int do_list(object env)
{
  object * comps;
  string * types;
  string ret;
  int i;

  if (!env || !env->query_location())
  {
    notify_fail("You are not standing in a location.\n");
    return 0;
  }

  comps = env->query_components();
  types = SECTOR_CONTRIB_COMPONENTS;
  ret = "";

  for (i = 0; i < sizeof(comps); i++)
    if (member_array(comps[i]->query_type(), types) != -1)
    {
      mixed * st;
      int j;
      ret += "  " + comps[i]->query_type();
      st = comps[i]->stats();
      for (j = 0; j < sizeof(st); j++)
        if (st[j][0] != "Component Type")
          ret += "  " + st[j][0] + "=" + st[j][1];
      ret += "\n";
    }

  if (!strlen(ret))
    write("No terrain components on this location.\n");
  else
    write("Terrain components here:\n" + ret);
  return 1;
}

static int cmd(string str, object me, string verb)
{
  string * tokens;
  string game, area, type;
  int do_remove, count;
  mapping props;

  tokens = explode(str ? str : "", " ") - ({ "" });

  // ===== no args: list current location =====
  if (!sizeof(tokens))
    return do_list(environment(me));

  game = game_name(me);
  if (!strlen(game))
  {
    notify_fail("Cannot infer game from your current environment. " +
                "Stand inside a game first.\n");
    return 0;
  }

  // ===== area form =====
  if (tokens[0] == "area")
  {
    if (sizeof(tokens) < 3)
    {
      notify_fail("Usage: terrain area <name> [remove] <type> [k=v ...]\n");
      return 0;
    }
    area = tokens[1];

    if (tokens[2] == "remove")
    {
      if (sizeof(tokens) < 4)
      {
        notify_fail("Usage: terrain area <name> remove <type>\n");
        return 0;
      }
      type = tokens[3];
      if (!_is_terrain_type(type))
      {
        notify_fail("Unknown terrain type '" + type + "'.\n");
        return 0;
      }
      count = load_object(LOCATION_HANDLER)->tag_area(game, area, type, ([ ]), 1);
      if (count < 0)
      {
        notify_fail("No area '" + area + "' in game '" + game + "'.\n");
        return 0;
      }
      write("Removed '" + type + "' from " + count + " location" +
            (count == 1 ? "" : "s") + " in '" + area + "'.\n");
      return 1;
    }

    type = tokens[2];
    if (!_is_terrain_type(type))
    {
      notify_fail("Unknown terrain type '" + type + "'.\n");
      return 0;
    }
    props = _parse_props(tokens[3 ..]);
    count = load_object(LOCATION_HANDLER)->tag_area(game, area, type, props);
    if (count < 0)
    {
      notify_fail("No area '" + area + "' in game '" + game + "'.\n");
      return 0;
    }
    write("Tagged " + count + " location" + (count == 1 ? "" : "s") +
          " in '" + area + "' as '" + type + "'.\n");
    return 1;
  }

  // ===== current-location remove =====
  if (tokens[0] == "remove")
  {
    object env;
    if (sizeof(tokens) < 2)
    {
      notify_fail("Usage: terrain remove <type>\n");
      return 0;
    }
    type = tokens[1];
    env = environment(me);
    if (!env || !env->query_location())
    {
      notify_fail("You are not standing in a location.\n");
      return 0;
    }
    env->remove_component(type);
    env->save_me();
    write("Removed '" + type + "' from this location.\n");
    return 1;
  }

  // ===== current-location add =====
  {
    object env;
    type = tokens[0];
    if (!_is_terrain_type(type))
    {
      notify_fail("Unknown terrain type '" + type + "'. " +
                  "One of: city, road, forest, coast, underground.\n");
      return 0;
    }
    env = environment(me);
    if (!env || !env->query_location())
    {
      notify_fail("You are not standing in a location.\n");
      return 0;
    }
    props = _parse_props(tokens[1 ..]);
    env->add_component(type, props);
    env->save_me();
    write("Tagged this location as '" + type + "'.\n");
    return 1;
  }
}
