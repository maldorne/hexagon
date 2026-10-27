#include <mud/cmd.h>
#include <living/family.h>
#include <areas/area.h>
#include <areas/vacancy.h>
#include <room/location.h>

inherit LOCATION_CMD;

// subcommands that act on a family; they cannot be surnames
#define FAMILY_VERBS ({ "found", "join", "leave", "marry", "parents", "rename" })

private int do_list(object me, string game, int whole_game, int extinct);
private int do_house(object me, string surname);
private object family_target(object me, string who);
private string family_display(object who);
private int do_found(object me, string * args);
private int do_join(object me, string game, string * args);
private int do_leave(object me, string * args);
private int do_marry(object me, string * args);
private int do_parents(object me, string * args);
private int do_rename(object me, string * args);

void setup()
{
  set_aliases(({ "families", "familias" }));
  set_usage("families [ all [extinct] | <surname> | found [surname] | " +
            "join <surname> <who> | leave <who> | marry <who> <to whom> | " +
            "parents <who> <a> [b] | rename <surname> <new surname> ]");
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
    "  families rename <surname> <new> give a house another surname\n" +
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
    "leave, marry, parents and rename are subcommands, so no house can be looked up " +
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

// The communities of a game: each root area, which keeps the books of its
// houses and the census of its people.
private object * game_communities(string game)
{
  string * paths;
  object * out;
  object area;
  int i;

  paths = (string *)AREA_HANDLER->query_area_paths(game);
  out = ({ });
  for (i = 0; i < sizeof(paths); i++)
  {
    area = AREA_HANDLER->query_area(paths[i]);
    if (area && area->query_root_area() == area)
      out += ({ area });
  }

  return out;
}

// A house file without what every one of the game shares, the save directory
// and the extension: "<area>/rooms/<file>", and " #<n>" for one household of a
// shared house.
private string short_house_path(string file)
{
  string rest, unit;

  unit = "";
  if (sscanf(file, "%s" + HOUSE_UNIT_MARK + "%s", file, unit) == 2)
    unit = " " + HOUSE_UNIT_MARK + unit;

  if (sscanf(file, "/save/games/%*s/locations/areas/%s", rest) != 2)
    rest = file;
  if (strlen(rest) > 2 && rest[strlen(rest) - 2 ..] == ".o")
    rest = rest[0 .. strlen(rest) - 3];

  return rest + unit;
}

// The census row of a member, from whichever community counts them; nil for a
// player or anybody no census knows.
private mapping census_row(object * communities, string id)
{
  string uuid;
  mapping row;
  int i;

  if (sscanf(id, FAMILY_NPC + "%s", uuid) != 1)
    return nil;

  for (i = 0; i < sizeof(communities); i++)
  {
    row = (mapping)communities[i]->query_npc_census()[uuid];
    if (row)
      return row;
  }

  return nil;
}

// What a member does: the job they hold, else the kind of person they were
// made as; "player" for a player.
private string member_trade(object * communities, string id)
{
  mapping row;
  string * parts;

  if (sscanf(id, FAMILY_PLAYER + "%*s") == 1)
    return "player";

  row = census_row(communities, id);
  if (!row)
    return "-";
  if (stringp(row[CENSUS_VACANCY]) && strlen(row[CENSUS_VACANCY]))
    return row[CENSUS_VACANCY];
  if (stringp(row["source"]))
  {
    parts = explode(row["source"], "/");
    return parts[sizeof(parts) - 1];
  }

  return "-";
}

// ===== families [all [extinct]] =====
private int do_list(object me, string game, int whole_game, int extinct)
{
  string * names, * members, * houses, * parts;
  string * * rows;
  string where, citizenship;
  object area, fam;
  int i, j, living, players, hidden;

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

  rows = ({ ({ "house", "of", "living", "players", "gone", "property",
                "where", "" }) });
  for (i = 0; i < sizeof(names); i++)
  {
    members = (string *)fam->query_members(names[i]);
    living = sizeof(members);
    players = 0;
    for (j = 0; j < living; j++)
      if (sscanf(members[j], FAMILY_PLAYER + "%*s") == 1)
        players++;

    houses = (string *)fam->query_houses(names[i]);
    citizenship = (string)fam->query_family(names[i])[FAMILY_CITIZENSHIP];
    parts = explode(citizenship ? citizenship : "", "/");

    rows += ({ ({ names[i],
                  sizeof(parts) ? replace_string(parts[sizeof(parts) - 1],
                                                 ".c", "") : "-",
                  "" + living,
                  "" + players,
                  "" + (map_sizeof((mapping)fam->query_history(
                                      names[i])) - living),
                  "" + sizeof(houses),
                  sizeof(houses) ? short_house_path(houses[0]) +
                    (sizeof(houses) > 1 ? " +" + (sizeof(houses) - 1) : "")
                    : "-",
                  fam->is_extinct(names[i]) ? "extinct"
                    : (living ? "" : "nobody has moved in yet") }) });
  }

  write("Houses of " + where + ": " + sizeof(names) + "\n" + columns(rows) +
        (hidden ? hidden + " extinct house" + (hidden == 1 ? "" : "s") +
                  " not shown ('families all extinct' lists them).\n" : ""));
  return 1;
}

// ===== families <surname> =====
private int do_house(object me, string surname)
{
  mapping history, row;
  object * communities;
  object fam, place;
  string * ids, * props, * parents, * residents, * names, * kind;
  string * * rows;
  string out, file;
  mixed spouse, fate;
  int i, j, k, shared;

  fam = handler("families", me);
  if (!fam->has_family(surname))
  {
    notify_fail("No house of that name here.\n");
    return 0;
  }

  ids = (string *)fam->query_members(surname);
  history = (mapping)fam->query_history(surname);
  props = (string *)fam->query_houses(surname);
  communities = game_communities(game_name(me));

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
    rows = ({ ({ "living", "trade", "married to", "child of" }) });
    for (i = 0; i < sizeof(ids); i++)
    {
      // no (string) cast on the spouse: the unmarried have none, and the cast
      // is a conversion kfun that errors on nil
      spouse = fam->query_spouse(ids[i]);
      parents = (string *)fam->query_parents(ids[i]);

      rows += ({ ({
        (string)fam->query_member_name(surname, ids[i]),
        member_trade(communities, ids[i]),
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
    rows = ({ ({ "property", "where", "who lives there", "" }) });
    for (i = 0; i < sizeof(communities); i++)
    {
      props = (string *)communities[i]->query_houses_owned_by(surname);
      for (j = 0; j < sizeof(props); j++)
      {
        // a household of a shared house is the location's file plus a mark
        file = (string)communities[i]->query_house_file(props[j]);
        place = nil;
        catch(place = load_object(LOCATION_HANDLER)->load_location(file));

        residents = (string *)communities[i]->query_house_residents(props[j]);
        names = ({ });
        for (k = 0; k < sizeof(residents); k++)
        {
          row = (mapping)communities[i]->query_npc_census()[residents[k]];
          names += ({ (row && stringp(row["name"])) ? capitalize(row["name"])
                                                     : residents[k] });
        }

        kind = ({ });
        if (communities[i]->is_kept_house(file))
          kind += ({ "kept" });
        if (communities[i]->is_communal_house(file))
          kind += ({ "communal" });
        shared = (int)communities[i]->query_shared_house(file);
        if (shared > 1)
          kind += ({ "shared by " + shared });

        rows += ({ ({ place ? (string)place->short() : "-",
                      short_house_path(props[j]),
                      sizeof(names) ? implode(names, ", ") : "nobody",
                      implode(kind, ", ") }) });
      }
    }
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

// ===== families rename <surname> <new surname> =====
private int do_rename(object me, string * args)
{
  object fam;
  string old_surname, new_surname;

  if (sizeof(args) != 3)
  {
    notify_fail("Usage: families rename <surname> <new surname>\n");
    return 0;
  }

  fam = handler("families", me);
  old_surname = capitalize(args[1]);
  new_surname = capitalize(args[2]);

  if (!fam->has_family(old_surname))
  {
    notify_fail("No house of that name here.\n");
    return 0;
  }
  if (fam->has_family(new_surname) ||
      member_array(lower_case(new_surname), FAMILY_VERBS) != -1)
  {
    notify_fail("'" + new_surname + "' cannot be taken: a house bears it, " +
                "or it is a subcommand.\n");
    return 0;
  }

  if (!fam->rename_family(old_surname, new_surname))
  {
    notify_fail("The house could not be renamed.\n");
    return 0;
  }

  write("House " + old_surname + " is now house " + new_surname + ". Its " +
        "people answer to it from the next time they are materialized.\n");
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
  if (args[0] == "rename")
    return do_rename(me, args);

  // any other single word is a surname; the subcommand words are reserved
  if (sizeof(args) == 1 && member_array(args[0], FAMILY_VERBS) == -1)
    return do_house(me, capitalize(args[0]));

  notify_fail("Usage: " + query_usage() + "\n");
  return 0;
}
