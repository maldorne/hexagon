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

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_HOME);
  home_owner = nil;
  residents = ({ });
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}

string query_home_owner() { return home_owner; }
void set_home_owner(string s) { home_owner = s; }

string * query_residents() { return residents ? residents : ({ }); }
void set_residents(string * r) { residents = r ? r : ({ }); }
void add_resident(string r)
{
  if (!residents)
    residents = ({ });
  if (member_array(r, residents) < 0)
    residents += ({ r });
}

mapping query_hooks()
{
  return ([ "short": HOOK_PRIORITY_STRUCTURE,
            "long":  HOOK_PRIORITY_STRUCTURE ]);
}

// The house interior's title. Exclusive: it is the whole short.
mixed hook_short(mixed * args)
{
  return ({ HOOK_EXCLUSIVE, _LANG_HOME_SHORT });
}

// The house interior's description. args = ({ str, dark }); a non-empty str
// means the player is looking at a specific item, so contribute nothing. Future
// furniture/decoration will extend this rather than replace it.
mixed hook_long(mixed * args)
{
  if (args[0] && strlen(args[0]))
    return "";
  return ({ HOOK_EXCLUSIVE, _LANG_HOME_LONG });
}

// Persisted state: owner + residents ride in the location's .o. Extend both
// halves in lockstep as furniture / décor fields are added.
mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "home_owner": home_owner,
            "residents":  residents ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!undefinedp(args["home_owner"]))
    home_owner = args["home_owner"];
  if (!undefinedp(args["residents"]))
    residents = args["residents"];
}

mixed * stats()
{
  return component::stats() +
         ({ ({ "Owner", home_owner }),
            ({ "Residents", residents }) });
}
