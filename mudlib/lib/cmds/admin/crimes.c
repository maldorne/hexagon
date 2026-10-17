// crimes -- admin tool to inspect the criminal records and edit the law.
//
// The crimes handler keeps each player's points per root citizenship; the law
// of a citizenship (what each offence costs, whether its guards kill, where its
// prisoners go) is stored in the diplomacy graph. Everything here acts on the
// game you are in.
//
//   crimes <player>                          a player's record
//   crimes pardon <player> <citizenship>     wipe it in a citizenship
//   crimes law <citizenship>                 the code in force there
//   crimes law <citizenship> <offence> <points|none>
//   crimes set <citizenship> lethal <0|1|none>
//   crimes set <citizenship> prison <file> [<file> ...] | none
//   crimes set <citizenship> prison add|remove <file>

#include <mud/cmd.h>
#include <crimes.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "crimes", "crimenes" }));
  set_usage("crimes <player> | pardon <player> <citizenship> | law <citizenship> " +
            "[<offence> <points|none>] | set <citizenship> <lethal|prison> <value|none>");
  set_help(
    "Inspect the criminal records and edit the law of the game you are in.\n" +
    "\n" +
    "  crimes <player>                 the record of a player, per citizenship:\n" +
    "                                  status, points (decay applied) and the\n" +
    "                                  latest offences\n" +
    "  crimes pardon <player> <cit>    wipe the record of a player in the\n" +
    "                                  country <cit> belongs to\n" +
    "  crimes law <cit>                the law code in force in <cit>: points\n" +
    "                                  for each offence (0 is legal), whether\n" +
    "                                  its guards kill and its prison; what is\n" +
    "                                  inherited from the parent citizenship\n" +
    "                                  or from the defaults is marked\n" +
    "  crimes law <cit> <offence> <n>  set what an offence costs in <cit>;\n" +
    "                                  'none' drops it, so it is inherited\n" +
    "  crimes set <cit> lethal <0|1>   whether its guards kill ('none' inherits)\n" +
    "  crimes set <cit> prison <file> [<file> ...]\n" +
    "                                  the cells its prisoners go to, each\n" +
    "                                  prisoner to one of them at random\n" +
    "                                  ('none' inherits)\n" +
    "  crimes set <cit> prison add|remove <file>\n" +
    "                                  add a cell to its own list, or take one\n" +
    "                                  out\n" +
    "\n" +
    "Offences: " + implode(CRIME_TYPES, ", ") + ".\n" +
    "Points are kept per root citizenship, the one at the top of the parent\n" +
    "chain. A place with the 'lawless' property, or in a stateless area, has\n" +
    "no law at all.");
}

private string when(int t)
{
  return t ? ctime(t, 4) : "-";
}

// A player's whole record.
private string show_record(object crimes, string name)
{
  mapping record;
  mixed * recent;
  string * cits;
  string ret;
  int i, j;

  record = crimes->query_record(name);
  if (!map_sizeof(record))
    return capitalize(name) + " has no criminal record.\n";

  ret = "Criminal record of " + capitalize(name) + "\n";
  cits = map_indices(record);

  for (i = 0; i < sizeof(cits); i++)
  {
    ret += sprintf("\n  %s: %s, %d points (first %s, last %s)\n", cits[i],
                   crimes->query_status_of(name, cits[i]),
                   record[cits[i]]["points"], when(record[cits[i]]["first"]),
                   when(record[cits[i]]["last"]));

    recent = record[cits[i]]["recent"];
    for (j = sizeof(recent) - 1; j >= 0; j--)
      ret += sprintf("    %-14s %s  victim: %s  place: %s\n", recent[j][0],
                     when(recent[j][1]),
                     strlen(recent[j][2]) ? recent[j][2] : "-",
                     strlen(recent[j][3]) ? recent[j][3] : "-");
  }

  return ret;
}

