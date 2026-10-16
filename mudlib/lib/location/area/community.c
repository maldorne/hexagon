// community.c -- moving a community's books when an area joins or leaves one.
//
// An area with a parent keeps none of the community on its own books -- people,
// jobs, houses, building land, the provenance its caps are summed from -- its
// root does (see query_root_area). Setting or clearing a parent therefore moves
// those books from one root to another. plan_parent_change says what would stop
// it and what it would leave behind; change_parent carries it out, and refuses
// while anything stops it. Nobody is created or retired here: everybody who
// existed before exists after, on the books of the community they now belong to.
//
// Leaving a parent splits one community in two. Which side each thing goes to:
//   - a job: where it is held
//   - a person: their job's side; with no job, their house's; with neither,
//     where they stand
//   - a house: the area it is booked with (HOUSE_BOOKS), or where it stands
//   - a plot, a location's provenance: where it stands
// The schedule index, the families and the fauna are kept per location or game
// wide, so they do not move; the roamer index and the hours are re-pointed for
// whoever ends up standing on the other side.

#include <room/location.h>
#include <areas/area.h>
#include <areas/vacancy.h>
#include <living/persisted.h>

#define SIDE_CHILD "child"
#define SIDE_ROOT  "root"

#define PLAN_LINK  "link"
#define PLAN_ADOPT "adopt"
#define PLAN_LEAVE "leave"
#define PLAN_NONE  "none"

// location file -> side, for the length of one plan
static mapping side_cache;

void create()
{
  side_cache = ([ ]);
}

// Whether `area` is the area at `top` or lies under it.
private int _within(object area, string top)
{
  string path;
  int steps;

  if (!area)
    return 0;

  path = area->query_area_path();
  for (steps = 0; steps <= AREA_MAX_ANCESTRY && path && strlen(path); steps++)
  {
    object up;

    if (path == top)
      return 1;
    up = AREA_HANDLER->query_area(path);
    if (!up)
      return 0;
    path = up->query_parent_area_path();
  }

  return 0;
}

// The side of a split a location falls on: the child's when its area lies
// under `top`, the root's otherwise (and for no location at all).
private string _side_of(string file, string top)
{
  object owner;

  if (!file || !strlen(file))
    return SIDE_ROOT;
  if (!undefinedp(side_cache[file]))
    return side_cache[file];

  owner = load_object(LOCATION_HANDLER)->query_area_from_location_file_name(file);
  side_cache[file] = _within(owner, top) ? SIDE_CHILD : SIDE_ROOT;
  return side_cache[file];
}

// The location a house key names: a household of a shared house is its file
// plus a number.
private string _house_base(string key)
{
  string file;

  if (sscanf(key, "%s" + HOUSE_UNIT_MARK + "%*s", file) == 2)
    return file;
  return key;
}

// How a person is named in a report.
private string _who(string uuid, mapping row)
{
  return (row && stringp(row["name"]) && strlen(row["name"]))
           ? capitalize(row["name"]) + " (" + uuid + ")" : uuid;
}

// The caps a set of books would give: each type's add_clone count summed over
// the provenance, leaving out the types its jobs are filled from, as
// rebuild_npc_caps does.
private mapping _caps_of(mapping provenance, mapping * jobs)
{
  mapping counts, excluded, t;
  string * files, * paths, game;
  int i, j;

  game = game_from_path((string)this_object()->query_area_path());

  excluded = ([ ]);
  for (i = 0; i < sizeof(jobs); i++)
  {
    if (stringp(jobs[i][VACANCY_SOURCE]))
      excluded[jobs[i][VACANCY_SOURCE]] = 1;
    if (pointerp(jobs[i][VACANCY_SOURCES]))
      for (j = 0; j < sizeof(jobs[i][VACANCY_SOURCES]); j++)
        excluded[jobs[i][VACANCY_SOURCES][j][0]] = 1;
  }
  paths = map_indices(excluded);
  for (i = 0; i < sizeof(paths); i++)
  {
    t = BESTIARY_HANDLER->query_template(game, paths[i]);
    if (t && stringp(t["extracted_from"]))
      excluded[(string)this_object()->query_template_from_source(
                 t["extracted_from"])] = 1;
  }

  counts = ([ ]);
  files = map_indices(provenance);
  for (i = 0; i < sizeof(files); i++)
  {
    paths = map_indices(provenance[files[i]]);
    for (j = 0; j < sizeof(paths); j++)
    {
      string tid;

      tid = (string)this_object()->query_template_from_source(paths[j]);
      if (excluded[tid])
        continue;
      counts[tid] = (counts[tid] ? counts[tid] : 0) +
                    provenance[files[i]][paths[j]];
    }
  }

  return counts;
}

