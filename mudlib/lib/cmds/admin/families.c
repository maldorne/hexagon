#include <mud/cmd.h>
#include <living/family.h>

inherit CMD_BASE;

private string columns(string * * rows);
private int do_list(string game, object me);
private int do_house(string game, object me, string surname);

void setup()
{
  set_aliases(({ "families", "familias" }));
  set_usage("families [ <surname> ]");
  set_help(
    "Report on the houses of the game you are standing in.\n" +
    "\n" +
    "  families             every house, and how it stands\n" +
    "  families <surname>   one house: its people, its roll, its property\n" +
    "\n" +
    "A family is a surname, the people who belong to it and what it owns. " +
    "It is kept in a register of its own rather than on any of its members, " +
    "which is what lets it outlive them: a member carries nothing but the " +
    "surname, and everything else -- who is married to whom, whose child " +
    "somebody is, which house a door belongs to -- is asked of the register.\n" +
    "\n" +
    "The 'roll' is everyone the house ever held and what became of them. An " +
    "NPC's savefile is deleted when it dies, so without the roll the dead " +
    "stop being nameable and a generation means nothing. A house whose roll " +
    "still has names but whose living count is zero is extinct: its property " +
    "is freed, but its surname is spent and never minted again.\n" +
    "\n" +
    "Houses are founded with the builder ring ('build family'), and by " +
    "'build homes', which gives each household it houses a house of its own.");
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
    for (j = 0; j < sizeof(rows[i]); j++)
    {
      int l;
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
private int do_list(string game, object me)
{
  string * names;
  string * * rows;
  int i, living;

  names = (string *)FAMILY_HANDLER->query_families(game);
  if (!sizeof(names))
  {
    write("No house has been founded in " + game + ".\n");
    return 1;
  }

  rows = ({ ({ "house", "living", "roll", "property", "" }) });
  for (i = 0; i < sizeof(names); i++)
  {
    living = sizeof((string *)FAMILY_HANDLER->query_members(game, names[i]));
    rows += ({ ({ names[i],
                  "" + living,
                  "" + map_sizeof((mapping)FAMILY_HANDLER->query_roll(
                                    game, names[i])),
                  "" + sizeof((string *)FAMILY_HANDLER->query_properties(
                                game, names[i])),
                  living ? "" : "extinct" }) });
  }

  write("Houses of " + game + ":\n" + columns(rows));
  return 1;
}

// ===== families <surname> =====
private int do_house(string game, object me, string surname)
{
  mapping roll;
  string * ids, * props, * parents;
  string * * rows;
  string out;
  mixed spouse, fate;
  int i;

  if (!FAMILY_HANDLER->has_family(game, surname))
  {
    notify_fail("No house of that name in " + game + ".\n");
    return 0;
  }

  ids = (string *)FAMILY_HANDLER->query_members(game, surname);
  roll = (mapping)FAMILY_HANDLER->query_roll(game, surname);
  props = (string *)FAMILY_HANDLER->query_properties(game, surname);

  out = "House " + surname + ", of " +
        (string)FAMILY_HANDLER->query_family(game, surname)[FAMILY_CITIZENSHIP] +
        "\n\n";

  if (!sizeof(ids))
    out += "Nobody is left of it: the house is extinct and its surname " +
           "spent.\n\n";
  else
  {
    rows = ({ ({ "living", "married to", "child of" }) });
    for (i = 0; i < sizeof(ids); i++)
    {
      // no (string) cast on the spouse: the unmarried have none, and the cast
      // is a conversion kfun that errors on nil
      spouse = FAMILY_HANDLER->query_spouse(game, ids[i]);
      parents = (string *)FAMILY_HANDLER->query_parents(game, ids[i]);

      rows += ({ ({
        (string)FAMILY_HANDLER->query_member_name(game, surname, ids[i]),
        stringp(spouse)
          ? (string)FAMILY_HANDLER->query_member_name(game, surname, spouse)
          : "-",
        sizeof(parents)
          ? (string)FAMILY_HANDLER->query_member_name(game, surname, parents[0])
              + (sizeof(parents) > 1 ? " and " +
                  (string)FAMILY_HANDLER->query_member_name(
                    game, surname, parents[1]) : "")
          : "-" }) });
    }
    out += columns(rows) + "\n";
  }

  // The roll is the point of the register: the dead and the departed are still
  // named here, wherever their savefile went.
  ids = map_indices(roll);
  rows = ({ ({ "roll", "" }) });
  for (i = 0; i < sizeof(ids); i++)
  {
    fate = roll[ids[i]][FAMILY_FATE];
    rows += ({ ({ roll[ids[i]][FAMILY_NAME],
                  stringp(fate) ? fate : "living" }) });
  }
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
    return do_list(game, me);
  if (sizeof(args) == 1)
    return do_house(game, me, capitalize(args[0]));

  notify_fail("Usage: families [ <surname> ]\n");
  return 0;
}
