// Acid component. Attach to a location whose floor is a pool of acid: whoever
// comes in is burned at once, and whoever stays is burned again every
// ACID_PULSE seconds. The damage is of type acid, so the acid resistance of
// the living (a spell, armour) lowers it.
//
// The stay is counted on the heart_beats of the livings in the pool (the
// location tells its components of each one), so a monster nobody can see,
// whose heart_beat is off, burns no more.

#include <room/location.h>
#include <translations/combat.h>
#include <language.h>

inherit component "/lib/location/component.c";

// seconds between two burns of whoever stays
#define ACID_PULSE 10
// damage when stepping in: ACID_ENTER_BASE + random(ACID_ENTER_SPREAD)
#define ACID_ENTER_BASE 4
#define ACID_ENTER_SPREAD 10
// damage of each burn while staying
#define ACID_STAY_BASE 4
#define ACID_STAY_SPREAD 8

// when each living was last burned here (never saved)
static mapping burned;

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_ACID);
  burned = ([ ]);
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}

/**
 * Burns a living with damage of type acid, so its resistance counts.
 */
private void burn(object who, int damage, string message)
{
  burned[who] = time();
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
 * Whoever steps in is burned at once.
 */
void event_enter(object ob, varargs string msg, object from, mixed avoid)
{
  if (!burns(ob))
    return;

  burn(ob, ACID_ENTER_BASE + random(ACID_ENTER_SPREAD), _LANG_ACID_ENTER);
}

/**
 * Whoever leaves is forgotten: coming back is a new step in.
 */
void event_exit(object ob, varargs string msg, object dest, mixed avoid)
{
  map_delete(burned, ob);
}

/**
 * A living in the pool has had another heart_beat: it is burned again when
 * ACID_PULSE seconds have passed since its last burn.
 *
 * @param who  the living
 */
void event_heart_beat(object who)
{
  if (!burns(who))
    return;

  // already in the pool when the component was loaded: its stay starts now
  if (!burned[who])
  {
    burned[who] = time();
    return;
  }

  if (time() - burned[who] < ACID_PULSE)
    return;

  burn(who, ACID_STAY_BASE + random(ACID_STAY_SPREAD), _LANG_ACID_BURN);
}
