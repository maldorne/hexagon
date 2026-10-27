#include <mud/cmd.h>
#include <living/family.h>
#include <areas/area.h>

inherit LOCATION_CMD;

// subcommands that act on a family; they cannot be surnames
#define FAMILY_VERBS ({ "found", "join", "leave", "marry", "parents" })

private int do_list(object me, string game, int whole_game, int extinct);
private int do_house(object me, string surname);
private object family_target(object me, string who);
private string family_display(object who);
private int do_found(object me, string * args);
private int do_join(object me, string game, string * args);
private int do_leave(object me, string * args);
private int do_marry(object me, string * args);
private int do_parents(object me, string * args);

void setup()
{
  set_aliases(({ "families", "familias" }));
  set_usage("families [ all [extinct] | <surname> | found [surname] | " +
            "join <surname> <who> | leave <who> | marry <who> <to whom> | " +
            "parents <who> <a> [b] ]");
  set_help(
    "Report on the houses of the game you are standing in.\n" +
    "\n" +
    "  families             the houses of the area you stand in\n" +
    "  families all         every living house of the game, and how it stands\n" +
    "  families all extinct the same, with the houses that died out\n" +
    "  families <surname>   its people, its history, what it owns\n" +
    "  families found [surname]        found a house in the area you stand in\n" +
    "                                  (a surname is minted when none is given)\n" +
    "  families join <surname> <who>   take somebody who is here into a house\n" +
    "  families leave <who>            take somebody out of their house\n" +
    "  families marry <who> <to whom>  wed two people who are here\n" +
    "  families parents <who> <a> [b]  record the parents of a member\n" +
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
    "Houses are founded with 'families found', and by 'houses assign', which " +
    "gives each household it houses a house of its own. The words found, join, " +
    "leave, marry and parents are subcommands, so no house can be looked up " +
    "under them. found works on the area you stand in, which must be settled " +
    "and grant a citizenship; join, leave, marry and parents name people " +
    "standing in the same location (or players online), and marry follows the " +
    "descent rule of the citizenship of the area it happens in.");
}

// The houses of the area you stand in, taken from the area it belongs to (the
// one keeping its census and its books): those of its people, and those owning
// a house on its books. A house that died out owns nothing and holds nobody, so
// it is only listed with the whole game.
private string * area_families(object me, object area)
{
  mapping census, owners;
  object fam;
  string * ids, * out;
  mixed surname;
  int i;

  fam = handler("families", me);
  area = (object)area->query_root_area();
  census = (mapping)area->query_npc_census();
  owners = (mapping)area->query_house_owners();
  out = ({ });

  ids = map_indices(census);
  for (i = 0; i < sizeof(ids); i++)
  {
    surname = fam->query_member_family(FAMILY_NPC + ids[i]);
    if (stringp(surname) && member_array(surname, out) == -1)
      out += ({ surname });
  }

  ids = map_indices(owners);
  for (i = 0; i < sizeof(ids); i++)
    if (stringp(owners[ids[i]]) && member_array(owners[ids[i]], out) == -1)
      out += ({ owners[ids[i]] });

  return sort_array(out);
}

// ===== families [all [extinct]] =====
private int do_list(object me, string game, int whole_game, int extinct)
{
  string * names;
  string * * rows;
  string where;
  object area, fam;
  int i, living, hidden;

  fam = handler("families", me);
  area = environment(me) ? (object)environment(me)->query_area() : nil;
  if (!whole_game && !area)
  {
    notify_fail("Stand in an area to see its houses, or ask for " +
                "'families all'.\n");
    return 0;
  }

  if (whole_game)
  {
    names = (string *)fam->query_families();
    where = game;

    // the houses that died out pile up with time; they are asked for by name
    if (!extinct)
    {
      string * alive;

      alive = ({ });
      for (i = 0; i < sizeof(names); i++)
        if (!fam->is_extinct(names[i]))
          alive += ({ names[i] });
      hidden = sizeof(names) - sizeof(alive);
      names = alive;
    }
  }
  else
  {
    names = area_families(me, area);
    where = (string)area->query_root_area()->query_area_name();
  }

  if (!sizeof(names))
  {
    write("No house has been founded in " + where + ".\n");
    return 1;
  }

  rows = ({ ({ "house", "living", "gone", "property", "" }) });
  for (i = 0; i < sizeof(names); i++)
  {
    living = sizeof((string *)fam->query_members(names[i]));
    rows += ({ ({ names[i],
                  "" + living,
                  "" + (map_sizeof((mapping)fam->query_history(
                                      names[i])) - living),
                  "" + sizeof((string *)fam->query_houses(
                                names[i])),
                  fam->is_extinct(names[i]) ? "extinct"
                    : (living ? "" : "nobody has moved in yet") }) });
  }

  write("Houses of " + where + ":\n" + columns(rows) +
        (hidden ? hidden + " extinct house" + (hidden == 1 ? "" : "s") +
                  " not shown ('families all extinct' lists them).\n" : ""));
  return 1;
}

