
/*
 * materials.c
 *
 * Materials catalogue. Maps material identifiers (see
 * include/item/material.h) to localised display strings.
 *
 * Identifiers are concatenable with ":" — `"wood"`, `"wood:oak"`,
 * `"metal:steel:damascus"`. Lookups walk up the hierarchy on miss:
 * `"wood:oak"` -> `"wood"` -> UNKNOWN. The table stays sparse — only
 * the materials that need their own localised name carry an entry.
 *
 * Two forms per material:
 *   query_material_phrase(id)  -> the adjectival form glued to a noun
 *                                (English "wooden", Spanish "de madera").
 *   query_material_name(id)    -> the bare noun form when referring
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
    MAT_UNKNOWN:   _LANG_MATERIAL_UNKNOWN_PHRASE,
    MAT_WOOD:      _LANG_MATERIAL_WOOD_PHRASE,
    MAT_METAL:     _LANG_MATERIAL_METAL_PHRASE,
    MAT_STONE:     _LANG_MATERIAL_STONE_PHRASE,
    MAT_GRANITE:   _LANG_MATERIAL_GRANITE_PHRASE,
    MAT_LEATHER:   _LANG_MATERIAL_LEATHER_PHRASE,
    MAT_CLOTH:     _LANG_MATERIAL_CLOTH_PHRASE,
    MAT_CRYSTAL:   _LANG_MATERIAL_CRYSTAL_PHRASE,
    MAT_SILK:      _LANG_MATERIAL_SILK_PHRASE,
    MAT_WEB:       _LANG_MATERIAL_WEB_PHRASE,
    MAT_COPPER: _LANG_MATERIAL_COPPER_PHRASE,
    MAT_SILVER: _LANG_MATERIAL_SILVER_PHRASE,
    MAT_GOLD: _LANG_MATERIAL_GOLD_PHRASE,
    MAT_MITHRIL: _LANG_MATERIAL_MITHRIL_PHRASE,
    MAT_MITHMEL: _LANG_MATERIAL_MITHMEL_PHRASE,
    MAT_OBSIDIAN: _LANG_MATERIAL_OBSIDIAN_PHRASE,
    MAT_GALENA: _LANG_MATERIAL_GALENA_PHRASE,
    MAT_CHALCOPYRITE: _LANG_MATERIAL_CHALCOPYRITE_PHRASE,
    MAT_SULFUR: _LANG_MATERIAL_SULFUR_PHRASE,
    MAT_COAL: _LANG_MATERIAL_COAL_PHRASE,
    MAT_SPHALERITE: _LANG_MATERIAL_SPHALERITE_PHRASE,
    MAT_HALITE: _LANG_MATERIAL_HALITE_PHRASE,
    MAT_CINNABAR: _LANG_MATERIAL_CINNABAR_PHRASE,
    MAT_PYRITE: _LANG_MATERIAL_PYRITE_PHRASE,
    MAT_ARSENIC: _LANG_MATERIAL_ARSENIC_PHRASE,
    MAT_ZHIRUM: _LANG_MATERIAL_ZHIRUM_PHRASE,
    MAT_DIAMOND: _LANG_MATERIAL_DIAMOND_PHRASE,
  ]);

  names = ([
    MAT_UNKNOWN:   _LANG_MATERIAL_UNKNOWN_NAME,
    MAT_WOOD:      _LANG_MATERIAL_WOOD_NAME,
    MAT_METAL:     _LANG_MATERIAL_METAL_NAME,
    MAT_STONE:     _LANG_MATERIAL_STONE_NAME,
    MAT_GRANITE:   _LANG_MATERIAL_GRANITE_NAME,
    MAT_LEATHER:   _LANG_MATERIAL_LEATHER_NAME,
    MAT_CLOTH:     _LANG_MATERIAL_CLOTH_NAME,
    MAT_CRYSTAL:   _LANG_MATERIAL_CRYSTAL_NAME,
    MAT_SILK:      _LANG_MATERIAL_SILK_NAME,
    MAT_WEB:       _LANG_MATERIAL_WEB_NAME,
    MAT_COPPER: _LANG_MATERIAL_COPPER_NAME,
    MAT_SILVER: _LANG_MATERIAL_SILVER_NAME,
    MAT_GOLD: _LANG_MATERIAL_GOLD_NAME,
    MAT_MITHRIL: _LANG_MATERIAL_MITHRIL_NAME,
    MAT_MITHMEL: _LANG_MATERIAL_MITHMEL_NAME,
    MAT_OBSIDIAN: _LANG_MATERIAL_OBSIDIAN_NAME,
    MAT_GALENA: _LANG_MATERIAL_GALENA_NAME,
    MAT_CHALCOPYRITE: _LANG_MATERIAL_CHALCOPYRITE_NAME,
    MAT_SULFUR: _LANG_MATERIAL_SULFUR_NAME,
    MAT_COAL: _LANG_MATERIAL_COAL_NAME,
    MAT_SPHALERITE: _LANG_MATERIAL_SPHALERITE_NAME,
    MAT_HALITE: _LANG_MATERIAL_HALITE_NAME,
    MAT_CINNABAR: _LANG_MATERIAL_CINNABAR_NAME,
    MAT_PYRITE: _LANG_MATERIAL_PYRITE_NAME,
    MAT_ARSENIC: _LANG_MATERIAL_ARSENIC_NAME,
    MAT_ZHIRUM: _LANG_MATERIAL_ZHIRUM_NAME,
    MAT_DIAMOND: _LANG_MATERIAL_DIAMOND_NAME,
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
    return table[MAT_UNKNOWN];

  parts = explode(id, ":");

  for (i = sizeof(parts); i > 0; i--)
  {
    key = implode(parts[0..i-1], ":");
    if (table[key]) return table[key];
  }

  return table[MAT_UNKNOWN];
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
 * inside player input ("silla de metal" -> "metal", "wooden chair"
 * -> "wood"). Returns nil when nothing matches.
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

/**
 * The material id of what an object answers to query_material: weapons,
 * armours and shields keep a number from their tables (1 wood, 2 metal,
 * 3 leather, 4 cloth); anything else already has its id.
 *
 * @param material the id, or one of those numbers
 * @return the id, nil when there is none
 */
string query_material_id(mixed material)
{
  if (stringp(material))
    return material;

  if (material == 1)
    return MAT_WOOD;
  if (material == 2)
    return MAT_METAL;
  if (material == 3)
    return MAT_LEATHER;
  if (material == 4)
    return MAT_CLOTH;

  return nil;
}

mapping query_phrases() { return phrases; }
mapping query_names()   { return names; }
