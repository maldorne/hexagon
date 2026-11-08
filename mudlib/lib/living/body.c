/*
 * The body of a living, as its race defines it: its size and the locations a
 * blow lands on. The living keeps nothing of its own here; every answer comes
 * from its race object.
 *
 * Location system based on an original idea by Iolo@Rl, adapted for CcMud by
 * neverbot 4/03. Moved out of social.c, neverbot 11/2026.
 */

/**
 * The names of the body locations of this living's race; used for debugging,
 * the fight itself picks one with obtain_location.
 *
 * @return the location names, or 0 for a living without a race
 */
mixed query_locations()
{
  string race;

  race = this_object()->query_race_ob();
  if (race)
    return race->query_locations();
  return 0;
}

/**
 * A random location of the body to hit, as the race decides.
 *
 * @return ({ damage multiplier, location name, body type }), or 0 for a living
 *         without a race
 */
mixed obtain_location()
{
  string race;

  race = this_object()->query_race_ob();
  if (race)
    return race->obtain_location();
  return 0;
}

/**
 * The size of the body, as the race gives it; 5 (a human) for a living
 * without a race. neverbot 6/03
 */
int query_body_size()
{
  string race;

  race = this_object()->query_race_ob();
  if (race)
    return (int)race->query_body_size();
  return 5;
}
