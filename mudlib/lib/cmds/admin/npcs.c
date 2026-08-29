#include <mud/cmd.h>
#include <areas/area.h>
#include <areas/vacancy.h>
#include <living/persisted.h>
#include <basic/gender.h>

inherit CMD_BASE;

private object area_of(object me);
private int do_orphans(object area, object me, string * args);
private string kind_of(object area, string source);
private string columns(string * * rows);
private string hours_of(object area, string source);

void setup()
{
  set_aliases(({ "npcs" }));
  set_usage("npcs [ list [type] | vacancies | roster | live | orphans [apply] |\n            verify [apply] ]");
  set_help(
    "Report on the people of the area you are standing in.\n" +
    "\n" +
    "  npcs                 what the area holds, in one screen\n" +
    "  npcs list [type]     everybody in its census, or one kind of them\n" +
    "  npcs vacancies       the jobs it offers and who holds them\n" +
    "  npcs roster          the types it spawns statistically, and their caps\n" +
    "  npcs live            only the people materialized right now\n" +
    "  npcs orphans [apply] people the area no longer accounts for\n" +
    "  npcs verify [apply]  audit the game's NPC savefiles against the census\n" +
    "\n" +
    "The census is the area's record of its individuals: each has a uuid and " +
    "a savefile, and only exists in the world while its location is loaded. " +
    "A listing therefore reads the name and level of anybody not materialized " +
    "off their savefile, so it can name them all.\n" +
    "\n" +
    "In a listing, 'location' is where the person is according to the books, " +
    "which for somebody out of the world is where they were when it last " +
    "unloaded; 'works at' is where their job is done, and the two differ for " +
    "anyone at home or on their way there. 'loaded' says whether they exist " +
    "in the world right now.\n" +
    "\n" +
    "'orphans' finds the other direction: somebody on the books that nothing " +
    "asks for any more -- holding no job, and of a type the roster no longer " +
    "counts. Promoting a type to a vacancy leaves exactly this behind, since " +
    "the roster stops counting it while the people it already made stay. " +
    "'apply' takes them off the books; run 'verify apply' afterwards to " +
    "delete the savefiles they leave behind.\n" +
    "\n" +
    "'verify' is the one that spans the whole game rather than one area: it " +
    "reports savefile folders with no census entry -- orphans left when an " +
    "entry was dropped without the person dying -- and 'apply' deletes them.");
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


// The kind word a type answers to, in the form matching a gender id. What
// somebody is called when they were never given a name of their own.
private string kind_of(object area, string source)
{
  mapping template;
  mixed name;

  if (!source || !strlen(source))
    return "";

  template = BESTIARY_HANDLER->query_template(
               game_from_path((string)area->query_area_path()), source);
  if (!template)
    return "";

  name = template["name"];
  // a bimodal type stores its names per gender, keyed by the driver's id; with
  // nobody in the world to ask, the masculine form stands for the type
  if (mappingp(name))
    name = name["" + GENDER_MALE];

  return stringp(name) ? name : "";
}

// The hours a day moves somebody, in order: "6/20" for out at six and back at
// eight. Read from the type, which is where a timetable lives; empty for
// anyone the day does not move.
private string hours_of(object area, string source)
{
  mapping template, timetable;
  string out;
  int h;

  if (!source || !strlen(source))
    return "";

  template = BESTIARY_HANDLER->query_template(
               game_from_path((string)area->query_area_path()), source);
  if (!template || !mappingp(template["timetable"]))
    return "";

  timetable = template["timetable"];
  out = "";

  // walk the clock rather than the mapping: its keys come out in no order, and
  // JSON left them as strings on some types and ints on others
  for (h = 0; h < 24; h++)
    if (!undefinedp(timetable[h]) || !undefinedp(timetable["" + h]))
      out += (strlen(out) ? "/" : "") + h;

  return out;
}

// Lay rows out in columns, the first row being the header: every column is as
// wide as its widest cell, and a rule under the header marks where the data
// starts.
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

// ===== npcs: what the area holds =====
private int do_summary(object area, object me)
{
  mapping census;
  mapping * jobs;
  object parent;
  string * ids;
  string out;
  int * band;
  int i, live, held, guards;

  census = (mapping)area->query_npc_census();
  ids = map_indices(census);
  jobs = (mapping *)area->query_vacancies();
  band = (int *)area->query_area_stats();

  for (i = 0; i < sizeof(ids); i++)
  {
    if (census[ids[i]]["guard"])
      guards++;
    else if (census[ids[i]][CENSUS_VACANCY])
      held++;
  }
  live = sizeof((object *)area->query_live_npcs());

  out = "Area '" + area->query_area_name() + "'\n";

  parent = (object)area->query_parent_area();
  if (parent)
    out += "  part of      " + parent->query_area_name() + "\n";

  out += "  citizenship  " +
         (strlen((string)area->query_citizenship())
            ? (string)area->query_citizenship() : "(none)") + "\n" +
         "  level band   " + area->query_area_level() + " +/- " +
         area->query_area_spread() + "\n" +
         "  stat band    " +
         (band[0] ? band[0] + "-" + band[1] : "(the type's own)") + "\n" +
         "\n" +
         "  census       " + sizeof(ids) + " individual(s): " + held +
         " holding a job, " + guards + " on guard, " +
         (sizeof(ids) - held - guards) + " unattached\n" +
         "  vacancies    " + sizeof(jobs) + " job(s) offered\n" +
         "  live         " + live + " materialized right now\n";

  write(out);
  return 1;
}

// ===== npcs vacancies =====
private int do_vacancies(object area, object me)
{
  mapping * jobs;
  string * * rows;
  int i;

  jobs = (mapping *)area->query_vacancies();
  if (!sizeof(jobs))
  {
    write("Area '" + area->query_area_name() + "' offers no jobs.\n");
    return 1;
  }

  rows = ({ ({ "job", "seats", "held", "held at", "house", "type" }) });
  for (i = 0; i < sizeof(jobs); i++)
    rows += ({ ({
      jobs[i][VACANCY_JOB],
      "" + jobs[i][VACANCY_COUNT],
      "" + sizeof((string *)area->query_vacancy_holders(jobs[i])),
      get_path_file_name(jobs[i][VACANCY_WORKS_AT]),
      jobs[i][VACANCY_HOME]
        ? get_path_file_name(jobs[i][VACANCY_HOME]) : "-",
      get_path_file_name(jobs[i][VACANCY_SOURCE])
    }) });

  write("Jobs offered by '" + area->query_area_name() + "':\n" + columns(rows));
  return 1;
}

// ===== npcs roster =====
private int do_roster(object area, object me)
{
  mapping caps;
  string * sources;
  string out;
  int i;

  caps = (mapping)area->query_npc_caps();
  sources = map_indices(caps);
  if (!sizeof(sources))
  {
    write("Area '" + area->query_area_name() + "' spawns no types of its " +
          "own.\n");
    return 1;
  }

  out = "Types '" + area->query_area_name() + "' spawns, and their caps:\n";
  for (i = 0; i < sizeof(sources); i++)
    out += sprintf("  %-28s live %-3s cap %-3s%s\n",
                   sources[i],
                   "" + (int)area->query_total_live_count(sources[i]),
                   "" + caps[sources[i]]["max"],
                   caps[sources[i]]["resident"] ? " resident" : "");

  write(out);
  return 1;
}

// ===== npcs live =====
private int do_live(object area, object me)
{
  object * live;
  string * * rows;
  int i;

  live = (object *)area->query_live_npcs();
  if (!sizeof(live))
  {
    write("Nobody of '" + area->query_area_name() + "' is loaded.\n");
    return 1;
  }

  rows = ({ ({ "name", "level", "race", "here", "works at", "home" }) });
  for (i = 0; i < sizeof(live); i++)
  {
    mixed race, work, home;

    // no (string) casts: that is a conversion kfun, and an NPC with no home
    // hands back nil
    race = live[i]->query_race_name();
    work = live[i]->query_work();
    home = live[i]->query_home();

    rows += ({ ({
      (string)live[i]->query_cap_name(),
      "" + (int)live[i]->query_level(),
      stringp(race) ? race : "-",
      environment(live[i])
        ? get_path_file_name(environment(live[i])->query_file_name()) : "-",
      (stringp(work) && strlen(work)) ? get_path_file_name(work) : "-",
      (stringp(home) && strlen(home)) ? get_path_file_name(home) : "-"
    }) });
  }

  write("People of '" + area->query_area_name() +
        "' in the world now:\n" + columns(rows));
  return 1;
}

// ===== npcs list =====
private int do_list(object area, object me, string want)
{
  mapping census;
  string * ids;
  string * * rows;
  int i, shown;

  census = (mapping)area->query_npc_census();
  ids = map_indices(census);
  if (!sizeof(ids))
  {
    write("Area '" + area->query_area_name() + "' has nobody on its books.\n");
    return 1;
  }

  rows = ({ ({ "name", "level", "type", "job", "day", "works at", "location",
               "house", "loaded" }) });

  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e, job;
    object npc;
    mixed home, given;
    string name, level, kind, day;

    e = census[ids[i]];
    kind = e["source"] ? get_path_file_name(e["source"]) : "-";

    // a listing can be asked for one kind of person only
    if (strlen(want) && kind != want)
      continue;

    day = hours_of(area, e["source"]);

    npc = AREA_HANDLER->find_live_npc(ids[i]);
    name = "";
    level = "";
    home = nil;

    if (npc)
    {
      // the proper name, not query_cap_name: that one deliberately answers
      // with the kind word ("Granjero") so room lists never read like players.
      // No (string) cast -- somebody never given a name hands back nil, and
      // the cast is a conversion kfun that errors on it.
      given = npc->query_given_name();
      name = (stringp(given) && strlen(given))
               ? given : (string)npc->query_cap_name();
      level = "" + (int)npc->query_level();
      home = npc->query_home();
    }
    else
    {
      // out of the world there is nobody to ask, so the books answer: the
      // census keeps a copy of who somebody is, refreshed every time they
      // appear. It has nothing for one who has never been in the world yet.
      name = e["name"] ? e["name"] : kind_of(area, e["source"]);
      level = e["level"] ? "" + e["level"] : "";
    }

    // where they live is not on the books and does not need to be: the houses
    // list their residents, and that is the area's own record
    home = area->query_house_of(ids[i]);
    if ((!stringp(home) || !strlen(home)) && e[CENSUS_VACANCY])
    {
      // a job with a house of its own houses whoever holds it
      job = (mapping)area->query_vacancy(e[CENSUS_VACANCY]);
      if (job)
        home = job[VACANCY_HOME];
    }

    shown++;
    rows += ({ ({
      strlen(name) ? capitalize(name) : "?",
      strlen(level) ? level : "?",
      kind,
      e["guard"] ? "guard"
                 : (e[CENSUS_VACANCY] ? e[CENSUS_VACANCY] : "-"),
      strlen(day) ? day : "-",
      e[CENSUS_WORKS_AT] ? get_path_file_name(e[CENSUS_WORKS_AT]) : "-",
      e[CENSUS_LOCATION] ? get_path_file_name(e[CENSUS_LOCATION]) : "-",
      (stringp(home) && strlen(home)) ? get_path_file_name(home) : "-",
      npc ? "yes" : "no"
    }) });
  }

  if (strlen(want) && !shown)
  {
    write("Nobody of '" + area->query_area_name() + "' is a " + want + ".\n");
    return 1;
  }

  write("People of '" + area->query_area_name() + "'" +
        (strlen(want) ? " of kind '" + want + "'" : "") +
        " (" + shown + "):\n" + columns(rows));
  return 1;
}

