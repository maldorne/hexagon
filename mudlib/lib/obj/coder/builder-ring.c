/*
 * One ring to build them all
 *
 * neverbot 06/06/24
 */

inherit "/lib/armour.c";

#include <room/location.h>
#include <areas/area.h>
#include <areas/poi.h>
#include <maps/maps.h>
#include <translations/armour.h>

#define COMPONENTS_DIR "/lib/location/components/"

#define BUILDER_RING_BUILD_VERB ({ "build" })
#define BUILDER_RING_OPTIONS ({ "selection", "convert", "component", "area", "poi", "npc" })
#define BUILDER_RING_SELECTION_SYNTAX "build selection < add | remove | list >"
#define BUILDER_RING_CONVERT_SYNTAX "build convert [< selection | filename | dirname | here >]"
#define BUILDER_RING_COMPONENT_SYNTAX "build component < add | remove > <type>"
#define BUILDER_RING_AREA_SYNTAX "build area < exploration <display name> | noexploration | level <n> [<spread>] | diplomacy <citizenship|none> >"
#define BUILDER_RING_POI_SYNTAX "build poi < add <kind> [label] | remove | list | guard_dir <dir> | vacancy <add <role> <source> | remove <role>> >"
#define BUILDER_RING_NPC_SYNTAX "build npc  (show this area's NPC roster, census and vacancies)"
#define BUILDER_RING_HELP "This ring can be used by coders to help them building areas.\n\n" + \
                "Available commands:\n" + \
                "\t" + BUILDER_RING_SELECTION_SYNTAX + "\n" + \
                "\t" + BUILDER_RING_CONVERT_SYNTAX + "\n" + \
                "\t" + BUILDER_RING_COMPONENT_SYNTAX + "\n" + \
                "\t" + BUILDER_RING_AREA_SYNTAX + "\n" + \
                "\t" + BUILDER_RING_POI_SYNTAX + "\n" + \
                "\t" + BUILDER_RING_NPC_SYNTAX

static string * selection;
static mapping objects;

void create()
{
  selection = ({ });
  objects = ([ ]);

  ::create();
  
  set_name("builder ring");
  set_short("Builder ring");
  add_alias(({ "builder", "ring" }));

  set_main_plural("Builder rings");
  add_plural(({ "builder rings", "rings" }));
  set_long("This ring can be used by coders to help them building areas.\n");

  set_base_armour(RING);

  reset_drop();
  set_weight(1);
}

string query_help(varargs string str) { return BUILDER_RING_HELP; }

void init()
{
  add_action("do_build", BUILDER_RING_BUILD_VERB);
  ::init();
}

int do_selection(string str);
int do_convert(string str);
int do_component(string str);
int do_area(string str);
int do_poi(string str);
int do_npc();

// Glob-style matcher for `*` (any sequence, including empty) and `?`
// (exactly one character). Recursive backtracking; pattern and string
// are short paths so depth is bounded.
static int _glob_match(string pattern, string str)
{
  int p, s, plen, slen;

  plen = strlen(pattern);
  slen = strlen(str);
  p = 0;
  s = 0;

  while (p < plen)
  {
    if (pattern[p] == '*')
    {
      while (p < plen && pattern[p] == '*')
        p++;
      if (p == plen)
        return 1;
      while (s <= slen)
      {
        if (_glob_match(pattern[p..], str[s..]))
          return 1;
        s++;
      }
      return 0;
    }

    if (s >= slen)
      return 0;

    if (pattern[p] != '?' && pattern[p] != str[s])
      return 0;

    p++;
    s++;
  }

  return s == slen;
}

// Pattern has wildcards if it contains `*` or `?`.
static int _has_wildcards(string pattern)
{
  int i, len;

  len = strlen(pattern);
  for (i = 0; i < len; i++)
    if (pattern[i] == '*' || pattern[i] == '?')
      return 1;

  return 0;
}

