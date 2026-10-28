// Base of every trait. A trait object holds behaviour only and is never
// cloned: what changes from one living to the next is the data mapping the
// living keeps for it, which every hook receives. A trait overrides the hooks
// it needs; the defaults do nothing. See traits.md.

#include <living/traits.h>

/**
 * The fixed contributions of the trait to the living that carries it, which
 * the living sums over all its traits whenever they change.
 *
 * @param  living  who carries it
 * @param  data    the data of this trait on that living
 * @return a mapping: TRAIT_EFFECT_STATS -> ([ "dex" : -1 ]) (see TRAIT_STATS
 *         in <living/traits.h> for the names) and TRAIT_EFFECT_FLAGS -> ({
 *         "flag", ... }); both optional
 */
mapping trait_effects(object living, mapping data)
{
  return ([ ]);
}

/**
 * One line the trait adds when somebody looks at the living, in the mud's
 * language; the living adds the newline if it is missing.
 *
 * @return the line, nil for none
 */
string trait_look(object living, mapping data)
{
  return nil;
}

/**
 * The trait was just added. `data` is the mapping the living stores, so the
 * hook may complete it in place (a counter, a default).
 */
void trait_added(object living, mapping data)
{
}

/**
 * The trait is gone from the living.
 *
 * @param  reason  one of TRAIT_END_*
 */
void trait_removed(object living, mapping data, string reason)
{
}

/**
 * One heart_beat of the trait, before combat. It may change `data` in place.
 *
 * @return 1 to say the trait is over (it ends with TRAIT_END_EXPIRED)
 */
int trait_tick(object living, mapping data)
{
  return 0;
}

/**
 * The living already carries this trait and it is added again: which data
 * stays. Returning `old` keeps the trait as it is; returning `incoming` replaces it
 * (trait_removed with TRAIT_END_REPLACED, then trait_added with the new data).
 */
mapping trait_merge(object living, mapping old, mapping incoming)
{
  return incoming;
}
