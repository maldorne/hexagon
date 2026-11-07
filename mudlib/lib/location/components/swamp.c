// Swamp component. Attach to marsh and bog: reeds, stagnant pools, wet ground
// that is not water to swim in (that is the water component). Contributes to
// the sector-level "swamp" tally so the world map draws a green wave glyph.
//
// The mud holds back whoever tries to leave: the weaker and clumsier the
// living (strength plus dexterity), the more often the step is lost. A race
// that walks the bog (query_swamp_walker in /lib/race.c) and a ghost are never
// held. The location asks it through query_exit_refusal, so a living moved by
// force_exit is not held either.

#include <room/location.h>
#include <language.h>

inherit component "/lib/location/component.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_SWAMP);
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
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
 * Whether the race of a living walks the bog freely.
 */
int query_walks_bog(object who)
{
  string race;
  object race_ob;

  race = who->query_race_ob();
  if (!race)
    return 0;

  race_ob = load_object(race);
  return race_ob && race_ob->query_swamp_walker();
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

  chance = query_stuck_chance(who);
  if (!chance || random(100) >= chance)
    return nil;

  return _LANG_SWAMP_STUCK;
}
