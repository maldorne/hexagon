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

void create()
{
  citizenship = "";
}

// The area's citizenship (a diplomacy-graph name), or "" if none.
string query_citizenship() { return citizenship; }
void set_citizenship(string name)
{
  citizenship = name ? name : "";
  this_object()->save_me();

}

// The citizenship object an NPC born in this area carries. Citizenship is the
// nationality: a resident belongs to the country at the top of the parent
// chain, not to the town they happen to live in, so a citizen of a town under
// a country carries the country. Empty when the area has no citizenship.
string query_root_citizenship_path()
{
  string game, root;

  if (!strlen(citizenship))
    return "";

  game = game_from_path((string)this_object()->query_area_path());
  root = DIPLOMACY_HANDLER->query_root_citizenship(game, citizenship);

  if (!root || !strlen(root))
    root = citizenship;

  return "/games/" + game + "/obj/citizenships/" + root;
}

// The citizenship whose naming pool applies to an NPC born in this area.
//
// Membership and naming are deliberately different questions. An area with no
// citizenship of its own hands out no nationality -- that is what
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
