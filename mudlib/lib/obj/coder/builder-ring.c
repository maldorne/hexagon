/*
 * One ring to build them all
 *
 * neverbot 06/06/24
 */

inherit "/lib/armour.c";

#include <room/location.h>
#include <areas/area.h>
#include <maps/maps.h>
#include <translations/armour.h>

#define BUILDER_RING_BUILD_VERB ({ "build" })
#define BUILDER_RING_OPTIONS ({ "selection", "convert" })
#define BUILDER_RING_SELECTION_SYNTAX "build selection < add | remove | list >"
#define BUILDER_RING_CONVERT_SYNTAX "build convert [< selection | filename | dirname | here >]"
#define BUILDER_RING_HELP "This ring can be used by coders to help them building areas.\n\n" + \
                "Available commands:\n" + \
                "\t" + BUILDER_RING_SELECTION_SYNTAX + "\n" + \
                "\t" + BUILDER_RING_CONVERT_SYNTAX

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

string pretty_selection()
{
  string ret;
  int i;

  ret = "Current selection:\n";

  for (i = 0; i < sizeof(selection); i++)
  ret += "  - " + selection[i] + 
         " " + trim(to_string(objects[selection[i]])) + 
         " (" + (objects[selection[i]]->query_location() ? "location" : "room") + ") \n";

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
  string * resolved;
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
    if (sizeof(args) < 2)
    {
      notify_fail(capitalize(verb) + " what?\n");
      return 0;
    }

    target = args[1];

    if (member_array(target, selection) != -1)
    {
      notify_fail("Already in the selection.\n");
      return 0;
    }

    // converting our environment
    if (target == "here")
    {
      selection += ({ base_name(environment(this_player())) });
    }
    else if (target == "*")
    {
      selection += get_files(this_user()->query_role()->query_current_path() + "/*");
    }
    else if (sizeof(resolved = load_object(LOCATION_HANDLER)->resolve_targets(target)))
    {
      selection += resolved;
    }
    // absolute path with wildcards (resolve_targets only handles literal paths)
    else if (target[0] == '/' && _has_wildcards(target) &&
             sizeof(get_files(target)) > 0)
    {
      selection += get_files(target);
    }
    // is a pattern in the current directory
    else if (sizeof(get_files(this_user()->query_role()->query_path() + "/" + target)) > 0)
    {
      selection += get_files(this_user()->query_role()->query_path() + "/" + target);
    }

    selection = unique_array(selection);
    selection = filter_array(selection, "_filter_loadable");

    if (!sizeof(selection))
    {
      notify_fail("No loadable files found.\n");
      return 0;
    }

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


