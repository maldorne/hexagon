/*
 * Families handler.
 *
 * The register of every family in every game, and the only thing that knows who
 * belongs to whom. A living carries nothing but its surname (the family slot of
 * its social objects); everything else -- who is married to whom, whose child
 * somebody is, what the house belongs to -- is here.
 *
 * Keeping the relations here rather than on each living is what lets a family
 * survive its people. An NPC's savefile is deleted the moment it dies, so a
 * relation written on the individual dies with them and a generation stops
 * being nameable. The roll below is the answer: every family keeps a record of
 * everyone who ever belonged to it and what became of them, and the living
 * members are simply those whose id still resolves to somebody.
 *
 * A member is named the same whether it is an NPC or a player -- "npc:<uuid>",
 * "player:<name>" -- so nothing here has to care which it holds. That is what
 * lets a player marry an NPC, or a family hold both.
 *
 * Everything is keyed by game first: a surname is unique within its game, not
 * across the mudlib, and a family belongs to the world it was founded in.
 *
 *   families[game][surname] == ([ "citizenship": name,
 *                                 "members":     ([ id : ([ "spouse":  id,
 *                                                           "parents": ({ id }) ]) ]),
 *                                 "roll":        ([ id : ([ "name": s,
 *                                                           "fate": s ]) ]),
 *                                 "properties":  ({ location files }) ])
 *
 * A surname is spent for good. When the last member dies the family is extinct:
 * its properties are freed, but the record and its roll stay, and the name is
 * never minted again. A family holding a player never goes extinct on its own.
 */

#include <mud/config.h>
#include <living/family.h>
#include <namegen.h>

inherit "/lib/core/object.c";

// ([ game : ([ surname : family record ]) ])
mapping families;
// Where every member is, so a living finds its own family without searching:
// ([ game : ([ member id : surname ]) ])
mapping member_of;

private void _save();
private mapping _game_families(string game);

void create()
{
  families = ([ ]);
  member_of = ([ ]);
  ::create();
  restore_object(FAMILY_SAVE, 1);
}

void setup()
{
  // anticloning, like the other handlers
  if (this_object() != find_object(FAMILY_HANDLER))
    destruct_object(this_object());
}

private void _save()
{
  save_object(FAMILY_SAVE, 1);
}

private mapping _game_families(string game)
{
  if (!game || !strlen(game))
    return ([ ]);
  if (!families[game])
    families[game] = ([ ]);
  return families[game];
}

private mapping _game_members(string game)
{
  if (!game || !strlen(game))
    return ([ ]);
  if (!member_of[game])
    member_of[game] = ([ ]);
  return member_of[game];
}

// ---------------------------------------------------------------------------
// Reading
// ---------------------------------------------------------------------------

string * query_families(string game)
{
  return map_indices(_game_families(game));
}

mapping query_family(string game, string surname)
{
  return _game_families(game)[surname];
}

int has_family(string game, string surname)
{
  return _game_families(game)[surname] ? 1 : 0;
}

// The surname somebody belongs to, or nil. This is the lookup every living
// does, so it is an index rather than a walk of the register.
string family_of(string game, string id)
{
  return _game_members(game)[id];
}

string * query_members(string game, string surname)
{
  mapping family;

  family = query_family(game, surname);
  return family ? map_indices(family[FAMILY_MEMBERS]) : ({ });
}

// Everyone the family ever held, living or not. What makes a genealogy possible
// after the people in it are gone.
mapping query_roll(string game, string surname)
{
  mapping family;

  family = query_family(game, surname);
  return family ? family[FAMILY_ROLL] : ([ ]);
}

string query_member_name(string game, string surname, string id)
{
  mapping roll;

  roll = query_roll(game, surname);
  return (roll[id] && roll[id][FAMILY_NAME]) ? roll[id][FAMILY_NAME] : id;
}

string query_spouse(string game, string id)
{
  mapping family;
  string surname;

  surname = family_of(game, id);
  family = surname ? query_family(game, surname) : nil;
  if (!family || !family[FAMILY_MEMBERS][id])
    return nil;

  return family[FAMILY_MEMBERS][id][FAMILY_SPOUSE];
}

string * query_parents(string game, string id)
{
  mapping family;
  string surname;
  mixed parents;

  surname = family_of(game, id);
  family = surname ? query_family(game, surname) : nil;
  if (!family || !family[FAMILY_MEMBERS][id])
    return ({ });

  parents = family[FAMILY_MEMBERS][id][FAMILY_PARENTS];
  return pointerp(parents) ? parents : ({ });
}