// A warning for each type the people of `census` outnumber its cap in, holders
// of a job aside. The sweep and the staffing only ever add, so the excess stays
// until enough of them die.
private string * _excess(mapping census, mapping caps, string where)
{
  mapping count;
  string * ids, * types, * out;
  int i;

  count = ([ ]);
  ids = map_indices(census);
  for (i = 0; i < sizeof(ids); i++)
  {
    mixed tid;

    if (census[ids[i]][CENSUS_VACANCY])
      continue;
    tid = census[ids[i]]["source"];
    if (stringp(tid))
      count[tid] = (count[tid] ? count[tid] : 0) + 1;
  }

  out = ({ });
  types = map_indices(count);
  for (i = 0; i < sizeof(types); i++)
    if (!undefinedp(caps[types[i]]) && count[types[i]] > caps[types[i]])
      out += ({ where + " would hold " + count[types[i]] + " of '" +
                get_path_file_name(types[i]) + "' and its cap is " +
                caps[types[i]] + ": nobody of that type is added again until " +
                "enough of them die." });

  return out;
}

// What this area keeps on its own books, as a phrase, or "" when nothing.
private string _own_books()
{
  string * parts;

  parts = ({ });
  if (map_sizeof((mapping)this_object()->query_own_census()))
    parts += ({ map_sizeof((mapping)this_object()->query_own_census()) +
                " people" });
  if (sizeof((mapping *)this_object()->query_own_vacancies()))
    parts += ({ sizeof((mapping *)this_object()->query_own_vacancies()) +
                " jobs" });
  if (map_sizeof((mapping)this_object()->query_own_houses()))
    parts += ({ map_sizeof((mapping)this_object()->query_own_houses()) +
                " houses" });
  if (sizeof((string *)this_object()->query_own_plots()))
    parts += ({ sizeof((string *)this_object()->query_own_plots()) +
                " plots" });

  return implode(parts, ", ");
}

// Joining this area's books to `root`'s: this area being a root itself (a
// link) or a delegated one still holding books nobody reads (an adoption).
private void _plan_join(object root, mapping plan)
{
  mapping * mine, * theirs;
  mapping provenance;
  int i;

  mine = (mapping *)this_object()->query_own_vacancies();
  for (i = 0; i < sizeof(mine); i++)
    if (root->query_vacancy(mine[i][VACANCY_JOB]))
      plan["conflicts"] += ({
        "The job '" + mine[i][VACANCY_JOB] + "' is offered on both sides. " +
        "Rename one of them first, standing in an area of that side: " +
        "vacancies rename " + mine[i][VACANCY_JOB] + " <new name>" });

  theirs = (mapping *)root->query_own_vacancies();
  provenance = ([ ]) + (mapping)root->query_own_npc_sources() +
               (mapping)this_object()->query_own_npc_sources();

  plan["warnings"] += _excess(([ ]) + (mapping)root->query_own_census() +
                              (mapping)this_object()->query_own_census(),
                              _caps_of(provenance, theirs + mine),
                              "'" + root->query_area_name() + "'");
}

