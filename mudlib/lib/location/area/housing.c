
// Housing: buildable plots and the houses raised on them.
//
// A plot is an empty lot the builder carved next to an existing location; a
// house is that same location once someone moves in. This file owns the plot
// registry, the act of raising a house on one, and the pass that gives every
// homeless settled citizen a home -- pairing a man and a woman into one house
// and giving whoever is left over a house of their own.
//
// Who is entitled to a house is a design-time fact, not a runtime guess: an NPC
// source is flagged "resident" on the area roster and only those are housed.

#include <room/location.h>
#include <basic/gender.h>

// Buildable lots waiting for a house, by location file.
string * plots;

// The area's fallback location file (where orphaned occupants go). "" if unset.
string principal;


void door_house_exits(object house);
private int _is_resident(object o);
private void _house_family(object * family);

void create()
{
  plots = ({ });
  principal = "";
}

// Buildable-plot registry. The builder ring registers a freshly carved empty lot
// with add_plot; the housing system consumes one (and calls remove_plot) when it
// raises a house on it.
string * query_plots() { return plots ? plots : ({ }); }

void add_plot(string file)
{
  if (!plots)
    plots = ({ });
  if (member_array(file, plots) < 0)
  {
    plots += ({ file });
    this_object()->save_me();
  }
}

void remove_plot(string file)
{
  if (plots && member_array(file, plots) >= 0)
  {
    plots -= ({ file });
    this_object()->save_me();
  }
}

string query_principal() { return principal ? principal : ""; }
void set_principal(string file)
{
  principal = file ? file : "";
  this_object()->save_me();
}

// Raise a house on a free plot and move its residents in. Picks a random free
// plot (no neighbourhood grading yet), turns it from a bare plot into a house
// (drops the `plot` component, adds `home` with the residents), and drops it
// from the plots registry. Returns the house's location file, or nil (and logs)
// when there is no free plot. `residents` are the find_living ids / uuids that
// will live there; a family shares one call.
string build_house_on_plot(string * residents)
{
  string plot_file;
  object house;

  if (!plots || !sizeof(plots))
  {
    this_object()->log_event("No free plot available to house " +
              (residents && sizeof(residents) ? implode(residents, ", ")
                                              : "an NPC") + ".");
    return nil;
  }

  plot_file = plots[random(sizeof(plots))];
  house = load_object(LOCATION_HANDLER)->load_location(plot_file);
  if (!house)
    return nil;

  house->remove_component(LOCATION_COMPONENT_PLOT);
  house->add_component(LOCATION_COMPONENT_HOME,
                       ([ "residents": residents ? residents : ({ }) ]));
  house->save_me();

  // a raised house has a real front door: re-type the plot's open exits (both
  // the house side and the neighbour's reciprocal) to "door"
  door_house_exits(house);

  // it is a house now, not an available plot
  remove_plot(plot_file);

  this_object()->log_event("Raised a house at " + plot_file + " for " +
            (residents && sizeof(residents) ? implode(residents, ", ")
                                            : "no residents") + ".");

  return plot_file;
}

// Re-type a raised house's exits to "door": the exit(s) the house carries (a
// plot-derived house has one, back to the location it was carved from) and each
// neighbour's reciprocal exit. A plot is carved with plain "open" passages; once
// it becomes a home it gets a real front door on both sides. A house door starts
// closed by default (the options ride in the exit map, so it reopens closed on
// every load); a resident opens it to come and go.
void door_house_exits(object house)
{
  mapping hex, nex;
  string * dirs, * ndirs;
  string hfile;
  int i, j;

  if (!house)
    return;

  hfile = house->query_file_name();
  hex = house->query_exit_map();
  dirs = hex ? map_indices(hex) : ({ });

  for (i = 0; i < sizeof(dirs); i++)
  {
    string dest;
    object neighbour;

    // exit_map[dir] = ({ dest, type, ... }); re-type this side to a closed door
    dest = hex[dirs[i]][0];
    house->add_exit(dirs[i], dest, "door", nil, ([ "closed" : 1 ]));

    // and the neighbour's exit that points back here
    neighbour = load_object(LOCATION_HANDLER)->load_location(dest);
    if (!neighbour)
      continue;
    nex = neighbour->query_exit_map();
    ndirs = nex ? map_indices(nex) : ({ });
    for (j = 0; j < sizeof(ndirs); j++)
      if (nex[ndirs[j]][0] == hfile)
        neighbour->add_exit(ndirs[j], hfile, "door", nil,
                            ([ "closed" : 1 ]));
    neighbour->save_me();
  }

  house->save_me();
}

// Raise one house for a family (one or two NPCs) and move them in: build the
// house, set each member's home to it, and persist them. No plot -> nothing
// happens (build_house_on_plot logged it).
private void _house_family(object * family)
{
  string house;
  string * ids;
  int i;

  ids = ({ });
  for (i = 0; i < sizeof(family); i++)
    ids += ({ family[i]->query_npc_uuid() });

  house = build_house_on_plot(ids);
  if (!house)
    return;

  for (i = 0; i < sizeof(family); i++)
  {
    family[i]->set_home(house);
    family[i]->save_npc();
  }
}

// Whether a live NPC is a settled resident -- one the design declared as such.
// Who gets a house is a design-time decision, not a runtime guess from the NPC's
// race or behaviour: an NPC source (template) is flagged "resident" in
// npc_intended by the builder, and only those sources are housed here. Animals,
// guards and any unflagged roster filler are never handed a house; POI vacancies
// (barman, shopkeeper) are not in npc_intended at all and carry their own fixed
// home instead.
private int _is_resident(object o)
{
  mapping spec;

  if (!o || !o->query_npc())
    return 0;

  spec = ((mapping)this_object()->query_npc_intended())[o->query_npc_source()];
  return spec && spec["resident"];
}

// Give every homeless resident a home, pairing a man and a woman into one house
// (a family) and giving leftovers a house of their own. Operates on the NPCs
// currently materialized in the area's loaded locations; residency is the
// design-time fact tested by _is_resident, not a runtime type guess. Stops
// quietly when plots run out (each miss is logged).
void assign_homes()
{
  object * everyone, * homeless, * males, * females, * loaded;
  int i;

  loaded = (object *)this_object()->query_loaded_locations();
  everyone = ({ });
  for (i = 0; i < sizeof(loaded); i++)
    if (loaded[i])
      everyone += all_inventory(loaded[i]);

  homeless = ({ });
  for (i = 0; i < sizeof(everyone); i++)
  {
    object o;
    o = everyone[i];
    // a resident placed by the design, still persisted and without a home yet
    if (o && o->query_persisted() && !o->query_home() && _is_resident(o))
      homeless += ({ o });
  }

  males = ({ });
  females = ({ });
  for (i = 0; i < sizeof(homeless); i++)
    if (homeless[i]->query_gender() == GENDER_FEMALE)
      females += ({ homeless[i] });
    else
      males += ({ homeless[i] });

  // a man and a woman share a house; whoever is left over gets one alone
  while (sizeof(males) && sizeof(females))
  {
    _house_family(({ males[0], females[0] }));
    males = males[1..];
    females = females[1..];
  }
  while (sizeof(males))
  {
    _house_family(({ males[0] }));
    males = males[1..];
  }
  while (sizeof(females))
  {
    _house_family(({ females[0] }));
    females = females[1..];
  }
}
