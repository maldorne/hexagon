// Spells catalogue: every spell by its id (its file name without .c) -> the
// path of its object, so a spell can move without breaking what the players
// learned (they keep the id); and the categories spells belong to, category
// id -> ({ kind, display name }). The kind is SPELL_CATEGORY_SPHERE (divine magic: clerics,
// paladins) or SPELL_CATEGORY_SCHOOL (arcane magic: mages, sorcerers). A spell
// names its categories with add_category, a guild says how well its members
// know them with set_spheres / set_schools (/lib/guild.c), and this table says
// what each one is and what the player reads.
//
// The mudlib knows no category of its own: every game declares its own at
// /games/<game>/tables/spells.c (standard hexagon table convention:
// table("spells") resolves the game one first), inheriting this one.

#include <living/effects.h>

private mapping categories;
private mapping spells;

void create()
{
  categories = ([ ]);
  spells = ([ ]);
}

/**
 * Declares a spell.
 * @param  id    the id the livings keep (its file name without .c)
 * @param  path  where its object is now
 */
void add_spell(string id, string path)
{
  spells[id] = path;
}

/**
 * Declares every spell found under a directory, one level of subdirectories
 * deep (<dir>/<sphere>/<spell>.c), by its file name. A file whose object has
 * no rounds (a base other spells inherit) is a spell too here: guilds teach
 * only the spells they name.
 * @param  dir  the directory, ending in /
 */
void add_spells_in(string dir)
{
  string * subdirs, * files;
  int i, j;

  subdirs = get_dir(dir + "*");
  for (i = 0; i < sizeof(subdirs); i++)
  {
    if (file_size(dir + subdirs[i]) != -2)
      continue;

    files = get_dir(dir + subdirs[i] + "/*.c");
    for (j = 0; j < sizeof(files); j++)
      add_spell(files[j][0..strlen(files[j]) - 3],
                dir + subdirs[i] + "/" + files[j]);
  }
}

mapping query_spells() { return ([ ]) + spells; }

// where the spell with that id is now, nil when the catalogue does not list it
string query_spell_path(string id) { return spells[id]; }

/**
 * Declares a category of spells.
 * @param  id    the id spells and guilds use (plain ASCII)
 * @param  kind  SPELL_CATEGORY_SPHERE or SPELL_CATEGORY_SCHOOL
 * @param  name  what the player reads
 */
void add_spell_category(string id, string kind, string name)
{
  categories[id] = ({ kind, name });
}

mapping query_spell_categories() { return ([ ]) + categories; }

int query_spell_category(string id) { return !!categories[id]; }

// SPELL_CATEGORY_SPHERE, SPELL_CATEGORY_SCHOOL, or nil for an unknown id
string query_spell_category_kind(string id)
{
  return categories[id] ? categories[id][0] : nil;
}

// the display name, or the id capitalised for an unknown one
string query_spell_category_name(string id)
{
  return categories[id] ? categories[id][1] : capitalize(id);
}

/**
 * Every category of one kind.
 * @param  kind  SPELL_CATEGORY_SPHERE or SPELL_CATEGORY_SCHOOL
 * @return their ids
 */
string * query_spell_categories_of(string kind)
{
  string * ids, * found;
  int i;

  ids = map_indices(categories);
  found = ({ });
  for (i = 0; i < sizeof(ids); i++)
    if (categories[ids[i]][0] == kind)
      found += ({ ids[i] });

  return found;
}