// This area leaving its community: every row, job, house, plot and
// provenance entry sorted to a side, with the conflicts and the warnings.
private void _plan_leave(object root, mapping plan)
{
  mapping census, houses, provenance, job_side, house_side, house_of, person;
  mapping split_census, split_provenance;
  mapping * jobs, * child_jobs, * root_jobs;
  mixed citizenship;
  string * keys, top, root_name, child_name;
  int i, j;

  top = (string)this_object()->query_area_path();
  root_name = "'" + root->query_area_name() + "'";
  child_name = "'" + this_object()->query_area_name() + "'";
  census = (mapping)root->query_own_census();
  jobs = (mapping *)root->query_own_vacancies();
  houses = (mapping)root->query_own_houses();
  provenance = (mapping)root->query_own_npc_sources();

  job_side = ([ ]);
  for (i = 0; i < sizeof(jobs); i++)
    job_side[jobs[i][VACANCY_JOB]] = _side_of(jobs[i][VACANCY_WORKS_AT], top);

  // each house's side, and who lives in which
  house_side = ([ ]);
  house_of = ([ ]);
  keys = map_indices(houses);
  for (i = 0; i < sizeof(keys); i++)
  {
    mixed books, living;

    books = houses[_house_base(keys[i])]
              ? houses[_house_base(keys[i])][HOUSE_BOOKS] : nil;
    if (stringp(books) && strlen(books))
      house_side[keys[i]] = _within(AREA_HANDLER->query_area(books), top)
                              ? SIDE_CHILD : SIDE_ROOT;
    else
      house_side[keys[i]] = _side_of(_house_base(keys[i]), top);

    living = houses[keys[i]][HOUSE_RESIDENTS];
    for (j = 0; pointerp(living) && j < sizeof(living); j++)
      house_of[living[j]] = keys[i];
  }

  person = ([ ]);
  keys = map_indices(census);
  for (i = 0; i < sizeof(keys); i++)
  {
    mapping row;

    row = census[keys[i]];
    if (row[CENSUS_VACANCY] && !undefinedp(job_side[row[CENSUS_VACANCY]]))
      person[keys[i]] = job_side[row[CENSUS_VACANCY]];
    else if (house_of[keys[i]])
      person[keys[i]] = house_side[house_of[keys[i]]];
    else
      person[keys[i]] = _side_of(row[CENSUS_LOCATION], top);
  }

  // a holder whose house would end up on the other side of their job
  for (i = 0; i < sizeof(keys); i++)
  {
    string house, base, target;
    string * units;
    mixed living;
    int movable, k;

    house = house_of[keys[i]];
    if (!house || !census[keys[i]][CENSUS_VACANCY] ||
        house_side[house] == person[keys[i]])
      continue;

    // the house can follow them if nobody else in it holds a job on the side
    // it stands on now
    target = person[keys[i]];
    base = _house_base(house);
    movable = 1;
    units = (string *)root->query_house_units(base);
    for (j = 0; movable && j < sizeof(units); j++)
    {
      living = houses[units[j]] ? houses[units[j]][HOUSE_RESIDENTS] : nil;
      for (k = 0; pointerp(living) && k < sizeof(living); k++)
        if (census[living[k]] && census[living[k]][CENSUS_VACANCY] &&
            person[living[k]] != target)
          movable = 0;
    }

    plan["conflicts"] += ({
      _who(keys[i], census[keys[i]]) + " works as '" +
      census[keys[i]][CENSUS_VACANCY] + "' on the " +
      (target == SIDE_CHILD ? child_name : root_name) + " side but lives in " +
      base + ", on the other." +
      (movable
        ? " To book the house with their side: houses books " +
          (target == SIDE_CHILD ? top : (string)root->query_area_path()) +
          " " + base + "."
        : " The house cannot follow them: others in it work on its side.") +
      " To have them housed again on their side: npcs rehouse " + keys[i] +
      "." });
  }

  // a job's own house on the other side of the job
  for (i = 0; i < sizeof(jobs); i++)
  {
    mixed home;

    home = jobs[i][VACANCY_HOME];
    if (!stringp(home) || !strlen(home) || undefinedp(house_side[home]) ||
        house_side[home] == job_side[jobs[i][VACANCY_JOB]])
      continue;

    plan["conflicts"] += ({
      "The job '" + jobs[i][VACANCY_JOB] + "' has its house " + home +
      " on the other side. Book the house with the job's side: houses " +
      "books " + (job_side[jobs[i][VACANCY_JOB]] == SIDE_CHILD
                    ? top : (string)root->query_area_path()) +
      " " + home + "." });
  }

  // what the child stops inheriting from its parent
  citizenship = this_object()->query_citizenship();
  if (!(stringp(citizenship) && strlen(citizenship)) &&
      !this_object()->query_stateless() &&
      stringp(this_object()->query_effective_citizenship()))
    plan["warnings"] += ({
      "It takes its citizenship ('" +
      this_object()->query_effective_citizenship() + "') from its parent and " +
      "will have none: areas diplomacy " +
      this_object()->query_effective_citizenship() + " keeps it." });
  if (!this_object()->query_own_area_level() &&
      this_object()->query_area_level())
    plan["warnings"] += ({
      "It takes its NPC level (" + this_object()->query_area_level() +
      ") from its parent: areas level " +
      this_object()->query_area_level() + " keeps it." });

  // the excess on each side, with everything split the same way
  split_census = ([ SIDE_CHILD: ([ ]), SIDE_ROOT: ([ ]) ]);
  for (i = 0; i < sizeof(keys); i++)
    split_census[person[keys[i]]][keys[i]] = census[keys[i]];
  child_jobs = ({ });
  root_jobs = ({ });
  for (i = 0; i < sizeof(jobs); i++)
    if (job_side[jobs[i][VACANCY_JOB]] == SIDE_CHILD)
      child_jobs += ({ jobs[i] });
    else
      root_jobs += ({ jobs[i] });
  split_provenance = ([ SIDE_CHILD: ([ ]), SIDE_ROOT: ([ ]) ]);
  keys = map_indices(provenance);
  for (i = 0; i < sizeof(keys); i++)
    split_provenance[_side_of(keys[i], top)][keys[i]] = provenance[keys[i]];

  plan["warnings"] +=
    _excess(split_census[SIDE_CHILD],
            _caps_of(split_provenance[SIDE_CHILD], child_jobs),
            child_name) +
    _excess(split_census[SIDE_ROOT],
            _caps_of(split_provenance[SIDE_ROOT], root_jobs), root_name);

  plan["job_side"] = job_side;
  plan["house_side"] = house_side;
  plan["person"] = person;
}

