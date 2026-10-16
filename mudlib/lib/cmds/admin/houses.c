#include <mud/cmd.h>
#include <areas/area.h>
#include <areas/vacancy.h>
#include <room/location.h>
#include <room/room.h>
#include <sector/sector.h>
#include <basic/gender.h>

inherit LOCATION_CMD;

private string who_is(object area, string uuid);
private string * resident_row(object area, string uuid);
private object house_area(object me, string failure);
private int do_assign(object me);
private int do_make(object me);
private int do_communal(object me, string * args);
private int do_shared(object me, string * args);
private int do_describe(object me, string * args);
private int do_books(object me, string * args);
private int do_remove(object me);
private int do_plot(object me, string * args);

void setup()
{
  set_aliases(({ "houses" }));
  set_usage("houses [ plots | free | audit | assign | make | communal [off] | " +
            "shared <n|off> | short <text> | long <text> | " +
            "books <area path|none> [house file] | remove | " +
            "plot <dir> | plot remove <dir> ]");
  set_help(
    "Report on the housing of the area you are standing in, and build it.\n" +
    "\n" +
    "  houses           every house it holds, and who lives in each\n" +
    "  houses plots     the building land still waiting for a house\n" +
    "  houses free      the houses nobody lives in\n" +
    "  houses audit     what the books say that cannot be right\n" +
    "\n" +
    "  houses assign                  house the community's homeless residents\n" +
    "  houses make                    make the plot (or location) you stand in a house\n" +
    "  houses communal [off]          mark the house you stand in as communal, or not\n" +
    "  houses shared <n|off>          room for n households in this house; off = one\n" +
    "  houses short <text|none>       give this house a title of its own\n" +
    "  houses long <text|none>        give this house a description of its own\n" +
    "  houses books <area|none> [file]  book a house with another area of the community\n" +
    "  houses remove                  turn the house you stand in back into a bare plot\n" +
    "  houses plot <dir>              carve an empty plot next to this location\n" +
    "  houses plot remove <dir>       delete the bare plot next to this location\n" +
    "\n" +
    "A house is a location carrying a home component; a plot is one carrying " +
    "a plot component, carved next to an existing location and waiting to be " +
    "built on. Both are kept on the area's books, and an area that delegates " +
    "its population shares one pool of them with the rest of its community.\n" +
    "\n" +
    "assign places each homeless resident: with a spouse, with somebody living " +
    "alone they marry, or in a new household in an empty house or on a free " +
    "plot; shortfalls go to the area's events log. make raises a house by " +
    "hand on a plot (a barracks, a guildhall, a house kept for somebody in " +
    "particular); it is kept out of assign, and a job takes it as its house " +
    "with 'vacancies home <job>'. communal marks a barracks or dormitory " +
    "shared by whoever holds its job, where nobody founds a family. shared " +
    "turns a house into a tenement for n families, each housed and named on " +
    "its own. short and long set the title and description of the house you " +
    "stand in, typed in the language of the running instance; 'none' makes it " +
    "read as an ordinary home again. books attaches a house to the community " +
    "of another area for when the two part (the path of the area, and " +
    "optionally the house file, by default the one you stand in); 'none' books " +
    "it where it stands again. remove evicts the residents (left homeless for " +
    "a later assign), turns the door into a doorway again and returns the " +
    "plot to the free list, so 'houses plot remove <dir>' can delete it. A " +
    "plot cannot be carved from inside a plot or a house, and only a bare " +
    "plot can be deleted.\n" +
    "\n" +
    "A house is a location carrying a home component; a plot is one carrying " +
    "a plot component, carved next to an existing location and waiting to be " +
    "built on. Both are kept on the area's books, and an area that delegates " +
    "its population shares one pool of them with the rest of its community.\n" +
    "\n" +
    "Who lives in each house and which family owns it are on the area's " +
    "books, so none of this loads a house or a person.");
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

/**
 * The area of the location `me` stands in; when there is none, the failure is
 * set for the command to report and nil is returned.
 */
private object house_area(object me, string failure)
{
  object area;

  area = area_of(me);
  if (!area)
    notify_fail(failure);
  return area;
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

    loc = (object)area->load_location(area->query_house_file(files[i]));
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

    // a household of a shared house is its file plus a number
    if (file_size((string)area->query_house_file(files[i])) < 0)
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

// ===== houses assign: house the homeless =====
// Each homeless resident of the community goes with a spouse, with somebody
// living alone they marry, or into a new household in an empty house or on a
// free plot. Shortfalls (no free plot) are recorded in the area's events.log.
private int do_assign(object me)
{
  object area;

  area = house_area(me, "Stand in a location to assign homes.\n");
  if (!area)
    return 0;

  area->assign_homes();
  write("Assigned homes to the community's homeless residents; see the area's " +
        "event log for any shortfall.\n");
  return 1;
}

// ===== houses make: turn the plot you stand on into a house =====
// With nobody living in it yet. The counterpart of 'houses assign' for a house
// placed by hand rather than handed to whoever is homeless -- a barracks, a
// guildhall, a house kept for somebody in particular.
private int do_make(object me)
{
  object loc, area;
  mixed file;

  loc = location_of(me);
  if (!loc)
  {
    notify_fail("Stand on the plot you want to build on.\n");
    return 0;
  }
  area = loc->query_area();
  if (!area)
  {
    notify_fail("This location has no area.\n");
    return 0;
  }

  // a location already built is made a house as it stands; a plot gets one
  // raised on it. Either is raised by hand for something in particular, so
  // never handed out
  if (!loc->has_component(LOCATION_COMPONENT_PLOT))
  {
    if (loc->has_component(LOCATION_COMPONENT_HOME))
    {
      notify_fail("This is a house already.\n");
      return 0;
    }
    file = area->furnish_house(loc->query_file_name());
    if (!stringp(file))
    {
      notify_fail("Could not make this location a house.\n");
      return 0;
    }
    write("Made this location a house. It is kept out of 'houses assign'; " +
          "make it a job's house with 'vacancies home <job>'.\n");
    return 1;
  }

  file = area->build_house_at(loc->query_file_name(), ({ }), 1);
  if (!stringp(file))
  {
    notify_fail("Could not raise a house here.\n");
    return 0;
  }

  write("Raised an empty house at " + file + ". It is kept out of " +
        "'houses assign'; make it a job's house with 'vacancies home <job>'.\n");
  return 1;
}

// ===== houses communal [off] =====
// Mark the house you stand in as communal, or not: a barracks or a dormitory,
// shared by whoever holds its job, where nobody founds a family nor marries
// anybody in.
private int do_communal(object me, string * args)
{
  object area;
  string file;
  int flag;

  area = house_area(me, "Stand in the house you want to mark.\n");
  if (!area)
    return 0;

  file = location_of(me)->query_file_name();
  if (!area->is_house(file))
  {
    notify_fail("This is not a house.\n");
    return 0;
  }

  flag = !(sizeof(args) > 1 && args[1] == "off");
  area->set_communal_house(file, flag);
  write(flag ? "This house is now communal: nobody founds a family by living " +
               "here.\n"
             : "This house is no longer communal.\n");
  return 1;
}

// ===== houses shared <n|off> =====
// Makes the house you stand in a shared house with room for n families, each
// housed and named on its own, or an ordinary house again with 1 (or off).
private int do_shared(object me, string * args)
{
  object area;
  string file;
  int households;

  area = house_area(me, "Stand in the house you want to share.\n");
  if (!area)
    return 0;

  file = location_of(me)->query_file_name();
  if (!area->is_house(file))
  {
    notify_fail("This is not a house.\n");
    return 0;
  }

  households = (sizeof(args) > 1 && args[1] == "off") ? 1
                 : (sizeof(args) > 1 ? to_int(args[1]) : 0);
  if (households < 1)
  {
    notify_fail("Usage: houses shared <n|off>\n");
    return 0;
  }
  if (!area->set_shared_house(file, households))
  {
    notify_fail("Could not: a communal house cannot be shared, and a household " +
                "that would go still has people in it.\n");
    return 0;
  }

  write(households > 1
          ? "This house now has room for " + households + " households; " +
            "'houses assign' fills them.\n"
          : "This house is an ordinary house again.\n");
  return 1;
}

// ===== houses short|long <text> =====
// Give the house you stand in a title or a description of its own, so a
// barracks or a guildhall does not read as one more house. The text is typed in
// the running instance's language, the way a POI label is; clear it with "none"
// and the dwelling reads as an ordinary home again.
private int do_describe(object me, string * args)
{
  object loc, home;
  string what, str;

  loc = location_of(me);
  if (!loc)
  {
    notify_fail("Stand inside the house you want to describe.\n");
    return 0;
  }

  what = args[0];
  str = rest_of(args, 1);
  if (!strlen(str))
  {
    notify_fail("Usage: houses " + what + " <text|none>\n");
    return 0;
  }

  home = loc->query_component_by_type(LOCATION_COMPONENT_HOME);
  if (!home)
  {
    notify_fail("You are not standing in a house.\n");
    return 0;
  }

  if (str == "none")
    str = "";

  if (what == "short")
    home->set_home_short(str);
  else
    home->set_home_long(str);

  loc->save_me();

  if (!strlen(str))
    write("The house reads as an ordinary home again.\n");
  else
    write("House " + what + " set.\n");

  return 1;
}

// ===== houses books <area path|none> [house file] =====
// Books a house with the community of another area, for when the two part:
// `file` names it, or the house the builder stands in. "none" books it where it
// stands again.
private int do_books(object me, string * args)
{
  object area, other;
  string area_path, file;

  area = house_area(me, "Stand in a location of the house's community.\n");
  if (!area)
    return 0;

  if (sizeof(args) < 2)
  {
    notify_fail("Usage: houses books <area path|none> [house file]\n");
    return 0;
  }

  area_path = args[1];
  file = sizeof(args) > 2 ? args[2] : location_of(me)->query_file_name();

  if (area_path != "none")
  {
    other = AREA_HANDLER->query_area(area_path);
    if (!other)
    {
      notify_fail("No area known at '" + area_path + "'.\n");
      return 0;
    }
    if (other->query_root_area() != area->query_root_area())
    {
      notify_fail("'" + area_path + "' is not part of this community.\n");
      return 0;
    }
  }

  if (!area->set_house_books(file, area_path == "none" ? nil : area_path))
  {
    notify_fail(file + " is not a house of this community.\n");
    return 0;
  }

  write(file + (area_path == "none"
                  ? " follows where it stands again.\n"
                  : " follows '" + other->query_area_name() + "' if they part.\n"));
  return 1;
}

// ===== houses remove: unbuild the house you stand in =====
// The inverse of a raised house: residents are evicted (left homeless, for a
// later 'houses assign' to place), the door becomes a doorway again and the
// plot returns to the free list, so 'houses plot remove <dir>' can then delete
// it.
private int do_remove(object me)
{
  object area;
  int evicted;

  area = house_area(me, "Stand inside the house you want to unbuild.\n");
  if (!area)
    return 0;

  evicted = (int)area->demote_house(location_of(me)->query_file_name());
  if (evicted < 0)
  {
    notify_fail("You are not standing in a house.\n");
    return 0;
  }

  write("Unbuilt the house" +
        (evicted ? ", evicting " + evicted + " resident" +
                   (evicted == 1 ? "" : "s") : "") +
        ". It is a bare plot again.\n");
  return 1;
}

// ===== houses plot <dir> | plot remove <dir> =====
// Carve an empty buildable lot next to this location, or delete a bare one.
private int do_plot(object me, string * args)
{
  object loc, area, principal;
  string dir, canon, pfile;
  mixed result;
  int * c, * d;
  int removing;

  removing = sizeof(args) > 1 && (args[1] == "remove" || args[1] == "delete");
  if (sizeof(args) < (removing ? 3 : 2))
  {
    notify_fail("Usage: houses plot < <dir> | remove <dir> >\n");
    return 0;
  }
  dir = removing ? args[2] : args[1];

  loc = location_of(me);
  if (!loc)
  {
    notify_fail("Stand in a location to build a plot.\n");
    return 0;
  }
  area = loc->query_area();

  if (removing)
  {
    // whatever wandered into the plot goes to the area's principal location
    // before the plot stops existing; with occupants and no principal the
    // plot is not deleted
    principal = nil;
    pfile = area ? area->query_principal() : "";
    if (pfile && strlen(pfile) && file_size(pfile) >= 0)
      principal = LOCATION_HANDLER->load_location(pfile);

    result = LOCATION_HANDLER->uncarve_location(loc, dir,
                                                LOCATION_COMPONENT_PLOT,
                                                principal);
    if (stringp(result) && !principal && result == LOCATION_NOT_EMPTY)
      result = "The plot has occupants and the area has no principal " +
               "location to move them to; set one with 'areas principal'. " +
               "Refusing to delete.";
    // the handler answers ({ the deleted file }), or why it would not
    if (stringp(result))
    {
      notify_fail(result + "\n");
      return 0;
    }

    if (area)
      area->remove_plot(result[0]);
    write("Removed the empty plot to the " +
          ROOM_HAND->localize_dir(ROOM_HAND->canonical_dir(dir)) + ".\n");
    return 1;
  }

  // no houses inside houses: a plot cannot be carved from a plot or a home
  if (loc->has_component(LOCATION_COMPONENT_PLOT) ||
      loc->has_component(LOCATION_COMPONENT_HOME))
  {
    notify_fail("You cannot carve a plot from inside a plot or a house.\n");
    return 0;
  }

  c = loc->query_coordinates();
  if (!c || sizeof(c) != 3)
  {
    notify_fail("This location has no coordinates.\n");
    return 0;
  }

  // a plot's file is named after the coordinates it lands on
  canon = ROOM_HAND->canonical_dir(dir);
  d = canon ? load_object(SECTORS_HANDLER)->query_dir_delta(canon) : nil;
  if (!d)
  {
    notify_fail("'" + dir + "' is not a compass direction.\n");
    return 0;
  }

  result = LOCATION_HANDLER->carve_location(loc, dir,
             "plot_" + (c[0] + d[0]) + "_" + (c[1] + d[1]) + "_" + (c[2] + d[2]),
             "", LOCATION_COMPONENT_PLOT);
  if (stringp(result))
  {
    notify_fail(result + "\n");
    return 0;
  }

  if (area)
    area->add_plot(result->query_file_name());

  write("Carved an empty plot to the " + ROOM_HAND->localize_dir(canon) +
        " at (" + (c[0] + d[0]) + "_" + (c[1] + d[1]) + "_" + (c[2] + d[2]) +
        ").\n");
  return 1;
}

static int cmd(string str, object me, string verb)
{
  string * args;
  object area;

  args = words(str);

  // the housing subcommands that act work on the location, and say what they
  // need themselves
  if (sizeof(args))
  {
    if (args[0] == "assign")
      return do_assign(me);
    if (args[0] == "make")
      return do_make(me);
    if (args[0] == "communal")
      return do_communal(me, args);
    if (args[0] == "shared")
      return do_shared(me, args);
    if (args[0] == "short" || args[0] == "long")
      return do_describe(me, args);
    if (args[0] == "books")
      return do_books(me, args);
    if (args[0] == "remove")
      return do_remove(me);
    if (args[0] == "plot")
      return do_plot(me, args);
  }

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

  notify_fail("Usage: " + query_usage() + "\n");
  return 0;
}
