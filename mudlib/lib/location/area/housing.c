
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
void claim_house(string uuid, string file);
private void _house_family(object * family, string house);
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

    npc = find_living(living_here[i]);
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


// Whether a census row is a settled resident -- one the design declared as such.
// Who gets a house is a design-time decision, not a runtime guess from the NPC's
// race or behaviour: an NPC source (template) is flagged "resident" in
// npc_caps by the builder, and only those sources are housed here. Animals,
// guards and any unflagged roster filler are never handed a house; POI vacancies
// (barman, shopkeeper) are not in npc_caps at all and carry their own fixed
// home instead.
private int _is_resident(mapping entry)
{
  mapping spec, job;

  if (!entry)
    return 0;

  spec = ((mapping)this_object()->query_npc_caps())[entry["source"]];
  if (spec && spec["resident"])
    return 1;

  // Somebody holding a job is not on the roster at all, so the flag it would
  // have carried there lives on the job instead. A post with a house of its own
  // does not come through here: its holder is housed by the post.
  if (!entry[CENSUS_VACANCY])
    return 0;

  job = (mapping)this_object()->query_vacancy(entry[CENSUS_VACANCY]);
  return job && job[VACANCY_RESIDENT] && !job[VACANCY_HOME];
}

// Who lives where, read off the houses: ([ uuid : house file ]).
private mapping _residences()
{
  mapping out;
  string * all;
  int i, j;

  out = ([ ]);
  all = query_houses();

  for (i = 0; i < sizeof(all); i++)
  {
    object house, home;
    string * living;

    house = (object)this_object()->load_location(all[i]);
    home = house ? house->query_component_by_type(LOCATION_COMPONENT_HOME)
                 : nil;
    if (!home)
      continue;

    living = (string *)home->query_residents();
    for (j = 0; j < sizeof(living); j++)
      out[living[j]] = all[i];
  }

  return out;
}

// Houses that belong to a post (a barracks, the barman's rooms): they are given
// with the job, never handed to whoever is homeless.
private string * _job_houses()
{
  mapping * jobs;
  string * out;
  int i;

  jobs = (mapping *)this_object()->query_vacancies();
  out = ({ });
  for (i = 0; i < sizeof(jobs); i++)
    if (stringp(jobs[i][VACANCY_HOME]) && strlen(jobs[i][VACANCY_HOME]))
      out += ({ jobs[i][VACANCY_HOME] });

  return out;
}

// Ordinary houses standing empty, ready for a new household. A house whose
// family has died out still naming it as theirs is cleared on the way: nobody
// is left to own it.
private string * _empty_houses()
{
  string * all, * posts, * out;
  int i;

  all = query_houses();
  posts = _job_houses();
  out = ({ });

  for (i = 0; i < sizeof(all); i++)
  {
    object house, home;
    mixed owner;

    if (member_array(all[i], posts) != -1)
      continue;

    house = (object)this_object()->load_location(all[i]);
    home = house ? house->query_component_by_type(LOCATION_COMPONENT_HOME)
                 : nil;

    // a dwelling with a name of its own was raised for something in particular
    if (!home || sizeof((string *)home->query_residents()) ||
        home->query_home_short())
      continue;

    owner = home->query_home_owner();
    if (stringp(owner) && strlen(owner))
    {
      if (handler("families", this_object())->has_family(owner) &&
          !handler("families", this_object())->is_extinct(owner))
        continue;

      home->set_home_owner(nil);
      house->save_me();
    }

    out += ({ all[i] });
  }

  return out;
}

// How descent runs in this community: its citizenship says, and a place with
// none follows the father.
private string _descent()
{
  mixed path;

  path = this_object()->query_root_citizenship_path();
  if (!stringp(path) || !strlen(path))
    return DESCENT_PATRILINEAL;

  return (string)load_object(path)->query_descent();
}

// Somebody married now or once. A widow keeps to herself: the pass never pairs
// her off again.
private int _has_been_married(object who)
{
  mixed id;

  id = who->query_family_id();
  return stringp(id) &&
         handler("families", this_object())->has_been_married(id);
}

