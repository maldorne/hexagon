// The citizenship an area belongs to.
//
// An area names the polity that holds it, and everything that needs to know who
// somebody answers to reads it from here: their nationality, the pool their
// names are drawn from, whether a gate lets them through. The name is resolved
// against the diplomacy graph, which owns the parents and the relations.

#include <mud/config.h>
#include <areas/area.h>
#include <areas/diplomacy.h>

string citizenship;
// an area that says it belongs to nobody, rather than to its parent's
int stateless;

void create()
{
  citizenship = "";
  stateless = 0;
}

// The area's own citizenship (a diplomacy-graph name), or "" if it names none.
string query_citizenship() { return citizenship; }
void set_citizenship(string name)
{
  citizenship = name ? name : "";
  if (strlen(citizenship))
    stateless = 0;
  this_object()->save_me();
}

// Whether the area belongs to nobody on purpose (a road between two towns):
// it then inherits no citizenship from the areas it is part of.
int query_stateless() { return stateless; }
void set_stateless(int flag)
{
  stateless = flag ? 1 : 0;
  if (stateless)
    citizenship = "";
  this_object()->save_me();
}

// The citizenship the area belongs to: its own, or else the one of the nearest
// area it is part of, up the parent chain. An area marked stateless, or one
// under it, belongs to none. "" when nobody up the chain names one.
string query_effective_citizenship()
{
  object area;
  int steps;

  area = this_object();
  for (steps = 0; area && steps < AREA_MAX_ANCESTRY; steps++)
  {
    if (area->query_stateless())
      return "";
    if (strlen((string)area->query_citizenship()))
      return (string)area->query_citizenship();
    area = (object)area->query_parent_area();
  }

  return "";
}

// The area that gives this one its citizenship (itself, or an ancestor), or
// nil when it belongs to none.
object query_citizenship_source()
{
  object area;
  int steps;

  area = this_object();
  for (steps = 0; area && steps < AREA_MAX_ANCESTRY; steps++)
  {
    if (area->query_stateless())
      return nil;
    if (strlen((string)area->query_citizenship()))
      return area;
    area = (object)area->query_parent_area();
  }

  return nil;
}

// One line saying which citizenship the area belongs to and why, for the
// coders' reports: its own, its parent's, or none (on purpose or not).
string query_citizenship_description()
{
  object source;

  if (stateless)
    return "(none: belongs to nobody, on purpose)";
  if (strlen(citizenship))
    return citizenship + " (its own)";

  source = query_citizenship_source();
  if (source)
    return (string)source->query_citizenship() + " (from " +
           (string)source->query_area_name() + ")";

  return "(none)";
}

// The citizenship object an NPC born in this area carries. Citizenship is the
// nationality: a resident belongs to the country at the top of the diplomacy
// graph, not to the town they happen to live in, so a citizen of a town under
// a country carries the country. The town is the area's effective citizenship,
// so the farms of a town hand out the town's. Empty when the area belongs to
// none.
string query_root_citizenship_path()
{
  string game, root, held;

  held = query_effective_citizenship();
  if (!strlen(held))
    return "";

  game = game_from_path((string)this_object()->query_area_path());
  // handler(), not the path: the graph belongs to this area's game
  root = handler("diplomacy", this_object())->query_root_citizenship(held);

  if (!root || !strlen(root))
    root = held;

  return "/games/" + game + "/obj/citizenships/" + root;
}

// The citizenship whose naming pool applies to an NPC born in this area.
//
// Membership and naming are deliberately different questions. An area that
// belongs to nobody (marked stateless) hands out no nationality -- that is what
// query_root_citizenship_path answers, and an empty answer there means its
// NPCs carry none. Their names, though, still come from the region they live
// in, so the pool is taken from the nearest ancestor area that does have a
// citizenship: a road or a wilderness between two towns names its travellers
// like the land around them without making them subjects of it.
//
// The ancestors are the ones the areas were told about, not the ones their
// directories suggest.
string query_naming_citizenship_path()
{
  object area;
  mixed own;
  int steps;

  own = this_object()->query_root_citizenship_path();
  if (stringp(own) && strlen(own))
    return own;

  area = this_object();

  for (steps = 0; steps < AREA_MAX_ANCESTRY; steps++)
  {
    mixed inherited;

    area = (object)area->query_parent_area();
    if (!area)
      return "";

    inherited = area->query_root_citizenship_path();
    if (stringp(inherited) && strlen(inherited))
      return inherited;
  }

  return "";
}
