#include <mud/cmd.h>
#include <areas/area.h>
#include <areas/vacancy.h>
#include <living/persisted.h>

inherit CMD_BASE;

// How many census rows a listing will read savefiles for. A listing reads the
// name and level of everybody who is not materialized straight off their
// savefile, and an area with a large population would spend the execution's
// whole tick budget doing it.
#define NPCS_READ_LIMIT 120

private string field_of(string savefile, string key);
private object area_of(object me);
private string kind_of(object area, string source, string gender);

void setup()
{
  set_aliases(({ "npcs" }));
  set_usage("npcs [ list | vacancies | roster | live | verify [apply] ]");
  set_help(
    "Report on the people of the area you are standing in.\n" +
    "\n" +
    "  npcs                 what the area holds, in one screen\n" +
    "  npcs list            everybody in its census, named\n" +
    "  npcs vacancies       the jobs it offers and who holds them\n" +
    "  npcs roster          the types it spawns statistically, and their caps\n" +
    "  npcs live            only the people materialized right now\n" +
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

// One value out of a saved NPC, read straight off its savefile. Used to name
// the people a listing covers who are not in the world at the moment.
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
      // strings come quoted; numbers do not
      if (strlen(value) > 1 && value[0] == '"')
        return value[1 .. strlen(value) - 2];
      return value;
    }

  return "";
}

// The kind word a type answers to, in the form matching a gender id. What
// somebody is called when they were never given a name of their own.
private string kind_of(object area, string source, string gender)
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
  // a bimodal type stores its names per gender, keyed by the driver's id
  if (mappingp(name))
    name = name[strlen(gender) ? gender : "1"];

  return stringp(name) ? name : "";
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
  string out;
  int i;

  jobs = (mapping *)area->query_vacancies();
  if (!sizeof(jobs))
  {
    write("Area '" + area->query_area_name() + "' offers no jobs.\n");
    return 1;
  }

  out = "Jobs offered by '" + area->query_area_name() + "':\n" +
        sprintf("  %-14s %-6s %-5s %-14s %-16s %s\n",
                "job", "seats", "held", "held at", "house", "type");

  for (i = 0; i < sizeof(jobs); i++)
    out += sprintf("  %-14s %-6s %-5s %-14s %-16s %s\n",
                   jobs[i][VACANCY_JOB],
                   "" + jobs[i][VACANCY_COUNT],
                   "" + sizeof((string *)area->query_vacancy_holders(jobs[i])),
                   get_path_file_name(jobs[i][VACANCY_WORKS_AT]),
                   jobs[i][VACANCY_HOME]
                     ? get_path_file_name(jobs[i][VACANCY_HOME]) : "-",
                   get_path_file_name(jobs[i][VACANCY_SOURCE]));

  write(out);
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
  string out;
  int i;

  live = (object *)area->query_live_npcs();
  if (!sizeof(live))
  {
    write("Nobody of '" + area->query_area_name() + "' is loaded.\n");
    return 1;
  }

  out = "People of '" + area->query_area_name() + "' in the world now:\n" +
        sprintf("  %-16s %-6s %-10s %-14s %-14s %s\n",
                "name", "level", "race", "here", "works at", "home");

  for (i = 0; i < sizeof(live); i++)
  {
    mixed race, work, home;

    // no (string) casts: that is a conversion kfun, and an NPC with no home
    // hands back nil
    race = live[i]->query_race_name();
    work = live[i]->query_work();
    home = live[i]->query_home();

    out += sprintf("  %-16s %-6s %-10s %-14s %-14s %s\n",
                   (string)live[i]->query_cap_name(),
                   "" + (int)live[i]->query_level(),
                   stringp(race) ? race : "-",
                   environment(live[i])
                     ? get_path_file_name(
                         environment(live[i])->query_file_name()) : "-",
                   (stringp(work) && strlen(work))
                     ? get_path_file_name(work) : "-",
                   (stringp(home) && strlen(home))
                     ? get_path_file_name(home) : "-");
  }

  write(out);
  return 1;
}

