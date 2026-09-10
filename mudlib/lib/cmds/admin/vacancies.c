
// Report on the jobs a settlement offers: what each one is, where it is
// worked, what it hands its holders and who holds it. Reading only -- the
// builder ring is what declares and edits a job.

#include <mud/cmd.h>
#include <areas/area.h>
#include <areas/vacancy.h>
#include <areas/poi.h>
#include <living/persisted.h>

inherit CMD_BASE;

private object area_of(object me);
private string columns(string * * rows);
private string books_of(object area);
private string seen_from(object area);
private string leaf(string path);
private string where(object area, string file);
private string kit_of(mapping job);
private string hours_of(mapping job);
private string type_note(object area, mapping job);
private string * holder_rows(object area, mapping job);
private int do_list(object area, int free_only);
private int do_holders(object area);
private int do_types(object area);
private int do_job(object area, string name);

void setup()
{
  set_aliases(({ "vacancies" }));
  set_usage("vacancies [ <job> | free | holders | types ]");
  set_help(
    "Report on the jobs offered by the settlement you are standing in.\n" +
    "\n" +
    "  vacancies          every job it offers, one line each\n" +
    "  vacancies <job>    everything one job carries, and who holds it\n" +
    "  vacancies free     only the jobs with a seat nobody fills\n" +
    "  vacancies holders  person by person: who works what, and where\n" +
    "  vacancies types    the types the jobs draw from, and their templates\n" +
    "\n" +
    "A job is a post the settlement wants filled: a name, a number of seats, " +
    "the type its holders are drawn from and the places it is worked in. It " +
    "owns what the job hands over -- the kit, the hours, the class, the " +
    "house -- while the type owns what its people are like. Nobody is hired " +
    "automatically: a seat stays empty until somebody asks for it to be " +
    "filled.\n" +
    "\n" +
    "An area that hands its population to a parent shows that community's " +
    "jobs, not a set of its own.");
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
    for (j = 0; j < sizeof(rows[i]) && j < sizeof(width); j++)
    {
      k = strlen(rows[i][j], TRUE);
      if (k > width[j])
        width[j] = k;
    }

  out = "";
  for (i = 0; i < sizeof(rows); i++)
  {
    string line;

    line = "  ";
    for (j = 0; j < sizeof(rows[i]); j++)
      line += sprintf("%-*s ", width[j] + 1, rows[i][j]);
    out += line + "\n";

    // a rule under the header
    if (!i)
    {
      line = "  ";
      for (j = 0; j < sizeof(width); j++)
      {
        for (k = 0; k < width[j]; k++)
          line += "-";
        line += "  ";
      }
      out += line + "\n";
    }
  }

  return out;
}

// The area whose books answer for this one, named.
private string books_of(object area)
{
  object owner;

  owner = (object)area->query_root_area();
  return "'" + (string)((owner ? owner : area)->query_area_name()) + "'";
}

// Where the report is being read from, when that is not where it is kept.
private string seen_from(object area)
{
  object owner;

  owner = (object)area->query_root_area();
  if (!owner || owner == area)
    return "";

  return ", seen from '" + area->query_area_name() + "'";
}

// The readable end of a path: a location file or a blueprint by its own name.
private string leaf(string path)
{
  if (!stringp(path) || !strlen(path))
    return "-";
  return get_path_file_name(path);
}

// A place, said as its file plus whether it is in the world right now.
private string where(object area, string file)
{
  if (!stringp(file) || !strlen(file))
    return "-";

  return leaf(file) +
         (area->query_loaded_location(file) ? " (loaded)" : "");
}

// The kit the job hands a new holder, one line per slot.
private string kit_of(mapping job)
{
  mixed spec;
  string out;
  int i, j;

  spec = job[VACANCY_EQUIPMENT];
  if (!pointerp(spec) || !sizeof(spec))
    return "-";

  out = "";
  for (i = 0; i < sizeof(spec); i++)
  {
    if (!pointerp(spec[i]))
      continue;
    if (strlen(out))
      out += ", ";
    for (j = 0; j < sizeof(spec[i]); j++)
      out += (j ? "|" : "") + leaf(spec[i][j]);
  }

  return strlen(out) ? out : "-";
}

