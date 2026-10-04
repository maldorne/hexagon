#include <mud/cmd.h>
#include <areas/area.h>
#include <areas/vacancy.h>
#include <room/location.h>
#include <basic/gender.h>

inherit CMD_BASE;

private object area_of(object me);
private string columns(string * * rows);
private string who_is(object area, string uuid);
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
    "  houses audit     what the books say that cannot be right\n" +
    "\n" +
    "A house is a location carrying a home component; a plot is one carrying " +
    "a plot component, carved next to an existing location and waiting to be " +
    "built on. Both are kept on the area's books, and an area that delegates " +
    "its population shares one pool of them with the rest of its community.\n" +
    "\n" +
    "Who lives in each house and which family owns it are on the area's " +
    "books, so none of this loads a house or a person.");
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

// Name a resident by their uuid: their own name, from them while they are in
// the world and from the books otherwise, and the kind of person they are when
// they never had a name. A warning when the census has never heard of them.
private string who_is(object area, string uuid)
{
  mapping census;
  object npc;
  mixed given;

  census = (mapping)area->query_npc_census();
  if (!census[uuid])
    return "(no census entry)";

  npc = find_living(uuid);
  if (npc)
  {
    given = npc->query_given_name();
    return (stringp(given) && strlen(given))
             ? capitalize(given) : (string)npc->query_cap_name();
  }

  if (census[uuid]["name"])
    return capitalize(census[uuid]["name"]);

  return census[uuid]["source"]
           ? get_path_file_name(census[uuid]["source"]) : uuid;
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
  npc = find_living(uuid);

  if (npc)
  {
    given = npc->query_given_name();
    name = (stringp(given) && strlen(given))
             ? capitalize(given) : (string)npc->query_cap_name();
    gender = "" + (int)npc->query_gender();
  }
  else
  {
    // out of the world the books answer: the census keeps a copy of who
    // somebody is, refreshed every time they appear
    name = census[uuid]["name"] ? capitalize(census[uuid]["name"]) : kind;
    gender = census[uuid]["gender"] ? "" + census[uuid]["gender"] : "";
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
    string * residents;

    residents = (string *)area->query_house_residents(files[i]);

    if (free_only && sizeof(residents))
      continue;

    shown++;

    if (!sizeof(residents))
    {
      rows += ({ ({ get_path_file_name(files[i]),
                    area->is_kept_house(files[i]) ? "(empty, kept)" : "(empty)",
                    "-", "-" }) });
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

// ===== houses audit: what the books say that cannot be right =====
private int do_audit(object area, object me)
{
  mapping census;
  mapping * jobs;
  string * files;
  string out;
  int i, j, faults;

  files = (string *)area->query_houses();
  census = (mapping)area->query_npc_census();
  out = "";

  for (i = 0; i < sizeof(files); i++)
  {
    string * residents;

    if (file_size(files[i]) < 0)
    {
      faults++;
      out += "  " + get_path_file_name(files[i]) +
             " is on the books but its location is gone\n";
    }

    residents = (string *)area->query_house_residents(files[i]);
    for (j = 0; j < sizeof(residents); j++)
      if (!census[residents[j]])
      {
        faults++;
        out += "  " + get_path_file_name(files[i]) +
               " lists somebody the census has never heard of\n";
      }
  }

  // a job with a house of its own houses whoever holds it
  jobs = (mapping *)area->query_vacancies();
  for (i = 0; i < sizeof(jobs); i++)
  {
    string * holders;

    if (!stringp(jobs[i][VACANCY_HOME]))
      continue;

    holders = (string *)area->query_vacancy_holders(jobs[i]);
    for (j = 0; j < sizeof(holders); j++)
      if (area->query_house_of(holders[j]) != jobs[i][VACANCY_HOME])
      {
        faults++;
        out += "  " + who_is(area, holders[j]) + " holds '" +
               jobs[i][VACANCY_JOB] + "' but does not live in its house\n";
      }
  }

  if (!faults)
  {
    write("The books of the houses of '" + area->query_area_name() +
          "' hold together.\n");
    return 1;
  }

  write("Housing faults in '" + area->query_area_name() + "' (" + faults +
        "):\n" + out);
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