static int _filter_loadable(string file)
{
  object what;

  // if file ends in .c, it should be a room
  if (file[strlen(file) - 2..] == ".c")
  {
    catch(what = load_object(file));
    if (what && 
        what->query_room())
    {
      objects[file] = what;
      return 1;
    }

    return 0;
  }
  else if (file[strlen(file) - 2..] == ".o")
  {
    what = load_object(LOCATION_HANDLER)->load_location(file);
    if (what && what->query_location())
    {
      objects[file] = what;
      return 1;
    }

    return 0;
  }

  return 0;
}

static int _filter_dot_c(string file)
{
  return file[strlen(file) - 2..] == ".c";
}

// Resolve one selection token into the file paths it names. Accepts, in
// order: `here` (the room you stand in), `*` (everything in your current
// directory), a `*`/`?` glob (absolute, or relative to your directory), a
// literal file or directory, and a bare room name with no extension (the
// `.c` is tried automatically). Returns ({ }) when nothing matches.
static string * _resolve_selection_target(string target)
{
  string cur;
  string * res;

  if (target == "here")
    return environment(this_player()) ?
             ({ base_name(environment(this_player())) }) : ({ });

  cur = this_user()->query_role()->query_current_path();

  if (target == "*")
    return get_files(cur + "/*");

  if (_has_wildcards(target))
    return get_files(target[0] == '/' ? target : cur + "/" + target);

  // literal path or name: resolve_targets handles absolute paths, dirs and
  // names relative to the current directory. Retry with `.c` when the token
  // carries no extension so `a1` finds `a1.c`.
  res = load_object(LOCATION_HANDLER)->resolve_targets(target);
  if (sizeof(res))
    return res;

  if (strlen(target) < 2 || target[strlen(target) - 2..] != ".c")
    return load_object(LOCATION_HANDLER)->resolve_targets(target + ".c");

  return ({ });
}

string pretty_selection()
{
  string ret;
  int i;

  ret = "Current selection:\n";

  for (i = 0; i < sizeof(selection); i++)
    ret += "  - " + selection[i] +
           " (" + (objects[selection[i]] &&
                   objects[selection[i]]->query_location() ?
                   "location" : "room") + ")\n";

  return ret + "\n";
}

int do_build(string str)
{
  string * args;
  string verb;

  args = explode(str, " ");

  if (!query_in_use())
  {
    notify_fail("You must equip the ring before using it.\n");
    return 0;
  }

  if (!this_player()->query_coder())
  {
    notify_fail("You won't be able to handle the power of this ring.\n");
    return 0;
  }

  if (!str || !strlen(str) || sizeof(args) < 1)
  {
    notify_fail("Build what?\n");
    return 0;
  }

  verb = args[0];

  if (member_array(verb, BUILDER_RING_OPTIONS) == -1)
  {
    notify_fail("Unknown build command.\n\n" + BUILDER_RING_HELP + "\n");
    return 0;
  }

  // npc takes no argument -- it just reports the current area's population
  if (verb == "npc")
    return do_npc();

  if (sizeof(args) < 2)
  {
    notify_fail("Build " + verb + " what?\n");
    return 0;
  }

  // write(to_string(verb));
  // write(to_string(implode(args[1..], " ")));

  if (verb == "selection")
    return do_selection(implode(args[1..], " "));
  else if (verb == "convert")
    return do_convert(implode(args[1..], " "));
  else if (verb == "component")
    return do_component(implode(args[1..], " "));
  else if (verb == "area")
    return do_area(implode(args[1..], " "));
  else if (verb == "poi")
    return do_poi(implode(args[1..], " "));
  else
  {
    notify_fail("Unknown build command.\n\n" + BUILDER_RING_HELP + "\n");
    return 0;
  }

  return 1;
}