// The law code in force for a citizenship, marking what it does not set itself.
private string show_law(object crimes, object diplomacy, string cit)
{
  mapping own, law;
  string * types;
  string ret, source;
  string * prisons;
  int i, lethal;

  own = diplomacy->query_own_law(cit);
  law = crimes->query_law(cit);
  types = CRIME_TYPES;
  ret = "Law of '" + cit + "' (points kept in '" +
        diplomacy->query_root_citizenship(cit) + "'):\n";

  for (i = 0; i < sizeof(types); i++)
  {
    if (intp(own[types[i]]))
      source = "";
    else if (diplomacy->query_law_points(cit, types[i]) >= 0)
      source = "  (inherited)";
    else
      source = "  (default)";

    ret += sprintf("  %-14s %4d%s\n", types[i], law[types[i]], source);
  }

  lethal = diplomacy->query_lethal(cit);
  ret += sprintf("  %-14s %4d%s\n", "lethal", crimes->query_lethal(cit),
                 lethal < 0 ? "  (default)" : "");

  prisons = crimes->query_prisons(cit);
  if (!sizeof(prisons))
    ret += sprintf("  %-14s %s\n", "prison", "-");
  for (i = 0; i < sizeof(prisons); i++)
    ret += sprintf("  %-14s %s%s\n", i ? "" : "prison", prisons[i],
                   (!i && !sizeof(diplomacy->query_own_prisons(cit))) ?
                     "  (inherited)" : "");

  return ret;
}

static int cmd(string str, object me, string verb)
{
  object crimes, diplomacy;
  string * words;
  string cit, field, value;
  string * cells;
  int points;

  if (!str || !strlen(str))
  {
    notify_fail(query_usage() + "\n");
    return 0;
  }

  crimes = handler("crimes", me);
  diplomacy = handler("diplomacy", me);
  words = explode(str, " ") - ({ "" });

  // --- pardon ---------------------------------------------------------------
  if (words[0] == "pardon")
  {
    if (sizeof(words) < 3)
    {
      notify_fail("Usage: crimes pardon <player> <citizenship>\n");
      return 0;
    }

    if (!crimes->pardon(lower_case(words[1]), words[2]))
    {
      notify_fail(capitalize(words[1]) + " has no record in '" + words[2] + "'.\n");
      return 0;
    }

    write("Pardoned " + capitalize(words[1]) + " in '" + words[2] + "'.\n");
    return 1;
  }

  // --- law and set: from here on words[1] is a citizenship ------------------
  if (words[0] == "law" || words[0] == "set")
  {
    if (sizeof(words) < 2)
    {
      notify_fail("Usage: crimes " + words[0] + " <citizenship> ...\n");
      return 0;
    }

    cit = words[1];
    if (member_array(cit, diplomacy->query_citizenships()) == -1)
    {
      notify_fail("No citizenship '" + cit + "' in this game's diplomacy graph.\n");
      return 0;
    }

    if (words[0] == "law" && sizeof(words) == 2)
    {
      write(show_law(crimes, diplomacy, cit));
      return 1;
    }

    if (words[0] == "law")
    {
      if (sizeof(words) != 4 || member_array(words[2], CRIME_TYPES) == -1)
      {
        notify_fail("Usage: crimes law <citizenship> <offence> <points|none>\n" +
                    "Offences: " + implode(CRIME_TYPES, ", ") + ".\n");
        return 0;
      }

      if (words[3] == "none")
        points = -1;
      else if (sscanf(words[3], "%d", points) != 1 || points < 0)
      {
        notify_fail("The points must be a number, 0 or more, or 'none'.\n");
        return 0;
      }

      diplomacy->set_law_points(cit, words[2], points);
      write(show_law(crimes, diplomacy, cit));
      return 1;
    }

    // set <cit> <lethal|prison> <value>
    if (sizeof(words) < 4)
    {
      notify_fail("Usage: crimes set <citizenship> <lethal|prison> <value|none>\n");
      return 0;
    }

    field = words[2];
    value = implode(words[3..], " ");

    if (field == "lethal")
    {
      if (value == "none")
        points = -1;
      else if (value == "0" || value == "1")
        points = to_int(value);
      else
      {
        notify_fail("lethal is 0, 1 or 'none'.\n");
        return 0;
      }

      diplomacy->set_lethal(cit, points);
    }
    else if (field == "prison")
    {
      // add and remove edit the citizenship's own list; anything else replaces it
      cells = diplomacy->query_own_prisons(cit);
      if ((words[3] == "add" || words[3] == "remove") && sizeof(words) == 5)
      {
        if (words[3] == "add")
          cells = (cells - ({ words[4] })) + ({ words[4] });
        else
          cells -= ({ words[4] });
      }
      else if (value == "none")
        cells = ({ });
      else
        cells = words[3..];

      diplomacy->set_prisons(cit, cells);
    }
    else
    {
      notify_fail("Unknown field '" + field + "': use lethal or prison.\n");
      return 0;
    }

    write(show_law(crimes, diplomacy, cit));
    return 1;
  }

  // --- a player's record ----------------------------------------------------
  write(show_record(crimes, lower_case(words[0])));
  return 1;
}
