/*
 * Families handler.
 *
 * The register of every family in every and the only thing that knows who
 * belongs to whom. A living carries nothing but its surname (the family slot of
 * its social objects); everything else -- who is married to whom, whose child
 * somebody is, what the house belongs to -- is here.
 *
 * Keeping the relations here rather than on each living is what lets a family
 * survive its people. An NPC's savefile is deleted the moment it dies, so a
 * relation written on the individual dies with them and a generation stops
 * being nameable. The history below is the answer: every house keeps a record of
 * everyone who ever belonged to it and what became of them, and the living
 * members are simply those whose id still resolves to somebody.
 *
 * A member is named the same whether it is an NPC or a player -- "npc:<uuid>",
 * "player:<name>" -- so nothing here has to care which it holds. That is what
 * lets a player marry an NPC, or a family hold both.
 *
 * Every game has its own handler: /games/<game>/handlers/families.c inherits
 * this one and points query_save_file at its own file, so a surname is unique
 * within its game and the houses of one world have nothing to say about
 * another's. Reach it with handler("families", ob) -- never by path -- so the
 * register you get is the one belonging to ob's game.
 *
 *   families[surname] == ([ "citizenship": name,
 *                                 "members":     ([ id : ([ "spouse":  id,
 *                                                           "parents": ({ id }) ]) ]),
 *                                 "history":     ([ id : ([ "name": s,
 *                                                           "fate": s ]) ]),
 *                                 "properties":  ({ location files }) ])
 *
 * A surname is spent for good. When the last member dies the family is extinct:
 * its properties are freed, but the record and its history stay, and the name is
 * never minted again. A family holding a player never goes extinct on its own.
 */

#include <mud/config.h>
#include <living/family.h>
#include <namegen.h>

inherit "/lib/core/object.c";

//   families:  ([ surname : family record ])
//   member_of: ([ member id : surname ]), so a living finds its house without
//              walking the register
mapping families;
mapping member_of;

private void _save();
string query_save_file();

void create()
{
  families = ([ ]);
  member_of = ([ ]);
  ::create();
  restore_object(query_save_file(), 1);
}

// Where this register is kept. A game's own handler overrides it to point at
// /save/games/<game>/families.o; the shared one answers for anybody without.
string query_save_file()
{
  return FAMILY_SAVE;
}

void setup()
{
  string name;
  int cnum;

  // Anticloning: only the master instance loaded through handler() should
  // exist. The path is not compared, so a game's own subclass passes; any
  // actual clone (file_name suffixed with #N) is destroyed.
  if (sscanf(file_name(this_object()), "%s#%d", name, cnum) == 2)
    dest_me();
}

private void _save()
{
  save_object(query_save_file(), 1);
}

// ---------------------------------------------------------------------------
// Reading
// ---------------------------------------------------------------------------

string * query_families()
{
  return map_indices(families);
}

mapping query_family(string surname)
{
  return families[surname];
}

int has_family(string surname)
{
  return families[surname] ? 1 : 0;
}

// The surname somebody belongs to, or nil. This is the lookup every living
// does, so it is an index rather than a walk of the register.
string family_of(string id)
{
  return member_of[id];
}

string * query_members(string surname)
{
  mapping family;

  family = query_family(surname);
  return family ? map_indices(family[FAMILY_MEMBERS]) : ({ });
}

// Everyone the house has ever held and what became of each: the living, the
// dead, and those who married into another house. This is what makes a
// genealogy possible after the people in it are gone.
mapping query_history(string surname)
{
  mapping family;

  family = query_family(surname);
  return family ? family[FAMILY_HISTORY] : ([ ]);
}

string query_member_name(string surname, string id)
{
  mapping history;

  history = query_history(surname);
  return (history[id] && history[id][FAMILY_NAME])
           ? history[id][FAMILY_NAME] : id;
}

