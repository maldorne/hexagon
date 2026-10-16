// Fines component. The location is the office where what a record owes is
// paid, and where somebody else's bail is posted. The law is the one of the
// place's area (the crimes handler's jurisdiction); the fine is the points the
// player has in that country times the fine per point, grown by the player's
// level (see /lib/handlers/crimes.md).
//
// Commands (Spanish and English): perdon / owe (what you owe), pagar / pay
// (settle it: the points are wiped), fianza / bail <player> (pay the fine of a
// prisoner and free them).

#include <room/location.h>
#include <language.h>

inherit component "/lib/location/component.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_FINES);
}

void dest_me() {}

void init()
{
  string * verbs;
  int i;

  verbs = _LANG_FINES_CMDS_OWED;
  for (i = 0; i < sizeof(verbs); i++)
    add_action("do_owed", verbs[i]);

  verbs = _LANG_FINES_CMDS_PAY;
  for (i = 0; i < sizeof(verbs); i++)
    add_action("do_pay", verbs[i]);

  verbs = _LANG_FINES_CMDS_BAIL;
  for (i = 0; i < sizeof(verbs); i++)
    add_action("do_bail", verbs[i]);
}

// The citizenship whose law rules the office, "" when none does.
private string _jurisdiction()
{
  if (!query_entity())
    return "";

  return (string)handler("crimes", query_entity())->query_jurisdiction(
           query_entity());
}

// "$tag$" -> value, for each pair of a mapping.
private string _fill(string text, mapping values)
{
  string * tags;
  int i;

  tags = map_indices(values);
  for (i = 0; i < sizeof(tags); i++)
    text = replace_string(text, "$" + tags[i] + "$", "" + values[tags[i]]);

  return text;
}

/** What the player owes. */
int do_owed(string str)
{
  object who, crimes;
  string law;
  int fine;

  who = this_player();
  law = _jurisdiction();
  if (!who)
    return 0;

  if (!strlen(law))
  {
    write(_LANG_FINES_NO_LAW);
    return 1;
  }

  crimes = handler("crimes", query_entity());
  fine = (int)crimes->query_fine_of(who, law);
  if (fine <= 0)
  {
    write(_LANG_FINES_OWE_NOTHING);
    return 1;
  }

  write(_fill(_LANG_FINES_OWE, ([
    "points" : (int)crimes->query_points(who->query_name(), law),
    "fine"   : fine ])));
  return 1;
}

/** Settles the fine: all of it, or nothing. */
int do_pay(string str)
{
  object who, crimes;
  string law;
  int fine, paid;

  who = this_player();
  law = _jurisdiction();
  if (!who)
    return 0;

  if (!strlen(law))
  {
    write(_LANG_FINES_NO_LAW);
    return 1;
  }

  crimes = handler("crimes", query_entity());
  fine = (int)crimes->query_fine_of(who, law);
  if (fine <= 0)
  {
    write(_LANG_FINES_OWE_NOTHING);
    return 1;
  }

  paid = (int)crimes->pay_fine(who, who, law);
  if (paid <= 0)
    write(_fill(_LANG_FINES_NO_MONEY, ([ "fine" : fine ])));
  else
    write(_fill(_LANG_FINES_PAID, ([ "fine" : paid ])));

  return 1;
}

/** Pays the fine of a prisoner, who is freed. */
int do_bail(string str)
{
  object who, prisoner, crimes;
  string law;
  int fine, paid;

  who = this_player();
  law = _jurisdiction();
  if (!who)
    return 0;

  if (!strlen(law))
  {
    write(_LANG_FINES_NO_LAW);
    return 1;
  }

  if (!str || !strlen(str))
  {
    notify_fail(_LANG_FINES_BAIL_USAGE);
    return 0;
  }

  prisoner = find_player(lower_case(str));
  if (!prisoner)
  {
    write(_LANG_FINES_BAIL_NOBODY);
    return 1;
  }

  crimes = handler("crimes", query_entity());
  fine = (int)crimes->query_fine_of(prisoner, law);
  paid = (int)crimes->bail(who, prisoner, law);

  switch (paid)
  {
    case -2:
      write(_LANG_FINES_BAIL_NOT_JAILED);
      break;
    case -1:
      write(_LANG_FINES_BAIL_NOTHING);
      break;
    case 0:
      write(_fill(_LANG_FINES_BAIL_NO_MONEY, ([ "fine" : fine ])));
      break;
    default:
      write(_fill(_LANG_FINES_BAIL_PAID, ([
        "fine" : paid, "who" : prisoner->query_cap_name() ])));
      break;
  }

  return 1;
}

string query_info()
{
  string law;

  law = _jurisdiction();
  return strlen(law) ? "law of " + law : "no law";
}