// The hours the job keeps, in the order of the clock.
private string hours_of(mapping job)
{
  mapping hours;
  string * keys;
  string out;
  int i, j, h, n;
  int * sorted;

  hours = job[VACANCY_TIMETABLE];
  if (!mappingp(hours) || !map_sizeof(hours))
    return "-";

  keys = map_indices(hours);
  sorted = allocate_int(sizeof(keys));
  for (i = 0; i < sizeof(keys); i++)
  {
    h = 0;
    sscanf(keys[i], "%d", h);
    sorted[i] = h;
  }
  // small list, plain insertion order by hour
  for (i = 1; i < sizeof(sorted); i++)
    for (j = i; j > 0 && sorted[j] < sorted[j - 1]; j--)
    {
      n = sorted[j];
      sorted[j] = sorted[j - 1];
      sorted[j - 1] = n;
    }

  out = "";
  for (i = 0; i < sizeof(sorted); i++)
  {
    mapping entry;

    entry = hours["" + sorted[i]];
    if (!mappingp(entry))
      continue;
    out += (strlen(out) ? ", " : "") + sorted[i] + "h " +
           (entry["goto"] ? entry["goto"] : "?") +
           (entry["msg"] ? " (says so)" : "");
  }

  return strlen(out) ? out : "-";
}

// What the type behind a job is, and whether it can actually staff one.
private string type_note(object area, mapping job)
{
  mapping t;
  string game, source;

  source = job[VACANCY_SOURCE];
  if (!stringp(source) || !strlen(source))
    return "(no type)";

  game = game_from_path((string)area->query_area_path());
  t = BESTIARY_HANDLER->query_template(game, source);

  if (!t)
    return source + "  -- NO TEMPLATE in this language";
  if (!t["sentient"])
    return source + "  -- not sentient";

  return source;
}

// One line per holder of a job: who, what level, where they are.
private string * holder_rows(object area, mapping job)
{
  mapping census;
  string * ids, * out;
  int i;

  census = (mapping)area->query_npc_census();
  ids = (string *)area->query_vacancy_holders(job);
  out = ({ });

  for (i = 0; i < sizeof(ids); i++)
  {
    object npc;
    mapping entry;
    string name, level;
    mixed given;

    entry = census[ids[i]];
    npc = AREA_HANDLER->find_live_npc(ids[i]);

    // In the world the person answers; out of it the census does, from the
    // copy it refreshes every time they appear. Somebody who has never been
    // materialized has neither, and is named after the type they came from.
    name = "?";
    level = "?";
    if (npc)
    {
      given = npc->query_given_name();
      name = (stringp(given) && strlen(given))
               ? capitalize(given) : (string)npc->query_cap_name();
      level = "" + (int)npc->query_level();
    }
    else if (entry)
    {
      if (entry["name"])
        name = capitalize(entry["name"]);
      else if (entry["source"])
        name = "a " + leaf(entry["source"]) + " with no name yet";
      if (entry["level"])
        level = "" + entry["level"];
    }

    out += ({ "    " + name + " (level " + level + ")  at " +
              (entry ? where(area, entry[CENSUS_LOCATION]) : "?") +
              "  works " +
              (entry ? leaf(entry[CENSUS_WORKS_AT]) : "-") +
              (npc ? "" : "  [not in the world]") });
  }

  return out;
}

// ===== vacancies / vacancies free =====
private int do_list(object area, int free_only)
{
  mapping * jobs;
  string * * rows;
  int i, shown;

  jobs = (mapping *)area->query_vacancies();
  if (!sizeof(jobs))
  {
    write("No jobs are offered by " + books_of(area) + seen_from(area) +
          ".\n");
    return 1;
  }

  rows = ({ ({ "job", "seats", "held", "worked at", "spots", "house",
               "type" }) });

  for (i = 0; i < sizeof(jobs); i++)
  {
    int held, seats;

    held = sizeof((string *)area->query_vacancy_holders(jobs[i]));
    seats = jobs[i][VACANCY_COUNT];
    if (free_only && held >= seats)
      continue;

    shown++;
    rows += ({ ({
      jobs[i][VACANCY_JOB],
      "" + seats,
      "" + held,
      leaf(jobs[i][VACANCY_WORKS_AT]),
      pointerp(jobs[i][VACANCY_SPOTS])
        ? "" + sizeof(jobs[i][VACANCY_SPOTS]) : "-",
      jobs[i][VACANCY_HOME] ? leaf(jobs[i][VACANCY_HOME])
                            : (jobs[i][VACANCY_RESIDENT] ? "in town" : "-"),
      leaf(jobs[i][VACANCY_SOURCE])
    }) });
  }

  if (free_only && !shown)
  {
    write("Every job of " + books_of(area) + " is filled.\n");
    return 1;
  }

  write((free_only ? "Jobs with a seat to fill in " : "Jobs offered by ") +
        books_of(area) + seen_from(area) + ":\n" + columns(rows) +
        "\nAsk 'vacancies <job>' for what one of them carries.\n");
  return 1;
}

// ===== vacancies holders =====
private int do_holders(object area)
{
  mapping * jobs;
  string out;
  int i, j;

  jobs = (mapping *)area->query_vacancies();
  out = "";

  for (i = 0; i < sizeof(jobs); i++)
  {
    string * lines;

    lines = holder_rows(area, jobs[i]);
    if (!sizeof(lines))
      continue;

    out += "  " + jobs[i][VACANCY_JOB] + "\n";
    for (j = 0; j < sizeof(lines); j++)
      out += lines[j] + "\n";
  }

  if (!strlen(out))
  {
    write("Nobody holds a job in " + books_of(area) + ".\n");
    return 1;
  }

  write("Who works what in " + books_of(area) + seen_from(area) + ":\n" + out);
  return 1;
}