// Whose children somebody is. Not stored: it is the inverse of parents, read
// off the members and the roll together, so it cannot fall out of step with
// them -- and the roll is what keeps a daughter who married into another house
// answerable here, where her parents still live.
string * query_children(string game, string id)
{
  mapping family, members;
  string surname;
  string * ids, * out;
  int i;

  surname = family_of(game, id);
  family = surname ? query_family(game, surname) : nil;
  if (!family)
    return ({ });

  members = family[FAMILY_MEMBERS];
  ids = map_indices(members);
  out = ({ });

  for (i = 0; i < sizeof(ids); i++)
    if (pointerp(members[ids[i]][FAMILY_PARENTS]) &&
        member_array(id, members[ids[i]][FAMILY_PARENTS]) != -1)
      out += ({ ids[i] });

  return out;
}

// Siblings share a parent. Derived for the same reason children are.
string * query_siblings(string game, string id)
{
  mapping family, members;
  string surname;
  string * ids, * mine, * out;
  int i;

  mine = query_parents(game, id);
  if (!sizeof(mine))
    return ({ });

  surname = family_of(game, id);
  family = query_family(game, surname);
  members = family[FAMILY_MEMBERS];
  ids = map_indices(members);
  out = ({ });

  for (i = 0; i < sizeof(ids); i++)
    if (ids[i] != id && pointerp(members[ids[i]][FAMILY_PARENTS]) &&
        sizeof(members[ids[i]][FAMILY_PARENTS] & mine))
      out += ({ ids[i] });

  return out;
}

// ---------------------------------------------------------------------------
// Founding
// ---------------------------------------------------------------------------

// Ask the citizenship's pool for a surname nobody in this game has used. The
// register never forgets a name, so an extinct family's surname is spent too:
// two houses of the same name, generations apart, would make the roll a lie.
string mint_surname(string game, string citizenship)
{
  mixed style, name;
  int i;

  if (!citizenship || !strlen(citizenship))
    return nil;

  // a culture with no surname pool founds no families; the generator would
  // otherwise fall back to the given-name list and mint a first name
  style = load_object(citizenship)->query_surname_style();
  if (!stringp(style) || !strlen(style))
    return nil;

  for (i = 0; i < FAMILY_MINT_TRIES; i++)
  {
    name = NAMEGEN_OB->generate_for(style, "surname", 3, 4, 10);
    if (!stringp(name) || !strlen(name))
      return nil;

    name = capitalize(name);
    if (!has_family(game, name))
      return name;
  }

  return nil;
}

// Start a family. The citizenship is the path of the culture it belongs to,
// which is what its surnames and its descent are read from later.
int found_family(string game, string surname, string citizenship)
{
  mapping all;

  if (!game || !strlen(game) || !surname || !strlen(surname))
    return 0;

  all = _game_families(game);
  if (all[surname])
    return 0;

  all[surname] = ([ FAMILY_CITIZENSHIP: citizenship ? citizenship : "",
                    FAMILY_MEMBERS:     ([ ]),
                    FAMILY_ROLL:        ([ ]),
                    FAMILY_PROPERTIES:  ({ }) ]);
  _save();
  return 1;
}

// ---------------------------------------------------------------------------
// Membership
// ---------------------------------------------------------------------------

// Take somebody in. `name` is what to call them afterwards: the roll keeps it
// so the dead stay nameable once their savefile is gone.
int add_member(string game, string surname, string id, string name)
{
  mapping family;

  family = query_family(game, surname);
  if (!family || !id || !strlen(id))
    return 0;

  if (family[FAMILY_MEMBERS][id])
    return 0;

  family[FAMILY_MEMBERS][id] = ([ ]);
  family[FAMILY_ROLL][id] = ([ FAMILY_NAME: name ? name : id ]);
  _game_members(game)[id] = surname;
  _save();
  return 1;
}

// Somebody married out. They leave the members but stay in the roll, marked
// with where they went -- which is how the house they left can still say whose
// children they were.
int member_married_out(string game, string id, string into)
{
  mapping family;
  string surname;

  surname = family_of(game, id);
  family = surname ? query_family(game, surname) : nil;
  if (!family || !family[FAMILY_MEMBERS][id])
    return 0;

  map_delete(family[FAMILY_MEMBERS], id);
  if (family[FAMILY_ROLL][id])
    family[FAMILY_ROLL][id][FAMILY_FATE] = FAMILY_MARRIED + " " + into;
  map_delete(_game_members(game), id);
  _save();
  return 1;
}

