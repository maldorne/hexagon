// Legacy spell system. Not used at the moment, kept in case it is useful
// later.

#include <language.h>

mapping environment_mods; /* Per-room-type damage modifiers for a spell.
                             room_type : percentage to apply */

void set_environment_mods(mapping m) { environment_mods = map_copy(m); }
mapping query_environment_mods() { return environment_mods; }

// Adjust the damage according to the type of room the caster is in, using the
// information in environment_mods.
int implement_environment_mods(object caster, int damage) {
  string location;
  int adj;

  location = environment(caster)->query_property("location");
  // Nothing to adjust: return the original damage.
  if(!location || !environment_mods ||
    undefinedp(environment_mods[location]))
      return damage;

  adj = environment_mods[location];

  // Warn the caster when the damage is modified.
  if(!adj)
    tell_object(caster, _LANG_EFFECT_ENV_NO_EFFECT);
  else if(adj < 100)
    tell_object(caster, _LANG_EFFECT_ENV_WEAKER);
  else if(adj > 100)
    tell_object(caster, _LANG_EFFECT_ENV_STRONGER);

  // Final damage.
  return (damage * adj) / 100;
}
