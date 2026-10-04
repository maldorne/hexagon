/*
 * Families handler.
 *
 * The register of every family in a game and the only thing that knows who
 * belongs to whom: who is married to whom, whose child somebody is. A living
 * stores nothing about it; it asks here. What a family owns is on the books of
 * the areas its houses stand in, which say who owns each one.
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
 *                                                           "widow_of": id,
 *                                                           "parents": ({ id }) ]) ]),
 *                                 "history":     ([ id : ([ "name": s,
 *                                                           "fate": s ]) ]) ])
 *
 * A surname is spent for good. When the last member dies the family is extinct:
 * its houses are freed, but the record and its history stay, and the name is
 * never minted again. A family holding a player never goes extinct on its own.
 */

#include <mud/config.h>
#include <living/family.h>
#include <namegen.h>
#include <areas/area.h>
#include <basic/gender.h>

inherit "/lib/core/object.c";

//   families:  ([ surname : family record ])
//   member_of: ([ member id : surname ]), so a living finds its house without
//              walking the register
mapping families;
mapping member_of;

private void _save();
string query_save_file();
private void _vacate_houses(string surname);
int is_extinct(string surname);

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

// Whether somebody is married or has been: a widow counts as much as a wife.
int has_been_married(string id)
{
  mapping family;
  string surname;

  surname = family_of(id);
  family = surname ? query_family(surname) : nil;
  if (!family || !family[FAMILY_MEMBERS][id])
    return 0;

  return family[FAMILY_MEMBERS][id][FAMILY_SPOUSE] ||
         family[FAMILY_MEMBERS][id][FAMILY_WIDOW_OF];
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
                    FAMILY_HISTORY:     ([ ]) ]);
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

  // a house that has died out stays history: its name is never taken up again
  if (family[FAMILY_MEMBERS][id] || is_extinct(surname))
    return 0;

  family[FAMILY_MEMBERS][id] = ([ ]);
  family[FAMILY_HISTORY][id] = ([ FAMILY_NAME: name ? name : id ]);
  member_of[id] = surname;
  _save();
  return 1;
}

// Write down the name somebody goes by, when the history only knows them by id.
// An NPC can join a house before it has been named, and the history is the only
// place that will still name it once its savefile is gone.
void name_member(string id, string name)
{
  mapping history;
  string surname;

  surname = family_of(id);
  if (!surname || !name || !strlen(name))
    return;

  history = query_history(surname);
  if (!history[id] || history[id][FAMILY_NAME] == name)
    return;

  if (history[id][FAMILY_NAME] && history[id][FAMILY_NAME] != id)
    return;

  history[id][FAMILY_NAME] = name;
  _save();
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
    _vacate_houses(surname);

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

  // a widow is not still married to the dead, but remembers having been
  ids = map_indices(members);
  for (i = 0; i < sizeof(ids); i++)
    if (members[ids[i]][FAMILY_SPOUSE] == id)
    {
      map_delete(members[ids[i]], FAMILY_SPOUSE);
      members[ids[i]][FAMILY_WIDOW_OF] = id;
    }

  map_delete(members, id);
  if (family[FAMILY_HISTORY][id])
    family[FAMILY_HISTORY][id][FAMILY_FATE] = FAMILY_DIED;
  map_delete(member_of, id);

  if (!map_sizeof(members))
    _vacate_houses(surname);

  _save();
  return 1;
}

// The areas of this register's game that keep books of their own: each
// community's root, where its houses are recorded.
private object * _root_areas()
{
  string * paths;
  object * out;
  string game;
  int i;

  game = game_from_path(object_name(this_object()));
  paths = (string *)AREA_HANDLER->query_area_paths(game);
  out = ({ });

  for (i = 0; i < sizeof(paths); i++)
  {
    object area;

    area = AREA_HANDLER->query_area(paths[i]);
    if (area && area->query_root_area() == area)
      out += ({ area });
  }

  return out;
}

// A house that has nobody left gives up everything it owned: every house on the
// books in its name stands empty and unowned, so the next family can move in.
private void _vacate_houses(string surname)
{
  object * areas;
  int i;

  areas = _root_areas();
  for (i = 0; i < sizeof(areas); i++)
    areas[i]->vacate_houses_of(surname);
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

// Marry two people, one of whom at least belongs to a house. Each is given as
// ([ "id": family id, "name": what to call them, "gender": GENDER_* ]), so
// nobody has to be in the world for it. Which of them moves is the land's
// business, not theirs: `rule` is the descent of the citizenship the wedding
// happens under, which is the only answer that does not depend on who is asked
// first. Returns the surname they share afterwards, or nil.
string wed(mapping who, mapping other, string rule)
{
  mapping keeps, joins, swap;
  string kept, left;

  if (!who || !other || who["id"] == other["id"])
    return nil;
  if (!family_of(who["id"]) && !family_of(other["id"]))
    return nil;

  if (rule == DESCENT_MATRILINEAL)
    keeps = (who["gender"] == GENDER_FEMALE) ? who : other;
  else
    keeps = (who["gender"] == GENDER_FEMALE) ? other : who;
  joins = (keeps == who) ? other : who;

  // the one who would keep the house has none: the other's stands instead
  if (!family_of(keeps["id"]))
  {
    swap = keeps;
    keeps = joins;
    joins = swap;
  }

  kept = family_of(keeps["id"]);
  left = family_of(joins["id"]);

  if (left && left != kept)
    member_married_out(joins["id"], kept);

  if (left != kept)
    add_member(kept, joins["id"], joins["name"]);

  if (!set_spouse(who["id"], other["id"]))
    return nil;

  return kept;
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

// The houses a family owns, read off the books of every community in the game.
string * query_properties(string surname)
{
  object * areas;
  string * out;
  int i;

  areas = _root_areas();
  out = ({ });
  for (i = 0; i < sizeof(areas); i++)
    out += (string *)areas[i]->query_houses_owned_by(surname);

  return out;
}

// The families holding property inside an area, so a builder can be told what a
// wipe would take with it.
string * families_of_area(string area_path)
{
  object * areas;
  string prefix;
  string * out;
  int i, j;

  if (!area_path || !strlen(area_path))
    return ({ });

  prefix = "/save/games/" + game_from_path(area_path) + "/locations/" +
           area_path;
  areas = _root_areas();
  out = ({ });

  for (i = 0; i < sizeof(areas); i++)
  {
    mapping owners;
    string * files;

    owners = (mapping)areas[i]->query_house_owners();
    files = map_indices(owners);
    for (j = 0; j < sizeof(files); j++)
      if (strlen(files[j]) >= strlen(prefix) &&
          files[j][0 .. strlen(prefix) - 1] == prefix &&
          member_array(owners[files[j]], out) == -1)
        out += ({ owners[files[j]] });
  }

  return out;
}
