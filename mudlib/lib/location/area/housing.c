
// Housing: buildable plots and the houses raised on them.
//
// A plot is an empty lot the builder carved next to an existing location; a
// house is that same location once someone moves in. This file owns the plot
// registry, the act of raising a house on one, and the pass that gives every
// homeless settled citizen a home -- pairing a man and a woman into one house
// and giving whoever is left over a house of their own.
//
// Who is entitled to a house is a design-time fact, not a runtime guess: an NPC
// source is flagged "resident" on the area roster, or the job somebody holds is
// flagged the same way, and only those are housed.

#include <room/location.h>
#include <living/family.h>
#include <areas/area.h>
#include <basic/gender.h>
#include <areas/vacancy.h>

// Buildable lots waiting for a house, by location file.
string * plots;
// the area's raised houses, so it can answer which one a resident belongs to
string * houses;

// defined further down; build_house_on_plot registers each house it raises
void add_house(string file);
// defined further down; release_house asks it which house holds a resident
string query_house_of(string uuid);

// The area's fallback location file (where orphaned occupants go). "" if unset.
string principal;


void door_house_exits(object house);
void open_plot_exits(object plot);
int demote_house(string file);
private int _is_resident(object o);
private void _house_family(object * family);
private string _family_for(object * group);

void create()
{
  plots = ({ });
  houses = ({ });
  principal = "";
}

// Buildable-plot registry. The builder ring registers a freshly carved empty lot
// with add_plot; the housing system consumes one (and calls remove_plot) when it
// raises a house on it.
// A delegated area owns no building land of its own: the community's pool is
// what it builds on, so a citizen can be given a cottage among the fields as
// readily as a house on the street.
string * query_plots()
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (string *)owner->query_plots();

  return plots ? plots : ({ });
}

void add_plot(string file)
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->add_plot(file);
    return;
  }

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
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->remove_plot(file);
    return;
  }

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
  object house, owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (string)owner->build_house_on_plot(residents);

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

  // it is a house now, not an available plot -- but it stays on the books:
  // an area that forgets its houses cannot answer who lives where
  remove_plot(plot_file);
  add_house(plot_file);

  this_object()->log_event("Raised a house at " + plot_file + " for " +
            (residents && sizeof(residents) ? implode(residents, ", ")
                                            : "no residents") + ".");

  return plot_file;
}

// Raise a house on one named plot, rather than on whichever one is free. Used
// when a job's house is chosen by hand: the builder stands on the plot that is
// to become it. Returns the house's location file, or nil if that location is
// not a plot of this area.
string build_house_at(string file, string * residents)
{
  object house, owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (string)owner->build_house_at(file, residents);

  if (member_array(file, query_plots()) < 0)
    return nil;

  house = (object)this_object()->load_location(file);
  if (!house)
    return nil;

  house->remove_component(LOCATION_COMPONENT_PLOT);
  house->add_component(LOCATION_COMPONENT_HOME,
                       ([ "residents": residents ? residents : ({ }) ]));
  house->save_me();

  door_house_exits(house);

  remove_plot(file);
  add_house(file);

  this_object()->log_event("Raised a house at " + file + " for " +
            (residents && sizeof(residents) ? implode(residents, ", ")
                                            : "no residents") + ".");

  return file;
}

// Re-type a raised house's exits to "door": the exit(s) the house carries (a
// plot-derived house has one, back to the location it was carved from) and each
// neighbour's reciprocal exit. A plot is carved with plain "open" passages; once
// it becomes a home it gets a real front door on both sides. A house door starts
// closed by default (the options ride in the exit map, so it reopens closed on
// every load); a resident opens it to come and go.
// The inverse of door_house_exits: a demoted house has no front door any more,
// so both sides of every exit go back to a plain doorway.
void open_plot_exits(object plot)
{
  mapping pex, nex;
  string * dirs, * ndirs;
  string pfile;
  int i, j;

  if (!plot)
    return;

  pfile = plot->query_file_name();
  pex = plot->query_exit_map();
  dirs = pex ? map_indices(pex) : ({ });

  for (i = 0; i < sizeof(dirs); i++)
  {
    string dest;
    object neighbour;

    dest = pex[dirs[i]][0];
    plot->add_exit(dirs[i], dest, "open");

    neighbour = (object)this_object()->load_location(dest);
    if (!neighbour)
      continue;

    nex = neighbour->query_exit_map();
    ndirs = nex ? map_indices(nex) : ({ });
    for (j = 0; j < sizeof(ndirs); j++)
      if (nex[ndirs[j]][0] == pfile)
        neighbour->add_exit(ndirs[j], pfile, "open");
    neighbour->save_me();
  }

  plot->save_me();
}

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
// Record a place as a family's, on both sides: the register lists it among the
// house's property, and the location itself names its owner so a door can ask
// without going through the handler. Two records of one fact, kept in step
// here, the way an NPC's address and its house's resident list are.
void claim_property(string surname, string file)
{
  object place, home;

  if (!surname || !strlen(surname) || !file || !strlen(file))
    return;

  handler("families", this_object())->add_property(surname, file);

  place = load_object(LOCATION_HANDLER)->load_location(file);
  if (!place)
    return;

  home = place->query_component_by_type(LOCATION_COMPONENT_HOME);
  if (!home)
    return;

  home->set_home_owner(surname);
  place->save_me();
}

