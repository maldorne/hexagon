// Acid component. Attach to a location whose floor is a pool of acid: whoever
// comes in or stays is burned, at most once every LOCATION_BURN_PULSE seconds:
// the time of the last acid burn is kept on the living, so walking through
// several pools does not burn it at every step. The damage is of type acid, so the acid resistance of
// the living (a spell, armour) lowers it.
//
// The stay is counted on the heart_beats of the livings in the pool (the
// location tells its components of each one), so a monster nobody can see,
// whose heart_beat is off, burns no more.

#include <room/location.h>
#include <translations/combat.h>
#include <language.h>

inherit component "/lib/location/component.c";

// damage when stepping in: ACID_ENTER_BASE + random(ACID_ENTER_SPREAD)
#define ACID_ENTER_BASE 4
#define ACID_ENTER_SPREAD 10
// damage of each burn while staying
#define ACID_STAY_BASE 4
#define ACID_STAY_SPREAD 8


void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_ACID);
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}

/**
 * Whether the living was burned by acid, here or in another pool, less than
 * LOCATION_BURN_PULSE seconds ago.
 */
private int burned_lately(object who)
{
  mixed last;

  last = who->query_static_property(LOCATION_BURN_PROP + DAMAGE_ACID);
  return intp(last) && last && time() - last < LOCATION_BURN_PULSE;
}

/**
 * Burns a living with damage of type acid, so its resistance counts.
 */
private void burn(object who, int damage, string message)
{
  who->add_static_property(LOCATION_BURN_PROP + DAMAGE_ACID, time());
  tell_object(who, message);
  who->spell_damage(damage, DAMAGE_ACID);
}

/**
 * Whether the acid burns this object: a living, alive.
 */
private int burns(object ob)
{
  return ob && living(ob) && !ob->query_dead();
}

/**
 * Whoever steps in is burned, unless it was burned lately.
 */
void event_enter(object ob, varargs string msg, object from, mixed avoid)
{
  if (!burns(ob) || burned_lately(ob))
    return;

  burn(ob, ACID_ENTER_BASE + random(ACID_ENTER_SPREAD), _LANG_ACID_ENTER);
}

/**
 * A living in the pool has had another heart_beat: it is burned again when
 * LOCATION_BURN_PULSE seconds have passed since its last acid burn.
 *
 * @param who  the living
 */
void event_heart_beat(object who)
{
  if (!burns(who) || burned_lately(who))
    return;

  burn(who, ACID_STAY_BASE + random(ACID_STAY_SPREAD), _LANG_ACID_BURN);
}
