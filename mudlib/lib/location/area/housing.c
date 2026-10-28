
// Housing: buildable plots and the houses raised on them.
//
// A plot is an empty lot the builder carved next to an existing location; a
// house is that same location once someone moves in. This file owns the plot
// registry, the books of the houses -- who lives in each and which family owns
// it -- the act of raising a house on a plot, and the pass that gives every
// homeless settled citizen a home.
//
// The books are the only record of who lives where. Nothing is written on the
// people or on the houses themselves, so housing somebody, marrying them off or
// emptying a house loads neither the person nor the place.
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
// The community's houses, see <areas/area.h>:
//   ([ location file : ([ HOUSE_RESIDENTS: ({ uuid }), HOUSE_OWNER: surname,
//                         HOUSE_KEPT: 1 ]) ])
mapping house_records;

void add_house(string file, varargs int kept);
void set_house_of(string uuid, string file);

// The area's fallback location file (where orphaned occupants go). "" if unset.
string principal;


void door_house_exits(object house);
void open_plot_exits(object plot);
int demote_house(string file);
int is_house(string file);
private void _house_family(string * family, string house);
private string _family_for(string * group);
void assign_homes();

void create()
{
  plots = ({ });
  house_records = ([ ]);
  principal = "";
}

// Buildable-plot registry. `houses plot` registers a freshly carved empty lot
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

/**
 * Gives a house the city component of the locations its exits lead to, with
 * their settings, unless it has its own: a house in a town is part of the town.
 * Raising a house calls it; `demote_house` takes the component away again.
 *
 * @param  house  the loaded house location
 */
void take_setting(object house)
{
  mapping exits;
  string * dirs, * types;
  object neighbour, comp;
  int i, j;

  if (!house)
    return;

  types = ({ LOCATION_COMPONENT_CITY });
  exits = house->query_exit_map();
  dirs = exits ? map_indices(exits) : ({ });

  for (i = 0; i < sizeof(dirs); i++)
  {
    neighbour = load_object(LOCATION_HANDLER)->load_location(exits[dirs[i]][0]);
    if (!neighbour)
      continue;

    for (j = 0; j < sizeof(types); j++)
    {
      if (house->has_component(types[j]))
        continue;
      comp = neighbour->query_component_by_type(types[j]);
      if (comp)
        house->add_component(types[j], comp->query_auto_load_attributes());
    }
  }
}

// Turn the plot at `file` into a house and put it on the books: the `plot`
// component goes, a `home` component comes, the plot's open passages become
// doors, and `residents` move in. The location itself has to be loaded for
// this: raising a house changes it.
private void _raise_house(string file, string * residents, int kept)
{
  object house;
  int i;

  house = load_object(LOCATION_HANDLER)->load_location(file);
  if (!house)
    return;

  house->remove_component(LOCATION_COMPONENT_PLOT);
  house->add_component(LOCATION_COMPONENT_HOME, ([ ]));
  take_setting(house);
  house->save_me();

  // a raised house has a real front door: re-type the plot's open exits (both
  // the house side and the neighbour's reciprocal) to "door"
  door_house_exits(house);

  // it is a house now, not an available plot -- but it stays on the books:
  // an area that forgets its houses cannot answer who lives where
  remove_plot(file);
  add_house(file, kept);

  for (i = 0; i < sizeof(residents); i++)
    set_house_of(residents[i], file);

  this_object()->log_event("Raised a house at " + file + " for " +
            (sizeof(residents) ? implode(residents, ", ") : "no residents") +
            ".");
}

// Raise a house on a free plot and move its residents in. Picks a random free
// plot (no neighbourhood grading yet). Returns the house's location file, or nil
// (and logs) when there is no free plot. `residents` are the uuids that will
// live there; a family shares one call.
string build_house_on_plot(string * residents)
{
  string plot_file;
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return owner->build_house_on_plot(residents);

  residents = residents ? residents : ({ });

  // a plot whose location no longer loads leaves the books, and another is
  // tried, so one lost file does not stop the housing of everybody else
  while (plots && sizeof(plots))
  {
    plot_file = plots[random(sizeof(plots))];
    if (load_object(LOCATION_HANDLER)->load_location(plot_file))
    {
      _raise_house(plot_file, residents, 0);
      return is_house(plot_file) ? plot_file : nil;
    }
    remove_plot(plot_file);
    this_object()->log_event("Dropped the plot " + plot_file +
              ": its location does not load.");
  }

  this_object()->log_event("No free plot available to house " +
            (sizeof(residents) ? implode(residents, ", ") : "an NPC") + ".");
  return nil;
}

