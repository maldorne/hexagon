/*
 * One ring to build them all
 *
 * neverbot 06/06/24
 */

inherit "/lib/armour.c";

#include <room/location.h>
#include <room/room.h>
#include <areas/area.h>
#include <areas/poi.h>
#include <living/persisted.h>
#include <maps/maps.h>
#include <translations/armour.h>
#include <language.h>

#define COMPONENTS_DIR "/lib/location/components/"

#define BUILDER_RING_BUILD_VERB ({ "build" })
#define BUILDER_RING_OPTIONS ({ "selection", "convert", "component", "area", "poi", "role", "npc", "plot", "homes" })
#define BUILDER_RING_SELECTION_SYNTAX "build selection < add | remove | list >"
#define BUILDER_RING_CONVERT_SYNTAX "build convert [< selection | filename | dirname | here >]"
#define BUILDER_RING_COMPONENT_SYNTAX "build component < add | remove > <type>"
#define BUILDER_RING_AREA_SYNTAX "build area < exploration <display name> | noexploration | level <n> [<spread>] | diplomacy <citizenship|none> | principal >"
#define BUILDER_RING_POI_SYNTAX "build poi < add <kind> [label] | remove | list | guard_dir <dir> | vacancy <add <role> <source> | remove <role> | home <role>> >"
#define BUILDER_RING_ROLE_SYNTAX "build role < add <name> <count> <source.c> | equip <name> <item.c[|alt.c...]>... | remove <name> | list >"
#define BUILDER_RING_NPC_SYNTAX "build npc  (show this area's NPC roster, census and vacancies)"
#define BUILDER_RING_PLOT_SYNTAX "build plot < <dir> | remove <dir> >  (carve / delete an empty buildable lot)"
#define BUILDER_RING_HOMES_SYNTAX "build homes  (house the area's homeless citizens on free plots, pairing families)"
// intro line + "commands:" header are translated (name/description/help);
// the command syntax below stays English -- coder verbs are not localized
#define BUILDER_RING_HELP _LANG_RING_HELP_INTRO + \
                "\t" + BUILDER_RING_SELECTION_SYNTAX + "\n" + \
                "\t" + BUILDER_RING_CONVERT_SYNTAX + "\n" + \
                "\t" + BUILDER_RING_COMPONENT_SYNTAX + "\n" + \
                "\t" + BUILDER_RING_AREA_SYNTAX + "\n" + \
                "\t" + BUILDER_RING_POI_SYNTAX + "\n" + \
                "\t" + BUILDER_RING_ROLE_SYNTAX + "\n" + \
                "\t" + BUILDER_RING_NPC_SYNTAX + "\n" + \
                "\t" + BUILDER_RING_PLOT_SYNTAX + "\n" + \
                "\t" + BUILDER_RING_HOMES_SYNTAX

static string * selection;
static mapping objects;