// ===== families <surname> =====
private int do_house(object me, string surname)
{
  mapping history;
  object fam;
  string * ids, * props, * parents;
  string * * rows;
  string out;
  mixed spouse, fate;
  int i;

  fam = handler("families", me);
  if (!fam->has_family(surname))
  {
    notify_fail("No house of that name here.\n");
    return 0;
  }

  ids = (string *)fam->query_members(surname);
  history = (mapping)fam->query_history(surname);
  props = (string *)fam->query_houses(surname);

  out = "House " + surname + ", of " +
        (string)fam->query_family(surname)[FAMILY_CITIZENSHIP] +
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
      spouse = fam->query_spouse(ids[i]);
      parents = (string *)fam->query_parents(ids[i]);

      rows += ({ ({
        (string)fam->query_member_name(surname, ids[i]),
        stringp(spouse)
          ? (string)fam->query_member_name(surname, spouse)
          : "-",
        sizeof(parents)
          ? (string)fam->query_member_name(surname, parents[0])
              + (sizeof(parents) > 1 ? " and " +
                  (string)fam->query_member_name(
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

/**
 * Resolves who somebody means by a name: anybody standing here, player or NPC.
 * A family is written in ids, but a builder talks in names.
 */
private object family_target(object me, string who)
{
  object found;

  if (!who || !strlen(who))
    return nil;

  found = present(lower_case(who), environment(me));
  if (!found)
    found = find_player(lower_case(who));

  return (found && found->query_living()) ? found : nil;
}

/**
 * What to write a member down as. A generated citizen has a proper name of its
 * own; query_cap_name deliberately answers with the kind word so an NPC does
 * not read like a player in a room list, and the history wants the name.
 */
private string family_display(object who)
{
  mixed shown;

  shown = who->query_given_name();
  if (!stringp(shown) || !strlen(shown))
    shown = who->query_cap_name();

  return stringp(shown) ? capitalize(shown) : (string)who->query_family_id();
}

// ===== families found [surname] =====
private int do_found(object me, string * args)
{
  object area, fam;
  string surname, citizenship;

  area = area_of(me);
  if (!area)
  {
    notify_fail("Stand in a location (not a plain room) to found a house.\n");
    return 0;
  }

  // A house outlives the locations it stands in, so it is not founded in a
  // place that may still be wiped and rebuilt.
  if (area->query_area_state() != AREA_SETTLED)
  {
    notify_fail("'" + area->query_area_name() + "' is still a draft. " +
                "Settle it first (areas state " + AREA_SETTLED +
                ") -- a house founded here would not survive a wipe.\n");
    return 0;
  }

  citizenship = area->query_root_citizenship_path();
  if (!citizenship || !strlen(citizenship))
  {
    notify_fail("This area grants no citizenship, and a house takes its " +
                "name from its people.\n");
    return 0;
  }

  fam = handler("families", me);
  if (sizeof(args) > 1)
    surname = capitalize(args[1]);
  else
    surname = fam->mint_surname(citizenship);

  if (!surname || !strlen(surname))
  {
    notify_fail("No surname: does " + citizenship + " declare a " +
                "surname_style, and does that wordlist exist?\n");
    return 0;
  }

  if (!fam->found_family(surname, citizenship))
  {
    notify_fail("House " + surname + " already exists.\n");
    return 0;
  }

  write("House " + surname + " founded, of " + citizenship + ".\n");
  return 1;
}

// ===== families join <surname> <who> =====
private int do_join(object me, string game, string * args)
{
  object who;
  string surname;

  if (sizeof(args) < 3)
  {
    notify_fail("Usage: families join <surname> <who>\n");
    return 0;
  }

  if (!location_of(me))
  {
    notify_fail("Stand in a location (not a plain room) to work on a " +
                "family.\n");
    return 0;
  }

  surname = capitalize(args[1]);
  who = family_target(me, args[2]);

  if (!who)
  {
    notify_fail("Nobody called '" + args[2] + "' is here.\n");
    return 0;
  }
  if (!handler("families", me)->has_family(surname))
  {
    notify_fail("No house of that name in " + game + ".\n");
    return 0;
  }
  if (who->query_family())
  {
    notify_fail(family_display(who) + " already belongs to house " +
                who->query_family() + ".\n");
    return 0;
  }

  if (!who->set_family(surname, family_display(who)))
  {
    notify_fail("Could not take " + family_display(who) + " in. Only " +
                "somebody the world keeps track of has a family.\n");
    return 0;
  }

  write(family_display(who) + " is now of house " + surname + ".\n");
  return 1;
}

// ===== families leave <who> =====
private int do_leave(object me, string * args)
{
  object who;
  string surname;

  if (sizeof(args) < 2)
  {
    notify_fail("Usage: families leave <who>\n");
    return 0;
  }

  if (!location_of(me))
  {
    notify_fail("Stand in a location (not a plain room) to work on a " +
                "family.\n");
    return 0;
  }

  who = family_target(me, args[1]);
  if (!who || !who->query_family())
  {
    notify_fail("Nobody of that name here belongs to a house.\n");
    return 0;
  }

  surname = who->query_family();
  who->set_family(nil);
  write(family_display(who) + " is no longer of house " + surname + ".\n");
  return 1;
}

// ===== families marry <who> <to whom> =====
private int do_marry(object me, string * args)
{
  object who, other, area;
  string surname, rule;

  if (sizeof(args) < 3)
  {
    notify_fail("Usage: families marry <who> <to whom>\n");
    return 0;
  }

  if (!location_of(me))
  {
    notify_fail("Stand in a location (not a plain room) to work on a " +
                "family.\n");
    return 0;
  }
  area = area_of(me);

  who = family_target(me, args[1]);
  other = family_target(me, args[2]);

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

  surname = (string)handler("families", me)->wed(
              ([ "id": who->query_family_id(), "name": family_display(who),
                 "gender": who->query_gender() ]),
              ([ "id": other->query_family_id(), "name": family_display(other),
                 "gender": other->query_gender() ]),
              rule);
  if (!surname)
  {
    notify_fail("Could not wed them.\n");
    return 0;
  }

  write(family_display(who) + " and " + family_display(other) +
        " are married, of house " + surname + " (" + rule + ").\n");
  return 1;
}

// ===== families parents <who> <a> [b] =====
private int do_parents(object me, string * args)
{
  object who, other;
  string * parents;

  if (sizeof(args) < 3)
  {
    notify_fail("Usage: families parents <who> <a> [b]\n");
    return 0;
  }

  if (!location_of(me))
  {
    notify_fail("Stand in a location (not a plain room) to work on a " +
                "family.\n");
    return 0;
  }

  who = family_target(me, args[1]);
  if (!who || !who->query_family())
  {
    notify_fail("Nobody of that name here belongs to a house.\n");
    return 0;
  }

  parents = ({ });
  other = family_target(me, args[2]);
  if (other)
    parents += ({ (string)other->query_family_id() });
  if (sizeof(args) > 3)
  {
    other = family_target(me, args[3]);
    if (other)
      parents += ({ (string)other->query_family_id() });
  }

  if (!sizeof(parents))
  {
    notify_fail("Name at least one parent who is here.\n");
    return 0;
  }

  // nobody is their own parent, nor the child of the one they married
  if (member_array(who->query_family_id(), parents) != -1 ||
      member_array(handler("families", me)->query_spouse(
                     who->query_family_id()), parents) != -1 ||
      (sizeof(parents) == 2 && parents[0] == parents[1]))
  {
    notify_fail("A parent is somebody else: not the child, not their " +
                "spouse, and not the same one twice.\n");
    return 0;
  }

  handler("families", me)->set_parents((string)who->query_family_id(), parents);
  write(family_display(who) + " is now the child of " +
        (sizeof(parents) == 1 ? "one parent" : "two parents") +
        " of house " + who->query_family() + ".\n");
  return 1;
}

static int cmd(string str, object me, string verb)
{
  string * args;
  string game;

  args = words(str);
  game = game_name(me);

  if (!strlen(game))
  {
    notify_fail("Stand in a game: a house belongs to the world it was " +
                "founded in.\n");
    return 0;
  }

  if (!sizeof(args))
    return do_list(me, game, 0, 0);
  if (sizeof(args) == 1 && args[0] == "all")
    return do_list(me, game, 1, 0);
  if (sizeof(args) == 2 && args[0] == "all" && args[1] == "extinct")
    return do_list(me, game, 1, 1);

  if (args[0] == "found")
    return do_found(me, args);
  if (args[0] == "join")
    return do_join(me, game, args);
  if (args[0] == "leave")
    return do_leave(me, args);
  if (args[0] == "marry")
    return do_marry(me, args);
  if (args[0] == "parents")
    return do_parents(me, args);

  // any other single word is a surname; the subcommand words are reserved
  if (sizeof(args) == 1 && member_array(args[0], FAMILY_VERBS) == -1)
    return do_house(me, capitalize(args[0]));

  notify_fail("Usage: " + query_usage() + "\n");
  return 0;
}