/**
 * What setting this area's parent to `path` would do, or clearing it with ""
 * or nil, without doing it: ([ "mode": link | adopt | leave | none,
 * "conflicts": ({ what stops it, with the command that settles it }),
 * "warnings": ({ what it would leave behind }), ... ]). Naming the parent it
 * already has adopts whatever its own books still hold into its community.
 */
mapping plan_parent_change(string path)
{
  mapping plan;
  object target, root;
  string current, books;

  side_cache = ([ ]);
  plan = ([ "conflicts": ({ }), "warnings": ({ }), "mode": PLAN_NONE ]);
  current = (string)this_object()->query_parent_area_path();
  books = _own_books();

  if (path && strlen(path))
  {
    target = AREA_HANDLER->query_area(path);
    if (!target)
    {
      plan["conflicts"] += ({ "No area is known at '" + path + "'." });
      return plan;
    }

    if (current && strlen(current) && current != path)
    {
      plan["conflicts"] += ({ "It is part of '" + current + "' already. " +
                              "Take it out first: areas parent none" });
      return plan;
    }

    if (current == path)
    {
      root = (object)this_object()->query_root_area();
      if (root == this_object() || !strlen(books))
        return plan;
      plan["mode"] = PLAN_ADOPT;
      plan["warnings"] += ({ "Its own books still hold " + books +
                             " nobody reads; they join its community's." });
    }
    else
    {
      if (target == this_object() ||
          _within(target, (string)this_object()->query_area_path()))
      {
        plan["conflicts"] += ({ "An area cannot be its own parent, nor be " +
                                "placed under one of its own descendants." });
        return plan;
      }
      root = (object)target->query_root_area();
      plan["mode"] = PLAN_LINK;
    }

    plan["root"] = root;
    _plan_join(root, plan);
    return plan;
  }

  if (!current || !strlen(current))
  {
    plan["conflicts"] += ({ "It has no parent." });
    return plan;
  }

  root = (object)this_object()->query_root_area();
  plan["root"] = root;
  plan["mode"] = PLAN_LEAVE;

  if (strlen(books))
  {
    plan["conflicts"] += ({ "Its own books still hold " + books + " nobody " +
                            "reads. Join them to its community first: " +
                            "areas parent " + current });
    return plan;
  }

  // a parent that no longer loads leaves nothing to split
  if (root != this_object())
    _plan_leave(root, plan);

  return plan;
}