// ===== vacancies types =====
private int do_types(object area)
{
  mapping * jobs;
  string * * rows;
  int i;

  jobs = (mapping *)area->query_vacancies();
  if (!sizeof(jobs))
  {
    write("No jobs are offered by " + books_of(area) + ".\n");
    return 1;
  }

  rows = ({ ({ "job", "type", "template", "sentient" }) });
  for (i = 0; i < sizeof(jobs); i++)
  {
    mapping t;
    string game, source;

    source = jobs[i][VACANCY_SOURCE];
    game = game_from_path((string)area->query_area_path());
    t = (stringp(source) && strlen(source))
          ? BESTIARY_HANDLER->query_template(game, source) : nil;

    rows += ({ ({
      jobs[i][VACANCY_JOB],
      stringp(source) ? source : "-",
      t ? "yes" : "MISSING",
      (t && t["sentient"]) ? "yes" : "NO"
    }) });
  }

  write("What the jobs of " + books_of(area) + " draw from:\n" +
        columns(rows) +
        "\nA job can only draw from a sentient type with a template in the " +
        "language the mud runs in. 'npcs template check' says what else is " +
        "wrong with one.\n");
  return 1;
}

// ===== vacancies <job> =====
private int do_job(object area, string name)
{
  mapping job, poi;
  string * lines;
  string out;
  int held, i;

  job = (mapping)area->query_vacancy(name);
  if (!job)
  {
    notify_fail("No job called '" + name + "' in " + books_of(area) + ".\n");
    return 0;
  }

  held = sizeof((string *)area->query_vacancy_holders(job));
  poi = (mapping)area->query_poi(job[VACANCY_WORKS_AT]);

  out = "Job '" + job[VACANCY_JOB] + "' of " + books_of(area) +
        seen_from(area) + "\n";
  out += "  type       " + type_note(area, job) + "\n";
  out += "  seats      " + job[VACANCY_COUNT] + ", held " + held + ", " +
         (job[VACANCY_COUNT] - held) + " free\n";
  out += "  worked at  " + where(area, job[VACANCY_WORKS_AT]) +
         (poi ? "  poi " + poi[POI_FIELD_KIND] +
                (poi[POI_FIELD_LABEL] ? " \"" + poi[POI_FIELD_LABEL] + "\""
                                      : "")
             : "") + "\n";

  if (pointerp(job[VACANCY_SPOTS]) && sizeof(job[VACANCY_SPOTS]))
  {
    out += "  spots      ";
    for (i = 0; i < sizeof(job[VACANCY_SPOTS]); i++)
      out += (i ? ", " : "") + leaf(job[VACANCY_SPOTS][i]);
    out += "\n";
  }
  else
    out += "  spots      -  (every holder stands where the job was " +
           "declared)\n";

  out += "  house      " +
         (job[VACANCY_HOME]
            ? where(area, job[VACANCY_HOME]) + "  (the post's own)"
            : (job[VACANCY_RESIDENT]
                 ? "-  (its holders are housed among the townsfolk)"
                 : "-  (its holders are never housed)")) + "\n";
  out += "  class      " +
         (job[VACANCY_CLASS] ? job[VACANCY_CLASS] : "-") + "\n";
  out += "  kit        " + kit_of(job) + "\n";
  out += "  hours      " + hours_of(job) + "\n";
  out += "  post       " +
         (job[VACANCY_FIXED] ? "fixed: one place, refilled there"
                             : "spread over the places it lists") + "\n";

  lines = holder_rows(area, job);
  if (sizeof(lines))
  {
    out += "  holders\n";
    for (i = 0; i < sizeof(lines); i++)
      out += lines[i] + "\n";
  }
  else
    out += "  holders    nobody\n";

  write(out);
  return 1;
}

static int cmd(string str, object me, string verb)
{
  object area;
  string * args;

  args = (str && strlen(str)) ? explode(str, " ") - ({ "" }) : ({ });

  area = area_of(me);
  if (!area)
  {
    notify_fail("Stand in a location (not a plain room) to report on the " +
                "jobs it offers.\n");
    return 0;
  }

  if (!sizeof(args))
    return do_list(area, 0);
  if (sizeof(args) > 1)
  {
    notify_fail("Usage: vacancies [ <job> | free | holders | types ]\n");
    return 0;
  }

  if (args[0] == "free")
    return do_list(area, 1);
  if (args[0] == "holders")
    return do_holders(area);
  if (args[0] == "types")
    return do_types(area);

  return do_job(area, args[0]);
}