// Somebody died. They leave the members and are marked in the roll. If nobody
// living is left the family is extinct: its properties are freed, and the
// record stays as history so the name is never minted again.
int member_died(string game, string id)
{
  mapping family, members;
  string surname;
  string * ids;
  int i;

  surname = family_of(game, id);
  family = surname ? query_family(game, surname) : nil;
  if (!family || !family[FAMILY_MEMBERS][id])
    return 0;

  members = family[FAMILY_MEMBERS];

  // a widow is not still married to the dead
  ids = map_indices(members);
  for (i = 0; i < sizeof(ids); i++)
    if (members[ids[i]][FAMILY_SPOUSE] == id)
      map_delete(members[ids[i]], FAMILY_SPOUSE);

  map_delete(members, id);
  if (family[FAMILY_ROLL][id])
    family[FAMILY_ROLL][id][FAMILY_FATE] = FAMILY_DIED;
  map_delete(_game_members(game), id);

  if (!map_sizeof(members))
    family[FAMILY_PROPERTIES] = ({ });

  _save();
  return 1;
}

// Whether a family still holds anybody. A family with a player in it is never
// extinct on its own, which this answers for free: the player is a member until
// they leave.
int is_extinct(string game, string surname)
{
  mapping family;

  family = query_family(game, surname);
  return family ? !map_sizeof(family[FAMILY_MEMBERS]) : 0;
}

// ---------------------------------------------------------------------------
// Relations
// ---------------------------------------------------------------------------

// Marry two members of the same family. Moving the incomer into their spouse's
// family is the caller's business (see the descent rule on the citizenship);
// by the time this runs they are both here.
int set_spouse(string game, string id, string other)
{
  mapping family;
  string surname;

  surname = family_of(game, id);
  family = surname ? query_family(game, surname) : nil;
  if (!family || !family[FAMILY_MEMBERS][id] || !family[FAMILY_MEMBERS][other])
    return 0;

  family[FAMILY_MEMBERS][id][FAMILY_SPOUSE] = other;
  family[FAMILY_MEMBERS][other][FAMILY_SPOUSE] = id;
  _save();
  return 1;
}

// Whose child somebody is. The parents may be in another house -- a mother who
// married out keeps her children here -- so they are ids, not members.
int set_parents(string game, string id, string * parents)
{
  mapping family;
  string surname;

  surname = family_of(game, id);
  family = surname ? query_family(game, surname) : nil;
  if (!family || !family[FAMILY_MEMBERS][id])
    return 0;

  family[FAMILY_MEMBERS][id][FAMILY_PARENTS] = parents ? parents : ({ });
  _save();
  return 1;
}

// ---------------------------------------------------------------------------
// Property
// ---------------------------------------------------------------------------

int add_property(string game, string surname, string file)
{
  mapping family;

  family = query_family(game, surname);
  if (!family || !file || !strlen(file))
    return 0;

  if (member_array(file, family[FAMILY_PROPERTIES]) != -1)
    return 0;

  family[FAMILY_PROPERTIES] += ({ file });
  _save();
  return 1;
}

int remove_property(string game, string surname, string file)
{
  mapping family;

  family = query_family(game, surname);
  if (!family)
    return 0;

  if (member_array(file, family[FAMILY_PROPERTIES]) == -1)
    return 0;

  family[FAMILY_PROPERTIES] -= ({ file });
  _save();
  return 1;
}

string * query_properties(string game, string surname)
{
  mapping family;

  family = query_family(game, surname);
  return family ? family[FAMILY_PROPERTIES] : ({ });
}

// Who owns a place. Walks the register rather than keeping a second index: a
// game holds hundreds of families at most, and a location already carries the
// surname on its home component for the common lookup.
string owner_of(string game, string file)
{
  mapping all;
  string * names;
  int i;

  all = _game_families(game);
  names = map_indices(all);

  for (i = 0; i < sizeof(names); i++)
    if (member_array(file, all[names[i]][FAMILY_PROPERTIES]) != -1)
      return names[i];

  return nil;
}

// The families holding property inside an area, so a builder can be told what a
// wipe would take with it.
string * families_of_area(string area_path)
{
  mapping all;
  string game, prefix;
  string * names, * out, * props;
  int i, j;

  if (!area_path || !strlen(area_path))
    return ({ });

  game = game_from_path(area_path);
  all = _game_families(game);
  names = map_indices(all);
  prefix = "/save/games/" + game + "/locations/" + area_path;
  out = ({ });

  for (i = 0; i < sizeof(names); i++)
  {
    props = all[names[i]][FAMILY_PROPERTIES];
    for (j = 0; j < sizeof(props); j++)
      if (strlen(props[j]) >= strlen(prefix) &&
          props[j][0 .. strlen(prefix) - 1] == prefix)
      {
        out += ({ names[i] });
        break;
      }
  }

  return out;
}
