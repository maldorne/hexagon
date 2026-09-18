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
#include <areas/vacancy.h>
#include <living/persisted.h>
#include <living/family.h>
#include <basic/gender.h>
#include <sector/sector.h>
#include <translations/armour.h>
#include <language.h>

#define COMPONENTS_DIR "/lib/location/components/"

#define BUILDER_RING_BUILD_VERB ({ "build" })
#define BUILDER_RING_OPTIONS ({ "selection", "convert", "component", "area", "poi", "vacancy", "npc", "location", "plot", "homes", "home", "sign", "desc", "temple", "family" })
#define BUILDER_RING_SELECTION_SYNTAX "build selection < add | remove | list >"
#define BUILDER_RING_CONVERT_SYNTAX "build convert [< selection | filename | dirname | here >]"
#define BUILDER_RING_COMPONENT_SYNTAX "build component < add | remove > <type>"
#define BUILDER_RING_AREA_SYNTAX \
  "build area < exploration <display name> | noexploration\n" + \
  "           | level <n> [<spread>] | stats <low> <high> | stats none\n" + \
  "           | diplomacy <citizenship|none>\n" + \
  "           | parent <area path|none> | principal | barracks [none]\n" + \
  "           | state [draft|settled]\n" + \
  "           | relevel >"
#define BUILDER_RING_POI_SYNTAX \
  "build poi < add <kind> [label] | remove | list | guard_dir <dir> >"
#define BUILDER_RING_VACANCY_SYNTAX \
  "build vacancy < add <name> <count> <source.c> [poi]\n" + \
  "               | equip <name> <item.c[|alt.c...]>...\n" + \
  "               | timetable <name> [<hour> <work|home|clear> [message]]\n" + \
  "               | class <name> <class.c|none> | home <name>\n" + \
  "               | remove <name> | list >"
#define BUILDER_RING_NPC_SYNTAX \
  "build npc < (report) | resident <source> [off] >"
#define BUILDER_RING_FAMILY_SYNTAX \
  "build family < found [surname] | join <surname> <who> | leave <who>\n" + \
  "             | marry <who> <to whom> | parents <who> <a> [b]\n" + \
  "             | show <surname> | list >"
#define BUILDER_RING_TEMPLE_SYNTAX \
  "build temple < <deity path> | none >  (consecrate this location, or unconsecrate it)"
#define BUILDER_RING_LOCATION_SYNTAX \
  "build location < <dir> <name> [title] | remove <dir> >  (carve / delete a location)"
