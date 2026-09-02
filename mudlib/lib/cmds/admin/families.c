#include <mud/cmd.h>
#include <living/family.h>

inherit CMD_BASE;

private string columns(string * * rows);
private int do_list(object me, string game);
private int do_house(object me, string surname);

void setup()
{
  set_aliases(({ "families", "familias" }));
  set_usage("families [ <surname> ]");
  set_help(
    "Report on the houses of the game you are standing in.\n" +
    "\n" +
    "  families             every house, and how it stands\n" +
    "  families <surname>   its people, its history, what it owns\n" +
    "\n" +
    "A family is a surname, the people who belong to it and what it owns. " +
    "It is kept in a register of its own rather than on any of its members, " +
    "which is what lets it outlive them: a member carries nothing but the " +
    "surname, and everything else -- who is married to whom, whose child " +
    "somebody is, which house a door belongs to -- is asked of the register.\n" +
    "\n" +
    "A house keeps a record of everyone who has ever belonged to it and what " +
    "became of each -- the living, the dead, and those who married into " +
    "another house. An NPC's savefile is deleted when it dies, so without it " +
    "the dead stop being nameable and a generation means nothing. A house " +
    "that held people once and holds none now is extinct: its property is " +
    "freed, but its surname is spent and never minted again.\n" +
    "\n" +
    "Houses are founded with the builder ring ('build family'), and by " +
    "'build homes', which gives each household it houses a house of its own.");
}

// Lay rows out in columns, the first row being the header.
private string columns(string * * rows)
{
  string out;
  int * width;
  int i, j, k, l;

  if (!sizeof(rows))
    return "";

  width = allocate_int(sizeof(rows[0]));
  for (i = 0; i < sizeof(rows); i++)
    for (j = 0; j < sizeof(rows[i]); j++)
    {
      l = strlen(rows[i][j], TRUE);
      if (l > width[j])
        width[j] = l;
    }

  out = "";
  for (i = 0; i < sizeof(rows); i++)
  {
    out += " ";
    for (j = 0; j < sizeof(rows[i]); j++)
      out += sprintf(" %-*s", width[j], rows[i][j]);
    out += "\n";

    if (i == 0)
    {
      out += " ";
      for (j = 0; j < sizeof(rows[i]); j++)
      {
        out += " ";
        for (k = 0; k < width[j]; k++)
          out += "-";
      }
      out += "\n";
    }
  }

  return out;
}

// ===== families =====
private int do_list(object me, string game)
{
  string * names;
  string * * rows;
  int i, living;

  names = (string *)handler("families", me)->query_families();
  if (!sizeof(names))
  {
    write("No house has been founded in " + game + ".\n");
    return 1;
  }

  rows = ({ ({ "house", "living", "gone", "property", "" }) });
  for (i = 0; i < sizeof(names); i++)
  {
    living = sizeof((string *)handler("families", me)->query_members(names[i]));
    rows += ({ ({ names[i],
                  "" + living,
                  "" + (map_sizeof((mapping)handler("families", me)->query_history(
                                      names[i])) - living),
                  "" + sizeof((string *)handler("families", me)->query_properties(
                                names[i])),
                  handler("families", me)->is_extinct(names[i]) ? "extinct"
                    : (living ? "" : "nobody has moved in yet") }) });
  }

  write("Houses of " + game + ":\n" + columns(rows));
  return 1;
}

// ===== families <surname> =====
private int do_house(object me, string surname)
{
  mapping history;
  string * ids, * props, * parents;
  string * * rows;
  string out;
  mixed spouse, fate;
  int i;

  if (!handler("families", me)->has_family(surname))
  {
    notify_fail("No house of that name here.\n");
    return 0;
  }

  ids = (string *)handler("families", me)->query_members(surname);
  history = (mapping)handler("families", me)->query_history(surname);
  props = (string *)handler("families", me)->query_properties(surname);

  out = "House " + surname + ", of " +
        (string)handler("families", me)->query_family(surname)[FAMILY_CITIZENSHIP] +
        "\n\n";

  if (!sizeof(ids) && !map_sizeof(history))
    out += "It was founded and nobody has moved in yet.\n\n";
  else if (!sizeof(ids))
    out += "Nobody is left of it: the house is extinct and its surname " +
           "spent.\n\n";
  else
  {
    rows = ({ ({ "living", "married to", "child of" }) });
    for (i = 0; i < sizeof(ids); i++)
    {
      // no (string) cast on the spouse: the unmarried have none, and the cast
      // is a conversion kfun that errors on nil
      spouse = handler("families", me)->query_spouse(ids[i]);
      parents = (string *)handler("families", me)->query_parents(ids[i]);

      rows += ({ ({
        (string)handler("families", me)->query_member_name(surname, ids[i]),
        stringp(spouse)
          ? (string)handler("families", me)->query_member_name(surname, spouse)
          : "-",
        sizeof(parents)
          ? (string)handler("families", me)->query_member_name(surname, parents[0])
              + (sizeof(parents) > 1 ? " and " +
                  (string)handler("families", me)->query_member_name(
                    surname, parents[1]) : "")
          : "-" }) });
    }
    out += columns(rows) + "\n";
  }

  // The point of keeping a history: the dead and the departed are still named
  // here, wherever their savefile went. Only they -- the living are above.
  ids = map_indices(history);
  rows = ({ ({ "history", "what became of them" }) });
  for (i = 0; i < sizeof(ids); i++)
  {
    fate = history[ids[i]][FAMILY_FATE];
    if (!stringp(fate))
      continue;
    rows += ({ ({ history[ids[i]][FAMILY_NAME], fate }) });
  }
  if (sizeof(rows) > 1)
    out += columns(rows) + "\n";

  if (!sizeof(props))
    out += "It owns nothing.\n";
  else
  {
    rows = ({ ({ "property" }) });
    for (i = 0; i < sizeof(props); i++)
      rows += ({ ({ props[i] }) });
    out += columns(rows);
  }

  write(out);
  return 1;
}

static int cmd(string str, object me, string verb)
{
  string * args;
  string game;

  args = (str && strlen(str)) ? explode(str, " ") - ({ "" }) : ({ });
  game = game_name(me);

  if (!strlen(game))
  {
    notify_fail("Stand in a game: a house belongs to the world it was " +
                "founded in.\n");
    return 0;
  }

  if (!sizeof(args))
    return do_list(me, game);
  if (sizeof(args) == 1)
    return do_house(me, capitalize(args[0]));

  notify_fail("Usage: families [ <surname> ]\n");
  return 0;
}
