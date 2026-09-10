
// Report on the points of interest of an area: the places the world cares
// about, what marks each one, and what is worked there. Reading only -- the
// builder ring is what attaches and edits a POI.

#include <mud/cmd.h>
#include <areas/area.h>
#include <areas/poi.h>
#include <areas/vacancy.h>
#include <room/location.h>

inherit CMD_BASE;

private object area_of(object me);
private string columns(string * * rows);
private string books_of(object area);
private string leaf(string path);
private string label_of(mapping poi);
private string jobs_at(object area, string file);
private int do_list(object area, object me, string kind);
private int do_here(object area, object me);
private int do_place(object area, object me, string file);

void setup()
{
  set_aliases(({ "pois" }));
  set_usage("pois [ <kind> | here ]");
  set_help(
    "Report on the points of interest of the area you are standing in.\n" +
    "\n" +
    "  pois           every one it holds, one line each\n" +
    "  pois <kind>    only the ones of that kind\n" +
    "  pois here      everything about the one under your feet\n" +
    "\n" +
    "Kinds: pub, shop, town_entrance, town_square, crossroads, shrine. A " +
    "location carries at most one, and ventures (pub, shop) get theirs from " +
    "the conversion; the rest are attached by hand.\n" +
    "\n" +
    "A POI marks a place, never a person: it holds no job of its own. What " +
    "is worked there is a vacancy that happens to name it as its workplace, " +
    "which is why a POI can have nobody at it and a job can be held where " +
    "the world has no particular interest.");
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

private string books_of(object area)
{
  return "'" + (string)area->query_area_name() + "'";
}

private string leaf(string path)
{
  if (!stringp(path) || !strlen(path))
    return "-";
  return get_path_file_name(path);
}

// A POI's label, with the colour codes taken out: a table lines its columns up
// by what it can measure, and a coded label measures wider than it prints.
private string label_of(mapping poi)
{
  if (!poi[POI_FIELD_LABEL])
    return "-";

  return "\"" +
         (string)handler("terminal")->clean_string(poi[POI_FIELD_LABEL]) +
         "\"";
}

// The jobs worked at a place, named. A POI holds none of its own: this is the
// other side of the link, read off the community's vacancies.
private string jobs_at(object area, string file)
{
  mapping * jobs;
  string out;
  int i;

  jobs = (mapping *)area->query_vacancies();
  out = "";

  for (i = 0; i < sizeof(jobs); i++)
  {
    int at_spot;

    at_spot = pointerp(jobs[i][VACANCY_SPOTS]) &&
              member_array(file, jobs[i][VACANCY_SPOTS]) >= 0;

    if (jobs[i][VACANCY_WORKS_AT] != file && !at_spot)
      continue;

    out += (strlen(out) ? ", " : "") + jobs[i][VACANCY_JOB] +
           " (" + sizeof((string *)area->query_vacancy_holders(jobs[i])) +
           "/" + jobs[i][VACANCY_COUNT] + ")";
  }

  return strlen(out) ? out : "-";
}

// ===== pois / pois <kind> =====
private int do_list(object area, object me, string kind)
{
  mapping pois;
  string * files, * * rows;
  string here;
  int i, shown;

  pois = (mapping)area->query_pois();
  files = map_indices(pois);
  if (!sizeof(files))
  {
    write("No points of interest in " + books_of(area) + ".\n");
    return 1;
  }

  here = environment(me) ? (string)environment(me)->query_file_name() : "";
  files = sort_array(files);
  rows = ({ ({ "place", "kind", "label", "guards", "worked by", "" }) });

  for (i = 0; i < sizeof(files); i++)
  {
    mapping poi;

    poi = pois[files[i]];
    if (kind && poi[POI_FIELD_KIND] != kind)
      continue;

    shown++;
    rows += ({ ({
      leaf(files[i]),
      poi[POI_FIELD_KIND],
      label_of(poi),
      poi[POI_FIELD_GUARD_DIR] ? poi[POI_FIELD_GUARD_DIR] : "-",
      jobs_at(area, files[i]),
      (files[i] == here ? "<- here"
                        : (area->query_loaded_location(files[i])
                             ? "loaded" : ""))
    }) });
  }

  if (!shown)
  {
    write("No '" + kind + "' point of interest in " + books_of(area) +
          ".\n");
    return 1;
  }

  write("Points of interest of " + books_of(area) + ":\n" + columns(rows) +
        "\nAsk 'pois here' for everything about the one you stand on.\n");
  return 1;
}

// ===== pois here =====
private int do_here(object area, object me)
{
  object env;

  env = environment(me);
  if (!env)
  {
    notify_fail("You are nowhere.\n");
    return 0;
  }

  return do_place(area, me, (string)env->query_file_name());
}

// Everything one POI carries, plus what its location says about itself.
private int do_place(object area, object me, string file)
{
  mapping poi;
  object loc, temple;
  string out;
  object * comps;
  int * coord;
  int i;

  poi = (mapping)area->query_poi(file);
  if (!poi)
  {
    notify_fail("This location is not a point of interest. The builder ring " +
                "attaches one: 'build poi add <kind> [label]'.\n");
    return 0;
  }

  loc = (object)area->query_loaded_location(file);

  out = "Point of interest at " + leaf(file) + " of " + books_of(area) + "\n";
  out += "  kind       " + poi[POI_FIELD_KIND] + "\n";
  out += "  label      " + label_of(poi) + "\n";

  if (poi[POI_FIELD_KIND] == POI_KIND_TOWN_ENTRANCE)
    out += "  guards     " +
           (poi[POI_FIELD_GUARD_DIR]
              ? "watch '" + poi[POI_FIELD_GUARD_DIR] + "', the way in"
              : "no direction set: nobody is stopped here") + "\n";

  out += "  worked by  " + jobs_at(area, file) + "\n";

  if (loc)
  {
    string comp_names;

    out += "  loaded     yes, " + object_name(loc) + "\n";
    coord = (int *)loc->query_coordinates();
    if (pointerp(coord) && sizeof(coord) == 3)
      out += "  at         (" + coord[0] + "," + coord[1] + "," + coord[2] +
             ")\n";

    comps = (object *)loc->query_components();
    comp_names = "";
    for (i = 0; i < sizeof(comps); i++)
      if (comps[i])
        comp_names += (strlen(comp_names) ? ", " : "") +
                      (string)comps[i]->query_type();
    out += "  components " + (strlen(comp_names) ? comp_names : "-") + "\n";

    temple = (object)loc->query_component_by_type("temple");
    if (temple)
    {
      string deity;

      deity = (string)temple->query_deity();
      out += "  deity      " +
             (strlen(deity) ? deity : "unconsecrated") + "\n";
    }
  }
  else
    out += "  loaded     no  (nobody is there right now)\n";

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
    notify_fail("Stand in a location (not a plain room) to report on its " +
                "points of interest.\n");
    return 0;
  }

  if (!sizeof(args))
    return do_list(area, me, nil);
  if (sizeof(args) > 1)
  {
    notify_fail("Usage: pois [ <kind> | here ]\n");
    return 0;
  }

  if (args[0] == "here")
    return do_here(area, me);

  if (member_array(args[0], POI_KINDS) < 0)
  {
    notify_fail("Kinds are: " + implode(POI_KINDS, ", ") + ". 'pois here' " +
                "reports the one you are standing on.\n");
    return 0;
  }

  return do_list(area, me, args[0]);
}
