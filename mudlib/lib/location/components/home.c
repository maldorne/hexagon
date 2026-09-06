// Home component. A dwelling raised on a plot: it provides the default title and
// description of a house interior and, crucially, persists who lives there and
// who owns it. That persisted state is the substrate the future builds on --
// furniture, decoration, and player-owned houses all hang off this component,
// saved in the location's own .o.
//
// A family (a man and a woman, and later their children) shares one home; guards
// instead share a barracks. The area assigns residents when it raises the house.

#include <room/location.h>
#include <translations/houses.h>

inherit component "/lib/location/component.c";

// Who owns the house: a citizenship path, a player name, or nil while
// unassigned. Kept generic on purpose so an NPC family today and a player owner
// tomorrow use the same field.
private string home_owner;
// The NPCs that live here (uuids / find_living ids). A family shares this list.
private string * residents;
// What this dwelling is called and looks like, when it is not just a house: a
// barracks, a guildhall, a mill. Left unset it reads as an ordinary home.
private string home_short;
private string home_long;

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_HOME);
  home_owner = nil;
  residents = ({ });
  home_short = nil;
  home_long = nil;
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}

string query_home_owner() { return home_owner; }
void set_home_owner(string s) { home_owner = s; }

string query_home_short() { return home_short; }
void set_home_short(string s) { home_short = (s && strlen(s)) ? s : nil; }

string query_home_long() { return home_long; }
void set_home_long(string s) { home_long = (s && strlen(s)) ? s : nil; }

string * query_residents() { return residents ? residents : ({ }); }
void set_residents(string * r) { residents = r ? r : ({ }); }
void add_resident(string r)
{
  if (!residents)
    residents = ({ });
  if (member_array(r, residents) < 0)
    residents += ({ r });
}

void remove_resident(string r)
{
  if (!residents)
    return;
  residents -= ({ r });
}

// Programmer summary: who lives here (and the owner, if set). Residents are the
// find_living ids / uuids the housing system stored.
string query_info()
{
  string s;

  s = "residents: " +
      (residents && sizeof(residents) ? implode(residents, ", ") : "none");
  if (home_owner && strlen(home_owner))
    s += "; owner: " + home_owner;
  return s;
}

mapping query_hooks()
{
  return ([ "short": HOOK_PRIORITY_STRUCTURE,
            "long":  HOOK_PRIORITY_STRUCTURE ]);
}

// The house interior's title. Exclusive: it is the whole short. A dwelling
// that is something more particular than a house says so itself.
mixed hook_short(mixed * args)
{
  return ({ HOOK_EXCLUSIVE, home_short ? home_short : _LANG_HOME_SHORT });
}

// The house interior's description. args = ({ str, dark }); a non-empty str
// means the player is looking at a specific item, so contribute nothing. Future
// furniture/decoration will extend this rather than replace it.
mixed hook_long(mixed * args)
{
  string text;

  if (args[0] && strlen(args[0]))
    return "";

  text = home_long ? home_long : _LANG_HOME_LONG;

  // A house that belongs to a family says whose it is. The register is asked
  // rather than the name being printed on trust, so a surname left behind by a
  // house that has died out stops claiming the place.
  if (home_owner && strlen(home_owner) &&
      handler("families", query_my_location())->has_family(home_owner))
    text += _LANG_HOME_FAMILY(home_owner);

  return ({ HOOK_EXCLUSIVE, text });
}

// Persisted state: owner + residents ride in the location's .o. Extend both
// halves in lockstep as furniture / décor fields are added.
mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "home_owner": home_owner,
            "residents":  residents,
            "home_short": home_short,
            "home_long":  home_long ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!undefinedp(args["home_owner"]))
    home_owner = args["home_owner"];
  if (!undefinedp(args["residents"]))
    residents = args["residents"];
  if (!undefinedp(args["home_short"]))
    home_short = args["home_short"];
  if (!undefinedp(args["home_long"]))
    home_long = args["home_long"];
}

mixed * stats()
{
  return component::stats() +
         ({ ({ "Owner", home_owner }),
            ({ "Residents", residents }),
            ({ "Short", home_short }),
            ({ "Long", home_long }) });
}