// ===== npcs list =====
private int do_list(object area, object me)
{
  mapping census;
  string * ids;
  string * * rows;
  string out;
  int * width;
  int i, j, read;

  census = (mapping)area->query_npc_census();
  ids = map_indices(census);
  if (!sizeof(ids))
  {
    write("Area '" + area->query_area_name() + "' has nobody on its books.\n");
    return 1;
  }

  rows = ({ ({ "name", "level", "type", "job", "works at", "location",
               "house", "loaded" }) });

  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e, job;
    object npc;
    mixed home, given;
    string name, level;

    e = census[ids[i]];
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
    else if (read < NPCS_READ_LIMIT)
    {
      read++;
      name = field_of(e["savefile"], "npc_given_name");
      level = field_of(e["savefile"], "class_level");
      home = field_of(e["savefile"], "npc_home");

      // Somebody never given a proper name is shown by their kind, the same
      // answer a live one gives. Those are the posts still staffed from a
      // named blueprint -- the barman, the healer -- which carry their name on
      // the type instead of generating one.
      if (!strlen(name))
        name = kind_of(area, e["source"],
                       field_of(e["savefile"], "gender"));
    }

    // a job with a house of its own houses whoever holds it
    if ((!stringp(home) || !strlen(home)) && e[CENSUS_VACANCY])
    {
      job = (mapping)area->query_vacancy(e[CENSUS_VACANCY]);
      if (job)
        home = job[VACANCY_HOME];
    }

    rows += ({ ({
      strlen(name) ? capitalize(name) : "?",
      strlen(level) ? level : "?",
      // what they are: the type they were drawn from. Without it a listing
      // says only that somebody holds no job, never whether they are a
      // citizen, a pilgrim or a stray dog.
      e["source"] ? get_path_file_name(e["source"]) : "-",
      e["guard"] ? "guard"
                 : (e[CENSUS_VACANCY] ? e[CENSUS_VACANCY] : "-"),
      e[CENSUS_WORKS_AT] ? get_path_file_name(e[CENSUS_WORKS_AT]) : "-",
      e[CENSUS_LOCATION] ? get_path_file_name(e[CENSUS_LOCATION]) : "-",
      (stringp(home) && strlen(home)) ? get_path_file_name(home) : "-",
      npc ? "yes" : "no"
    }) });
  }

  // measure every column over the rows themselves, so a long name or a long
  // location file widens its column instead of running into the next one
  width = allocate_int(sizeof(rows[0]));
  for (i = 0; i < sizeof(rows); i++)
    for (j = 0; j < sizeof(rows[i]); j++)
    {
      int l;
      l = strlen(rows[i][j], TRUE);
      if (l > width[j])
        width[j] = l;
    }

  out = "People of '" + area->query_area_name() + "' (" + sizeof(ids) +
        "):\n";
  for (i = 0; i < sizeof(rows); i++)
  {
    out += " ";
    for (j = 0; j < sizeof(rows[i]); j++)
      out += sprintf(" %-*s", width[j], rows[i][j]);
    out += "\n";

    // rule under the header, so the eye finds where the data starts
    if (i == 0)
    {
      out += " ";
      for (j = 0; j < sizeof(rows[i]); j++)
      {
        int k;
        out += " ";
        for (k = 0; k < width[j]; k++)
          out += "-";
      }
      out += "\n";
    }
  }

  if (read >= NPCS_READ_LIMIT)
    out += "  (stopped naming the unloaded after " + NPCS_READ_LIMIT + ")\n";

  write(out);
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
    return do_list(area, me);
  if (args[0] == "vacancies")
    return do_vacancies(area, me);
  if (args[0] == "roster")
    return do_roster(area, me);
  if (args[0] == "live")
    return do_live(area, me);

  notify_fail("Usage: npcs [ list | vacancies | roster | live | " +
              "verify [apply] ]\n");
  return 0;
}
