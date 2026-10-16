
// The jobs a settlement offers: what each one is, where it is worked, what it
// hands its holders and who holds it, and the subcommands that declare and edit
// them.

#include <mud/cmd.h>
#include <areas/area.h>
#include <areas/vacancy.h>
#include <areas/poi.h>
#include <room/location.h>
#include <living/persisted.h>

inherit LOCATION_CMD;

private string table_of(string * * rows);
private string books_of(object area);
private string seen_from(object area);
private string leaf(string path);
private string where(object area, string file);
private string kit_of(mixed spec);
private string hours_of(mapping job);
private string type_note(object area, mapping job);
private string * holder_rows(object area, mapping job);
private int do_list(object area, int free_only);
private int do_holders(object area);
private int do_types(object area);
private int do_job(object area, string name);
private int reserved_name(string name);
private int do_add(object area, object loc, string * args);
private int do_equip(object area, string * args);
private int do_timetable(object area, string * args);
private int do_class(object area, string * args);
private int do_home(object area, object loc, string * args);
private int do_resident(object area, string * args);
private int do_spot(object area, object loc, string * args);
private int do_reseat(object area, string * args);
private int do_rename(object area, string * args);
private int do_remove(object area, string * args);

void setup()
{
  set_aliases(({ "vacancies" }));
  set_usage("vacancies [ <job> | free | holders | types\n" +
            "            | add <name> <count> <source.c>[*w,...] [fixed]\n" +
            "            | equip <name> [for <type>] <item.c[|alt.c...]>...\n" +
            "            | timetable <name> [<hour> <work|home|out|clear>]\n" +
            "            | timetable <name> <hour> <leave|arrive> [message]\n" +
            "            | class <name> <class.c|none> | home <name> [none]\n" +
            "            | resident <name> [none] | spot <name> [remove]\n" +
            "            | reseat <name> | rename <name> <new name>\n" +
            "            | remove <name> ]");
  set_help(
    "Report on, declare and edit the jobs offered by the settlement you are " +
    "standing in. Stand in a location, not a plain room.\n" +
    "\n" +
    "  vacancies          every job it offers, one line each\n" +
    "  vacancies <job>    everything one job carries (places, kit, hours, " +
    "class, house), and who holds it\n" +
    "  vacancies free     only the jobs with a seat nobody fills\n" +
    "  vacancies holders  person by person: who works what, and where\n" +
    "  vacancies types    the types the jobs draw from, and their templates\n" +
    "\n" +
    "  vacancies add <name> <count> <source.c>[*w,...] [fixed]\n" +
    "                     declare a job, worked where you stand; several " +
    "types are drawn by weight (source.c*3,other.c)\n" +
    "  vacancies equip <name> [for <type>] <item.c[|alt.c...]>...\n" +
    "                     the kit of the job, one slot per item (alternatives " +
    "joined by '|'); 'for <type>' gives that type its own; 'none' clears\n" +
    "  vacancies timetable <name>\n" +
    "                     list the hours the job keeps\n" +
    "  vacancies timetable <name> <hour> <work|home|out|clear>\n" +
    "                     where its holders are at that hour (0-23), or a " +
    "location file; 'clear' drops the hour\n" +
    "  vacancies timetable <name> <hour> <leave|arrive> [message]\n" +
    "                     the line told as they set off or arrive; no text " +
    "clears it\n" +
    "  vacancies class <name> <class.c|none>\n" +
    "                     what the job trains its people in\n" +
    "  vacancies home <name> [none]\n" +
    "                     bind the plot or house you stand in to the job, or " +
    "with 'none' take it off\n" +
    "  vacancies resident <name> [none]\n" +
    "                     its holders are housed among the townsfolk, or not\n" +
    "  vacancies spot <name> [remove]\n" +
    "                     add the place you stand in to where the job is " +
    "worked, or take it off\n" +
    "  vacancies reseat <name>\n" +
    "                     seat its holders again where it would put them today\n" +
    "  vacancies rename <name> <new name>\n" +
    "                     its holders keep the job\n" +
    "  vacancies remove <name>\n" +
    "                     close the job and cull its holders\n" +
    "\n" +
    "A job is a post the settlement wants filled: a name, a number of seats, " +
    "the type its holders are drawn from and the places it is worked in. It " +
    "owns what the job hands over -- the kit, the hours, the class, the " +
    "house -- while the type owns what its people are like. Nobody is hired " +
    "automatically: a seat stays empty until somebody asks for it to be " +
    "filled. A fixed job is one post, refilled there promptly; the rest " +
    "spread over the places they list.\n" +
    "\n" +
    "The words add, equip, timetable, class, home, resident, spot, reseat, " +
    "rename, remove, list, spots, free, holders and types are commands, so " +
    "no job can be called by them. Places a job is worked are shown by " +
    "'vacancies <job>'.\n" +
    "\n" +
    "An area that hands its population to a parent shows that community's " +
    "jobs, not a set of its own.");
}