// ===== npcs orphans =====
// Somebody the area no longer accounts for: holding no job, and of a type its
// roster no longer counts. Declaring a vacancy for a type is what usually makes
// them -- the roster drops the type, and the people it had already scattered
// stay on the books with nothing to belong to.
private int do_orphans(object area, object me, string * args)
{
  mapping census, caps;
  string * ids;
  string * * rows;
  int i, apply, taken;

  apply = (sizeof(args) > 1 && args[1] == "apply");

  census = (mapping)area->query_npc_census();
  caps = (mapping)area->query_npc_caps();
  ids = map_indices(census);

  rows = ({ ({ "name", "type", "location", "house" }) });

  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e;
    object npc;
    mixed home;
    string kind, name;

    e = census[ids[i]];

    // holding a post is reason enough to be here, guards included
    if (e[CENSUS_VACANCY] || e["guard"])
      continue;
    // and so is being one the roster still counts
    if (e["source"] && caps[e["source"]])
      continue;

    kind = e["source"] ? get_path_file_name(e["source"]) : "-";
    npc = AREA_HANDLER->find_live_npc(ids[i]);
    name = e["name"] ? e["name"] : kind_of(area, e["source"]);
    home = area->query_house_of(ids[i]);

    rows += ({ ({
      strlen(name) ? capitalize(name) : "?",
      kind,
      e[CENSUS_LOCATION] ? get_path_file_name(e[CENSUS_LOCATION]) : "-",
      (stringp(home) && strlen(home)) ? get_path_file_name(home) : "-"
    }) });
    taken++;

    if (apply)
    {
      // out of the world first, so nothing is left standing in a location
      // pointing at a row that no longer exists
      if (npc)
        npc->dest_me();
      area->release_house(ids[i]);
      area->drop_census_entry(ids[i]);
    }
  }

  if (!taken)
  {
    write("Everybody on the books of '" + area->query_area_name() +
          "' is accounted for.\n");
    return 1;
  }

  write((apply ? "Taken off the books of '" : "Unaccounted for in '") +
        area->query_area_name() + "' (" + taken + "):\n" + columns(rows) +
        (apply
          ? "\nTheir savefiles are still there; 'npcs verify apply' removes them.\n"
          : "\n'npcs orphans apply' takes them off the books.\n"));
  return 1;
}

