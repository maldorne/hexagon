
// neverbot, 03/2014

// Override kfuns

mixed * map_indices(mapping map)
{
  if (map == nil)
    return ({ });
  return ::map_indices(map);
}

mixed * map_values(mapping map)
{
  if (map == nil)
    return ({ });
  return ::map_values(map);
}

// Mudos function names

mixed * keys(mapping map) { return map_indices(map); }
mixed * values(mapping map) { return map_values(map); }

mixed * m_indices(mapping map) { return map_indices(map); }
mixed * m_values(mapping map) { return map_values(map); }
int m_sizeof(mapping map) { return map_sizeof(map); }

// Added, non existant in dgd

mapping map_delete(mapping map, mixed key) 
{
  // old mudos version
  // map = map + ([ ]);
  // map_delete(map, key);

  map[key] = nil;
  
  return map;
} 

mapping m_delete(mapping map, mixed key) { return map_delete(map, key); }

mapping map_copy(mapping map) 
{
  mapping result;

  if (typeof(map) != T_MAPPING)
    return ([ ]);

  result = map[..];

  return result;
}

mapping m_copy(mapping map) { return map_copy(map); }