// Lay rows out in columns, the first row being the header.
private string table_of(string * * rows)
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

// A kit as one line: its slots, with the alternatives of each joined by '|'.
private string kit_of(mixed spec)
{
  string out;
  int i, j;

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
           (entry["leave"] ? " (leave)" : "") +
           (entry["arrive"] ? " (arrive)" : "");
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

  // several types: name them all with their weights; `vacancies types` checks
  // each of them
  if (pointerp(job[VACANCY_SOURCES]) && sizeof(job[VACANCY_SOURCES]) > 1)
  {
    string * parts;
    int i;

    parts = ({ });
    for (i = 0; i < sizeof(job[VACANCY_SOURCES]); i++)
      parts += ({ job[VACANCY_SOURCES][i][0] + " *" + job[VACANCY_SOURCES][i][1] });
    return implode(parts, ", ");
  }

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
    npc = find_living(ids[i]);

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
        books_of(area) + seen_from(area) + ":\n" + table_of(rows) +
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
    string * sources;
    string game;
    int j;

    // one row per type: a job may draw from several
    sources = (string *)area->query_vacancy_source_list(jobs[i]);
    if (!sizeof(sources))
      sources = ({ nil });
    game = game_from_path((string)area->query_area_path());
    for (j = 0; j < sizeof(sources); j++)
    {
      mapping t;

      t = (stringp(sources[j]) && strlen(sources[j]))
            ? BESTIARY_HANDLER->query_template(game, sources[j]) : nil;

      rows += ({ ({
        j ? "" : jobs[i][VACANCY_JOB],
        stringp(sources[j]) ? sources[j] : "-",
        t ? "yes" : "MISSING",
        (t && t["sentient"]) ? "yes" : "NO"
      }) });
    }
  }

  write("What the jobs of " + books_of(area) + " draw from:\n" +
        table_of(rows) +
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
  out += "  kit        " + kit_of(job[VACANCY_EQUIPMENT]) + "\n";
  if (mappingp(job[VACANCY_KITS]))
  {
    string * types;
    int k;

    types = map_indices(job[VACANCY_KITS]);
    for (k = 0; k < sizeof(types); k++)
      out += "    for " + leaf(types[k]) + ": " +
             kit_of(job[VACANCY_KITS][types[k]]) + "\n";
  }
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

// Whether a word is a subcommand of this command, and so no name for a job.
private int reserved_name(string name)
{
  return member_array(name, ({ "add", "equip", "timetable", "class", "home",
                               "resident", "spot", "spots", "reseat",
                               "rename", "remove", "list", "free", "holders",
                               "types" })) != -1;
}

// ===== vacancies add =====
private int do_add(object area, object loc, string * args)
{
  string name, source;
  int count, fixed, templated, i;
  mapping extra;

  if (sizeof(args) < 4 || sscanf(args[2], "%d", count) != 1)
  {
    notify_fail("Usage: vacancies add <name> <count> <source.c>[*w,...] " +
                "[fixed]\n");
    return 0;
  }
  name = args[1];
  source = args[3];

  if (reserved_name(name))
  {
    notify_fail("'" + name + "' is a subcommand of vacancies: a job needs " +
                "another name.\n");
    return 0;
  }

  // a fixed job is one post: it stays where it is and its holder is replaced
  // there promptly; the rest spread over the area's like places
  fixed = (sizeof(args) > 4 && args[4] == "fixed");
  extra = fixed ? ([ VACANCY_FIXED: 1 ]) : ([ ]);

  // several types, each holder drawn from one by weight: <src>*<weight>,...
  // with a weight of 1 for any that names none
  if (sizeof(explode(source, ",")) > 1 || sizeof(explode(source, "*")) > 1)
  {
    mixed * sources;

    sources = area->parse_vacancy_sources(source);
    if (!sources)
    {
      notify_fail("Each type is <source>[*<weight>], the weight above 0.\n");
      return 0;
    }
    extra[VACANCY_SOURCES] = sources;
    source = sources[0][0];
  }

  // A job is filled from an authored template, so what is named here is a
  // template id ("areas/<area>/npcs/<type>"). A path still works -- it
  // normalises to the same id. The job may be declared before its type is
  // written; nobody can be taken on until it is, so say so.
  templated = 1;
  if (extra[VACANCY_SOURCES])
  {
    for (i = 0; i < sizeof(extra[VACANCY_SOURCES]); i++)
      if (!BESTIARY_HANDLER->has_template(game_name(area),
            (string)area->query_template_from_source(
              extra[VACANCY_SOURCES][i][0])))
        templated = 0;
  }
  else
    templated = (int)BESTIARY_HANDLER->has_template(
                  game_name(area),
                  (string)area->query_template_from_source(source));

  // The area stores the job: how many, where (wherever the admin stands), and
  // the type its holders are drawn from. Behaviour is the type's: the template
  // is marked sentient in its template, so a holder is a named citizen.
  //
  // Opening a job does not staff it. Declaring that a settlement wants a
  // barman and deciding that somebody walks into the bar are two different
  // acts, and only the first belongs to a builder.
  area->open_vacancy(name, count, loc->query_file_name(), source, extra);
  write("Vacancy '" + name + "' x" + count + " <- " + args[3] +
        ", held here. Nobody taken on.\n");
  if (!templated)
    write("There is no template for '" + args[3] + "' yet: write one before " +
          "staffing it.\n");
  return 1;
}

// ===== vacancies remove =====
private int do_remove(object area, string * args)
{
  if (sizeof(args) < 2)
  {
    notify_fail("Usage: vacancies remove <name>\n");
    return 0;
  }
  if (!area->query_vacancy(args[1]))
  {
    notify_fail("No vacancy '" + args[1] + "' in this area.\n");
    return 0;
  }
  area->close_vacancy(args[1]);
  write("Vacancy '" + args[1] + "' closed.\n");
  return 1;
}

// ===== vacancies class =====
private int do_class(object area, string * args)
{
  // what the trade trains its people in. A farmer is not a soldier, and the
  // NPC type cannot say so: one source staffs several settlements.
  if (sizeof(args) < 3)
  {
    notify_fail("Usage: vacancies class <name> <class.c|none>\n");
    return 0;
  }

  if (args[2] != "none" && file_size(args[2]) < 0 &&
      file_size(args[2] + ".c") < 0)
  {
    notify_fail("No class object at '" + args[2] + "'.\n");
    return 0;
  }

  if (!area->set_vacancy_class(args[1], args[2] == "none" ? "" : args[2]))
  {
    notify_fail("No vacancy '" + args[1] + "' in this area.\n");
    return 0;
  }

  if (args[2] == "none")
    write("Role '" + args[1] + "' no longer sets a class; its people keep " +
          "the type's.\n");
  else
    write("Holders of '" + args[1] + "' are now trained as " + args[2] +
          ".\n");
  return 1;
}

// ===== vacancies equip =====
private int do_equip(object area, string * args)
{
  mixed * spec;
  string * tokens;
  string source;
  int t;

  if (sizeof(args) < 3)
  {
    notify_fail("Usage: vacancies equip <name> [for <type>] " +
                "<item.c[|alt.c...]> ...\n" +
                "  Each argument is one slot; join alternatives with '|' and " +
                "each NPC rolls one (e.g. weapons/club|weapons/sickle).\n" +
                "  'for <type>' gives the holders drawn from that type their " +
                "own kit; 'none' as the only item clears a kit.\n");
    return 0;
  }
  if (!area->query_vacancy(args[1]))
  {
    notify_fail("No job '" + args[1] + "' in this area.\n");
    return 0;
  }

  // a kit for one of the job's types rather than for the whole job
  if (sizeof(args) > 4 && args[2] == "for")
  {
    source = args[3];
    tokens = args[4 ..];
  }
  else
    tokens = args[2 ..];
  if (sizeof(tokens) == 1 && tokens[0] == "none")
    tokens = ({ });

  // one slot per argument; '|' inside a slot lists interchangeable items.
  // Validate every blueprint exists before storing the kit.
  spec = ({ });
  for (t = 0; t < sizeof(tokens); t++)
  {
    string * alts;
    int a;

    alts = explode(tokens[t], "|") - ({ "" });
    if (!sizeof(alts))
      continue;
    for (a = 0; a < sizeof(alts); a++)
      if (file_size(alts[a]) < 0 && file_size(alts[a] + ".c") < 0)
      {
        notify_fail("No item blueprint at '" + alts[a] + "'.\n");
        return 0;
      }
    spec += ({ alts });
  }

  // The kit belongs to the post, not to the people it draws from: the same
  // trade is armed differently from one town to the next. Whoever holds the
  // post now keeps what they were given at birth, the way a player keeps what
  // they own; the kit takes effect on the next person to fill it.
  if (source)
  {
    area->set_vacancy_kit(args[1], source, spec);
    write("Job '" + args[1] + "' kit for " + source + " set: " + sizeof(spec) +
          " slot" + (sizeof(spec) == 1 ? "" : "s") +
          ". Whoever of that type fills it next rolls their gear.\n");
    return 1;
  }
  area->set_vacancy_equipment(args[1], spec);
  write("Job '" + args[1] + "' kit set: " + sizeof(spec) +
        " slot" + (sizeof(spec) == 1 ? "" : "s") +
        ". Whoever fills it next rolls their gear.\n");
  return 1;
}

// ===== vacancies timetable =====
private int do_timetable(object area, string * args)
{
  // the hours the job keeps; the area edits them, this only reads the words
  mapping job, hours;
  string msg, error;
  int hour;

  if (sizeof(args) < 2)
  {
    notify_fail("Usage: vacancies timetable <name> [<hour 0-23> " +
                "<work|home|out|clear> | <leave|arrive> [message]]\n");
    return 0;
  }

  job = area->query_vacancy(args[1]);
  if (!job)
  {
    notify_fail("No vacancy '" + args[1] + "' in this area.\n");
    return 0;
  }

  if (sizeof(args) == 2)
  {
    hours = job[VACANCY_TIMETABLE];
    if (!mappingp(hours) || !map_sizeof(hours))
    {
      write("Job '" + args[1] + "' keeps no hours.\n");
      return 1;
    }
    write("Job '" + args[1] + "' hours: " +
          implode(map_indices(hours), ", ") + ".\n");
    return 1;
  }

  if (sizeof(args) < 4 || sscanf(args[2], "%d", hour) != 1 ||
      hour < 0 || hour > 23)
  {
    notify_fail("Usage: vacancies timetable <name> <hour 0-23> " +
                "<work|home|out|clear> | <leave|arrive> [message]\n");
    return 0;
  }

  msg = rest_of(args, 4);
  error = area->edit_vacancy_hour(args[1], hour, args[3],
                                          strlen(msg) ? msg : nil);
  if (error)
  {
    notify_fail(error);
    return 0;
  }

  if (args[3] == "clear")
    write("Job '" + args[1] + "' no longer does anything at " + hour + ".\n");
  else if (args[3] == "leave" || args[3] == "arrive")
    write("Job '" + args[1] + "' at " + hour + ": " +
          (strlen(msg) ? args[3] + " line set." : "no " + args[3] + " line.") +
          "\n");
  else
    write("Job '" + args[1] + "' goes to " + args[3] + " at " + hour + ".\n");
  return 1;
}

// ===== vacancies home =====
private int do_home(object area, object loc, string * args)
{
  // the house that comes with the job: stand in a plot or a house and bind it
  string file;

  if (sizeof(args) < 2)
  {
    notify_fail("Usage: vacancies home <name> [none]\n");
    return 0;
  }

  if (!area->query_vacancy(args[1]))
  {
    notify_fail("No vacancy '" + args[1] + "' in this area.\n");
    return 0;
  }

  // a job that needs no particular roof: its people are housed like anyone
  if (sizeof(args) > 2 && args[2] == "none")
  {
    if (!area->clear_vacancy_home(args[1]))
    {
      notify_fail("The '" + args[1] + "' vacancy has no house of its own.\n");
      return 0;
    }
    write("The '" + args[1] + "' vacancy has no house of its own any more; " +
          "its holders left it, to be housed again by 'houses assign'.\n");
    return 1;
  }

  // a job's house is kept for it: set_vacancy_home moves its holders in
  file = loc->query_file_name();
  if (loc->has_component(LOCATION_COMPONENT_PLOT))
    area->build_house_at(file, ({ }), 1);
  else if (!loc->has_component(LOCATION_COMPONENT_HOME))
  {
    notify_fail("Stand in a plot or a house to make it the '" + args[1] +
                "' vacancy's home.\n");
    return 0;
  }

  area->set_vacancy_home(args[1], file);
  write("The '" + args[1] + "' vacancy's home is now " + file + ".\n");
  return 1;
}

// ===== vacancies resident =====
private int do_resident(object area, string * args)
{
  // whether the job's people are housed among the settlement's own. The
  // opposite of `home`, which pins every holder to the one house it names: a
  // barman sleeps over his bar, farmers each want a roof of their own.
  int flag;

  if (sizeof(args) < 2)
  {
    notify_fail("Usage: vacancies resident <name> [none]\n");
    return 0;
  }

  flag = !(sizeof(args) > 2 && args[2] == "none");

  if (!area->set_vacancy_resident(args[1], flag))
  {
    notify_fail("No vacancy '" + args[1] + "' in this area.\n");
    return 0;
  }

  write("The '" + args[1] + "' vacancy's holders " +
        (flag ? "are housed with the rest of the town"
              : "are no longer housed by the town") + ".\n");
  return 1;
}

// ===== vacancies spot =====
private int do_spot(object area, object loc, string * args)
{
  if (sizeof(args) < 2)
  {
    notify_fail("Usage: vacancies spot <name> [remove]\n");
    return 0;
  }

  if (!area->query_vacancy(args[1]))
  {
    notify_fail("No vacancy '" + args[1] + "' here.\n");
    return 0;
  }

  if (sizeof(args) > 2 && args[2] == "remove")
    write(area->remove_vacancy_spot(args[1], loc->query_file_name())
            ? "Taken off the places '" + args[1] + "' is worked.\n"
            : "'" + args[1] + "' was not worked here.\n");
  else
    write(area->add_vacancy_spot(args[1], loc->query_file_name())
            ? "'" + args[1] + "' is worked here too.\n"
            : "'" + args[1] + "' was already worked here.\n");
  return 1;
}

// ===== vacancies rename =====
private int do_rename(object area, string * args)
{
  // a name is the job's identity, so two jobs never share one: this settles
  // a clash before two communities join
  if (sizeof(args) < 3)
  {
    notify_fail("Usage: vacancies rename <name> <new name>\n");
    return 0;
  }

  if (!area->query_vacancy(args[1]))
  {
    notify_fail("No vacancy '" + args[1] + "' in this area.\n");
    return 0;
  }

  if (reserved_name(args[2]))
  {
    notify_fail("'" + args[2] + "' is a subcommand of vacancies: a job needs " +
                "another name.\n");
    return 0;
  }

  if (!area->rename_vacancy(args[1], args[2]))
  {
    notify_fail("There is a vacancy '" + args[2] + "' already.\n");
    return 0;
  }

  write("The '" + args[1] + "' vacancy, and whoever holds it, is now '" +
        args[2] + "'.\n");
  return 1;
}

// ===== vacancies reseat =====
private int do_reseat(object area, string * args)
{
  // hand each holder of a spread job the place it would get if the job were
  // filled today: a job filled before its workplaces were recognisable put
  // everybody on its anchor and left them there
  int moved;

  if (sizeof(args) < 2)
  {
    notify_fail("Usage: vacancies reseat <name>\n");
    return 0;
  }

  if (!area->query_vacancy(args[1]))
  {
    notify_fail("No vacancy '" + args[1] + "' in this area.\n");
    return 0;
  }

  moved = area->reseat_vacancy(args[1]);
  write(moved ? "Reseated " + moved + " holder" + (moved == 1 ? "" : "s") +
                " of '" + args[1] + "'.\n"
              : "Nobody of '" + args[1] + "' moved: the job does not spread, " +
                "or everybody already sits where it would put them.\n");
  return 1;
}

static int cmd(string str, object me, string verb)
{
  object loc, area;
  string * args;

  args = words(str);

  loc = location_of(me);
  area = loc ? loc->query_area() : nil;
  if (!area)
  {
    notify_fail("Stand in a location (not a plain room) to work on the " +
                "jobs it offers.\n");
    return 0;
  }

  if (!sizeof(args))
    return do_list(area, 0);

  // the subcommands that edit; any of them alone is a call without its name
  switch (args[0])
  {
    case "add":       return do_add(area, loc, args);
    case "equip":     return do_equip(area, args);
    case "timetable": return do_timetable(area, args);
    case "class":     return do_class(area, args);
    case "home":      return do_home(area, loc, args);
    case "resident":  return do_resident(area, args);
    case "spot":      return do_spot(area, loc, args);
    case "reseat":    return do_reseat(area, args);
    case "rename":    return do_rename(area, args);
    case "remove":    return do_remove(area, args);
    case "list":
    case "spots":
      notify_fail("'vacancies' lists the jobs and 'vacancies <job>' shows " +
                  "one with its places.\n");
      return 0;
  }

  if (sizeof(args) > 1)
  {
    notify_fail("Usage: vacancies [ <job> | free | holders | types ]\n" +
                "       (see help vacancies for the subcommands)\n");
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