#define BUILDER_RING_PLOT_SYNTAX "build plot < <dir> | remove <dir> >  (carve / delete an empty buildable lot)"
#define BUILDER_RING_HOMES_SYNTAX "build homes  (house the area's homeless citizens on free plots, pairing families)"
// intro line + "commands:" header are translated (name/description/help);
// the command syntax below stays English -- coder verbs are not localized.
// One line per subverb, grouped by verb: the SYNTAX defines above are the
// compact one-liners `notify_fail` throws back on a bad call, and they are too
// dense to read as a menu.
#define BUILDER_RING_HELP _LANG_RING_HELP_INTRO + "\n" + \
  "  build selection add|remove|list      working set of locations\n" + \
  "  build convert [selection|<file>|<dir>|here]\n" + \
  "  build component add|remove <type>    on the selection\n" + \
  "  build location <dir> <name> [title]  carve a new location that way\n" + \
  "  build location remove <dir>          delete it, if it is bare\n" + \
  "  build temple <deity|none>            consecrate this location\n" + \
  "\n" + \
  "  build area exploration <name>        entering here is a diary event\n" + \
  "  build area noexploration\n" + \
  "  build area level <n> [<spread>]      NPC level band\n" + \
  "  build area stats <low> <high>        how strong its people are\n" + \
  "  build area diplomacy <name|none>     citizenship; guards use it\n" + \
  "  build area parent <area path|none>   what this place is part of\n" + \
  "  build area principal                 fallback location for occupants\n" + \
  "  build area barracks [none]           where its guards sleep\n" + \
  "  build area state [draft|settled]     how far along it is\n" + \
  "  build area relevel                   raise NPCs to the current band\n" + \
  "\n" + \
  "  build poi add <kind> [label]         one per location\n" + \
  "  build poi remove\n" + \
  "  build poi list\n" + \
  "  build poi guard_dir <dir>            on a town_entrance: the way in\n" + \
  "\n" + \
  "  build vacancy add <name> <count> <source.c> [poi]\n" + \
  "  build vacancy equip <name> <item.c[|alt.c]>...\n" + \
  "  build vacancy class <name> <class.c|none>  what the job trains in\n" + \
  "  build vacancy home <name>                  bind a house to the job\n" + \
  "  build vacancy resident <name> [none]       its holders live in town\n" + \
  "  build vacancy spot <name> [remove]         a place this job is worked\n" + \
  "  build vacancy spots <name>                 list them\n" + \
  "  build vacancy reseat <name>                seat its holders again\n" + \
  "  build vacancy remove <name>\n" + \
  "  build vacancy list\n" + \
  "\n" + \
  "  build npc                            roster, census and vacancies\n" + \
  "  build npc resident <source> [off]    who is entitled to a house\n" + \
  "  build plot <dir>                     carve an empty buildable lot\n" + \
  "  build plot remove <dir>              delete one, if still bare\n" + \
  "  build homes                          house the homeless citizens\n" + \
  "  build home make                      raise an empty house on this plot\n" + \
  "  build home short|long <text>         what this house is, if not a house\n" + \
  "  build home remove                    turn this house back into a plot\n" + \
  "  build sign <text>                    post a sign here (remove: take it down)\n" + \
  "  build desc [long] <text>             the prose of this location (reset: drop it)\n" + \
  "  build desc short <text>              its title, read in one line\n" + \
  "\n" + \
  "  build family found [surname]         start a house in this area\n" + \
  "  build family join <surname> <who>    take somebody into it\n" + \
  "  build family leave <who>             put somebody out of theirs\n" + \
  "  build family marry <who> <to whom>   wed two people, moving one house\n" + \
  "  build family parents <who> <a> [b]   say whose child somebody is\n" + \
  "  build family show <surname>          its people, its history, its property\n" + \
  "  build family list                    every house of this game"

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
int do_vacancy(string str);
int do_npc(string str);
int do_location(string str);
int do_plot(string str);
int do_homes();
int do_home_remove();
int do_home_make();
int do_home_describe(string what, string str);
int do_sign(string str);
int do_desc(string str);
int do_temple(string str);
int do_family(string str);

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
  // `build home remove` unbuilds the house you stand in; a bare `build home`
  // has no meaning, so the subcommand is required
  if (verb == "home")
  {
    if (sizeof(args) > 1 && args[1] == "remove")
      return do_home_remove();
    if (sizeof(args) > 1 && args[1] == "make")
      return do_home_make();
    if (sizeof(args) > 2 && (args[1] == "short" || args[1] == "long"))
      return do_home_describe(args[1], implode(args[2..], " "));
    notify_fail("Usage: build home < make | short <text> | long <text> | " +
                "remove >\n");
    return 0;
  }

  if (verb == "location")
    return do_location(implode(args[1..], " "));

  if (verb == "sign")
    return do_sign(implode(args[1..], " "));

  if (verb == "desc")
    return do_desc(implode(args[1..], " "));

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
  else if (verb == "vacancy")
    return do_vacancy(implode(args[1..], " "));
  else if (verb == "plot")
    return do_plot(implode(args[1..], " "));
  else if (verb == "temple")
    return do_temple(implode(args[1..], " "));
  else if (verb == "family")
    return do_family(implode(args[1..], " "));
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
// Consecrate the location the coder is standing in to a deity, attaching the
// temple component if it is not there yet. `none` unconsecrates it and leaves
// the component in place: the building is still a temple, it just has no god.
int do_temple(string str)
{
  object loc, temple;
  string deity;

  loc = environment(this_player());
  if (!loc || !loc->query_property("location"))
  {
    notify_fail("Stand in a location to consecrate it.\n");
    return 0;
  }

  deity = str ? trim(str) : "";
  if (!strlen(deity))
  {
    notify_fail("Usage: " + BUILDER_RING_TEMPLE_SYNTAX + "\n");
    return 0;
  }

  if (deity == "none")
    deity = "";
  else if (!load_object(deity))
  {
    notify_fail("No deity loads at '" + deity + "'.\n");
    return 0;
  }

  temple = (object)loc->query_component_by_type(LOCATION_COMPONENT_TEMPLE);
  if (!temple)
  {
    loc->add_component(LOCATION_COMPONENT_TEMPLE, ([ ]));
    temple = (object)loc->query_component_by_type(LOCATION_COMPONENT_TEMPLE);
  }

  if (!temple)
  {
    notify_fail("The temple component would not attach here.\n");
    return 0;
  }

  temple->set_deity(deity);
  loc->save_me();

  write(strlen(deity)
          ? "Consecrated to " + deity + ".\n"
          : "Unconsecrated; the temple keeps no god.\n");
  return 1;
}

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

  // How far along the area is. Moving it back to draft is allowed -- sometimes a
  // place really has to be rebuilt -- but it is the moment to say out loud what
  // a wipe would take with it, because that is the only thing the state gates.
  if (verb == "state")
  {
    mapping census;
    int people, families;

    if (sizeof(args) < 2)
    {
      write("Area '" + area->query_area_name() + "' is " +
            area->query_area_state() + ".\n");
      return 1;
    }

    if (args[1] != AREA_DRAFT && args[1] != AREA_SETTLED)
    {
      notify_fail("An area is '" + AREA_DRAFT + "' or '" + AREA_SETTLED +
                  "'.\n");
      return 0;
    }

    census = (mapping)area->query_npc_census();
    people = map_sizeof(census);
    families = sizeof((string *)handler("families", this_player())->families_of_area(
                        (string)area->query_area_path()));

    area->set_area_state(args[1]);

    write("Area '" + area->query_area_name() + "' is now " + args[1] + ".\n");
    if (args[1] == AREA_DRAFT && (people || families))
      write("It holds " + people + " person" + (people == 1 ? "" : "s") +
            " and " + families + " famil" + (families == 1 ? "y" : "ies") +
            ". A draft may be wiped, and a wipe leaves their records naming " +
            "things that no longer exist.\n");
    return 1;
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
  else if (verb == "stats")
  {
    // how strong the people of this place are, as the range their eight stats
    // are rolled in. Belongs to the place: the same race makes a weak village
    // and a hard stronghold.
    int low, high;
    int * band;

    if (sizeof(args) > 1 && args[1] == "none")
    {
      area->set_area_stats(0, 0);
      write("Area '" + area->query_area_name() + "' rolls no stats of its " +
            "own; its people keep whatever their type carries.\n");
      return 1;
    }

    if (sizeof(args) < 3 || sscanf(args[1], "%d", low) != 1 ||
        sscanf(args[2], "%d", high) != 1)
    {
      notify_fail("Usage: build area stats <low> <high>, or " +
                  "build area stats none\n");
      return 0;
    }

    area->set_area_stats(low, high);
    band = (int *)area->query_area_stats();
    write("People of '" + area->query_area_name() + "' now roll each stat " +
          "in " + band[0] + "-" + band[1] + ".\n");
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
  else if (verb == "relevel")
  {
    // an NPC keeps the level it was born with, so raising an area's band leaves
    // the people born under the old one behind. This brings them up to it.
    int touched;

    touched = (int)area->relevel_census();
    write("Re-levelled " + touched + " NPC(s) of '" + area->query_area_name() +
          "' to the current band (" + area->query_area_level() + " +/- " +
          area->query_area_spread() + ").\n");
    return 1;
  }
  else if (verb == "parent")
  {
    // what this area is part of. Everything shared down the chain is resolved
    // through the link -- today the community (roster, census, jobs, houses),
    // whatever comes next tomorrow. "none" detaches it. Always set by hand: a
    // wilderness that happens to sit under a town's folder is not its suburb.
    object parent;

    if (sizeof(args) < 2)
    {
      notify_fail("Usage: build area parent <area path|none>\n");
      return 0;
    }

    if (args[1] == "none")
    {
      area->set_parent_area("");
      write("Area '" + area->query_area_name() + "' stands on its own.\n");
      return 1;
    }

    parent = AREA_HANDLER->query_area(args[1]);
    if (!parent)
    {
      notify_fail("No area known at '" + args[1] + "'.\n");
      return 0;
    }

    if (parent == area || parent->query_root_area() == area)
    {
      notify_fail("An area cannot be its own parent, nor be placed under one " +
                  "of its own descendants.\n");
      return 0;
    }

    area->set_parent_area(args[1]);
    write("Area '" + area->query_area_name() + "' is now part of '" +
          parent->query_area_name() + "'.\n");
    return 1;
  }
  else if (verb == "barracks")
  {
    // where the guards this settlement fields sleep, one building for all
    area->set_barracks((sizeof(args) > 1 && args[1] == "none")
                         ? "" : loc->query_file_name());
    write(strlen((string)area->query_barracks())
            ? "The guards of '" + area->query_area_name() +
              "' are barracked here.\n"
            : "The guards of '" + area->query_area_name() +
              "' have no barracks.\n");
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
    string out;
    int i, w_loc, w_kind;

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
    w_loc = w_kind = 0;
    for (i = 0; i < sizeof(keys); i++)
    {
      mapping p;
      int l;

      l = strlen(get_path_file_name(keys[i]), TRUE);
      if (l > w_loc) w_loc = l;

      p = pois[keys[i]];
      l = strlen(p[POI_FIELD_KIND], TRUE);
      if (l > w_kind) w_kind = l;

    }

    // Header names the area; the shared directory goes on its own line so a
    // long path never breaks mid-sentence. POI rows and vacancy rows below
    // print only basenames against that directory.
    out = "POIs in area '" + area->query_area_name() + "':\n" +
          "  paths under " + path(keys[0]) + "\n";

    for (i = 0; i < sizeof(keys); i++)
    {
      mapping p;
      string label, guard, here;

      p = pois[keys[i]];
      label = p[POI_FIELD_LABEL] ? "  \"" + p[POI_FIELD_LABEL] + "\"" : "";
      guard = p[POI_FIELD_GUARD_DIR]
              ? "  guard:" + p[POI_FIELD_GUARD_DIR] : "";
      here  = keys[i] == file ? "  <- here" : "";

      out += sprintf("  %-*s  %-*s%s%s%s\n",
                     w_loc, get_path_file_name(keys[i]),
                     w_kind, p[POI_FIELD_KIND],
                     label, guard, here);
    }

    write(out);
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

  notify_fail("Usage: " + BUILDER_RING_POI_SYNTAX + "\n");
  return 0;
}

// Manage the jobs a settlement offers: the posts it wants held by named
// citizens. `add` declares one with a count and the type its holders are drawn
// from, its workplace being wherever you stand; the rest of the subverbs say
// what the post carries (kit, hours, class, house, places); `list` shows them
// with their live count; `remove` drops one and culls its holders. A seat
// freed by a death stays empty until somebody asks for it to be filled.
int do_vacancy(string str)
{
  string * args, verb;
  object loc, area;

  loc = environment(this_player());
  if (!loc || !loc->query_location())
  {
    notify_fail("Stand in a location (not a plain room) to manage its area's " +
                "vacancies.\n");
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
    notify_fail("Usage: " + BUILDER_RING_VACANCY_SYNTAX + "\n");
    return 0;
  }
  verb = args[0];

  if (verb == "add")
  {
    string name, source;
    int count, fixed, templated;

    if (sizeof(args) < 4 || sscanf(args[2], "%d", count) != 1)
    {
      notify_fail("Usage: build vacancy add <name> <count> <source> [fixed]\n");
      return 0;
    }
    name = args[1];
    source = args[3];
    // a fixed job is one post: it stays where it is and its holder is replaced
    // there promptly; the rest spread over the area's like places
    fixed = (sizeof(args) > 4 && args[4] == "fixed");
    // A job is filled from an authored template, so what is named here is a
    // template id ("areas/<area>/npcs/<type>"). A path still works -- it
    // normalises to the same id. The job may be declared before its type is
    // written; nobody can be taken on until it is, so say so.
    templated = (int)BESTIARY_HANDLER->has_template(
                  game_name(area), (string)area->query_template_from_source(source));
    // The area stores the job: how many, where (wherever the coder stands), and
    // the type its holders are drawn from. Behaviour is the type's: the template
    // is marked sentient in its template, so a holder is a named citizen.
    //
    // Opening a job does not staff it. Declaring that a settlement wants a
    // barman and deciding that somebody walks into the bar are two different
    // acts, and only the first belongs to a builder.
    area->open_vacancy(name, count, loc->query_file_name(), source,
                       fixed ? ([ VACANCY_FIXED: 1 ]) : ([ ]));
    write("Vacancy '" + name + "' x" + count + " <- " + source +
          ", held here. Nobody taken on.\n");
    if (!templated)
      write("There is no template for '" + source + "' yet: write one before " +
            "staffing it.\n");
    return 1;
  }

  if (verb == "remove")
  {
    if (sizeof(args) < 2)
    {
      notify_fail("Usage: build vacancy remove <name>\n");
      return 0;
    }
    area->close_vacancy(args[1]);
    write("Vacancy '" + args[1] + "' closed.\n");
    return 1;
  }

  if (verb == "class")
  {
    // what the trade trains its people in. A farmer is not a soldier, and the
    // NPC type cannot say so: one source staffs several settlements.
    if (sizeof(args) < 3)
    {
      notify_fail("Usage: build vacancy class <name> <class.c|none>\n");
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

  if (verb == "equip")
  {
    mixed * spec;
    string * tokens;
    int t;

    if (sizeof(args) < 3)
    {
      notify_fail("Usage: build vacancy equip <name> <item.c[|alt.c...]> ...\n" +
                  "  Each argument is one slot; join alternatives with '|' and " +
                  "each NPC rolls one (e.g. weapons/club|weapons/sickle).\n");
      return 0;
    }
    if (!area->query_vacancy(args[1]))
    {
      notify_fail("No job '" + args[1] + "' in this area.\n");
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

    // The kit belongs to the post, not to the people it draws from: the same
    // trade is armed differently from one town to the next. Whoever holds the
    // post now keeps what they were given at birth, the way a player keeps what
    // they own; the kit takes effect on the next person to fill it.
    area->set_vacancy_equipment(args[1], spec);
    write("Job '" + args[1] + "' kit set: " + sizeof(spec) +
          " slot" + (sizeof(spec) == 1 ? "" : "s") +
          ". Whoever fills it next rolls their gear.\n");
    return 1;
  }

  if (verb == "timetable")
  {
    // the hours the job keeps. Hour keys are strings, the way JSON leaves them
    // and the way the census reads them back.
    mapping hours;
    mapping job;
    string msg;
    int hour;

    if (sizeof(args) < 2)
    {
      notify_fail("Usage: build vacancy timetable <name> <hour> " +
                  "<work|home|clear> [message]\n");
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
      notify_fail("Usage: build vacancy timetable <name> <hour 0-23> " +
                  "<work|home|clear> [message]\n");
      return 0;
    }

    hours = mappingp(job[VACANCY_TIMETABLE]) ? job[VACANCY_TIMETABLE] : ([ ]);
    hours = ([ ]) + hours;

    if (args[3] == "clear")
    {
      map_delete(hours, "" + hour);
      area->set_vacancy_timetable(args[1], hours);
      write("Job '" + args[1] + "' no longer does anything at " + hour + ".\n");
      return 1;
    }

    if (args[3] != "work" && args[3] != "home" && file_size(args[3]) < 0)
    {
      notify_fail("A destination is 'work', 'home' or a location file.\n");
      return 0;
    }

    // the rest of the line is the departure message, a souls template rendered
    // against whoever is leaving
    msg = sizeof(args) > 4 ? implode(args[4 ..], " ") : nil;
    hours["" + hour] = msg ? ([ "goto": args[3], "msg": msg ])
                           : ([ "goto": args[3] ]);
    area->set_vacancy_timetable(args[1], hours);
    write("Job '" + args[1] + "' goes to " + args[3] + " at " + hour +
          (msg ? ", saying so." : ", silently.") + "\n");
    return 1;
  }

  if (verb == "home")
  {
    // the house that comes with the job: stand in a plot or a house and bind it
    object home;
    mapping job;
    string file;

    if (sizeof(args) < 2)
    {
      notify_fail("Usage: build vacancy home <name>\n");
      return 0;
    }

    job = area->query_vacancy(args[1]);
    if (!job)
    {
      notify_fail("No vacancy '" + args[1] + "' in this area.\n");
      return 0;
    }

    file = loc->query_file_name();
    if (loc->query_component_by_type(LOCATION_COMPONENT_PLOT))
    {
      object holder;

      holder = area->query_vacancy_holder(job);
      home = loc;
      area->build_house_at(file,
        holder ? ({ holder->query_npc_uuid() }) : ({ }));
    }
    else if (!loc->query_component_by_type(LOCATION_COMPONENT_HOME))
    {
      notify_fail("Stand in a plot or a house to make it the '" + args[1] +
                  "' vacancy's home.\n");
      return 0;
    }

    area->set_vacancy_home(args[1], file);
    write("The '" + args[1] + "' vacancy's home is now " + file + ".\n");
    return 1;
  }

  if (verb == "resident")
  {
    // whether the job's people are housed among the settlement's own. The
    // opposite of `home`, which pins every holder to the one house it names: a
    // barman sleeps over his bar, farmers each want a roof of their own.
    int flag;

    if (sizeof(args) < 2)
    {
      notify_fail("Usage: build vacancy resident <name> [none]\n");
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

  if (verb == "spot" || verb == "spots")
  {
    mapping vacancy;
    mixed spots;

    if (sizeof(args) < 2)
    {
      notify_fail("Usage: build vacancy " + verb + " <name>" +
                  (verb == "spot" ? " [remove]" : "") + "\n");
      return 0;
    }

    vacancy = (mapping)area->query_vacancy(args[1]);
    if (!vacancy)
    {
      notify_fail("No vacancy '" + args[1] + "' here.\n");
      return 0;
    }

    if (verb == "spot")
    {
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

    spots = vacancy[VACANCY_SPOTS];
    if (!pointerp(spots) || !sizeof(spots))
    {
      write("'" + args[1] + "' names no places; it is worked where it was " +
            "declared.\n");
      return 1;
    }

    write("'" + args[1] + "' is worked in " + sizeof(spots) + " place" +
          (sizeof(spots) == 1 ? "" : "s") + ":\n  " +
          implode(spots, "\n  ") + "\n");
    return 1;
  }

  if (verb == "reseat")
  {
    // hand each holder of a spread job the place it would get if the job were
    // filled today: a job filled before its workplaces were recognisable put
    // everybody on its anchor and left them there
    int moved;

    if (sizeof(args) < 2)
    {
      notify_fail("Usage: build vacancy reseat <name>\n");
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

  if (verb == "list")
  {
    mapping * all;
    string out;
    int i;

    all = area->query_vacancies();
    if (!sizeof(all))
    {
      write("No vacancies in area '" + area->query_area_name() + "'.\n");
      return 1;
    }

    // one string, one write: each write is a round through the user object
    out = "Vacancies in area '" + area->query_area_name() + "':\n";
    for (i = 0; i < sizeof(all); i++)
      out += sprintf("  %-14s %-6s %-5s %-22s <- %s\n",
                     all[i][VACANCY_JOB],
                     "x" + all[i][VACANCY_COUNT],
                     "" + sizeof(area->query_vacancy_holders(all[i])),
                     get_path_file_name(all[i][VACANCY_WORKS_AT]),
                     get_path_file_name(all[i][VACANCY_SOURCE]));
    write(out);
    return 1;
  }

  notify_fail("Usage: " + BUILDER_RING_VACANCY_SYNTAX + "\n");
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
      if (!area->set_npc_resident(source, flag))
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
  intended = area->query_npc_caps();
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

  // Every job the settlement offers: how many hold it, where, and the type
  // its holders are drawn from.
  {
    mapping * all;

    all = area->query_vacancies();
    if (sizeof(all))
    {
      out += "Vacancies:\n";
      for (i = 0; i < sizeof(all); i++)
        out += sprintf("  %-14s %-6s %-5s %-22s <- %s\n",
                       all[i][VACANCY_JOB],
                       "x" + all[i][VACANCY_COUNT],
                       "" + sizeof(area->query_vacancy_holders(all[i])),
                       get_path_file_name(all[i][VACANCY_WORKS_AT]),
                       get_path_file_name(all[i][VACANCY_SOURCE]));
    }
  }

  // Live NPCs grouped by their source (type). Under each type, one indented line
  // per materialized NPC: its position, the work its schedule walks it to, and
  // the home it lives in. Covers roster and vacancy NPCs alike.
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
// build location <dir> <name> [title] -- carve a new location one step in
// <dir>, named <name>.o in this area's rooms directory, with reciprocal open
// exits. It is how a place is added to a converted area: the .c rooms are the
// initial build of an area, not something to go back to.
//
// The new location carries no component: what it is comes from what the coder
// adds to it afterwards (a post office, a shop, a temple).
//
// build location remove <dir> -- delete it, refusing anything that is not
// bare: no components, nothing inside, and not a converted room.
int do_location(string str)
{
  string * args;
  string verb, dir_in, name, title, canon, ldir, rdir;
  string map, loc_file, dir_part, new_file, key;
  object loc, made, sector, area;
  int * c, * d;
  int nx, ny, nz, slash, i;

  args = explode(str ? str : "", " ") - ({ "" });

  if (!sizeof(args))
  {
    notify_fail("Usage: " + BUILDER_RING_LOCATION_SYNTAX + "\n");
    return 0;
  }

  if (args[0] == "remove" || args[0] == "delete")
  {
    if (sizeof(args) < 2)
    {
      notify_fail("Usage: build location remove <dir>\n");
      return 0;
    }
    verb = "remove";
    dir_in = args[1];
  }
  else
  {
    if (sizeof(args) < 2)
    {
      notify_fail("Usage: " + BUILDER_RING_LOCATION_SYNTAX + "\n");
      return 0;
    }
    verb = "create";
    dir_in = args[0];
    name = args[1];
    title = (sizeof(args) > 2) ? implode(args[2..], " ") : "";
  }

  loc = environment(this_player());
  if (!loc || !loc->query_location())
  {
    notify_fail("Stand in a location to carve another one.\n");
    return 0;
  }

  c = loc->query_coordinates();
  if (!c || sizeof(c) != 3)
  {
    notify_fail("This location has no coordinates.\n");
    return 0;
  }

  // the canonical direction gives the coordinate step; the localized word is
  // what the exits store, since exits are kept in the mud's language
  canon = ROOM_HAND->canonical_dir(dir_in);
  d = load_object(SECTORS_HANDLER)->query_dir_delta(canon);
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
  area = loc->query_area();

  // the new .o lives in the same area rooms directory as this location
  slash = strsrch(loc_file, "/", -1);
  dir_part = loc_file[0..slash];

  sector = load_object(SECTORS_HANDLER)->query_sector_for_coord(
             game_from_path(loc_file), map, nx, ny, nz);

  if (verb == "create")
  {
    // a plain file name: no path, no extension, nothing a directory listing
    // would not take
    for (i = 0; i < strlen(name); i++)
      if (!((name[i] >= 'a' && name[i] <= 'z') ||
            (name[i] >= 'A' && name[i] <= 'Z') ||
            (name[i] >= '0' && name[i] <= '9') ||
            name[i] == '_' || name[i] == '-'))
      {
        notify_fail("'" + name + "' is not a plain name (letters, digits, " +
                    "dash and underscore).\n");
        return 0;
      }

    new_file = dir_part + name + ".o";

    if (sector && sector->query_nodes()[key])
    {
      notify_fail("(" + key + ") is already occupied by a location.\n");
      return 0;
    }
    if (file_size(new_file) >= 0)
    {
      notify_fail("A file already exists at " + new_file + ".\n");
      return 0;
    }

    made = clone_object(BASE_LOCATION_OBJ);
    if (!made)
    {
      notify_fail("Could not clone a location.\n");
      return 0;
    }
    made->set_file_name(new_file);
    made->set_map_name(map);
    made->set_coordinates(nx, ny, nz);
    if (strlen(title))
      made->set_specific_short(title);

    // reciprocal open exits: a plain way through, not a road on the map
    loc->add_exit(ldir, new_file, "open");
    made->add_exit(rdir, loc_file, "open");
    made->save_me();
    loc->save_me();

    // index the new location and re-index this one so both exit graphs update
    load_object(SECTORS_HANDLER)->add_location(made);
    load_object(SECTORS_HANDLER)->add_location(loc);

    write("Carved a location to the " + ldir + " at (" + key + "), " +
          new_file + ".\n");
    return 1;
  }

  // verb == "remove"
  if (!sector || !sector->query_nodes()[key])
  {
    notify_fail("There is no location to the " + ldir + ".\n");
    return 0;
  }

  new_file = sector->query_nodes()[key]["file"];
  made = load_object(LOCATION_HANDLER)->load_location(new_file);
  if (!made)
  {
    notify_fail("Could not load the location to the " + ldir + ".\n");
    return 0;
  }

  if (sizeof(made->query_components()))
  {
    notify_fail("That location carries components; take them off first " +
                "(build selection add " + new_file + ", build component " +
                "remove <type>).\n");
    return 0;
  }
  if (strlen(made->query_original_room_file_name()))
  {
    notify_fail("That location was converted from a room; refusing to " +
                "delete it.\n");
    return 0;
  }
  if (sizeof(all_inventory(made)))
  {
    notify_fail("That location is not empty.\n");
    return 0;
  }

  // sever the exit, re-index this location, unindex the other, destroy the
  // object and delete its file
  loc->remove_exit(ldir);
  loc->save_me();
  load_object(SECTORS_HANDLER)->add_location(loc);
  load_object(SECTORS_HANDLER)->remove_location_from_map(new_file, map,
                                                        nx, ny, nz);
  destruct(made);
  remove_file(new_file);

  write("Removed the location to the " + ldir + ".\n");
  return 1;
}

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
  d = load_object(SECTORS_HANDLER)->query_dir_delta(canon);
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

  sector = load_object(SECTORS_HANDLER)->query_sector_for_coord(game, map,
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
    load_object(SECTORS_HANDLER)->add_location(plot);
    load_object(SECTORS_HANDLER)->add_location(loc);
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
  load_object(SECTORS_HANDLER)->add_location(loc);
  load_object(SECTORS_HANDLER)->remove_location_from_map(plot_file, map,
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
// build home remove -- turn the house you are standing in back into a bare plot.
// The inverse of a raised house: residents are evicted (left homeless, for a
// later `build homes` to place), the door becomes a doorway again and the plot
// returns to the free list, so `build plot remove <dir>` can then delete it.
// Turn the plot you stand on into a house, with nobody living in it yet. The
// counterpart of `build homes` for a house placed by hand rather than handed
// to whoever is homeless -- a barracks, a guildhall, a house kept for somebody
// in particular.
int do_home_make()
{
  object loc, area;
  mixed file;

  loc = environment(this_player());
  if (!loc || !loc->query_location())
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

  if (!loc->query_component_by_type(LOCATION_COMPONENT_PLOT))
  {
    notify_fail("This is not a bare plot. Carve one with " +
                "'build plot <dir>' first.\n");
    return 0;
  }

  file = area->build_house_at(loc->query_file_name(), ({ }));
  if (!stringp(file))
  {
    notify_fail("Could not raise a house here.\n");
    return 0;
  }

  write("Raised an empty house at " + file + ". Give it residents with " +
        "'build homes', or set somebody's home to it by hand.\n");
  return 1;
}

// Give the house you stand in a title or a description of its own, so a
// barracks or a guildhall does not read as one more house. The text is typed in
// the running instance's language, the way a POI label is; clear it with "none"
// and the dwelling reads as an ordinary home again.
int do_home_describe(string what, string str)
{
  object loc, home;

  loc = environment(this_player());
  if (!loc || !loc->query_location())
  {
    notify_fail("Stand inside the house you want to describe.\n");
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

// Post a sign in this location, with `str` written on it. The text is typed in
// the running instance's language, the way a POI label is.
int do_sign(string str)
{
  object loc;

  loc = environment(this_player());
  if (!loc || !loc->query_location())
  {
    notify_fail("Stand in a location (not a plain room) to post a sign.\n");
    return 0;
  }

  if (str == "remove")
  {
    if (!loc->query_component_by_type(LOCATION_COMPONENT_SIGN))
    {
      notify_fail("There is no sign here.\n");
      return 0;
    }
    loc->remove_component(LOCATION_COMPONENT_SIGN);
    loc->save_me();
    write("Sign taken down.\n");
    return 1;
  }

  if (!strlen(str))
  {
    notify_fail("Usage: build sign <text>, or build sign remove\n");
    return 0;
  }

  loc->add_component(LOCATION_COMPONENT_SIGN, ([ "sign_text": str ]));
  loc->save_me();
  write("Posted a sign reading: " + str + "\n");
  return 1;
}

// The prose of a location: what the author has to say that no component can
// work out. A location with none composes its body from its components and,
// failing those, from the description the room it was converted from had.
// build desc -- the title and the prose a location has of its own, the two
// things no component can work out. `short` is the title read in one line,
// anything else is the prose; both fall back to what the room it was converted
// from said, if it came from one.
int do_desc(string str)
{
  object loc;
  string * args;
  int title;

  loc = environment(this_player());
  if (!loc || !loc->query_location())
  {
    notify_fail("Stand in a location (not a plain room) to describe it.\n");
    return 0;
  }

  args = explode(str ? str : "", " ") - ({ "" });
  title = sizeof(args) && args[0] == "short";

  if (title)
  {
    args = args[1..];
    str = implode(args, " ");
  }
  else if (sizeof(args) && args[0] == "long")
  {
    args = args[1..];
    str = implode(args, " ");
  }

  if (!strlen(str))
  {
    mixed current, original;

    current  = title ? loc->query_specific_short() : loc->query_specific_long();
    original = title ? loc->query_original_short() : loc->query_original_long();

    write("Its own " + (title ? "title" : "prose") + ":\n  " +
          (stringp(current) && strlen(current) ? "\"" + current + "\""
                                               : "(none)") + "\n");
    write("The room it came from:\n  " +
          (stringp(original) && strlen(original) ? "\"" + original + "\""
                                                 : "(none)") + "\n");
    return 1;
  }

  if (str == "reset")
  {
    if (title)
      loc->set_specific_short("");
    else
      loc->set_specific_long("");
    loc->save_me();
    write("Dropped its own " + (title ? "title" : "prose") + ".\n");
    return 1;
  }

  if (title)
    loc->set_specific_short(str);
  else
    loc->set_specific_long(str);

  loc->save_me();
  write(title ? "Titled.\n" : "Described.\n");
  return 1;
}

int do_home_remove()
{
  object loc, area;
  int evicted;

  loc = environment(this_player());
  if (!loc || !loc->query_location())
  {
    notify_fail("Stand inside the house you want to unbuild.\n");
    return 0;
  }
  area = loc->query_area();
  if (!area)
  {
    notify_fail("This location has no area.\n");
    return 0;
  }

  evicted = (int)area->demote_house(loc->query_file_name());
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


// ---------------------------------------------------------------------------
// Families
// ---------------------------------------------------------------------------

// Resolve who somebody means by a name: anybody standing here, player or NPC.
// A family is written in ids, but a builder talks in names.
private object _family_target(string who)
{
  object found;

  if (!who || !strlen(who))
    return nil;

  found = present(lower_case(who), environment(this_player()));
  if (!found)
    found = find_player(lower_case(who));

  return (found && found->query_living()) ? found : nil;
}

// What to write a member down as. A generated citizen has a proper name of its
// own; query_cap_name deliberately answers with the kind word ("Ciudadana") so
// an NPC does not read like a player in a room list, and the history wants the name.
private string _family_display(object who)
{
  mixed shown;

  shown = who->query_given_name();
  if (!stringp(shown) || !strlen(shown))
    shown = who->query_cap_name();

  return stringp(shown) ? capitalize(shown) : (string)who->query_family_id();
}

int do_family(string str)
{
  mapping history;
  object loc, area, who, other, joins, keeps, swap;
  string * args, * names, * ids, * props, * parents;
  string verb, game, surname, citizenship, rule, out;
  mixed spouse;
  int i;

  args = explode(str ? str : "", " ") - ({ "" });

  if (!sizeof(args))
  {
    notify_fail("Usage: " + BUILDER_RING_FAMILY_SYNTAX + "\n");
    return 0;
  }

  verb = args[0];
  loc = environment(this_player());
  game = game_name(this_player());

  if (!strlen(game))
  {
    notify_fail("Stand in a game location: a family belongs to its world.\n");
    return 0;
  }

  if (verb == "list")
  {
    names = (string *)handler("families", this_player())->query_families();
    if (!sizeof(names))
    {
      write("No house has been founded in " + game + " yet.\n");
      return 1;
    }

    out = "Houses of " + game + ":\n";
    for (i = 0; i < sizeof(names); i++)
      out += sprintf("  %-20s %2d living, %2d gone, %d propert%s\n",
               names[i],
               sizeof((string *)handler("families", this_player())->query_members(names[i])),
               map_sizeof((mapping)handler("families", this_player())->query_history(names[i]))
                 - sizeof((string *)handler("families", this_player())->query_members(names[i])),
               sizeof((string *)handler("families", this_player())->query_properties(names[i])),
               sizeof((string *)handler("families", this_player())->query_properties(names[i]))
                 == 1 ? "y" : "ies");
    write(out);
    return 1;
  }

  if (verb == "show")
  {
    if (sizeof(args) < 2)
    {
      notify_fail("Usage: build family show <surname>\n");
      return 0;
    }

    surname = capitalize(args[1]);
    if (!handler("families", this_player())->has_family(surname))
    {
      notify_fail("No house of that name in " + game + ".\n");
      return 0;
    }

    ids = (string *)handler("families", this_player())->query_members(surname);
    history = (mapping)handler("families", this_player())->query_history(surname);
    props = (string *)handler("families", this_player())->query_properties(surname);

    out = "House " + surname + "\n";
    out += "  living   " + (sizeof(ids) ? "" : "(nobody -- extinct)") + "\n";
    for (i = 0; i < sizeof(ids); i++)
    {
      // no (string) cast on the spouse: an unmarried member has none, and the
      // cast is a conversion kfun that errors on nil
      spouse = handler("families", this_player())->query_spouse(ids[i]);
      out += sprintf("    %-24s %s%s\n",
               handler("families", this_player())->query_member_name(surname, ids[i]),
               stringp(spouse) ? "married to " +
                 handler("families", this_player())->query_member_name(surname, spouse) : "",
               sizeof((string *)handler("families", this_player())->query_parents(ids[i]))
                 ? "  (has parents)" : "");
    }

    out += "  history\n";
    ids = map_indices(history);
    for (i = 0; i < sizeof(ids); i++)
      if (history[ids[i]][FAMILY_FATE])
        out += sprintf("    %-24s %s\n", history[ids[i]][FAMILY_NAME],
                       history[ids[i]][FAMILY_FATE]);

    out += "  property " + (sizeof(props) ? implode(props, ", ") : "none") +
           "\n";
    write(out);
    return 1;
  }

  // everything below acts on the area under our feet
  if (!loc || !loc->query_location())
  {
    notify_fail("Stand in a location (not a plain room) to work on a " +
                "family.\n");
    return 0;
  }
  area = loc->query_area();

  if (verb == "found")
  {
    if (!area)
    {
      notify_fail("This location has no area.\n");
      return 0;
    }

    // A house outlives the locations it stands in, so it is not founded in a
    // place that may still be wiped and rebuilt.
    if (area->query_area_state() != AREA_SETTLED)
    {
      notify_fail("'" + area->query_area_name() + "' is still a draft. " +
                  "Settle it first (build area state " + AREA_SETTLED +
                  ") -- a house founded here would not survive a wipe.\n");
      return 0;
    }

    citizenship = (string)area->query_root_citizenship_path();
    if (!citizenship || !strlen(citizenship))
    {
      notify_fail("This area grants no citizenship, and a house takes its " +
                  "name from its people.\n");
      return 0;
    }

    if (sizeof(args) > 1)
      surname = capitalize(args[1]);
    else
      surname = (string)handler("families", this_player())->mint_surname(citizenship);

    if (!surname || !strlen(surname))
    {
      notify_fail("No surname: does " + citizenship + " declare a " +
                  "surname_style, and does that wordlist exist?\n");
      return 0;
    }

    if (!handler("families", this_player())->found_family(surname, citizenship))
    {
      notify_fail("House " + surname + " already exists.\n");
      return 0;
    }

    write("House " + surname + " founded, of " + citizenship + ".\n");
    return 1;
  }

  if (verb == "join")
  {
    if (sizeof(args) < 3)
    {
      notify_fail("Usage: build family join <surname> <who>\n");
      return 0;
    }

    surname = capitalize(args[1]);
    who = _family_target(args[2]);

    if (!who)
    {
      notify_fail("Nobody called '" + args[2] + "' is here.\n");
      return 0;
    }
    if (!handler("families", this_player())->has_family(surname))
    {
      notify_fail("No house of that name in " + game + ".\n");
      return 0;
    }
    if (who->query_family())
    {
      notify_fail(_family_display(who) + " already belongs to house " +
                  (string)who->query_family() + ".\n");
      return 0;
    }

    if (!who->set_family(surname, _family_display(who)))
    {
      notify_fail("Could not take " + _family_display(who) + " in. Only " +
                  "somebody the world keeps track of has a family.\n");
      return 0;
    }

    write(_family_display(who) + " is now of house " + surname + ".\n");
    return 1;
  }

  if (verb == "leave")
  {
    if (sizeof(args) < 2)
    {
      notify_fail("Usage: build family leave <who>\n");
      return 0;
    }

    who = _family_target(args[1]);
    if (!who || !who->query_family())
    {
      notify_fail("Nobody of that name here belongs to a house.\n");
      return 0;
    }

    surname = (string)who->query_family();
    handler("families", this_player())->member_married_out((string)who->query_family_id(),
                                       "nowhere");
    who->set_family(nil);
    write(_family_display(who) + " is no longer of house " + surname + ".\n");
    return 1;
  }

  if (verb == "marry")
  {
    if (sizeof(args) < 3)
    {
      notify_fail("Usage: build family marry <who> <to whom>\n");
      return 0;
    }

    who = _family_target(args[1]);
    other = _family_target(args[2]);

    if (!who || !other || who == other)
    {
      notify_fail("Name two people who are here.\n");
      return 0;
    }
    if (!who->query_family() && !other->query_family())
    {
      notify_fail("Neither of them has a house to marry into.\n");
      return 0;
    }

    // Which of the two moves is the land's business, not theirs: the
    // citizenship of the area the wedding happens in says how descent runs
    // here, which is the only answer that does not depend on who you ask
    // first.
    rule = DESCENT_PATRILINEAL;
    if (area && strlen((string)area->query_root_citizenship_path()))
      rule = (string)load_object(
               (string)area->query_root_citizenship_path())->query_descent();

    if (rule == DESCENT_MATRILINEAL)
    {
      keeps = (who->query_gender() == GENDER_FEMALE) ? who : other;
      joins = (keeps == who) ? other : who;
    }
    else
    {
      keeps = (who->query_gender() == GENDER_FEMALE) ? other : who;
      joins = (keeps == who) ? other : who;
    }

    if (!keeps->query_family())
    {
      // the one who would keep the house has none: the other's stands instead
      swap = keeps;
      keeps = joins;
      joins = swap;
    }

    if (joins->query_family() &&
        joins->query_family() != keeps->query_family())
    {
      handler("families", this_player())->member_married_out((string)joins->query_family_id(), (string)keeps->query_family());
      joins->set_family(nil);
    }

    if (!joins->query_family())
      joins->set_family((string)keeps->query_family(), _family_display(joins));

    if (!handler("families", this_player())->set_spouse((string)who->query_family_id(),
                                    (string)other->query_family_id()))
    {
      notify_fail("Could not wed them.\n");
      return 0;
    }

    write(_family_display(who) + " and " + _family_display(other) +
          " are married, of house " + (string)keeps->query_family() +
          " (" + rule + ").\n");
    return 1;
  }

  if (verb == "parents")
  {
    if (sizeof(args) < 3)
    {
      notify_fail("Usage: build family parents <who> <a> [b]\n");
      return 0;
    }

    who = _family_target(args[1]);
    if (!who || !who->query_family())
    {
      notify_fail("Nobody of that name here belongs to a house.\n");
      return 0;
    }

    parents = ({ });
    other = _family_target(args[2]);
    if (other)
      parents += ({ (string)other->query_family_id() });
    if (sizeof(args) > 3)
    {
      other = _family_target(args[3]);
      if (other)
        parents += ({ (string)other->query_family_id() });
    }

    if (!sizeof(parents))
    {
      notify_fail("Name at least one parent who is here.\n");
      return 0;
    }

    handler("families", this_player())->set_parents((string)who->query_family_id(), parents);
    write(_family_display(who) + " is now the child of " + sizeof(parents) +
          " of house " + (string)who->query_family() + ".\n");
    return 1;
  }

  notify_fail("Usage: " + BUILDER_RING_FAMILY_SYNTAX + "\n");
  return 0;
}