// Raise a house on one named plot, rather than on whichever one is free. Used
// when a house is placed by hand: the builder stands on the plot that is to
// become it. A `kept` house was raised for something in particular -- a
// barracks, a job's house -- and is never handed to whoever is homeless.
// Returns the house's location file, or nil if that location is not a plot of
// this area.
string build_house_at(string file, string * residents, varargs int kept)
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return owner->build_house_at(file, residents, kept);

  if (member_array(file, query_plots()) < 0)
    return nil;

  _raise_house(file, residents ? residents : ({ }), kept);
  return is_house(file) ? file : nil;
}

// Make a location that is already built -- a barracks, a room over a shop --
// a house kept for whoever is given it. Its exits stay as they are: it was
// built with the doors it needs. Returns the file, or nil if it is not a
// location of this community or is a plot.
string furnish_house(string file)
{
  object owner, house, home;
  string short_text, long_text;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return owner->furnish_house(file);

  if (member_array(file, query_plots()) >= 0)
    return nil;
  if (is_house(file))
    return file;

  house = load_object(LOCATION_HANDLER)->load_location(file);
  if (!house)
    return nil;

  // it keeps what it is called and looks like: the home component speaks for
  // a house's short and long, so it is handed the location's own
  short_text = (string)house->short();
  long_text = (string)house->query_specific_long();
  if (!long_text || !strlen(long_text))
    long_text = (string)house->query_original_long();

  house->add_component(LOCATION_COMPONENT_HOME, ([ ]));
  home = house->query_component_by_type(LOCATION_COMPONENT_HOME);
  if (home)
  {
    home->set_home_short(short_text);
    home->set_home_long(long_text);
  }
  house->save_me();
  add_house(file, 1);

  this_object()->log_event("Made " + file + " a house.");
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

// ---------------------------------------------------------------------------
// The books
// ---------------------------------------------------------------------------
//
// Every call is answered by the community's root area, which keeps them. A
// house is named by its location file.

string * query_houses()
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (string *)owner->query_houses();

  if (!house_records)
    house_records = ([ ]);

  return house_records ? map_indices(house_records) : ({ });
}

int is_house(string file)
{
  return member_array(file, query_houses()) != -1;
}

/**
 * Returns the location a house stands in: a household of a shared house is
 * kept under its location file plus a number, everything else under the file.
 */
string query_house_file(string house)
{
  string file;

  if (!house)
    return nil;
  if (sscanf(house, "%s" + HOUSE_UNIT_MARK + "%*s", file) == 2)
    return file;
  return house;
}

/**
 * Returns the households on the books for the location `file`: just `file` for
 * an ordinary house, every household's key for a shared one, none if it is not
 * a house.
 */
string * query_house_units(string file)
{
  object owner;
  string * out;
  int i, n;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return owner->query_house_units(file);

  if (!house_records || !house_records[file])
    return ({ });

  out = ({ file });
  n = house_records[file][HOUSE_SHARED] ? house_records[file][HOUSE_SHARED] : 1;
  for (i = 2; i <= n; i++)
    if (house_records[file + HOUSE_UNIT_MARK + i])
      out += ({ file + HOUSE_UNIT_MARK + i });

  return out;
}

/**
 * Makes the house at `file` a shared house with room for `households`
 * families, or an ordinary one again with 1. A shared house is ordinary
 * housing, handed to whoever is homeless, so it stops being kept.
 *
 * @return  0 if it is not a house, is communal, or a household that would go
 *          still has people in it
 */