int do_selection(string str)
{
  string * args;
  string verb;
  string target;

  args = explode(str, " ");
  verb = args[0];

  if (sizeof(args) < 1)
  {
    notify_fail("What?\n");
    return 0;
  }

  if (verb == "add")
  {
    string * added;
    int i;

    if (sizeof(args) < 2)
    {
      notify_fail(capitalize(verb) + " what?\n");
      return 0;
    }

    // every remaining token is a target; each may name one file, a glob,
    // a directory or a bare room name (see _resolve_selection_target)
    added = ({ });
    for (i = 1; i < sizeof(args); i++)
      added += _resolve_selection_target(args[i]);

    // keep only loadable rooms / locations, drop what is already selected
    added = filter_array(unique_array(added), "_filter_loadable");
    added -= selection;

    if (!sizeof(added))
    {
      notify_fail("No new loadable files found.\n");
      return 0;
    }

    selection += added;

    write("Added " + sizeof(added) + " entr" +
          (sizeof(added) == 1 ? "y" : "ies") + " to the selection.\n");
    write(pretty_selection());
  }
  else if (verb == "remove")
  {
    if (sizeof(args) < 2)
    {
      notify_fail(capitalize(verb) + " what?\n");
      return 0;
    }

    target = args[1];

    if (_has_wildcards(target))
    {
      // glob match: against the basename if pattern has no '/',
      // against the full path otherwise
      string * matched;
      int by_basename;
      int i;

      by_basename = (sizeof(explode(target, "/")) == 1);
      matched = ({ });

      for (i = 0; i < sizeof(selection); i++)
      {
        string candidate;

        candidate = (by_basename ? get_path_file_name(selection[i]) : selection[i]);
        if (_glob_match(target, candidate))
          matched += ({ selection[i] });
      }

      if (!sizeof(matched))
      {
        notify_fail("No selection entries match '" + target + "'.\n");
        return 0;
      }

      selection -= matched;
      for (i = 0; i < sizeof(matched); i++)
        objects = m_delete(objects, matched[i]);

      write("Removed " + sizeof(matched) + " entr" +
            (sizeof(matched) == 1 ? "y" : "ies") + " from the selection.\n");
    }
    else
    {
      if (member_array(target, selection) == -1)
      {
        notify_fail("Not in the selection.\n");
        return 0;
      }

      selection -= ({ target });
      objects = m_delete(objects, target);

      write("Removed " + target + " from the selection.\n");
    }
  }
  else if (verb == "clean")
  {
    selection = ({ });
    objects = ([ ]);
    write("Selection cleaned.\n");
  }
  else if (verb == "list")
  {
    if (!sizeof(selection))
    {
      write("No selection.\n");
      return 1;
    }

    write(pretty_selection());
  }
  else
  {
    notify_fail("Unknown selection command.\n\n" + BUILDER_RING_SELECTION_SYNTAX + "\n");
    return 0;
  }

  return 1;
}

int do_convert(string str)
{
  // objects (rooms) to be converted to locations
  object * obs; 
  string * files;
  int i;

  files = ({ });

  // without arguments, use the current selection
  if (!str || !strlen(str) || str == "selection")
  {
    files = selection;
  }
  else
  {
    // converting our environment
    if (str == "here")
    {
      obs = ({ environment(this_player()) });
    }
    else if (sizeof(files = load_object(LOCATION_HANDLER)->resolve_targets(str)))
    {
      // resolved by the shared helper
    }
    // is a file pattern (maybe using "*")
    else
    {
      files = get_cfiles(str);

      if (!sizeof(files))
      {
        notify_fail("No such file or directory.\n");
        return 0;
      }
    }
  }

  // for conversion, filter just .c files (rooms), so 
  // we do not try to reconvert locations to locations
  files = filter_array(files, "_filter_dot_c");

  if (sizeof(files))
    call_out("do_convert_files", 0, files);
  else
    write("No files to convert.\n");
 
  return 1;
} 

int do_convert_files(string * files)
{
  return load_object(LOCATION_HANDLER)->batch_convert(files);
}