// The house a group of people will live under. Whoever already belongs to one
// brings the others into it; a group of strangers founds a new one, named from
// the citizenship they were born into. A draft area founds nothing -- a house
// outlives the locations it stands in, so it waits until the place is settled.
private string _family_for(object * group)
{
  string citizenship, surname;
  int i;

  for (i = 0; i < sizeof(group); i++)
    if (group[i]->query_family())
      surname = (string)group[i]->query_family();

  if (!surname)
  {
    if ((string)this_object()->query_area_state() != AREA_SETTLED)
      return nil;

    citizenship = (string)this_object()->query_root_citizenship_path();
    surname = (string)handler("families", this_object())->mint_surname(citizenship);
    if (!surname || !strlen(surname))
      return nil;
    if (!handler("families", this_object())->found_family(surname, citizenship))
      return nil;
  }

  for (i = 0; i < sizeof(group); i++)
    if (!group[i]->query_family())
      group[i]->set_family(surname,
        group[i]->query_given_name()
          ? capitalize((string)group[i]->query_given_name())
          : (string)group[i]->query_cap_name());

  // a couple housed together is a couple
  if (sizeof(group) == 2)
    handler("families", this_object())->set_spouse((string)group[0]->query_family_id(),
                                     (string)group[1]->query_family_id());

  return surname;
}

private void _house_family(object * family)
{
  string house, surname;
  string * ids;
  int i;

  // the house they will live under, founded now if they had none
  surname = _family_for(family);

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

  // A house belongs to the family living in it, not to the people one by one:
  // that is what lets it outlast them, and what a door asks before it opens.
  if (!surname || !strlen(surname))
    return;

  claim_property(surname, house);
}

string * query_houses()
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (string *)owner->query_houses();

  return houses ? houses : ({ });
}

void add_house(string file)
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->add_house(file);
    return;
  }

  if (!houses)
    houses = ({ });
  if (member_array(file, houses) < 0)
  {
    houses += ({ file });
    this_object()->save_me();
  }
}


// Move `uuid` out of whatever house lists it. Called when an NPC dies: the
// census entry goes, and the house has to stop expecting a tenant who is never
// coming back -- otherwise its resident list only ever grows, and the NPC that
// replaces the dead one finds no room to move into.
void release_house(string uuid)
{
  object house, home, owner;
  string file;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->release_house(uuid);
    return;
  }

  file = query_house_of(uuid);
  if (!strlen(file))
    return;

  house = (object)this_object()->load_location(file);
  if (!house)
    return;

  home = house->query_component_by_type(LOCATION_COMPONENT_HOME);
  if (!home)
    return;

  home->remove_resident(uuid);
  house->save_me();
}

// Make the houses agree with the address an NPC carries: the one it names lists
// it, and no other does.
//
// The NPC's address and a house's resident list are two records of one fact,
// written by different paths -- housing writes both when it raises a house, a
// POI vacancy writes only the NPC's side when it re-homes a replacement, and a
// move writes the new house without telling the old one. Reconciling from the
// address here covers all three, and clears the stale entry a move leaves
// behind.
void claim_house(string uuid, string file)
{
  string * all;
  object owner;
  int i;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->claim_house(uuid, file);
    return;
  }

  if (!uuid || !strlen(uuid) || !file || !strlen(file))
    return;

  all = query_houses();

  for (i = 0; i < sizeof(all); i++)
  {
    object house, home;
    int listed;

    house = (object)this_object()->load_location(all[i]);
    if (!house)
      continue;

    home = house->query_component_by_type(LOCATION_COMPONENT_HOME);
    if (!home)
      continue;

    listed = (member_array(uuid, (string *)home->query_residents()) != -1);

    if (all[i] == file && !listed)
      home->add_resident(uuid);
    else if (all[i] != file && listed)
      home->remove_resident(uuid);
    else
      continue;

    house->save_me();
  }
}