int set_shared_house(string file, int households)
{
  object owner;
  int i, n;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return owner->set_shared_house(file, households);

  if (!house_records || !house_records[file] || households < 1 ||
      house_records[file][HOUSE_COMMUNAL])
    return 0;

  n = house_records[file][HOUSE_SHARED] ? house_records[file][HOUSE_SHARED] : 1;
  for (i = households + 1; i <= n; i++)
    if (house_records[file + HOUSE_UNIT_MARK + i] &&
        sizeof(house_records[file + HOUSE_UNIT_MARK + i][HOUSE_RESIDENTS]))
      return 0;

  for (i = households + 1; i <= n; i++)
    map_delete(house_records, file + HOUSE_UNIT_MARK + i);
  for (i = 2; i <= households; i++)
    if (!house_records[file + HOUSE_UNIT_MARK + i])
      house_records[file + HOUSE_UNIT_MARK + i] = ([ HOUSE_RESIDENTS: ({ }) ]);

  if (households > 1)
  {
    house_records[file][HOUSE_SHARED] = households;
    map_delete(house_records[file], HOUSE_KEPT);
  }
  else
    map_delete(house_records[file], HOUSE_SHARED);

  this_object()->save_me();
  return 1;
}

/**
 * Returns how many households the house at `file` has room for: 1 for an
 * ordinary house, 0 if it is not a house.
 */
int query_shared_house(string file)
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return owner->query_shared_house(file);

  if (!house_records || !house_records[file])
    return 0;
  return house_records[file][HOUSE_SHARED] ? house_records[file][HOUSE_SHARED] : 1;
}

// Put a location on the books as a house, with nobody in it yet.
void add_house(string file, varargs int kept)
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->add_house(file, kept);
    return;
  }

  if (!house_records)
    house_records = ([ ]);

  if (!file || !strlen(file) || house_records[file])
    return;

  house_records[file] = ([ HOUSE_RESIDENTS: ({ }) ]);
  if (kept)
    house_records[file][HOUSE_KEPT] = 1;
  this_object()->save_me();
}

int is_kept_house(string file)
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (int)owner->is_kept_house(file);

  if (!house_records)
    house_records = ([ ]);

  return house_records[file] && house_records[file][HOUSE_KEPT];
}

/**
 * Says whether the house at `file` is kept for something in particular (a
 * post, a barracks), and so never handed to whoever is homeless.
 *
 * @return  0 if `file` is not a house
 */
int set_kept_house(string file, int flag)
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (int)owner->set_kept_house(file, flag);

  if (!house_records || !house_records[file])
    return 0;

  if (flag)
    house_records[file][HOUSE_KEPT] = 1;
  else
    map_delete(house_records[file], HOUSE_KEPT);

  this_object()->save_me();
  return 1;
}

int is_communal_house(string file)
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (int)owner->is_communal_house(file);

  if (!house_records)
    house_records = ([ ]);

  return house_records[file] && house_records[file][HOUSE_COMMUNAL];
}

// Mark a house as communal, or not. Only a house already on the books.
int set_communal_house(string file, int flag)
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (int)owner->set_communal_house(file, flag);

  if (!house_records || !house_records[file])
    return 0;

  if (flag)
    house_records[file][HOUSE_COMMUNAL] = 1;
  else
    map_delete(house_records[file], HOUSE_COMMUNAL);

  this_object()->save_me();
  return 1;
}

string * query_house_residents(string file)
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (string *)owner->query_house_residents(file);

  if (!house_records)
    house_records = ([ ]);

  return house_records[file] ? ({ }) + house_records[file][HOUSE_RESIDENTS]
                             : ({ });
}

// The house `uuid` lives in, or "" when nobody has given them one.
string query_house_of(string uuid)
{
  object owner;
  string * files;
  int i;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return owner->query_house_of(uuid);

  if (!house_records)
    house_records = ([ ]);

  if (!uuid || !strlen(uuid))
    return "";

  files = map_indices(house_records);
  for (i = 0; i < sizeof(files); i++)
    if (member_array(uuid, house_records[files[i]][HOUSE_RESIDENTS]) != -1)
      return files[i];

  return "";
}