// build component < add | remove > <type>
// Applies a component change to every location in the current selection,
// delegating to the shared LOCATION_HANDLER batch helpers.
int do_component(string str)
{
  string * args;
  string verb, type;
  int changed;

  args = explode(str ? str : "", " ") - ({ "" });

  if (sizeof(args) < 2 || (args[0] != "add" && args[0] != "remove"))
  {
    notify_fail("Usage: " + BUILDER_RING_COMPONENT_SYNTAX + "\n");
    return 0;
  }

  verb = args[0];
  type = args[1];

  if (!sizeof(selection))
  {
    notify_fail("The selection is empty. Add locations first with " +
                "'build selection add ...'.\n");
    return 0;
  }

  if (file_size(COMPONENTS_DIR + type + ".c") <= 0)
  {
    notify_fail("No component blueprint '" + type + "' under "
                + COMPONENTS_DIR + ".\n");
    return 0;
  }

  if (verb == "add")
    changed = load_object(LOCATION_HANDLER)->batch_add_component(
                selection, type, ([ ]));
  else
    changed = load_object(LOCATION_HANDLER)->batch_remove_component(
                selection, type);

  write((verb == "add" ? "Adding" : "Removing") + " component '" + type +
        "' " + (verb == "add" ? "to " : "from ") + changed + " location" +
        (changed == 1 ? "" : "s") +
        " in the background; a summary will follow.\n");
  return 1;
}

// Set area-level metadata on the area of the location the coder is standing
// in. Currently the exploration achievement: whether entering the area is
// recorded in the player's diary, and its display name (a literal in the
// running instance's language). Run once per instance in that language.
int do_area(string str)
{
  string * args;
  string verb, name;
  object loc, area;

  args = explode(str ? str : "", " ") - ({ "" });

  if (sizeof(args) < 1)
  {
    notify_fail("Usage: " + BUILDER_RING_AREA_SYNTAX + "\n");
    return 0;
  }

  verb = args[0];

  loc = environment(this_player());
  if (!loc || !loc->query_location())
  {
    notify_fail("Stand in a location (not a plain room) to set area metadata.\n");
    return 0;
  }

  area = loc->query_area();
  if (!area)
  {
    notify_fail("This location has no area.\n");
    return 0;
  }

  if (verb == "exploration")
  {
    if (sizeof(args) < 2)
    {
      notify_fail("Usage: build area exploration <display name>\n");
      return 0;
    }

    name = implode(args[1..], " ");
    area->set_gives_exploration(1);
    area->set_exploration_name(name);
    write("Area '" + area->query_area_name() + "' now grants exploration: \"" +
          name + "\".\n");
    return 1;
  }
  else if (verb == "noexploration")
  {
    area->set_gives_exploration(0);
    area->set_exploration_name("");
    write("Area '" + area->query_area_name() +
          "' no longer grants exploration.\n");
    return 1;
  }
  else if (verb == "level")
  {
    int lvl, spread;

    // level is required; spread is optional and left unchanged when omitted
    if (sizeof(args) < 2 || sscanf(args[1], "%d", lvl) != 1)
    {
      notify_fail("Usage: build area level <n> [<spread>]\n");
      return 0;
    }

    spread = area->query_area_spread();
    if (sizeof(args) > 2)
      sscanf(args[2], "%d", spread);

    area->set_area_level(lvl);
    area->set_area_spread(spread);
    write("Area '" + area->query_area_name() + "' average level set to " +
          area->query_area_level() + " (spread " + area->query_area_spread() +
          ").\n");
    return 1;
  }
  else if (verb == "diplomacy")
  {
    // the citizenship this area belongs to; guards are fielded from it. "none"
    // clears it (and, on the next fill, removes the area's guards).
    if (sizeof(args) < 2)
    {
      notify_fail("Usage: build area diplomacy <citizenship|none>\n");
      return 0;
    }

    name = (args[1] == "none") ? "" : args[1];
    area->set_citizenship(name);
    if (strlen(name))
      write("Area '" + area->query_area_name() + "' now belongs to " +
            "citizenship '" + name + "'. Guards field from it.\n");
    else
      write("Area '" + area->query_area_name() +
            "' no longer belongs to a citizenship.\n");
    return 1;
  }

  notify_fail("Usage: " + BUILDER_RING_AREA_SYNTAX + "\n");
  return 0;
}

