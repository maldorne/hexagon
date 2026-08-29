#include <mud/cmd.h>
#include <areas/area.h>
#include <areas/vacancy.h>
#include <room/location.h>
#include <basic/gender.h>

inherit CMD_BASE;

private object area_of(object me);
private string columns(string * * rows);
private string who_is(object area, string uuid);
private string field_of(string savefile, string key);
private string * resident_row(object area, string uuid);

void setup()
{
  set_aliases(({ "houses" }));
  set_usage("houses [ plots | free | audit ]");
  set_help(
    "Report on the housing of the area you are standing in.\n" +
    "\n" +
    "  houses           every house it holds, and who lives in each\n" +
    "  houses plots     the building land still waiting for a house\n" +
    "  houses free      the houses nobody lives in\n" +
    "  houses audit     where the books and the houses disagree\n" +
    "\n" +
    "A house is a location carrying a home component; a plot is one carrying " +
    "a plot component, carved next to an existing location and waiting to be " +
    "built on. Both are kept on the area's books, and an area that delegates " +
    "its population shares one pool of them with the rest of its community.\n" +
    "\n" +
    "Who lives where is written twice: the resident list on the house, and " +
    "the address each person carries. 'audit' is what compares the two.");
}

// The area of the location the player is standing in, or nil.
private object area_of(object me)
{
  object env;

  env = environment(me);
  if (!env || !env->query_location())
    return nil;

  return env->query_area();
}

// Lay rows out in columns, the first row being the header.
private string columns(string * * rows)
{
  string out;
  int * width;
  int i, j, k;

  if (!sizeof(rows))
    return "";

  width = allocate_int(sizeof(rows[0]));
  for (i = 0; i < sizeof(rows); i++)
    for (j = 0; j < sizeof(rows[i]); j++)
    {
      int l;
      l = strlen(rows[i][j], TRUE);
      if (l > width[j])
        width[j] = l;
    }

  out = "";
  for (i = 0; i < sizeof(rows); i++)
  {
    out += " ";
    for (j = 0; j < sizeof(rows[i]); j++)
      out += sprintf(" %-*s", width[j], rows[i][j]);
    out += "\n";

    if (i == 0)
    {
      out += " ";
      for (j = 0; j < sizeof(rows[i]); j++)
      {
        out += " ";
        for (k = 0; k < width[j]; k++)
          out += "-";
      }
      out += "\n";
    }
  }

  return out;
}

// Name a resident by their uuid: their own name, read off their savefile when
// they are not in the world, and the kind of person they are when they never
// had a name of their own. A warning when the census has never heard of them.
private string who_is(object area, string uuid)
{
  mapping census;
  object npc;
  mixed given;

  census = (mapping)area->query_npc_census();
  if (!census[uuid])
    return "(no census entry)";

  npc = AREA_HANDLER->find_live_npc(uuid);
  if (npc)
  {
    given = npc->query_given_name();
    return (stringp(given) && strlen(given))
             ? capitalize(given) : (string)npc->query_cap_name();
  }

  given = field_of(census[uuid]["savefile"], "npc_given_name");
  if (strlen(given))
    return capitalize(given);

  return census[uuid]["source"]
           ? get_path_file_name(census[uuid]["source"]) : uuid;
}

// One value out of a saved NPC, read straight off its savefile. Used to
// describe the residents who are not in the world at the moment.
private string field_of(string savefile, string key)
{
  string * lines, want;
  string body;
  int i;

  if (!savefile || file_size(savefile) < 0)
    return "";

  body = read_file(savefile);
  if (!body)
    return "";

  want = key + " ";
  lines = explode(body, "\n");
  for (i = 0; i < sizeof(lines); i++)
    if (strlen(lines[i]) > strlen(want) &&
        lines[i][0 .. strlen(want) - 1] == want)
    {
      string value;
      value = lines[i][strlen(want) ..];
      if (strlen(value) > 1 && value[0] == '"')
        return value[1 .. strlen(value) - 2];
      return value;
    }

  return "";
}

// Name, kind and gender of one resident, whether or not they are in the world.
private string * resident_row(object area, string uuid)
{
  mapping census;
  object npc;
  mixed given;
  string name, kind, gender;

  census = (mapping)area->query_npc_census();
  if (!census[uuid])
    return ({ "(no census entry)", "-", "-" });

  kind = census[uuid]["source"]
           ? get_path_file_name(census[uuid]["source"]) : "-";
  npc = AREA_HANDLER->find_live_npc(uuid);

  if (npc)
  {
    given = npc->query_given_name();
    name = (stringp(given) && strlen(given))
             ? capitalize(given) : (string)npc->query_cap_name();
    gender = "" + (int)npc->query_gender();
  }
  else
  {
    name = field_of(census[uuid]["savefile"], "npc_given_name");
    name = strlen(name) ? capitalize(name) : "?";
    gender = field_of(census[uuid]["savefile"], "gender");
  }

  return ({ name, kind,
            gender == "" + GENDER_FEMALE ? "female"
              : (gender == "" + GENDER_MALE ? "male" : "-") });
}