string query_spouse(string id)
{
  mapping family;
  string surname;

  surname = family_of(id);
  family = surname ? query_family(surname) : nil;
  if (!family || !family[FAMILY_MEMBERS][id])
    return nil;

  return family[FAMILY_MEMBERS][id][FAMILY_SPOUSE];
}

string * query_parents(string id)
{
  mapping family;
  string surname;
  mixed parents;

  surname = family_of(id);
  family = surname ? query_family(surname) : nil;
  if (!family || !family[FAMILY_MEMBERS][id])
    return ({ });

  parents = family[FAMILY_MEMBERS][id][FAMILY_PARENTS];
  return pointerp(parents) ? parents : ({ });
}

// Whose children somebody is. Not stored: it is the inverse of parents, read
// off the members and the history together, so it cannot fall out of step with
// them -- and the history is what keeps a daughter who married into another house
// answerable here, where her parents still live.
string * query_children(string id)
{
  mapping family, members;
  string surname;
  string * ids, * out;
  int i;

  surname = family_of(id);
  family = surname ? query_family(surname) : nil;
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
string * query_siblings(string id)
{
  mapping family, members;
  string surname;
  string * ids, * mine, * out;
  int i;

  mine = query_parents(id);
  if (!sizeof(mine))
    return ({ });

  surname = family_of(id);
  family = query_family(surname);
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

// Ask the citizenship's pool for a surname nobody here has used. The
// register never forgets a name, so an extinct family's surname is spent too:
// two houses of the same name, generations apart, would make the history a lie.
string mint_surname(string citizenship)
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
    if (!has_family(name))
      return name;
  }

  return nil;
}

// Start a family. The citizenship is the path of the culture it belongs to,
// which is what its surnames and its descent are read from later.
int found_family(string surname, string citizenship)
{
  mapping all;

  if (!surname || !strlen(surname))
    return 0;

  all = families;
  if (all[surname])
    return 0;

  all[surname] = ([ FAMILY_CITIZENSHIP: citizenship ? citizenship : "",
                    FAMILY_MEMBERS:     ([ ]),
                    FAMILY_HISTORY:        ([ ]),
                    FAMILY_PROPERTIES:  ({ }) ]);
  _save();
  return 1;
}

// ---------------------------------------------------------------------------
// Membership
// ---------------------------------------------------------------------------

// Take somebody in. `name` is what to call them afterwards: the history keeps it
// so the dead stay nameable once their savefile is gone.
int add_member(string surname, string id, string name)
{
  mapping family;

  family = query_family(surname);
  if (!family || !id || !strlen(id))
    return 0;

  if (family[FAMILY_MEMBERS][id])
    return 0;

  family[FAMILY_MEMBERS][id] = ([ ]);
  family[FAMILY_HISTORY][id] = ([ FAMILY_NAME: name ? name : id ]);
  member_of[id] = surname;
  _save();
  return 1;
}

// Somebody married out. They leave the members but stay in the history, marked
// with where they went -- which is how the house they left can still say whose
// children they were.
int member_married_out(string id, string into)
{
  mapping family;
  string surname;

  surname = family_of(id);
  family = surname ? query_family(surname) : nil;
  if (!family || !family[FAMILY_MEMBERS][id])
    return 0;

  map_delete(family[FAMILY_MEMBERS], id);
  if (family[FAMILY_HISTORY][id])
    family[FAMILY_HISTORY][id][FAMILY_FATE] = FAMILY_MARRIED + " " + into;
  map_delete(member_of, id);

  // a house nobody lives in holds nothing, however it emptied: the last of a
  // line marrying away leaves the roof as free as the last of it dying
  if (!map_sizeof(family[FAMILY_MEMBERS]))
    family[FAMILY_PROPERTIES] = ({ });

  _save();
  return 1;
}

