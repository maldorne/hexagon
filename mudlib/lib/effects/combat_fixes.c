// Combat helpers mixed into the effect base: weapon-type checks and the
// touch-effect protector resolution.

#include <language.h>

// True if the caster is wielding a weapon of any of the given types.
int check_weapon_type(object caster, string * types)
{
	object * obs;
	int i;

	obs = caster->query_held_ob();

	for (i = 0; i < sizeof(obs); i++)
	{
		if (obs[i] && (member_array(obs[i]->query_weapon_name(), types) != -1) )
			return 1;
	}
	return 0;
}

// For touch effects, take protectors into account: if the victim has a valid
// protector in the same room, the protector takes the effect instead. Returns
// the object that actually receives it.
object check_melee_protector(object victim, object doer)
{
  object prot;

  // anti-error check, Eressea 1/2002
  if ((!victim) || (!doer))
      return nil;
  if ( (prot = victim->query_protector() ) &&
    environment(prot) == environment(victim) &&
    prot->query_protect_valid(doer, victim) )
  {
    tell_object(prot, _LANG_EFFECT_PROTECT_YOU_PRE +
      victim->query_cap_name() + _LANG_EFFECT_PROTECT_YOU_POST);
    tell_object(victim, prot->query_cap_name() +
      _LANG_EFFECT_PROTECT_VICTIM);
    tell_object(doer, prot->query_cap_name() +
      _LANG_EFFECT_PROTECT_DOER);
    tell_room(environment(victim), prot->query_cap_name() +
      _LANG_EFFECT_PROTECT_ROOM_1 + victim->query_cap_name() +
      _LANG_EFFECT_PROTECT_ROOM_2 + doer->query_cap_name() +
      _LANG_EFFECT_PROTECT_ROOM_3,
      ({ doer, victim, prot }) );
    return prot;
  }
  return victim;
}