// Move this area's own books into `root`'s, leaving its own empty.
private void _join(object root)
{
  mapping theirs, mine, caps, own_caps;
  string * keys;
  int i;

  theirs = (mapping)root->query_own_census();
  mine = (mapping)this_object()->query_own_census();
  keys = map_indices(mine);
  for (i = 0; i < sizeof(keys); i++)
    theirs[keys[i]] = mine[keys[i]];

  root->set_own_vacancies((mapping *)root->query_own_vacancies() +
                          (mapping *)this_object()->query_own_vacancies());

  theirs = (mapping)root->query_own_houses();
  mine = (mapping)this_object()->query_own_houses();
  keys = map_indices(mine);
  for (i = 0; i < sizeof(keys); i++)
    theirs[keys[i]] = mine[keys[i]];

  root->set_own_plots((string *)root->query_own_plots() |
                      (string *)this_object()->query_own_plots());

  theirs = (mapping)root->query_own_npc_sources();
  mine = (mapping)this_object()->query_own_npc_sources();
  keys = map_indices(mine);
  for (i = 0; i < sizeof(keys); i++)
    theirs[keys[i]] = mine[keys[i]];

  // the design-time flags on the caps travel; the counts are derived again
  caps = (mapping)root->query_own_npc_caps();
  own_caps = (mapping)this_object()->query_own_npc_caps();
  keys = map_indices(own_caps);
  for (i = 0; i < sizeof(keys); i++)
    if (own_caps[keys[i]]["resident"])
    {
      if (!caps[keys[i]])
        caps[keys[i]] = ([ "max": 0 ]);
      caps[keys[i]]["resident"] = 1;
    }

  this_object()->set_own_census(([ ]));
  this_object()->set_own_vacancies(({ }));
  this_object()->set_own_houses(([ ]));
  this_object()->set_own_plots(({ }));
  this_object()->set_own_npc_sources(([ ]));
  this_object()->set_own_npc_caps(([ ]));
}

// Point the roamer index, the body and the hours of somebody who has just
// changed sides at where they now belong. `home` is the root of their side.
private void _reposition(string uuid, mapping row, string side, object home,
                         object root, string top)
{
  object target, npc, here;
  object * candidates;
  mixed roster, loc;
  int * hours, * found;
  int i;

  loc = row[CENSUS_LOCATION];
  here = (stringp(loc) && strlen(loc))
           ? load_object(LOCATION_HANDLER)->query_area_from_location_file_name(loc)
           : nil;
  roster = AREA_HANDLER->query_foreign_roster(uuid);

  // standing on their own side, the location's area places them from the
  // census; on the other, they are a roamer of their side's root
  if (here && _side_of(loc, top) == side)
  {
    target = here;
    AREA_HANDLER->set_foreign_position(uuid, nil, nil);
  }
  else
  {
    target = home;
    if (stringp(loc) && strlen(loc))
      AREA_HANDLER->set_foreign_position(uuid,
                                         (string)home->query_area_path(), loc);
  }

  npc = find_living(uuid);
  if (npc)
    npc->set_npc_area_path((string)target->query_area_path());

  // the hours go to the area that will place them; one that indexed them
  // elsewhere would find nobody on its books when the hour came
  candidates = ({ root, this_object() });
  if (here)
    candidates |= ({ here });
  if (stringp(roster) && AREA_HANDLER->query_area(roster))
    candidates |= ({ AREA_HANDLER->query_area(roster) });
  candidates -= ({ target });

  hours = ({ });
  for (i = 0; i < sizeof(candidates); i++)
  {
    found = (int *)candidates[i]->query_schedule_hours_of(uuid);
    if (!sizeof(found))
      continue;
    hours |= found;
    candidates[i]->index_schedule_hours(uuid, ({ }));
  }

  if (sizeof(hours))
    target->index_schedule_hours(uuid,
      (int *)target->query_schedule_hours_of(uuid) | hours);
}

