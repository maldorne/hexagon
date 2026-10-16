// The fines office's behaviour, shared by the fines office room
// (/lib/room/justice/fines-office.c) and the fines location component
// (/lib/location/components/fines.c). The office is where what a record owes
// is paid, and where somebody else's bail is posted. The law is the one of the
// place's area (the crimes handler's jurisdiction), unless the office names
// its own citizenship (set_fines_law); the fine is the points the player has
// in that country times the fine per point, grown by the player's level (see
// /lib/handlers/crimes.md).
//
// Commands (Spanish and English): perdon / owe (what you owe), pagar / pay
// (settle it: the points are wiped), fianza / bail <player> (pay the fine of a
// prisoner and free them).
//
// Whoever inherits it says where the office stands (query_fines_place) and
// puts up its sign (create_fines_sign).

#include <language.h>
#include <translations/language.h>

// the help document the office sign shows, read every time the sign is put up
#define FINES_SIGN_DOC "help/world/fines"

string fines_law;
static object fines_sign;

/** Marks the place as a fines office; the conversion looks for it. */
int query_fines_office() { return 1; }

/** The room or location the office stands in. */
object query_fines_place() { return this_object(); }

/** Names the citizenship whose law rules the office, instead of the area's. */
void set_fines_law(string citizenship) { fines_law = citizenship; }

/** The citizenship named with set_fines_law, nil when the area's rules. */
string query_fines_law() { return (fines_law && strlen(fines_law)) ? fines_law : nil; }

/**
 * The citizenship whose law rules the office.
 *
 * @return the citizenship, "" when none does
 */
string query_fines_jurisdiction()
{
  object place;

  if (fines_law && strlen(fines_law))
    return fines_law;

  place = query_fines_place();
  if (!place)
    return "";

  return (string)handler("crimes", place)->query_jurisdiction(place);
}

/**
 * Puts up the sign with the instructions, read now from the help document so
 * changing it changes every office sign.
 *
 * @param where the room or location that holds the sign
 * @return the sign
 */
object create_fines_sign(object where)
{
  if (fines_sign)
    fines_sign->dest_me();

  fines_sign = where->add_sign(_LANG_FINES_SIGN_LONG,
    handler("help")->query_document_text(doc(FINES_SIGN_DOC)),
    nil, nil, nil, STD_LANG);
  return fines_sign;
}

/** Takes the sign down again, when the office leaves its place. */
void take_down_fines_sign()
{
  if (fines_sign)
    fines_sign->dest_me();
  fines_sign = nil;
}

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
  law = query_fines_jurisdiction();
  if (!who)
    return 0;

  if (!strlen(law))
  {
    write(_LANG_FINES_NO_LAW);
    return 1;
  }

  crimes = handler("crimes", query_fines_place());
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
  law = query_fines_jurisdiction();
  if (!who)
    return 0;

  if (!strlen(law))
  {
    write(_LANG_FINES_NO_LAW);
    return 1;
  }

  crimes = handler("crimes", query_fines_place());
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
  law = query_fines_jurisdiction();
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

  crimes = handler("crimes", query_fines_place());
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

mixed * stats()
{
  string law;

  law = query_fines_jurisdiction();
  return ({
    ({ "Fines Law", strlen(law) ? law : "none", }),
  });
}