// Declare points of interest and their vacancies on the location the coder
// is standing in. Ventures (pub, shop) are attached automatically at
// conversion; this command is for the rest (town entrance, square,
// crossroads, shrine) and for binding vacancy NPCs to any POI. A location
// holds at most one POI. See include/areas/poi.h.
int do_poi(string str)
{
  string * args;
  string verb;
  object loc, area;
  string file;

  args = explode(str ? str : "", " ") - ({ "" });

  if (sizeof(args) < 1)
  {
    notify_fail("Usage: " + BUILDER_RING_POI_SYNTAX + "\n");
    return 0;
  }

  verb = args[0];

  loc = environment(this_player());
  if (!loc || !loc->query_location())
  {
    notify_fail("Stand in a location (not a plain room) to manage its POI.\n");
    return 0;
  }

  area = loc->query_area();
  if (!area)
  {
    notify_fail("This location has no area.\n");
    return 0;
  }

  file = loc->query_file_name();

  if (verb == "add")
  {
    string kind, label;

    if (sizeof(args) < 2)
    {
      notify_fail("Usage: build poi add <kind> [label]\n");
      return 0;
    }

    kind = args[1];
    if (member_array(kind, POI_KINDS) < 0)
    {
      notify_fail("Unknown POI kind '" + kind + "'. Kinds: " +
                  implode(POI_KINDS, ", ") + ".\n");
      return 0;
    }

    label = sizeof(args) > 2 ? implode(args[2..], " ") : nil;
    area->add_poi(file, kind, label);
    write("POI '" + kind + "'" + (label ? " (\"" + label + "\")" : "") +
          " set on this location.\n");
    return 1;
  }

  if (verb == "remove")
  {
    if (!area->is_poi(file))
    {
      notify_fail("This location is not a POI.\n");
      return 0;
    }
    area->remove_poi(file);
    write("POI removed from this location.\n");
    return 1;
  }

  if (verb == "list")
  {
    mapping pois;
    string * keys;
    int i;

    pois = area->query_pois();
    keys = map_indices(pois);
    if (!sizeof(keys))
    {
      write("No POIs in area '" + area->query_area_name() + "'.\n");
      return 1;
    }

    write("POIs in area '" + area->query_area_name() + "':\n");
    for (i = 0; i < sizeof(keys); i++)
    {
      mapping p;
      mapping * vs;
      int j;

      p = pois[keys[i]];
      write("  " + keys[i] + " -- " + p[POI_FIELD_KIND] +
            (p[POI_FIELD_LABEL] ? " (\"" + p[POI_FIELD_LABEL] + "\")" : "") +
            (p[POI_FIELD_GUARD_DIR] ? "   guard_dir " + p[POI_FIELD_GUARD_DIR] : "") +
            (keys[i] == file ? "   <- here" : "") + "\n");

      vs = p[POI_FIELD_VACANCIES];
      for (j = 0; vs && j < sizeof(vs); j++)
        write("      vacancy " + vs[j][VACANCY_FIELD_ROLE] + " <- " +
              vs[j][VACANCY_FIELD_SOURCE] +
              (vs[j][VACANCY_FIELD_UUID] ? "  [filled]" : "  [empty]") + "\n");
    }
    return 1;
  }

  if (verb == "guard_dir")
  {
    if (!area->is_poi(file))
    {
      notify_fail("This location is not a POI. Add one first with " +
                  "'build poi add town_entrance'.\n");
      return 0;
    }
    if (area->query_pois()[file][POI_FIELD_KIND] != POI_KIND_TOWN_ENTRANCE)
    {
      notify_fail("guard_dir only applies to a town_entrance POI.\n");
      return 0;
    }
    if (sizeof(args) < 2)
    {
      notify_fail("Usage: build poi guard_dir <direction into the town>\n");
      return 0;
    }

    // the exit into town its guards watch; re-post so live guards pick it up
    area->set_poi_guard_dir(file, args[1]);
    area->repost_guards(file);
    write("Entrance guards here now watch '" + args[1] + "'.\n");
    return 1;
  }

  if (verb == "vacancy")
  {
    string vverb;

    if (!area->is_poi(file))
    {
      notify_fail("This location is not a POI. Add one first with " +
                  "'build poi add <kind>'.\n");
      return 0;
    }

    if (sizeof(args) < 2)
    {
      notify_fail("Usage: build poi vacancy < add <role> <source> | " +
                  "remove <role> >\n");
      return 0;
    }

    vverb = args[1];

    if (vverb == "add")
    {
      string role, source;

      if (sizeof(args) < 4)
      {
        notify_fail("Usage: build poi vacancy add <role> <source>\n");
        return 0;
      }
      role = args[2];
      source = args[3];

      if (file_size(source) < 0 && file_size(source + ".c") < 0)
      {
        notify_fail("No NPC blueprint at '" + source + "'.\n");
        return 0;
      }

      area->add_vacancy(file, role, source);
      write("Vacancy '" + role + "' <- " + source + " on this POI.\n");
      return 1;
    }

    if (vverb == "remove")
    {
      if (sizeof(args) < 3)
      {
        notify_fail("Usage: build poi vacancy remove <role>\n");
        return 0;
      }
      area->remove_vacancy(file, args[2]);
      write("Vacancy '" + args[2] + "' removed.\n");
      return 1;
    }

    notify_fail("Usage: build poi vacancy < add <role> <source> | " +
                "remove <role> >\n");
    return 0;
  }

  notify_fail("Usage: " + BUILDER_RING_POI_SYNTAX + "\n");
  return 0;
}

