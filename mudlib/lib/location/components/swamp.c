// Swamp component. Attach to marsh and bog: reeds, stagnant pools, wet ground
// that is not water to swim in (that is the water component). Contributes to
// the sector-level "swamp" tally so the world map draws a green wave glyph.
//
// The mud holds back whoever tries to leave: the weaker and clumsier the
// living (strength plus dexterity), the more often the step is lost. A race
// whose object answers query_swamp_walker() with 1 and a ghost are never held.
// A rider is judged by its mount: the mount walks the bog freely when its race
// does or when its spec says so (RIDE_SWAMP_WALKER), and otherwise its own
// strength and dexterity decide. The location asks it through
// query_exit_refusal, so a living moved by force_exit is not held either.
//
// Kind, depth and the plants that grow come from the vegetation base; with no
// kind named by the area, it takes the game's "swamp" kind if there is one.

#include <room/location.h>
#include <language.h>
#include <living/riding.h>

inherit vegetation "/lib/location/components/vegetation.c";

void create()
{
  vegetation::create();
  set_type(LOCATION_COMPONENT_SWAMP);
}

/**
 * Percent chance of being held back, from strength plus dexterity.
 *
 * @param  who  the living trying to leave
 * @return a percentage, 0 for somebody strong and quick enough
 */
int query_stuck_chance(object who)
{
  int power;

  power = who->query_str() + who->query_dex();

  if (power > 50)
    return 0;
  if (power > 40)
    return 20;
  if (power > 35)
    return 30;
  if (power > 30)
    return 40;
  return 50;
}

/**
 * Whether a living walks the bog freely: its race does, or the mount it rides
 * (by its race or by its spec).
 */
int query_walks_bog(object who)
{
  mapping spec;
  object mount;
  string race;

  mount = who->query_mount();
  if (mount)
  {
    spec = handler("riding", who)->query_spec(mount);
    if (spec && spec[RIDE_SWAMP_WALKER])
      return 1;
    who = mount;
  }

  race = who->query_race_ob();
  if (!stringp(race) || !strlen(race))
    return 0;

  return race->query_swamp_walker() ? 1 : 0;
}

/**
 * The location asks this before every typed exit; a message means the living
 * is stuck in the mud and stays where it is.
 *
 * @param  dir  the exit tried
 * @param  who  the living trying it
 * @return the message, or nil if it goes through
 */
string query_exit_refusal(string dir, object who)
{
  int chance;

  if (!who || who->query_dead() || query_walks_bog(who))
    return nil;

  // a rider is held by the mount's strength, not its own
  chance = query_stuck_chance(who->query_mount() ? who->query_mount() : who);
  if (!chance || random(100) >= chance)
    return nil;

  return _LANG_SWAMP_STUCK;
}