// ===== npcs verify =====
private int do_verify(object me, string * args)
{
  object env;
  string game;
  int apply;

  apply = (sizeof(args) > 1 && args[1] == "apply");

  env = environment(me);
  if (!env || !env->query_file_name())
  {
    notify_fail("Stand in a game location to pick the game to verify.\n");
    return 0;
  }
  game = game_from_path(env->query_file_name());
  if (!game)
  {
    notify_fail("Cannot tell which game you are in.\n");
    return 0;
  }

  // Runs in the background (it may span thousands of areas / folders); the
  // summary is sent when it finishes.
  load_object(AREA_HANDLER)->verify_npc_saves(game, apply, me);
  write("Verifying NPC saves for '" + game + "'" +
        (apply ? " (deleting orphans)" : "") +
        " in the background; the summary will follow.\n");

  return 1;
}

static int cmd(string str, object me, string verb)
{
  string * args;
  object area;

  args = (str && strlen(str)) ? explode(str, " ") - ({ "" }) : ({ });

  // the game-wide audit is the one report that does not need an area
  if (sizeof(args) && args[0] == "verify")
    return do_verify(me, args);

  area = area_of(me);
  if (!area)
  {
    notify_fail("Stand in a location (not a plain room) to report on its " +
                "people.\n");
    return 0;
  }

  if (!sizeof(args))
    return do_summary(area, me);
  if (args[0] == "list")
    return do_list(area, me,
                   sizeof(args) > 1 ? args[1] : "");
  if (args[0] == "vacancies")
    return do_vacancies(area, me);
  if (args[0] == "roster")
    return do_roster(area, me);
  if (args[0] == "live")
    return do_live(area, me);
  if (args[0] == "orphans")
    return do_orphans(area, me, args);

  notify_fail("Usage: npcs [ list [type] | vacancies | roster | live | " +
              "orphans [apply] | verify [apply] ]\n");
  return 0;
}