// Report the current area's NPC population: the average level and spread, the
// roster (each blueprint's cap and how many are live in the census), and every
// POI vacancy. Read-only inspection -- before this there was no command, only
// exec snippets.
int do_npc()
{
  object loc, area;
  mapping intended, pois;
  string * sources, * pkeys;
  string out;
  int i;

  loc = environment(this_player());
  if (!loc || !loc->query_location())
  {
    notify_fail("Stand in a location (not a plain room) to inspect its area.\n");
    return 0;
  }

  area = loc->query_area();
  if (!area)
  {
    notify_fail("This location has no area.\n");
    return 0;
  }

  out = "Area '" + area->query_area_name() + "'  average level " +
        area->query_area_level() + " (spread " + area->query_area_spread() +
        ")\n";

  // roster: each blueprint's live census count against its area cap
  intended = area->query_npc_intended();
  sources = map_indices(intended);
  out += "Roster (" + sizeof(sources) + " blueprint" +
         (sizeof(sources) == 1 ? "" : "s") + "):\n";
  for (i = 0; i < sizeof(sources); i++)
    out += "  " + sources[i] + "  live " +
           area->query_npc_live_count(sources[i]) + " / cap " +
           intended[sources[i]]["max"] + "\n";
  if (!sizeof(sources))
    out += "  (none)\n";

  // vacancies: the named single-instance roles bound to the area's POIs
  pois = area->query_pois();
  pkeys = map_indices(pois);
  {
    int any, j;
    any = 0;
    for (i = 0; i < sizeof(pkeys); i++)
    {
      mapping * vs;
      vs = pois[pkeys[i]][POI_FIELD_VACANCIES];
      for (j = 0; vs && j < sizeof(vs); j++)
      {
        if (!any)
        {
          out += "Vacancies:\n";
          any = 1;
        }
        out += "  " + pkeys[i] + "  " + vs[j][VACANCY_FIELD_ROLE] + " <- " +
               vs[j][VACANCY_FIELD_SOURCE] +
               (vs[j][VACANCY_FIELD_UUID] ? "  [filled]" : "  [empty]") + "\n";
      }
    }
  }

  write(out);
  return 1;
}