void create()
{
  selection = ({ });
  objects = ([ ]);

  ::create();
  
  set_name(_LANG_RING_NAME);
  set_short(_LANG_RING_SHORT);
  add_alias(_LANG_RING_ALIAS);

  set_main_plural(_LANG_RING_PLURAL);
  add_plural(_LANG_RING_PLURALS);
  set_long(_LANG_RING_LONG);

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
int do_role(string str);
int do_npc(string str);
int do_plot(string str);
int do_homes();

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

// The basename of a location/save path, without directories or a trailing ".o",
// for the compact `build npc` listing. "-" for an empty path.
static string _base(string path)
{
  string * parts;

  if (!path || !strlen(path))
    return "-";
  parts = explode(path, "/");
  path = parts[sizeof(parts) - 1];
  if (strlen(path) > 2 && path[strlen(path) - 2 ..] == ".o")
    path = path[0 .. strlen(path) - 3];
  return path;
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

  // homes takes no argument; npc takes an optional subcommand (a bare `build
  // npc` reports, `build npc resident <source> [off]` flags residency)
  if (verb == "npc")
    return do_npc(sizeof(args) > 1 ? implode(args[1..], " ") : "");
  if (verb == "homes")
    return do_homes();

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
  else if (verb == "role")
    return do_role(implode(args[1..], " "));
  else if (verb == "plot")
    return do_plot(implode(args[1..], " "));
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
  else if (verb == "principal")
  {
    // mark this location as the area's fallback (where orphaned occupants go
    // when a location is destroyed, e.g. deleting an occupied plot)
    area->set_principal(loc->query_file_name());
    write("Area '" + area->query_area_name() + "' principal location is now " +
          loc->query_file_name() + ".\n");
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
    int i, w_loc, w_kind, w_role, w_src;

    pois = area->query_pois();
    keys = map_indices(pois);
    if (!sizeof(keys))
    {
      write("No POIs in area '" + area->query_area_name() + "'.\n");
      return 1;
    }

    // Column widths, measured over the basenames actually printed (the full
    // save paths share a directory and would wrap the terminal). The common
    // directory is stated once in the header instead.
    w_loc = w_kind = w_role = w_src = 0;
    for (i = 0; i < sizeof(keys); i++)
    {
      mapping p;
      mapping * vs;
      int j, l;

      l = strlen(get_path_file_name(keys[i]), TRUE);
      if (l > w_loc) w_loc = l;

      p = pois[keys[i]];
      l = strlen(p[POI_FIELD_KIND], TRUE);
      if (l > w_kind) w_kind = l;

      vs = p[POI_FIELD_VACANCIES];
      for (j = 0; vs && j < sizeof(vs); j++)
      {
        // the role is quoted (it is a name we chose); measure it with quotes
        l = strlen(vs[j][VACANCY_FIELD_ROLE], TRUE) + 2;
        if (l > w_role) w_role = l;
        // show the full template id so the same role name in two areas is
        // unambiguous (barman exists once per town)
        l = strlen(vs[j][VACANCY_FIELD_SOURCE], TRUE);
        if (l > w_src) w_src = l;
      }
    }

    // Header names the area; the shared directory goes on its own line so a
    // long path never breaks mid-sentence. POI rows and vacancy rows below
    // print only basenames against that directory.
    write("POIs in area '" + area->query_area_name() + "':\n");
    write("  paths under " + path(keys[0]) + "\n");
    for (i = 0; i < sizeof(keys); i++)
    {
      mapping p;
      mapping * vs;
      int j;
      string label, guard, here;

      p = pois[keys[i]];
      label = p[POI_FIELD_LABEL] ? "  \"" + p[POI_FIELD_LABEL] + "\"" : "";
      guard = p[POI_FIELD_GUARD_DIR]
              ? "  guard:" + p[POI_FIELD_GUARD_DIR] : "";
      here  = keys[i] == file ? "  <- here" : "";

      write(sprintf("  %-*s  %-*s%s%s%s\n",
                    w_loc, get_path_file_name(keys[i]),
                    w_kind, p[POI_FIELD_KIND],
                    label, guard, here));

      vs = p[POI_FIELD_VACANCIES];
      for (j = 0; vs && j < sizeof(vs); j++)
        write(sprintf("      vacancy %-*s  template %-*s  %s\n",
                      w_role, "\"" + vs[j][VACANCY_FIELD_ROLE] + "\"",
                      w_src, vs[j][VACANCY_FIELD_SOURCE],
                      vs[j][VACANCY_FIELD_UUID] ? "[filled]" : "[empty]"));
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

    if (sizeof(args) < 2)
    {
      notify_fail("Usage: build poi vacancy < add <role> <source> | " +
                  "remove <role> | home <role> >\n");
      return 0;
    }

    vverb = args[1];

    // "home" binds the current location (a plot or a house) as a role's fixed
    // vacancy home; unlike add/remove it acts on the house, not the POI, so it
    // does not require standing on the POI itself
    if (vverb == "home")
    {
      string role;
      int n;

      if (sizeof(args) < 3)
      {
        notify_fail("Usage: build poi vacancy home <role>\n");
        return 0;
      }
      role = args[2];

      if (!loc->query_component_by_type(LOCATION_COMPONENT_PLOT) &&
          !loc->query_component_by_type(LOCATION_COMPONENT_HOME))
      {
        notify_fail("Stand in a plot or a house to make it a vacancy's " +
                    "home.\n");
        return 0;
      }

      n = area->bind_vacancy_house(role, file);
      if (!n)
      {
        notify_fail("No vacancy with role '" + role + "' in this area.\n");
        return 0;
      }
      write("Bound the '" + role + "' vacancy's home to " + file + ".\n");
      return 1;
    }

    if (!area->is_poi(file))
    {
      notify_fail("This location is not a POI. Add one first with " +
                  "'build poi add <kind>'.\n");
      return 0;
    }

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

// Manage the area's role board: the named jobs a
// settlement staffs with sentient citizens. `add` declares a role with a count
// and a transitional blueprint, its work location being wherever you stand;
// `list` shows them with their live count; `remove` drops one (culling its
// NPCs). A role slot does not auto-respawn on death -- the settlement pass
// refills it.
int do_role(string str)
{
  string * args, verb;
  object loc, area;

  loc = environment(this_player());
  if (!loc || !loc->query_location())
  {
    notify_fail("Stand in a location (not a plain room) to manage its area's " +
                "roles.\n");
    return 0;
  }
  area = loc->query_area();
  if (!area)
  {
    notify_fail("This location has no area.\n");
    return 0;
  }

  args = (str && strlen(str)) ? explode(str, " ") : ({ });
  if (!sizeof(args))
  {
    notify_fail("Usage: build role < add <name> <count> <source.c> | " +
                "equip <name> <item.c>... | remove <name> | list >\n");
    return 0;
  }
  verb = args[0];

  if (verb == "add")
  {
    string name, source;
    int count;

    if (sizeof(args) < 4 || sscanf(args[2], "%d", count) != 1)
    {
      notify_fail("Usage: build role add <name> <count> <source.c>\n");
      return 0;
    }
    name = args[1];
    source = args[3];
    if (file_size(source) < 0 && file_size(source + ".c") < 0)
    {
      notify_fail("No NPC blueprint at '" + source + "'.\n");
      return 0;
    }
    // The area stores the cap (count + workplace = wherever the coder stands).
    // Behaviour is the type's: mark the template sentient through the bestiary,
    // its authoritative home, so a role is a named citizen by default.
    area->add_role(name, count, loc->query_file_name(), source);
    BESTIARY_HANDLER->set_template_behaviour(
      game_from_path(area->query_area_path()),
      area->query_role(name)["source"], ([ "sentient": 1 ]));
    area->fill_role(name);
    write("Role '" + name + "' x" + count + " <- " + source +
          ", working here.\n");
    return 1;
  }

  if (verb == "remove")
  {
    if (sizeof(args) < 2)
    {
      notify_fail("Usage: build role remove <name>\n");
      return 0;
    }
    area->remove_role(args[1]);
    write("Role '" + args[1] + "' removed.\n");
    return 1;
  }

  if (verb == "equip")
  {
    mixed * spec;
    string * tokens;
    int t;

    if (sizeof(args) < 3)
    {
      notify_fail("Usage: build role equip <name> <item.c[|alt.c...]> ...\n" +
                  "  Each argument is one slot; join alternatives with '|' and " +
                  "each NPC rolls one (e.g. weapons/club|weapons/sickle).\n");
      return 0;
    }
    if (!area->query_role(args[1]))
    {
      notify_fail("No role '" + args[1] + "' in this area.\n");
      return 0;
    }

    // one slot per argument; '|' inside a slot lists interchangeable items.
    // Validate every blueprint exists before storing the kit.
    tokens = args[2 ..];
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

    // Equipment is the type's: write the kit to the template through the
    // bestiary, then have the area re-gear any live empty-handed holders.
    BESTIARY_HANDLER->set_template_behaviour(
      game_from_path(area->query_area_path()),
      area->query_role(args[1])["source"], ([ "equipment": spec ]));
    area->reequip_role_holders(args[1]);
    write("Role '" + args[1] + "' kit set: " + sizeof(spec) +
          " slot" + (sizeof(spec) == 1 ? "" : "s") +
          " (new citizens roll their gear; existing keep theirs).\n");
    return 1;
  }

  if (verb == "list")
  {
    mapping roles;
    string * names;
    int i;

    roles = area->query_roles();
    names = map_indices(roles);
    if (!sizeof(names))
    {
      write("No roles in area '" + area->query_area_name() + "'.\n");
      return 1;
    }

    write("Roles in area '" + area->query_area_name() + "':\n");
    for (i = 0; i < sizeof(names); i++)
    {
      mapping r;
      r = roles[names[i]];
      write(sprintf("  %-14s  x%-2d  live %d  <- %s\n",
                    names[i], r["count"], area->count_role_npcs(names[i]),
                    get_path_file_name(r["source"])));
    }
    return 1;
  }

  notify_fail("Usage: build role < add <name> <count> <source.c> | " +
              "equip <name> <item.c>... | remove <name> | list >\n");
  return 0;
}

// Report the current area's NPC population: the average level and spread, the
// roster (each blueprint's cap and how many are live in the census), and every
// POI vacancy. Read-only inspection -- before this there was no command, only
// exec snippets.
int do_npc(string str)
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

  // `build npc resident <source> [off]` -- the design-time switch for who gets a
  // house. Only sources flagged here are housed by `build homes`; a bare `build
  // npc` (no argument) falls through to the roster/live report below.
  if (str && strlen(str))
  {
    string * a;

    a = explode(str, " ") - ({ "" });
    if (a[0] == "resident")
    {
      string source;
      int flag;

      if (sizeof(a) < 2)
      {
        notify_fail("Usage: build npc resident <source> [off]\n");
        return 0;
      }
      source = a[1];
      flag = !(sizeof(a) >= 3 &&
               (a[2] == "off" || a[2] == "no" || a[2] == "0"));
      if (!area->set_intended_resident(source, flag))
      {
        notify_fail("No intended NPC source '" + source +
                    "' in this area.\n");
        return 0;
      }
      write("Source '" + source + "' is " +
            (flag ? "now a resident source; build homes will house it."
                  : "no longer a resident source.") + "\n");
      return 1;
    }

    notify_fail("Usage: build npc  (report)  |  " +
                "build npc resident <source> [off]\n");
    return 0;
  }

  out = "Area '" + area->query_area_name() + "'  average level " +
        area->query_area_level() + " (spread " + area->query_area_spread() +
        ")\n";

  // roster: each template's live census count against its area cap. Sources
  // are template ids (areas/<area>/<npc>); show the full id so the same npc
  // name in two areas (or a cross-area template) is unambiguous. Right-align
  // the counts into columns.
  intended = area->query_npc_intended();
  sources = map_indices(intended);
  {
    int * lives;
    int w_name, w_live, w_cap;

    w_name = w_live = w_cap = 0;
    lives = allocate(sizeof(sources));
    for (i = 0; i < sizeof(sources); i++)
    {
      int l;
      lives[i] = area->query_total_live_count(sources[i]);
      l = strlen(sources[i], TRUE);
      if (l > w_name) w_name = l;
      l = strlen("" + lives[i]);
      if (l > w_live) w_live = l;
      l = strlen("" + intended[sources[i]]["max"]);
      if (l > w_cap) w_cap = l;
    }

    out += "Roster (" + sizeof(sources) + " template" +
           (sizeof(sources) == 1 ? "" : "s") + "):\n";
    for (i = 0; i < sizeof(sources); i++)
      out += sprintf("  %-*s  live %*d / cap %*d%s\n",
                     w_name, sources[i],
                     w_live, lives[i],
                     w_cap, intended[sources[i]]["max"],
                     intended[sources[i]]["resident"] ? "  resident" : "");
    if (!sizeof(sources))
      out += "  (none)\n";
  }

  // vacancies: named single-instance roles bound to POIs, each filled from a
  // data template (the same template id the roster uses). Columns: POI, the
  // quoted role we chose, and the template it spawns from.
  pois = area->query_pois();
  pkeys = map_indices(pois);
  {
    string * v_poi, * v_role, * v_tmpl, * v_state;
    int w_poi, w_role, w_tmpl, j;

    v_poi = v_role = v_tmpl = v_state = ({ });
    for (i = 0; i < sizeof(pkeys); i++)
    {
      mapping * vs;
      vs = pois[pkeys[i]][POI_FIELD_VACANCIES];
      for (j = 0; vs && j < sizeof(vs); j++)
      {
        v_poi   += ({ get_path_file_name(pkeys[i]) });
        v_role  += ({ "\"" + vs[j][VACANCY_FIELD_ROLE] + "\"" });
        v_tmpl  += ({ vs[j][VACANCY_FIELD_SOURCE] });
        v_state += ({ vs[j][VACANCY_FIELD_UUID] ? "[filled]" : "[empty]" });
      }
    }

    if (sizeof(v_poi))
    {
      w_poi = w_role = w_tmpl = 0;
      for (i = 0; i < sizeof(v_poi); i++)
      {
        int l;
        l = strlen(v_poi[i], TRUE);  if (l > w_poi)  w_poi = l;
        l = strlen(v_role[i], TRUE); if (l > w_role) w_role = l;
        l = strlen(v_tmpl[i], TRUE); if (l > w_tmpl) w_tmpl = l;
      }

      out += "Vacancies:\n";
      for (i = 0; i < sizeof(v_poi); i++)
        out += sprintf("  %-*s  %-*s  template %-*s  %s\n",
                       w_poi, v_poi[i], w_role, v_role[i],
                       w_tmpl, v_tmpl[i], v_state[i]);
    }
  }

  // Live NPCs grouped by their source (type). Under each type, one indented line
  // per materialized NPC: its position, the work its schedule walks it to, and
  // the home it lives in. Covers roster, role and vacancy NPCs alike.
  {
    object * live_npcs;
    string * seen;
    int k, m;

    live_npcs = area->query_live_npcs();
    seen = ({ });
    out += "\nLive NPCs (" + sizeof(live_npcs) + "):\n";
    if (!sizeof(live_npcs))
      out += "  (none loaded)\n";

    for (k = 0; k < sizeof(live_npcs); k++)
    {
      string src;

      src = live_npcs[k]->query_npc_source();
      if (member_array(src, seen) != -1)
        continue;
      seen += ({ src });

      out += "  " + (src ? src : "(no source)") + ":\n";
      for (m = 0; m < sizeof(live_npcs); m++)
        if (live_npcs[m]->query_npc_source() == src)
        {
          object npc, sched, env;
          string name;

          npc = live_npcs[m];
          env = environment(npc);
          sched = npc->query_component_by_type("schedule");
          name = npc->query_given_name()
                   ? capitalize(npc->query_given_name())
                   : npc->query_cap_name();
          out += sprintf("      %-16s  loc %-12s  work %-12s  home %-12s\n",
                         name,
                         env ? _base(env->query_file_name()) : "nowhere",
                         _base(sched ? sched->query_work() : nil),
                         _base(npc->query_home()));
        }
    }
  }

  write(out);
  return 1;
}

// build plot <dir>         -- carve an empty buildable lot one step in <dir>.
// build plot remove <dir>  -- delete the lot in <dir>, only if it is a bare plot
//                             (the plot component and nothing else, i.e. not yet
//                             built into a house).
//
// Never procedural: a programmer runs it, and it refuses (a message, no change)
// if the target coordinate is already occupied, or if the thing to remove is not
// a bare plot. It creates/deletes a pure-.o location (no .c source) plus the
// reciprocal exits, (un)indexes it in the sector map, re-indexes the current
// location's exits, and (un)registers it with the area's plot list.
int do_plot(string str)
{
  string * args;
  string verb, dir_in, canon, ldir, rdir;
  string map, game, loc_file, dir_part, plot_file, key;
  object loc, plot, sector, area;
  int * c, * d;
  int nx, ny, nz, slash;

  args = explode(str ? str : "", " ") - ({ "" });
  if (!sizeof(args))
  {
    notify_fail("Usage: " + BUILDER_RING_PLOT_SYNTAX + "\n");
    return 0;
  }

  if (args[0] == "remove" || args[0] == "delete")
  {
    if (sizeof(args) < 2)
    {
      notify_fail("Usage: build plot remove <dir>\n");
      return 0;
    }
    verb = "remove";
    dir_in = args[1];
  }
  else
  {
    verb = "create";
    dir_in = args[0];
  }

  loc = environment(this_player());
  if (!loc || !loc->query_location())
  {
    notify_fail("Stand in a location to build a plot.\n");
    return 0;
  }

  c = loc->query_coordinates();
  if (!c || sizeof(c) != 3)
  {
    notify_fail("This location has no coordinates.\n");
    return 0;
  }

  // canonical direction gives the coordinate step; the localized word is what
  // the exits store (exits are kept in the mud's language)
  canon = ROOM_HAND->canonical_dir(dir_in);
  d = load_object(MAPS_HANDLER)->query_dir_delta(canon);
  if (!d)
  {
    notify_fail("'" + dir_in + "' is not a compass direction.\n");
    return 0;
  }
  ldir = ROOM_HAND->localize_dir(canon);
  rdir = ROOM_HAND->query_opposite(ldir);

  nx = c[0] + d[0];
  ny = c[1] + d[1];
  nz = c[2] + d[2];
  key = "" + nx + "_" + ny + "_" + nz;

  map = loc->query_map_name();
  loc_file = loc->query_file_name();
  game = game_from_path(loc_file);
  area = loc->query_area();

  // the plot .o lives in the same area rooms directory as this location
  slash = strsrch(loc_file, "/", -1);
  dir_part = loc_file[0..slash];
  plot_file = dir_part + "plot_" + nx + "_" + ny + "_" + nz + ".o";

  sector = load_object(MAPS_HANDLER)->query_sector_for_coord(game, map,
                                                             nx, ny, nz);

  if (verb == "create")
  {
    // no houses inside houses: a plot cannot be carved from a plot or a home
    if (loc->query_component_by_type(LOCATION_COMPONENT_PLOT) ||
        loc->query_component_by_type(LOCATION_COMPONENT_HOME))
    {
      notify_fail("You cannot carve a plot from inside a plot or a house.\n");
      return 0;
    }
    if (sector && sector->query_nodes()[key])
    {
      notify_fail("(" + key + ") is already occupied by a location.\n");
      return 0;
    }
    if (file_size(plot_file) >= 0)
    {
      notify_fail("A file already exists at " + plot_file + ".\n");
      return 0;
    }

    plot = clone_object(BASE_LOCATION_OBJ);
    if (!plot)
    {
      notify_fail("Could not clone a location.\n");
      return 0;
    }
    plot->set_file_name(plot_file);
    plot->set_map_name(map);
    plot->set_coordinates(nx, ny, nz);
    plot->add_component(LOCATION_COMPONENT_PLOT, ([ ]));

    // reciprocal open exits (a plain doorway, not a road/path map way)
    loc->add_exit(ldir, plot_file, "open");
    plot->add_exit(rdir, loc_file, "open");
    plot->save_me();
    loc->save_me();

    // index the new plot and re-index this location so both exit graphs update
    load_object(MAPS_HANDLER)->add_location(plot);
    load_object(MAPS_HANDLER)->add_location(loc);
    if (area)
      area->add_plot(plot_file);

    write("Carved an empty plot to the " + ldir + " at (" + key + ").\n");
    return 1;
  }

  // verb == "remove"
  if (!sector || !sector->query_nodes()[key])
  {
    notify_fail("There is no location to the " + ldir + ".\n");
    return 0;
  }
  plot_file = sector->query_nodes()[key]["file"];
  plot = load_object(LOCATION_HANDLER)->load_location(plot_file);
  if (!plot)
  {
    notify_fail("Could not load the location to the " + ldir + ".\n");
    return 0;
  }

  // only a bare plot may be deleted: exactly the plot component, nothing else
  {
    object * comps;
    comps = plot->query_components();
    if (sizeof(comps) != 1 ||
        comps[0]->query_type() != LOCATION_COMPONENT_PLOT)
    {
      notify_fail("That location is not a bare plot (it is built or carries " +
                  "other components); refusing to delete.\n");
      return 0;
    }
  }

  // move any occupants (a wandered-in NPC, a stray object) to the area's
  // principal location before destroying the plot, so nothing is orphaned in
  // the void. If there are occupants and no principal is set, refuse.
  {
    object * occ;
    object principal;
    string pfile;
    int i;

    occ = all_inventory(plot);
    if (sizeof(occ))
    {
      pfile = area ? area->query_principal() : "";
      if (!pfile || !strlen(pfile) || file_size(pfile) < 0)
      {
        notify_fail("The plot has occupants and the area has no principal " +
                    "location to move them to; set one with 'build area " +
                    "principal'. Refusing to delete.\n");
        return 0;
      }
      principal = load_object(LOCATION_HANDLER)->load_location(pfile);
      if (!principal)
      {
        notify_fail("Could not load the area's principal location; refusing.\n");
        return 0;
      }
      for (i = 0; i < sizeof(occ); i++)
        catch(occ[i]->move(principal));
    }
  }

  // sever the exit, re-index this location, unindex + unregister the plot,
  // destroy the object and delete its file
  loc->remove_exit(ldir);
  loc->save_me();
  load_object(MAPS_HANDLER)->add_location(loc);
  load_object(MAPS_HANDLER)->remove_location_from_map(plot_file, map,
                                                      nx, ny, nz);
  if (area)
    area->remove_plot(plot_file);
  destruct(plot);
  remove_file(plot_file);

  write("Removed the empty plot to the " + ldir + ".\n");
  return 1;
}

// build homes -- house the current area's homeless citizens on its free plots,
// pairing a man and a woman into each family home. Shortfalls (no free plot)
// are recorded in the area's events.log.
int do_homes()
{
  object loc, area;

  loc = environment(this_player());
  if (!loc || !loc->query_location())
  {
    notify_fail("Stand in a location to assign homes.\n");
    return 0;
  }
  area = loc->query_area();
  if (!area)
  {
    notify_fail("This location has no area.\n");
    return 0;
  }

  area->assign_homes();
  write("Assigned homes to the area's homeless citizens; see the area's " +
        "event log for any shortfall.\n");
  return 1;
}


