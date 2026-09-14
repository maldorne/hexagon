
// Where something is: anything alive that answers to a name, anywhere in the
// mud, plus whatever is present where you stand.

#include <std.h>
#include <mud/cmd.h>

inherit CMD_BASE;

private string describe(object ob);
private string trail(object ob);

void setup()
{
  set_aliases(({ "whereis" }));
  set_usage("whereis <name>");
  set_help(
    "Say where something is.\n" +
    "\n" +
    "Everything alive answering to that name is listed -- players, NPCs and " +
    "monsters alike -- along with whatever is present where you stand. Each " +
    "line gives the chain of places the thing is inside, innermost first, " +
    "naming a location by the file it restored from rather than by its " +
    "clone number.\n" +
    "\n" +
    "Locating a connected player takes an admin: a coder asking for one is " +
    "refused, and the attempt is logged.");
}

// What to call the thing on screen: its short description, else its name,
// else the file it came from.
private string describe(object ob)
{
  mixed str;

  if (!ob)
    return "(gone)";

  if (!catch(str = ob->short()) && stringp(str) && strlen(str))
    return str;

  if (!catch(str = ob->query_name()) && stringp(str) && strlen(str))
    return str;

  return file_name(ob);
}

// The places something is inside, innermost first. A location is a clone of
// the generic location object, so it is named by the file it restored from.
private string trail(object ob)
{
  string out;
  object env;

  out = "";
  env = ob;

  while (env = environment(env))
  {
    mixed fname;

    fname = env->query_file_name();
    if (!stringp(fname) || !strlen(fname))
      fname = file_name(env);

    out += (strlen(out) ? " / " : "") + fname;
  }

  return strlen(out) ? out : "(nowhere)";
}

static int cmd(string str, object me, string verb)
{
  object * found, * livings;
  string out;
  int i;

  if (!str || !strlen(str))
  {
    notify_fail("Where is what?\n");
    return 0;
  }

  found = (object *)me->wiz_present(str, me);
  if (!found)
    found = ({ });

  // wiz_present only reaches your own vicinity. Every living registers the
  // name it answers to, so ask for those as well and the strays and the
  // duplicates turn up too.
  livings = find_all_livings(str);
  for (i = 0; i < sizeof(livings); i++)
    if (member_array(livings[i], found) == -1)
      found += ({ livings[i] });

  if (!sizeof(found))
  {
    notify_fail("Nothing is called " + str + ".\n");
    return 0;
  }

  out = "";

  for (i = 0; i < sizeof(found); i++)
  {
    mixed invis;

    // an object that is not a player answers nil here, and is visible
    invis = found[i]->query_invis();
    if (intp(invis) && invis > 1)
      continue;

    if (interactive(found[i]) && !me->query_admin() && !me->query_thane())
    {
      log_file("whereis", "[" + ctime(time(), 4) + "] " +
               me->query_cap_name() + " tried to locate " +
               found[i]->query_cap_name() + ".\n");
      out += "  " + describe(found[i]) + "  ->  not for you to know\n";
      continue;
    }

    out += "  " + describe(found[i]) + "  ->  " + trail(found[i]) + "\n";
  }

  write(out);
  return 1;
}