// Move `uuid` into the house at `file`, out of any other; nil leaves them
// homeless. Somebody lives in one house at a time.
void set_house_of(string uuid, string file)
{
  object owner;
  string * files;
  int i, changed;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->set_house_of(uuid, file);
    return;
  }

  if (!house_records)
    house_records = ([ ]);

  if (!uuid || !strlen(uuid))
    return;

  files = map_indices(house_records);
  for (i = 0; i < sizeof(files); i++)
  {
    string * living;

    living = house_records[files[i]][HOUSE_RESIDENTS];
    if (files[i] == file && member_array(uuid, living) == -1)
    {
      house_records[files[i]][HOUSE_RESIDENTS] = living + ({ uuid });
      changed = 1;
    }
    else if (files[i] != file && member_array(uuid, living) != -1)
    {
      house_records[files[i]][HOUSE_RESIDENTS] = living - ({ uuid });
      changed = 1;
    }
  }

  if (changed)
    this_object()->save_me();
}

/**
 * Books the house at `file` (every household of it) with the community of the
 * area at `area_path`, for when it should follow that area rather than the one
 * it stands in if the two ever part. nil or "" clears it.
 *
 * @return  0 if `file` is not a house
 */
int set_house_books(string file, string area_path)
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (int)owner->set_house_books(file, area_path);

  if (!house_records || !house_records[file])
    return 0;

  if (area_path && strlen(area_path))
    house_records[file][HOUSE_BOOKS] = area_path;
  else
    map_delete(house_records[file], HOUSE_BOOKS);

  this_object()->save_me();
  return 1;
}

// The family a house belongs to, or nil. A house whose family has died out
// belongs to nobody.
string query_house_owner(string file)
{
  object owner;
  mixed surname;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return owner->query_house_owner(file);

  if (!house_records)
    house_records = ([ ]);

  surname = house_records[file] ? house_records[file][HOUSE_OWNER] : nil;
  if (!stringp(surname) ||
      !handler("families", this_object())->has_family(surname) ||
      handler("families", this_object())->is_extinct(surname))
    return nil;

  return surname;
}

// Put a house in a family's name; nil leaves it unowned. A house belongs to the
// family living in it, not to the people one by one: that is what lets it
// outlast them, and what a door asks before it opens.
void set_house_owner(string file, string surname)
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->set_house_owner(file, surname);
    return;
  }

  if (!house_records)
    house_records = ([ ]);

  if (!house_records[file])
    return;

  if (surname && strlen(surname))
    house_records[file][HOUSE_OWNER] = surname;
  else
    map_delete(house_records[file], HOUSE_OWNER);
  this_object()->save_me();
}

// Every house on the books with the family it belongs to: ([ file : surname ]).
mapping query_house_owners()
{
  object owner;
  mapping out;
  string * files;
  int i;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (mapping)owner->query_house_owners();

  if (!house_records)
    house_records = ([ ]);

  out = ([ ]);
  files = map_indices(house_records);
  for (i = 0; i < sizeof(files); i++)
    if (house_records[files[i]][HOUSE_OWNER])
      out[files[i]] = house_records[files[i]][HOUSE_OWNER];

  return out;
}

string * query_houses_owned_by(string surname)
{
  mapping owners;
  string * files, * out;
  int i;

  owners = query_house_owners();
  files = map_indices(owners);
  out = ({ });
  for (i = 0; i < sizeof(files); i++)
    if (owners[files[i]] == surname)
      out += ({ files[i] });

  return out;
}

// A family that has died out leaves its houses: they stand unowned, ready for
// the next household. Called by the families handler.
void vacate_houses_of(string surname)
{
  string * files;
  int i;

  files = query_houses_owned_by(surname);
  for (i = 0; i < sizeof(files); i++)
    set_house_owner(files[i], nil);
}