// The census uuid of somebody's spouse, or nil.
private string _spouse_uuid(object who)
{
  mixed spouse;

  spouse = who->query_spouse_id();
  if (!stringp(spouse) || strlen(spouse) <= strlen(FAMILY_NPC) ||
      spouse[0 .. strlen(FAMILY_NPC) - 1] != FAMILY_NPC)
    return nil;

  return spouse[strlen(FAMILY_NPC) ..];
}

// Put a house in a family's name, taking it off whoever held it before.
private void _transfer_house(string surname, string file)
{
  mixed previous;

  if (!surname || !strlen(surname))
    return;

  previous = handler("families", this_object())->owner_of(file);
  if (previous && previous != surname)
    handler("families", this_object())->remove_property(previous, file);

  claim_property(surname, file);
}

// Move somebody into a house that already stands.
private void _move_in(object who, string file)
{
  who->set_home(file);
  who->save_npc();
  claim_house((string)who->query_uuid(), file);
}

// The house a group of people will live under. A couple marries, and the
// descent of the community decides whose house they are of; a group of strangers
// founds a new one, named from the citizenship they were born into. A draft area
// founds nothing -- a house outlives the locations it stands in, so it waits
// until the place is settled.
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

    citizenship = this_object()->query_root_citizenship_path();
    surname = handler("families", this_object())->mint_surname(citizenship);
    if (!surname || !strlen(surname))
      return nil;
    if (!handler("families", this_object())->found_family(surname, citizenship))
      return nil;

    group[0]->set_family(surname);
  }

  // a couple housed together is a couple
  if (sizeof(group) == 2)
    return handler("families", this_object())->wed(group[0], group[1],
                                                   _descent());

  if (!group[0]->query_family())
    group[0]->set_family(surname);

  return group[0]->query_family();
}

// Give a family (one or two NPCs) a home and move them in: the empty house
// offered, or one raised on a free plot. Each member's address is set and
// persisted. No house and no plot -> nothing happens (build_house_on_plot
// logged it).
private void _house_family(object * family, string house)
{
  string surname;
  string * ids;
  int i;

  // the house they will live under, founded now if they had none
  surname = _family_for(family);

  ids = ({ });
  for (i = 0; i < sizeof(family); i++)
    ids += ({ family[i]->query_uuid() });

  if (!house)
    house = build_house_on_plot(ids);
  if (!house)
    return;

  for (i = 0; i < sizeof(family); i++)
    _move_in(family[i], house);

  // A house belongs to the family living in it, not to the people one by one:
  // that is what lets it outlast them, and what a door asks before it opens.
  _transfer_house(surname, house);
}

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