// ===== houses: every house and who lives in it =====
private int do_houses(object area, object me, int free_only)
{
  string * files;
  string * * rows;
  int i, j, shown;

  files = (string *)area->query_houses();
  if (!sizeof(files))
  {
    write("Area '" + area->query_area_name() + "' has no houses.\n");
    return 1;
  }

  // one row per resident, so a house with two people takes two lines and the
  // house is named on the first of them
  rows = ({ ({ "house", "resident", "type", "gender" }) });

  for (i = 0; i < sizeof(files); i++)
  {
    object loc, home;
    string * residents;

    loc = (object)area->load_location(files[i]);
    home = loc ? loc->query_component_by_type(LOCATION_COMPONENT_HOME) : nil;
    residents = home ? (string *)home->query_residents() : ({ });

    if (free_only && sizeof(residents))
      continue;

    shown++;

    if (!sizeof(residents))
    {
      rows += ({ ({ get_path_file_name(files[i]),
                    home ? "(empty)" : "(no home component)", "-", "-" }) });
      continue;
    }

    for (j = 0; j < sizeof(residents); j++)
      rows += ({ ({ j ? "" : get_path_file_name(files[i]) }) +
                 resident_row(area, residents[j]) });
  }

  if (!shown)
  {
    write("Every house of '" + area->query_area_name() + "' is lived in.\n");
    return 1;
  }

  write((free_only ? "Empty houses of '" : "Houses of '") +
        area->query_area_name() + "' (" + shown + "):\n" + columns(rows));
  return 1;
}

// ===== houses plots: the land still waiting =====
private int do_plots(object area, object me)
{
  string * files;
  string * * rows;
  int i;

  files = (string *)area->query_plots();
  if (!sizeof(files))
  {
    write("Area '" + area->query_area_name() + "' has no free plots left.\n");
    return 1;
  }

  rows = ({ ({ "plot", "reached from" }) });

  for (i = 0; i < sizeof(files); i++)
  {
    object loc;
    mapping exits;
    string * dirs;
    string from;

    loc = (object)area->load_location(files[i]);
    exits = loc ? loc->query_exit_map() : nil;
    dirs = exits ? map_indices(exits) : ({ });

    // a plot is carved off one location and keeps the way back to it
    from = sizeof(dirs) ? get_path_file_name(exits[dirs[0]][0]) : "-";

    rows += ({ ({ get_path_file_name(files[i]), from }) });
  }

  write("Free plots of '" + area->query_area_name() + "' (" + sizeof(files) +
        "):\n" + columns(rows));
  return 1;
}

// ===== houses audit: where the two records disagree =====
private int do_audit(object area, object me)
{
  mapping census, claimed;
  string * files, * ids;
  string out;
  int i, j, faults;

  files = (string *)area->query_houses();
  census = (mapping)area->query_npc_census();
  claimed = ([ ]);
  out = "";

  // what the houses say
  for (i = 0; i < sizeof(files); i++)
  {
    object loc, home;
    string * residents;

    loc = (object)area->load_location(files[i]);
    home = loc ? loc->query_component_by_type(LOCATION_COMPONENT_HOME) : nil;
    if (!home)
    {
      faults++;
      out += "  " + get_path_file_name(files[i]) +
             " is on the books but carries no home component\n";
      continue;
    }

    residents = (string *)home->query_residents();
    for (j = 0; j < sizeof(residents); j++)
    {
      if (!census[residents[j]])
      {
        faults++;
        out += "  " + get_path_file_name(files[i]) +
               " lists somebody the census has never heard of\n";
        continue;
      }
      claimed[residents[j]] = files[i];
    }
  }

  // what the people say. Everybody is asked, not only those in the world: an
  // address that outlived its house is exactly what an unloaded person carries
  // around, since unbuilding a house can only reach the residents it finds
  // standing in it.
  ids = map_indices(census);
  for (i = 0; i < sizeof(ids); i++)
  {
    object npc;
    mixed home;

    npc = AREA_HANDLER->find_live_npc(ids[i]);
    home = npc ? npc->query_home()
               : field_of(census[ids[i]]["savefile"], "npc_home");
    if (!stringp(home) || !strlen(home))
    {
      if (claimed[ids[i]])
      {
        faults++;
        out += "  " + who_is(area, ids[i]) + " lives nowhere, but " +
               get_path_file_name(claimed[ids[i]]) + " keeps a bed for them\n";
      }
      continue;
    }

    if (!claimed[ids[i]])
    {
      faults++;
      out += "  " + who_is(area, ids[i]) + " says they live at " +
             get_path_file_name(home) + ", which does not list them\n";
    }
    else if (claimed[ids[i]] != home)
    {
      faults++;
      out += "  " + who_is(area, ids[i]) + " says they live at " +
             get_path_file_name(home) + ", but " +
             get_path_file_name(claimed[ids[i]]) + " lists them too\n";
    }
  }

  if (!faults)
  {
    write("The houses of '" + area->query_area_name() +
          "' and the people in them agree.\n");
    return 1;
  }

  write("Housing faults in '" + area->query_area_name() + "' (" + faults +
        "):\n" + out +
        "\nEverybody is checked from both sides: those in the world are asked " +
        "directly, the rest are read off their savefile.\n");
  return 1;
}

static int cmd(string str, object me, string verb)
{
  string * args;
  object area;

  args = (str && strlen(str)) ? explode(str, " ") - ({ "" }) : ({ });

  area = area_of(me);
  if (!area)
  {
    notify_fail("Stand in a location (not a plain room) to report on its " +
                "housing.\n");
    return 0;
  }

  if (!sizeof(args))
    return do_houses(area, me, 0);
  if (args[0] == "free")
    return do_houses(area, me, 1);
  if (args[0] == "plots")
    return do_plots(area, me);
  if (args[0] == "audit")
    return do_audit(area, me);

  notify_fail("Usage: houses [ plots | free | audit ]\n");
  return 0;
}