// Split `root`'s books with this area as `plan` sorted them, and stand alone.
private void _leave(object root, mapping plan)
{
  mapping census, houses, provenance, person, house_side, job_side;
  mapping child_census, child_houses, child_provenance, caps, child_caps;
  mapping * jobs, * child_jobs, * root_jobs;
  string * keys, * plots, * child_plots, top;
  int i;

  top = (string)this_object()->query_area_path();
  person = plan["person"];
  house_side = plan["house_side"];
  job_side = plan["job_side"];

  census = (mapping)root->query_own_census();
  child_census = ([ ]);
  keys = map_indices(census);
  for (i = 0; i < sizeof(keys); i++)
    if (person[keys[i]] == SIDE_CHILD)
    {
      child_census[keys[i]] = census[keys[i]];
      map_delete(census, keys[i]);
    }

  jobs = (mapping *)root->query_own_vacancies();
  child_jobs = ({ });
  root_jobs = ({ });
  for (i = 0; i < sizeof(jobs); i++)
    if (job_side[jobs[i][VACANCY_JOB]] == SIDE_CHILD)
      child_jobs += ({ jobs[i] });
    else
      root_jobs += ({ jobs[i] });
  root->set_own_vacancies(root_jobs);

  houses = (mapping)root->query_own_houses();
  child_houses = ([ ]);
  keys = map_indices(houses);
  for (i = 0; i < sizeof(keys); i++)
    if (house_side[keys[i]] == SIDE_CHILD)
    {
      child_houses[keys[i]] = houses[keys[i]];
      map_delete(houses, keys[i]);
    }

  plots = (string *)root->query_own_plots();
  child_plots = ({ });
  for (i = 0; i < sizeof(plots); i++)
    if (_side_of(plots[i], top) == SIDE_CHILD)
      child_plots += ({ plots[i] });
  root->set_own_plots(plots - child_plots);

  provenance = (mapping)root->query_own_npc_sources();
  child_provenance = ([ ]);
  keys = map_indices(provenance);
  for (i = 0; i < sizeof(keys); i++)
    if (_side_of(keys[i], top) == SIDE_CHILD)
    {
      child_provenance[keys[i]] = provenance[keys[i]];
      map_delete(provenance, keys[i]);
    }

  // both sides keep the design-time flags; the counts are derived again
  caps = (mapping)root->query_own_npc_caps();
  child_caps = ([ ]);
  keys = map_indices(caps);
  for (i = 0; i < sizeof(keys); i++)
    if (caps[keys[i]]["resident"])
      child_caps[keys[i]] = ([ "max": 0, "resident": 1 ]);

  this_object()->set_own_census(child_census);
  this_object()->set_own_vacancies(child_jobs);
  this_object()->set_own_houses(child_houses);
  this_object()->set_own_plots(child_plots);
  this_object()->set_own_npc_sources(child_provenance);
  this_object()->set_own_npc_caps(child_caps);
  this_object()->set_parent_link("");

  keys = map_indices(child_census);
  for (i = 0; i < sizeof(keys); i++)
    _reposition(keys[i], child_census[keys[i]], SIDE_CHILD, this_object(),
                root, top);
  keys = map_indices(census);
  for (i = 0; i < sizeof(keys); i++)
    _reposition(keys[i], census[keys[i]], SIDE_ROOT, root, root, top);
}

/**
 * Sets this area's parent to `path`, or clears it with "" or nil, moving the
 * community's books to the right root: the plan of plan_parent_change, carried
 * out only when it has no conflicts. Whoever was taken out of their house to
 * settle one is housed again on their side.
 *
 * @return  the plan, with "done" set when it was carried out
 */
mapping change_parent(string path)
{
  mapping plan;
  object root;

  plan = plan_parent_change(path);
  if (sizeof(plan["conflicts"]))
    return plan;

  root = plan["root"];

  switch (plan["mode"])
  {
    case PLAN_LINK:
      _join(root);
      this_object()->set_parent_link(path);
      break;
    case PLAN_ADOPT:
      _join(root);
      break;
    case PLAN_LEAVE:
      if (root && root != this_object())
        _leave(root, plan);
      else
        this_object()->set_parent_link("");
      break;
    default:
      return plan;
  }

  if (root && root != this_object())
  {
    root->rebuild_npc_caps();
    root->save_me();
  }
  this_object()->rebuild_npc_caps();
  this_object()->save_me();

  if (plan["mode"] == PLAN_LEAVE)
  {
    if (root && root != this_object())
      root->assign_homes();
    this_object()->assign_homes();
  }

  plan["done"] = 1;
  return plan;
}