// Turn a house back into a bare plot: evict its residents, drop the home
// component for a plot one, and put it back on the free-plot list.
//
// The inverse of build_house_on_plot, and the only way a house that was raised
// in the wrong place can be undone -- plot removal refuses anything that is not
// a bare plot, so without this a misplaced house is permanent. Residents are
// left homeless on purpose: the caller decides where they go next, and a plain
// `houses assign` will re-house them on any free plot.
//
// Returns the number of residents evicted, or -1 if `file` is not a house here.
int demote_house(string file)
{
  object house, owner;
  string * units;
  int evicted, i;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (int)owner->demote_house(file);

  if (!house_records)
    house_records = ([ ]);

  if (!file || !house_records[file])
    return -1;

  // every household of a shared house goes with it
  units = query_house_units(file);
  for (i = 0; i < sizeof(units); i++)
  {
    evicted += sizeof(house_records[units[i]][HOUSE_RESIDENTS]);
    map_delete(house_records, units[i]);
  }

  house = (object)this_object()->load_location(file);
  if (house)
  {
    // a bare plot again: the setting the house took goes with it, and any
    // forest it was given by hand
    house->remove_component(LOCATION_COMPONENT_HOME);
    house->remove_component(LOCATION_COMPONENT_CITY);
    house->remove_component(LOCATION_COMPONENT_FOREST);
    house->add_component(LOCATION_COMPONENT_PLOT, ([ ]));
    house->save_me();

    // the front door goes back to being an open doorway, on both sides
    open_plot_exits(house);
  }

  add_plot(file);
  this_object()->save_me();

  this_object()->log_event("Demoted the house at " + file + " back to a plot" +
            (evicted ? ", evicting " + evicted + " resident(s)" : "") + ".");

  return evicted;
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

// Who lives where, read off the books: ([ uuid : house file ]).
private mapping _residences()
{
  mapping out;
  string * files;
  int i, j;

  out = ([ ]);
  files = query_houses();
  for (i = 0; i < sizeof(files); i++)
  {
    string * living;

    living = query_house_residents(files[i]);
    for (j = 0; j < sizeof(living); j++)
      out[living[j]] = files[i];
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

// Ordinary houses standing empty, ready for a new household: nobody lives in
// them, no living family owns them, and they were not raised for anything in
// particular.
private string * _empty_houses()
{
  string * files, * posts, * out;
  int i;

  files = query_houses();
  posts = _job_houses();
  out = ({ });

  for (i = 0; i < sizeof(files); i++)
    if (member_array(files[i], posts) == -1 && !is_kept_house(files[i]) &&
        !sizeof(query_house_residents(files[i])) &&
        !query_house_owner(files[i]))
      out += ({ files[i] });

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

// A census person as the families handler takes them: id, name and gender, all
// read off the census row.
private mapping _person(string uuid)
{
  mapping entry;

  entry = ((mapping)this_object()->query_npc_census())[uuid];
  return ([ "id":     FAMILY_NPC + uuid,
            "name":   (entry && entry["name"]) ? capitalize(entry["name"])
                                               : FAMILY_NPC + uuid,
            "gender": entry ? entry["gender"] : 0 ]);
}

// Somebody married now or once. A widow keeps to herself: the pass never pairs
// her off again.
private int _has_been_married(string uuid)
{
  return handler("families", this_object())->has_been_married(FAMILY_NPC + uuid);
}

// The census uuid of somebody's spouse, or nil.
private string _spouse_uuid(string uuid)
{
  mixed spouse;

  spouse = handler("families", this_object())->query_spouse(FAMILY_NPC + uuid);
  if (!stringp(spouse) || strlen(spouse) <= strlen(FAMILY_NPC) ||
      spouse[0 .. strlen(FAMILY_NPC) - 1] != FAMILY_NPC)
    return nil;

  return spouse[strlen(FAMILY_NPC) ..];
}

// The house a group of people will live under. A couple marries, and the
// descent of the community decides whose house they are of; a group of strangers
// founds a new one, named from the citizenship they were born into. A draft area
// founds nothing -- a house outlives the locations it stands in, so it waits
// until the place is settled.
private string _family_for(string * group)
{
  string citizenship, surname;
  mapping first;
  int i;

  for (i = 0; i < sizeof(group); i++)
    if (handler("families", this_object())->query_member_family(FAMILY_NPC + group[i]))
      surname = handler("families", this_object())->query_member_family(FAMILY_NPC + group[i]);

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

    first = _person(group[0]);
    handler("families", this_object())->add_member(surname, first["id"],
                                                   first["name"]);
  }

  // a couple housed together is a couple
  if (sizeof(group) == 2)
    return handler("families", this_object())->wed(_person(group[0]),
                                                   _person(group[1]),
                                                   _descent());

  if (!handler("families", this_object())->query_member_family(FAMILY_NPC + group[0]))
  {
    first = _person(group[0]);
    handler("families", this_object())->add_member(surname, first["id"],
                                                   first["name"]);
  }

  return handler("families", this_object())->query_member_family(FAMILY_NPC + group[0]);
}

// The family somebody belongs to, founding one for them if they have none. What
// staffing calls for a holder housed by their post, whose house is the job's and
// is never the family's.
string found_family_for(string uuid)
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return owner->found_family_for(uuid);

  return _family_for(({ uuid }));
}

// Somebody who held a job with a house of its own has died: whoever lived there
// with them leaves, since the house goes with the post to whoever holds it next,
// and is housed again with the rest of the town. A communal house keeps its
// people, who live there by their own job.
void release_job_house(string uuid)
{
  object owner;
  string house;
  string * others;
  int i;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->release_job_house(uuid);
    return;
  }

  house = query_house_of(uuid);
  if (!house || member_array(house, _job_houses()) == -1 ||
      is_communal_house(house))
    return;

  others = query_house_residents(house) - ({ uuid });
  for (i = 0; i < sizeof(others); i++)
  {
    set_house_of(others[i], nil);
    this_object()->log_event(_person(others[i])["name"] + " leaves " + house +
                             ", the house of the job its late holder had.");
  }

  if (sizeof(others))
    assign_homes();
}

// Give a family (one or two people) a home and move them in: the empty house
// offered, or one raised on a free plot. No house and no plot -> nothing
// happens (build_house_on_plot logged it).
private void _house_family(string * family, string house)
{
  string surname;
  int i;

  // the house they will live under, founded now if they had none
  surname = _family_for(family);

  if (!house)
    house = build_house_on_plot(family);
  if (!house)
    return;

  for (i = 0; i < sizeof(family); i++)
    set_house_of(family[i], house);

  if (surname)
    set_house_owner(house, surname);
}

// Drop residents no census row accounts for. A house is written by one path and
// the census by another, so a person taken off the books without dying -- a
// retired type, a repaired id -- leaves an address behind that nothing can
// resolve to a name.
private void _drop_unknown_residents()
{
  mapping census;
  string * files;
  int i, j;

  census = (mapping)this_object()->query_npc_census();
  files = query_houses();

  for (i = 0; i < sizeof(files); i++)
  {
    string * living;

    living = query_house_residents(files[i]);
    for (j = 0; j < sizeof(living); j++)
      if (!census[living[j]])
        set_house_of(living[j], nil);
  }
}

// Found a family for everybody housed without one (see assign_homes).
private void _found_housed_families(mapping census, mapping lives_at,
                                    string * posts)
{
  mapping by_house;
  string * ids, * houses;
  string surname;
  int i;

  by_house = ([ ]);
  ids = map_indices(lives_at);
  for (i = 0; i < sizeof(ids); i++)
    if (census[ids[i]] && !is_communal_house(lives_at[ids[i]]) &&
        !handler("families", this_object())->query_member_family(FAMILY_NPC + ids[i]))
      by_house[lives_at[ids[i]]] = by_house[lives_at[ids[i]]]
                                     ? by_house[lives_at[ids[i]]] + ({ ids[i] })
                                     : ({ ids[i] });

  houses = map_indices(by_house);
  for (i = 0; i < sizeof(houses); i++)
  {
    string * group;

    group = by_house[houses[i]];

    // two who share a roof and may marry are a couple; anybody else is a
    // household of their own
    if (sizeof(group) == 2 && !_has_been_married(group[0]) &&
        !_has_been_married(group[1]) &&
        ((census[group[0]]["gender"] == GENDER_MALE &&
          census[group[1]]["gender"] == GENDER_FEMALE) ||
         (census[group[0]]["gender"] == GENDER_FEMALE &&
          census[group[1]]["gender"] == GENDER_MALE)))
      surname = _family_for(group);
    else
    {
      int j;

      for (j = 0; j < sizeof(group); j++)
        surname = _family_for(({ group[j] }));
    }

    if (surname && member_array(houses[i], posts) == -1 &&
        !query_house_owner(houses[i]))
      set_house_owner(houses[i], surname);
  }
}

// Give every homeless resident of the community a home. In turn, each one:
//   - joins a spouse who already has a house;
//   - or, never having been married, marries somebody of the other sex who lives
//     alone and never has been either, and moves in with them;
//   - or pairs with another homeless resident free to marry, founding a house;
//   - or, failing all of that, gets a house of their own.
// A new household takes an empty house before it raises one on a free plot. The
// whole census is housed, not only whoever is loaded, and everything is read off
// the census, the books and the family register: nobody and no place is loaded,
// except a plot a new house is raised on. Residency is the design-time fact
// tested by _is_resident, not a runtime type guess. Stops quietly when plots run
// out (each miss is logged).
void assign_homes()
{
  mapping census, lives_at, heads;
  object owner;
  string * ids, * homeless, * males, * females, * alone, * singles, * empty;
  string * posts;
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

  posts = _job_houses();

  // Whoever already has a roof and no family gets one: people housed while the
  // area was a draft, and the holders of a job with a house of its own. A couple
  // under one roof is wed; an ordinary house goes into the family's name, a
  // job's house stays the job's. A communal house founds nothing.
  _found_housed_families(census, lives_at, posts);

  // a resident placed by the design, without a home yet
  homeless = ({ });
  ids = map_indices(census);
  for (i = 0; i < sizeof(ids); i++)
    if (!lives_at[ids[i]] && _is_resident(census[ids[i]]))
      homeless += ({ ids[i] });

  // those living alone in a house that is not communal, who have never married:
  // a homeless newcomer may marry in, into a job's house as well
  heads = ([ ]);
  for (i = 0; i < sizeof(ids); i++)
    if (lives_at[ids[i]] && !is_communal_house(lives_at[ids[i]]))
      heads[lives_at[ids[i]]] = heads[lives_at[ids[i]]]
                                  ? heads[lives_at[ids[i]]] + ({ ids[i] })
                                  : ({ ids[i] });

  singles = ({ });
  ids = map_indices(heads);
  for (i = 0; i < sizeof(ids); i++)
    if (sizeof(heads[ids[i]]) == 1 &&
        (_is_resident(census[heads[ids[i]][0]]) ||
         member_array(ids[i], posts) != -1) &&
        !_has_been_married(heads[ids[i]][0]))
      singles += ({ heads[ids[i]][0] });

  males = ({ });
  females = ({ });
  alone = ({ });

  for (i = 0; i < sizeof(homeless); i++)
  {
    string who, spouse;
    int gender, found;

    who = homeless[i];
    gender = census[who]["gender"];

    // already housed in this pass, alongside a spouse further up the list
    if (lives_at[who])
      continue;

    spouse = _spouse_uuid(who);
    if (spouse)
    {
      if (lives_at[spouse])
      {
        set_house_of(who, lives_at[spouse]);
        lives_at[who] = lives_at[spouse];
        continue;
      }

      if (member_array(spouse, homeless) != -1)
      {
        _house_family(({ who, spouse }), sizeof(empty) ? empty[0] : nil);
        if (sizeof(empty))
          empty = empty[1..];
        lives_at[who] = query_house_of(who);
        lives_at[spouse] = lives_at[who];
        continue;
      }
    }

    // marriage is between a man and a woman who have never been married
    if (_has_been_married(who) ||
        (gender != GENDER_MALE && gender != GENDER_FEMALE))
    {
      alone += ({ who });
      continue;
    }

    for (j = 0; j < sizeof(singles) && !found; j++)
    {
      int other;

      other = census[singles[j]]["gender"];
      if (other == gender || (other != GENDER_MALE && other != GENDER_FEMALE))
        continue;

      _family_for(({ who, singles[j] }));
      set_house_of(who, lives_at[singles[j]]);
      if (handler("families", this_object())->query_member_family(FAMILY_NPC + who) &&
          member_array(lives_at[singles[j]], posts) == -1)
        set_house_owner(lives_at[singles[j]],
          handler("families", this_object())->query_member_family(FAMILY_NPC + who));
      lives_at[who] = lives_at[singles[j]];
      singles -= ({ singles[j] });
      found = 1;
    }
    if (found)
      continue;

    if (gender == GENDER_FEMALE)
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
}