// Give every homeless resident of the community a home. In turn, each one:
//   - joins a spouse who already has a house;
//   - or, never having been married, marries somebody of the other sex who lives
//     alone and never has been either, and moves in with them;
//   - or pairs with another homeless resident free to marry, founding a house;
//   - or, failing all of that, gets a house of their own.
// A new household takes an empty house before it raises one on a free plot. The
// whole census is housed, not only whoever is loaded: anybody the pass needs is
// read from their npc.o (load_npc), without loading where they are. Residency is the design-time fact tested by
// _is_resident, not a runtime type guess. Stops quietly when plots run out (each
// miss is logged).
void assign_homes()
{
  mapping census, lives_at, gender_of, heads;
  object owner;
  object * homeless, * males, * females, * alone, * loaded;
  string * ids, * singles, * empty, * posts;
  int i, j;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->assign_homes();
    return;
  }

  _drop_unknown_residents();

  census = (mapping)this_object()->query_npc_census();
  lives_at = _residences();
  empty = _empty_houses();

  // people are loaded from their npc.o where they are not in the world, and
  // unloaded again at the end: nobody's location is loaded for this
  loaded = ({ });

  homeless = ({ });
  ids = map_indices(census);
  for (i = 0; i < sizeof(ids); i++)
  {
    object npc;

    // a resident placed by the design, without a home yet
    if (lives_at[ids[i]] || !_is_resident(census[ids[i]]))
      continue;

    npc = (object)this_object()->load_npc(ids[i]);
    if (!npc)
      continue;
    if (!environment(npc))
      loaded += ({ npc });
    homeless += ({ npc });
  }

  // those living alone in an ordinary house, who have never married: a homeless
  // newcomer may marry in
  singles = ({ });
  gender_of = ([ ]);
  heads = ([ ]);
  posts = _job_houses();
  for (i = 0; i < sizeof(ids); i++)
    if (lives_at[ids[i]] && member_array(lives_at[ids[i]], posts) == -1)
      heads[lives_at[ids[i]]] = heads[lives_at[ids[i]]]
                                  ? heads[lives_at[ids[i]]] + ({ ids[i] })
                                  : ({ ids[i] });

  // read off the census and the family register, without loading anybody
  ids = map_indices(heads);
  for (i = 0; i < sizeof(ids); i++)
  {
    string single;

    single = heads[ids[i]][0];
    if (sizeof(heads[ids[i]]) != 1 || !_is_resident(census[single]) ||
        handler("families", this_object())->has_been_married(FAMILY_NPC + single))
      continue;

    singles += ({ single });
    gender_of[single] = census[single]["gender"];
  }

  males = ({ });
  females = ({ });
  alone = ({ });

  for (i = 0; i < sizeof(homeless); i++)
  {
    object who, partner;
    string spouse;
    int found;

    who = homeless[i];

    // already housed in this pass, alongside a spouse further up the list
    if (lives_at[(string)who->query_uuid()])
      continue;

    spouse = _spouse_uuid(who);
    if (spouse)
    {
      if (lives_at[spouse])
      {
        _move_in(who, lives_at[spouse]);
        lives_at[(string)who->query_uuid()] = lives_at[spouse];
        continue;
      }

      partner = find_living(spouse);
      if (partner && member_array(partner, homeless) != -1)
      {
        _house_family(({ who, partner }), sizeof(empty) ? empty[0] : nil);
        if (sizeof(empty))
          empty = empty[1..];
        lives_at[(string)who->query_uuid()] = who->query_home();
        lives_at[spouse] = who->query_home();
        continue;
      }
    }

    // marriage is between a man and a woman who have never been married
    if (_has_been_married(who) ||
        (who->query_gender() != GENDER_MALE &&
         who->query_gender() != GENDER_FEMALE))
    {
      alone += ({ who });
      continue;
    }

    for (j = 0; j < sizeof(singles) && !found; j++)
    {
      if (gender_of[singles[j]] == who->query_gender() ||
          gender_of[singles[j]] == GENDER_NEUTER)
        continue;

      partner = (object)this_object()->load_npc(singles[j]);
      if (!partner)
        continue;
      if (!environment(partner))
        loaded += ({ partner });

      _family_for(({ who, partner }));
      _move_in(who, lives_at[singles[j]]);
      _transfer_house(who->query_family(), lives_at[singles[j]]);
      lives_at[(string)who->query_uuid()] = lives_at[singles[j]];
      singles -= ({ singles[j] });
      found = 1;
    }
    if (found)
      continue;

    if (who->query_gender() == GENDER_FEMALE)
      females += ({ who });
    else
      males += ({ who });
  }

  // a man and a woman share a house; whoever is left over gets one alone
  while (sizeof(males) && sizeof(females))
  {
    _house_family(({ males[0], females[0] }), sizeof(empty) ? empty[0] : nil);
    if (sizeof(empty))
      empty = empty[1..];
    males = males[1..];
    females = females[1..];
  }

  alone += males + females;
  for (i = 0; i < sizeof(alone); i++)
  {
    _house_family(({ alone[i] }), sizeof(empty) ? empty[0] : nil);
    if (sizeof(empty))
      empty = empty[1..];
  }

  for (i = 0; i < sizeof(loaded); i++)
    this_object()->unload_npc(loaded[i]);
}
