
// Every fight going on right now. Livings register nowhere, so the places that
// are loaded are asked instead: the cleaner knows all of them.

#include <std.h>
#include <mud/cmd.h>
#include <room/location-cleaner.h>

inherit CMD_BASE;

private string describe(object ob);
private string place_of(object ob);

void setup()
{
  set_aliases(({ "fights" }));
  set_usage("fights");
  set_help(
    "List every fight going on right now.\n" +
    "\n" +
    "One line per fight: who is fighting, whom, and where. Both sides are " +
    "named once, so a pair trading blows reads as a single line, and " +
    "monsters fighting each other are listed as well as players.\n" +
    "\n" +
    "Only the places currently loaded are walked, which is every place a " +
    "fight could be happening in.");
}

// What to call a fighter on screen: its short description, else its name.
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

// Where something stands, by the file its place restored from.
private string place_of(object ob)
{
  object env;
  mixed fname;

  env = environment(ob);
  if (!env)
    return "nowhere";

  fname = env->query_file_name();
  return (stringp(fname) && strlen(fname)) ? fname : file_name(env);
}

static int cmd(string str, object me, string verb)
{
  object * places, * counted;
  string * games, out;
  int g, i, j, found;

  places = ({ });
  games = map_indices((mapping)LOCATION_CLEANER->query_game_counts());

  for (g = 0; g < sizeof(games); g++)
    places += (object *)LOCATION_CLEANER->query_registered(games[g]);

  counted = ({ });
  out = "";

  for (i = 0; i < sizeof(places); i++)
  {
    object * here;

    if (!places[i])
      continue;

    here = all_inventory(places[i]);

    for (j = 0; j < sizeof(here); j++)
    {
      mixed atts;
      string * names;
      int a;

      if (!here[j] || !living(here[j]))
        continue;

      atts = here[j]->query_attacker_list();
      if (!pointerp(atts) || !sizeof(atts))
        continue;

      // the other side of a fight already reported is not a second fight
      if (member_array(here[j], counted) != -1)
        continue;

      counted += ({ here[j] }) + atts;

      names = ({ });
      for (a = 0; a < sizeof(atts); a++)
        names += ({ describe(atts[a]) });

      out += "  " + describe(here[j]) + "  vs  " + implode(names, ", ") +
             "  (" + place_of(here[j]) + ")\n";
      found++;
    }
  }

  if (!found)
  {
    notify_fail("Nobody is fighting.\n");
    return 0;
  }

  write("" + found + " fight" + (found == 1 ? "" : "s") + ":\n" + out);
  return 1;
}
