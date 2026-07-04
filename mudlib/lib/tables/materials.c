
/*
 * materials.c
 *
 * Materials catalogue. Maps material identifiers (see
 * include/item/material.h) to localised display strings.
 *
 * Identifiers are concatenable with ":" — `"wood"`, `"wood:oak"`,
 * `"metal:steel:damascus"`. Lookups walk up the hierarchy on miss:
 * `"wood:oak"` → `"wood"` → UNKNOWN. The table stays sparse — only
 * the materials that need their own localised name carry an entry.
 *
 * Two forms per material:
 *   query_material_phrase(id)  → the adjectival form glued to a noun
 *                                (English "wooden", Spanish "de madera").
 *   query_material_name(id)    → the bare noun form when referring
 *                                to the material itself (English
 *                                "wood", Spanish "madera").
 *
 * Callers reach this via the singleton accessor `table("materials")`.
 */

#include <item/material.h>
#include <translations/materials.h>

private mapping phrases;
private mapping names;

void create()
{
  phrases = ([
    UNKNOWN:   _LANG_MATERIAL_UNKNOWN_PHRASE,
    WOOD:      _LANG_MATERIAL_WOOD_PHRASE,
    METAL:     _LANG_MATERIAL_METAL_PHRASE,
    STONE:     _LANG_MATERIAL_STONE_PHRASE,
    LEATHER:   _LANG_MATERIAL_LEATHER_PHRASE,
    CLOTH:     _LANG_MATERIAL_CLOTH_PHRASE,
    CRYSTAL:   _LANG_MATERIAL_CRYSTAL_PHRASE,
  ]);

  names = ([
    UNKNOWN:   _LANG_MATERIAL_UNKNOWN_NAME,
    WOOD:      _LANG_MATERIAL_WOOD_NAME,
    METAL:     _LANG_MATERIAL_METAL_NAME,
    STONE:     _LANG_MATERIAL_STONE_NAME,
    LEATHER:   _LANG_MATERIAL_LEATHER_NAME,
    CLOTH:     _LANG_MATERIAL_CLOTH_NAME,
    CRYSTAL:   _LANG_MATERIAL_CRYSTAL_NAME,
  ]);
}

// Walks the ":"-separated hierarchy from most specific to most
// general, returning the first table entry that exists. Falls back to
// UNKNOWN when nothing matches.
private string _resolve(mapping table, string id)
{
  string * parts;
  string key;
  int i;

  if (!id || !strlen(id))
    return table[UNKNOWN];

  parts = explode(id, ":");

  for (i = sizeof(parts); i > 0; i--)
  {
    key = implode(parts[0..i-1], ":");
    if (table[key]) return table[key];
  }

  return table[UNKNOWN];
}

string query_material_phrase(string id)
{
  return _resolve(phrases, id);
}

string query_material_name(string id)
{
  return _resolve(names, id);
}

/*
 * Reverse lookup: given a localised phrase, return the material id
 * that produces it. Used by the props parser to detect qualifiers
 * inside player input ("silla de metal" → "metal", "wooden chair"
 * → "wood"). Returns nil when nothing matches.
 */
string query_id_from_phrase(string phrase)
{
  string * ids;
  int i;

  if (!phrase || !strlen(phrase)) return nil;

  ids = map_indices(phrases);
  for (i = 0; i < sizeof(ids); i++)
    if (phrases[ids[i]] == phrase)
      return ids[i];

  return nil;
}

mapping query_phrases() { return phrases; }
mapping query_names()   { return names; }
