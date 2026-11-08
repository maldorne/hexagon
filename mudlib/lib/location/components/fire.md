# Fire component

`/lib/location/components/fire.c` (type `LOCATION_COMPONENT_FIRE`, `"fire"`): a
burning location. It works like `acid`: whoever enters or stays takes
`6 + random(10)` damage, at most once every `LOCATION_BURN_PULSE` (5) seconds.
The time of the last fire burn is kept on the living (static property
`LOCATION_BURN_PROP + DAMAGE_FIRE`), so running through several burning
locations burns once per pulse, not at every step. The stay is counted on the
`event_heart_beat` of each living. The damage is
`spell_damage(n, DAMAGE_FIRE)`, so the fire resistance of the living lowers it;
at 100 the living takes nothing and is told nothing.

## Settings

Saved with the component (`query_auto_load_attributes`):

- `fire_end`: absolute `time()` at which the fire ends, 0 for a permanent one.
  Being absolute and saved, the fire is over at the first check past that
  time, even after a reboot.
- `fire_quenchable`: 0 or 1.

When seeding the component the caller gives instead `duration` (seconds, 0 or
missing: permanent) and `quenchable`; the component turns `duration` into
`fire_end`.

## Flames

On attach the component adds the fixture `/lib/obj/flames.c` to the location
(`add_fixture`, only if the location does not list it yet, so a reload never
duplicates it). The fixture only makes the fire visible. When the component is
dropped (`dest_me`) it removes the fixture.

## API

- `douse(int seconds)`: takes seconds off the time left, for a water or a cold
  spell. Only a timed, quenchable fire responds (returns 1); a permanent fire is
  never doused, quenchable or not (returns 0). When the time left reaches zero
  the fire ends.
- `query_fire_time_left()`: seconds left, 0 if permanent or over.
- `query_permanent()`, `query_quenchable()`.
- `end_fire()`: tells the room and drops the component.
- `check_over()`: ends the fire if its time is over. Called on every enter and
  heart_beat; there is no call_out, so a fire nobody visits stays until the
  next living comes in. The end time is saved with the location and survives
  a reboot.

## Setting a location on fire

From a spell or any code:

    place->add_component(LOCATION_COMPONENT_FIRE,
                         ([ "duration": 120, "quenchable": 1 ]));

Adding it to a location that already burns reseeds the live fire. To douse:

    object fire;

    fire = place->query_component_by_type(LOCATION_COMPONENT_FIRE);
    if (fire)
      fire->douse(30);

## Rooms

A room that answers `query_fire()` with 1 gets a permanent fire that cannot be
doused when `room2loc` converts it.
