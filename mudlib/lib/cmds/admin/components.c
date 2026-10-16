
#include <mud/cmd.h>
#include <room/location.h>

inherit CMD_BASE;

#define COMPONENTS_DIR "/lib/location/components/"

private int do_list(object env);
private int do_add(object env, string type);
private int do_remove(object env, string type);
private int do_batch(string verb, string type, string * targets);
private string * resolve_target(string target);
private string * loadable(string * files);

void setup()
{
  set_aliases(({ "components" }));
  set_usage("components\n" +
            "components add <type> [<target> ...]\n" +
            "components remove <type> [<target> ...]");
  set_help(
    "Manage the components attached to locations.\n" +
    "\n" +
    "  components                          list the components on this location\n" +
    "  components add <type>               attach a component (city, forest, ...)\n" +
    "  components remove <type>            detach a component\n" +
    "  components add <type> <target> ...  attach it to every target\n" +
    "  components remove <type> <target> ... detach it from every target\n" +
    "\n" +
    "The type is the component blueprint name under " + COMPONENTS_DIR + ".\n" +
    "Without targets the location you are standing in is changed, and saved so " +
    "its sector reclassifies.\n" +
    "\n" +
    "Each target is one of: here (the room you stand in), * (everything in your " +
    "current directory), a glob with * and ? (absolute, or relative to your " +
    "directory), a file or a directory, or a bare name relative to your " +
    "directory (.c is tried when it has no extension). Only loadable rooms and " +
    "locations are kept. With targets the work is done in the background and " +
    "a summary follows.\n" +
    "\n" +
    "To convert rooms into locations first, use room2loc.");
}

static int cmd(string str, object me, string verb)
{
  string * tokens;
  object env;
  string type;

  tokens = explode(str ? str : "", " ") - ({ "" });

  if (sizeof(tokens) && tokens[0] != "add" && tokens[0] != "remove")
  {
    notify_fail("Usage: components [ add | remove <type> [<target> ...] ]\n");
    return 0;
  }
  if (sizeof(tokens) == 1)
  {
    notify_fail("Usage: components [ add | remove <type> [<target> ...] ]\n");
    return 0;
  }

  // with targets, the work is not on the location we stand in
  if (sizeof(tokens) > 2)
    return do_batch(tokens[0], tokens[1], tokens[2 ..]);

  env = environment(me);
  if (!env || !env->query_location())
  {
    notify_fail("You are not standing in a location.\n");
    return 0;
  }

  if (!sizeof(tokens))
    return do_list(env);

  type = tokens[1];
  return tokens[0] == "remove" ? do_remove(env, type) : do_add(env, type);
}

/**
 * components: lists the components on the location.
 */
private int do_list(object env)
{
  object * comps;
  string ret, info;
  int i;

  comps = env->query_components();
  if (!comps || !sizeof(comps))
  {
    write("This location has no components.\n");
    return 1;
  }

  ret = "Components on this location:\n";
  for (i = 0; i < sizeof(comps); i++)
  {
    info = comps[i]->query_info();
    ret += "  - " + comps[i]->query_type() +
           (info && strlen(info) ? "  (" + info + ")" : "") + "\n";
  }
  write(ret);
  return 1;
}

/**
 * components remove <type>: detaches a component from the location.
 */
private int do_remove(object env, string type)
{
  if (!env->has_component(type))
  {
    notify_fail("This location has no '" + type + "' component.\n");
    return 0;
  }
  env->remove_component(type);
  write("Removed component '" + type + "'.\n");
  return 1;
}

/**
 * components add <type>: attaches a component to the location.
 */
private int do_add(object env, string type)
{
  if (file_size(COMPONENTS_DIR + type + ".c") <= 0)
  {
    notify_fail("No component blueprint '" + type + "' under " +
                COMPONENTS_DIR + ".\n");
    return 0;
  }
  if (env->has_component(type))
  {
    notify_fail("This location already has a '" + type + "' component.\n");
    return 0;
  }
  env->add_component(type, ([ ]));
  write("Added component '" + type + "'.\n");
  return 1;
}

/**
 * components add|remove <type> <target> ...: changes every location the
 * targets name, in the background through the location handler.
 */
private int do_batch(string verb, string type, string * targets)
{
  string * files;
  int i, changed;

  if (file_size(COMPONENTS_DIR + type + ".c") <= 0)
  {
    notify_fail("No component blueprint '" + type + "' under " +
                COMPONENTS_DIR + ".\n");
    return 0;
  }

  // every target may name one file, a glob, a directory or a bare room name
  files = ({ });
  for (i = 0; i < sizeof(targets); i++)
    files += resolve_target(targets[i]);

  // keep only loadable rooms / locations, each once
  files = loadable(files);
  if (!sizeof(files))
  {
    notify_fail("No loadable files found.\n");
    return 0;
  }

  if (verb == "add")
    changed = LOCATION_HANDLER->batch_add_component(files, type, ([ ]));
  else
    changed = LOCATION_HANDLER->batch_remove_component(files, type);

  write((verb == "add" ? "Adding" : "Removing") + " component '" + type +
        "' " + (verb == "add" ? "to " : "from ") + changed + " location" +
        (changed == 1 ? "" : "s") +
        " in the background; a summary will follow.\n");
  return 1;
}

/**
 * Whether a pattern has wildcards: it contains `*` or `?`.
 */
private int has_wildcards(string pattern)
{
  int i, len;

  len = strlen(pattern);
  for (i = 0; i < len; i++)
    if (pattern[i] == '*' || pattern[i] == '?')
      return 1;

  return 0;
}

/**
 * Resolves one target into the file paths it names. Accepts, in order: `here`
 * (the room you stand in), `*` (everything in your current directory), a
 * `*`/`?` glob (absolute, or relative to your directory), a literal file or
 * directory, and a bare room name with no extension (the `.c` is tried
 * automatically). Returns ({ }) when nothing matches.
 */
private string * resolve_target(string target)
{
  string cur;
  string * res;
  object env;

  if (target == "here")
  {
    env = environment(this_player());
    if (!env)
      return ({ });
    return ({ env->query_location() ? env->query_file_name() : base_name(env) });
  }

  cur = this_user()->query_role()->query_current_path();

  if (target == "*")
    return get_files(cur + "/*");

  if (has_wildcards(target))
    return get_files(target[0] == '/' ? target : cur + "/" + target);

  // literal path or name: resolve_targets handles absolute paths, dirs and
  // names relative to the current directory. Retry with `.c` when the target
  // carries no extension so `a1` finds `a1.c`.
  res = LOCATION_HANDLER->resolve_targets(target);
  if (sizeof(res))
    return res;

  if (strlen(target) < 2 || target[strlen(target) - 2 ..] != ".c")
    return LOCATION_HANDLER->resolve_targets(target + ".c");

  return ({ });
}

/**
 * The files that load as a room (.c) or a location (.o), each once.
 */
private string * loadable(string * files)
{
  string * kept;
  object what;
  int i;

  kept = ({ });
  for (i = 0; i < sizeof(files); i++)
  {
    if (member_array(files[i], kept) != -1 || strlen(files[i]) < 2)
      continue;

    what = nil;
    // a .c should be a room; a .o a saved location
    if (files[i][strlen(files[i]) - 2 ..] == ".c")
    {
      catch(what = load_object(files[i]));
      if (what && what->query_room())
        kept += ({ files[i] });
    }
    else if (files[i][strlen(files[i]) - 2 ..] == ".o")
    {
      what = LOCATION_HANDLER->load_location(files[i]);
      if (what && what->query_location())
        kept += ({ files[i] });
    }
  }

  return kept;
}