// Turn a house back into a bare plot: evict its residents, drop the home
// component for a plot one, and put it back on the free-plot list.
//
// The inverse of build_house_on_plot, and the only way a house that was raised
// in the wrong place can be undone -- plot removal refuses anything that is not
// a bare plot, so without this a misplaced house is permanent. Residents are
// left homeless on purpose: the caller decides where they go next, and a plain
// `build homes` will re-house them on any free plot.
//
// Returns the number of residents evicted, or -1 if `file` is not a house here.
int demote_house(string file)
{
  object house, home, owner;
  string * living_here;
  int i, evicted;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (int)owner->demote_house(file);

  if (!file || !strlen(file))
    return -1;

  house = (object)this_object()->load_location(file);
  if (!house)
    return -1;

  home = house->query_component_by_type(LOCATION_COMPONENT_HOME);
  if (!home)
    return -1;

  living_here = (string *)home->query_residents();

  for (i = 0; i < sizeof(living_here); i++)
  {
    object npc;

    npc = (object)this_object()->live_npc(living_here[i]);
    if (npc)
    {
      npc->set_home(nil);
      npc->save_npc();
    }
    evicted++;
  }

  house->remove_component(LOCATION_COMPONENT_HOME);
  house->add_component(LOCATION_COMPONENT_PLOT, ([ ]));
  house->save_me();

  // the front door goes back to being an open doorway, on both sides
  open_plot_exits(house);

  houses -= ({ file });
  add_plot(file);
  this_object()->save_me();

  this_object()->log_event("Demoted the house at " + file + " back to a plot" +
            (evicted ? ", evicting " + evicted + " resident(s)" : "") + ".");

  return evicted;
}


// The house that lists `uuid` among its residents, or "" when nobody does.
//
// A housed NPC keeps its address on its own .o, but the house keeps the
// resident list on the location, so the link is recorded at both ends. The
// house is the authoritative end: the location .o outlives any individual, and
// an NPC that lost its save file (or was rebuilt) comes back with no address
// while the house still names it. Reading the link back from here is what lets
// the pair heal instead of silently drifting apart.
string query_house_of(string uuid)
{
  object owner;
  int i;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (string)owner->query_house_of(uuid);

  if (!uuid || !strlen(uuid))
    return "";

  for (i = 0; i < sizeof(houses); i++)
  {
    object house, home;

    house = (object)this_object()->load_location(houses[i]);
    if (!house)
      continue;

    home = house->query_component_by_type(LOCATION_COMPONENT_HOME);
    if (!home)
      continue;

    if (member_array(uuid, (string *)home->query_residents()) != -1)
      return houses[i];
  }

  return "";
}

// Whether a live NPC is a settled resident -- one the design declared as such.
// Who gets a house is a design-time decision, not a runtime guess from the NPC's
// race or behaviour: an NPC source (template) is flagged "resident" in
// npc_caps by the builder, and only those sources are housed here. Animals,
// guards and any unflagged roster filler are never handed a house; POI vacancies
// (barman, shopkeeper) are not in npc_caps at all and carry their own fixed
// home instead.
private int _is_resident(object o)
{
  mapping spec, entry, job;

  if (!o || !o->query_npc())
    return 0;

  spec = ((mapping)this_object()->query_npc_caps())[o->query_npc_source()];
  if (spec && spec["resident"])
    return 1;

  // Somebody holding a job is not on the roster at all, so the flag it would
  // have carried there lives on the job instead. A post with a house of its own
  // does not come through here: its holder is housed by the post.
  entry = ((mapping)this_object()->query_npc_census())[(string)o->query_npc_uuid()];
  if (!entry || !entry[CENSUS_VACANCY])
    return 0;

  job = (mapping)this_object()->query_vacancy(entry[CENSUS_VACANCY]);
  return job && job[VACANCY_RESIDENT] && !job[VACANCY_HOME];
}

// Give every homeless resident a home, pairing a man and a woman into one house
// (a family) and giving leftovers a house of their own. Operates on the NPCs
// currently materialized in the area's loaded locations; residency is the
// design-time fact tested by _is_resident, not a runtime type guess. Stops
// quietly when plots run out (each miss is logged).
// Drop residents no census row accounts for. A house is written by one path and
// the census by another, so a person taken off the books without dying -- a
// retired type, a repaired id -- leaves an address behind that nothing can
// resolve to a name.
private void _drop_unknown_residents()
{
  mapping census;
  string * all;
  int i;

  census = (mapping)this_object()->query_npc_census();
  all = query_houses();

  for (i = 0; i < sizeof(all); i++)
  {
    object house, home;
    string * living, * known;
    int j;

    house = (object)this_object()->load_location(all[i]);
    if (!house)
      continue;

    home = house->query_component_by_type(LOCATION_COMPONENT_HOME);
    if (!home)
      continue;

    living = (string *)home->query_residents();
    known = ({ });
    for (j = 0; j < sizeof(living); j++)
      if (census[living[j]])
        known += ({ living[j] });

    if (sizeof(known) == sizeof(living))
      continue;

    home->set_residents(known);
    house->save_me();
  }
}

void assign_homes()
{
  object * everyone, * homeless, * males, * females, * loaded;
  int i;

  _drop_unknown_residents();

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
