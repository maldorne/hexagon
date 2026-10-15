// Home component. A dwelling raised on a plot: it provides the title and the
// description of a house interior, and is what furniture and decoration will
// hang off, saved in the location's own .o.
//
// Who lives here and which family owns the place are not kept here: they are on
// the area's books of houses (/lib/location/area/housing.c), which can be read
// and written without loading the house.

#include <room/location.h>
#include <translations/houses.h>

inherit component "/lib/location/component.c";

// What this dwelling is called and looks like, when it is not just a house: a
// barracks, a guildhall, a mill. Left unset it reads as an ordinary home.
private string home_short;
private string home_long;

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_HOME);
  home_short = nil;
  home_long = nil;
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}

string query_home_short() { return home_short; }
void set_home_short(string s) { home_short = (s && strlen(s)) ? s : nil; }

string query_home_long() { return home_long; }
void set_home_long(string s) { home_long = (s && strlen(s)) ? s : nil; }

// Programmer summary: who lives here and who owns it, from the area's books.
string query_info()
{
  object area;
  string * living, * units, * owners;
  mixed owner;
  string file, s;
  int i;

  area = query_entity() ? query_entity()->query_area() : nil;
  file = query_entity() ? query_entity()->query_file_name() : nil;
  units = (area && file) ? area->query_house_units(file) : ({ });

  // a shared house answers for every household in it
  living = ({ });
  owners = ({ });
  for (i = 0; i < sizeof(units); i++)
  {
    living += (string *)area->query_house_residents(units[i]);
    owner = area->query_house_owner(units[i]);
    if (stringp(owner))
      owners += ({ owner });
  }

  s = "residents: " + (sizeof(living) ? implode(living, ", ") : "none");
  if (sizeof(owners))
    s += "; owner" + (sizeof(owners) > 1 ? "s" : "") + ": " + implode(owners, ", ");
  if (sizeof(units) > 1)
    s += "; households: " + sizeof(units);
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
  string * units, * owners;
  mixed owner;
  object area;
  int i;

  if (args[0] && strlen(args[0]))
    return "";

  text = home_long ? home_long : _LANG_HOME_LONG;

  // A house that belongs to a family says whose it is, as the area's books have
  // it; a family that has died out owns nothing. A shared house names every
  // family living in it.
  area = query_entity()->query_area();
  units = area ? area->query_house_units(query_entity()->query_file_name())
               : ({ });
  owners = ({ });
  for (i = 0; i < sizeof(units); i++)
  {
    owner = area->query_house_owner(units[i]);
    if (stringp(owner) && strlen(owner) && member_array(owner, owners) == -1)
      owners += ({ owner });
  }
  if (sizeof(owners) == 1)
    text += _LANG_HOME_FAMILY(owners[0]);
  else if (sizeof(owners) > 1)
    text += _LANG_HOME_FAMILIES(query_multiple_short(owners));

  return ({ HOOK_EXCLUSIVE, text });
}

// Persisted state: what the dwelling is called and looks like rides in the
// location's .o. Extend both halves in lockstep as furniture / decor fields are
// added.
mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "home_short": home_short,
            "home_long":  home_long ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!undefinedp(args["home_short"]))
    home_short = args["home_short"];
  if (!undefinedp(args["home_long"]))
    home_long = args["home_long"];
}

mixed * stats()
{
  return component::stats() +
         ({ ({ "Short", home_short }),
            ({ "Long", home_long }) });
}