// Somebody died. They leave the members and are marked in the history. If nobody
// living is left the family is extinct: its properties are freed, and the
// record stays as history so the name is never minted again.
int member_died(string id)
{
  mapping family, members;
  string surname;
  string * ids;
  int i;

  surname = family_of(id);
  family = surname ? query_family(surname) : nil;
  if (!family || !family[FAMILY_MEMBERS][id])
    return 0;

  members = family[FAMILY_MEMBERS];

  // a widow is not still married to the dead
  ids = map_indices(members);
  for (i = 0; i < sizeof(ids); i++)
    if (members[ids[i]][FAMILY_SPOUSE] == id)
      map_delete(members[ids[i]], FAMILY_SPOUSE);

  map_delete(members, id);
  if (family[FAMILY_HISTORY][id])
    family[FAMILY_HISTORY][id][FAMILY_FATE] = FAMILY_DIED;
  map_delete(member_of, id);

  if (!map_sizeof(members))
    family[FAMILY_PROPERTIES] = ({ });

  _save();
  return 1;
}

// Whether a house has died out: it held people once and holds none now. A house
// just founded and not yet moved into is empty but not extinct, and the history is
// what tells the two apart. A house with a player in it is never extinct on its
// own, which this answers for free: the player is a member until they leave.
int is_extinct(string surname)
{
  mapping family;

  family = query_family(surname);
  if (!family)
    return 0;

  return !map_sizeof(family[FAMILY_MEMBERS]) &&
          map_sizeof(family[FAMILY_HISTORY]);
}

// ---------------------------------------------------------------------------
// Relations
// ---------------------------------------------------------------------------

// Marry two members of the same family. Moving the incomer into their spouse's
// family is the caller's business (see the descent rule on the citizenship);
// by the time this runs they are both here.
int set_spouse(string id, string other)
{
  mapping family;
  string surname;

  surname = family_of(id);
  family = surname ? query_family(surname) : nil;
  if (!family || !family[FAMILY_MEMBERS][id] || !family[FAMILY_MEMBERS][other])
    return 0;

  family[FAMILY_MEMBERS][id][FAMILY_SPOUSE] = other;
  family[FAMILY_MEMBERS][other][FAMILY_SPOUSE] = id;
  _save();
  return 1;
}

// Whose child somebody is. The parents may be in another house -- a mother who
// married out keeps her children here -- so they are ids, not members.
int set_parents(string id, string * parents)
{
  mapping family;
  string surname;

  surname = family_of(id);
  family = surname ? query_family(surname) : nil;
  if (!family || !family[FAMILY_MEMBERS][id])
    return 0;

  family[FAMILY_MEMBERS][id][FAMILY_PARENTS] = parents ? parents : ({ });
  _save();
  return 1;
}

// ---------------------------------------------------------------------------
// Property
// ---------------------------------------------------------------------------

int add_property(string surname, string file)
{
  mapping family;

  family = query_family(surname);
  if (!family || !file || !strlen(file))
    return 0;

  if (member_array(file, family[FAMILY_PROPERTIES]) != -1)
    return 0;

  family[FAMILY_PROPERTIES] += ({ file });
  _save();
  return 1;
}

int remove_property(string surname, string file)
{
  mapping family;

  family = query_family(surname);
  if (!family)
    return 0;

  if (member_array(file, family[FAMILY_PROPERTIES]) == -1)
    return 0;

  family[FAMILY_PROPERTIES] -= ({ file });
  _save();
  return 1;
}

string * query_properties(string surname)
{
  mapping family;

  family = query_family(surname);
  return family ? family[FAMILY_PROPERTIES] : ({ });
}

// Who owns a place. Walks the register rather than keeping a second index: a
// game holds hundreds of families at most, and a location already carries the
// surname on its home component for the common lookup.
string owner_of(string file)
{
  mapping all;
  string * names;
  int i;

  all = families;
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
  string prefix;
  string * names, * out, * props;
  int i, j;

  if (!area_path || !strlen(area_path))
    return ({ });

  all = families;
  names = map_indices(all);
  prefix = "/save/games/" + game_from_path(area_path) + "/locations/" +
           area_path;
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
