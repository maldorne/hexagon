/*
 * damage-types.c
 *
 * Damage types catalogue. Maps damage type ids (see
 * include/living/damage-types.h) to the names a player reads, and back.
 * The ids are what the code stores and compares; the names only come out
 * in text and go in when a player types one.
 *
 * Callers reach this via the singleton accessor `table("damage-types")`.
 */

#include <living/damage-types.h>
#include <translations/combat.h>

private mapping names;   // id -> name
private mapping ids;     // name or alias -> id

void create()
{
  mapping aliases;
  string * keys, * words;
  int i, j;

  names = _LANG_DAMAGE_TYPE_NAMES;
  aliases = _LANG_DAMAGE_TYPE_ALIASES;
  ids = ([ ]);

  keys = map_indices(names);
  for (i = 0; i < sizeof(keys); i++)
    ids[names[keys[i]]] = keys[i];

  keys = map_indices(aliases);
  for (i = 0; i < sizeof(keys); i++)
  {
    words = aliases[keys[i]];
    for (j = 0; j < sizeof(words); j++)
      ids[words[j]] = keys[i];
  }
}

/**
 * The name of a damage type as a player reads it.
 *
 * @param id the damage type id (DAMAGE_FIRE...)
 * @return the name, the id itself when it has none
 */
string query_damage_type_name(string id)
{
  if (!id)
    return "";

  if (stringp(names[id]))
    return names[id];

  return id;
}

/**
 * The id of a damage type from what a player typed, or from the id itself.
 *
 * @param name a name or alias in the language of the mud, or an id
 * @return the damage type id, nil when it is none
 */
string query_damage_type_id(string name)
{
  if (!name)
    return nil;

  name = lower_case(name);

  if (stringp(names[name]))
    return name;

  if (stringp(ids[name]))
    return ids[name];

  return nil;
}

/**
 * Renames the keys that are the translated name of a damage type to its id,
 * for data saved before the ids were English. The values stay; a key that is
 * not a name, or whose id is already there, is left as it is.
 *
 * @param data a mapping keyed by damage types, or with some keys that are
 * @return the mapping with the ids for keys
 */
mapping migrate_legacy_keys(mapping data)
{
  mapping result;
  mixed * keys;
  string id;
  int i;

  if (!data)
    return data;

  result = ([ ]);
  keys = map_indices(data);

  for (i = 0; i < sizeof(keys); i++)
  {
    id = nil;
    if (stringp(keys[i]) && !stringp(names[keys[i]]))
      id = ids[keys[i]];

    if (id && !undefinedp(data[id]))
      id = nil;

    result[id ? id : keys[i]] = data[keys[i]];
  }

  return result;
}
