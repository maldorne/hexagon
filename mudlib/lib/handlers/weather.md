# Giving a game its own clock

This handler is the shared one. Every game that does not provide its own falls
back to it, and they all get the *same* object and the same save file, so they
share one clock and one sky. A game only needs its own when its world should run
on a different date, hour or climate.

Resolution is by game: `handler("weather", ob)` looks for
`/games/<game>/handlers/weather` first and comes here when there is none. The
game is taken from `ob`, so pass the player or the location — a file under
`/lib/` belongs to no game and would always resolve to this one.

## The two steps

**1. The handler.** `/games/<game>/handlers/weather.c`, inheriting this and
pointing its state somewhere of its own. Without the override it would write
over the shared save file and the separation would be lost:

    inherit "/lib/handlers/weather";

    string query_save_file()
    {
      return "/save/games/<game>/weather.o";
    }

**2. The crontab entry.** Nothing advances a clock by itself. Add a line beside
the ones already there:

    */2 * /games/<game>/handlers/weather advance

Each call moves that clock one game hour, so the interval decides how long a day
lasts. Keep the weather line above the areas one: the areas handler reads the
hour on the same minute to drive the NPC schedules, and wants it already
advanced.

Miss the second step and the game reads a date frozen at whatever hour the mud
last booted on, which is easy to overlook because everything else keeps working.

# Climate zones

A zone is a stretch of world that shares one sky. The table lists them with
their rain, wind and temperature, the averages the handler drifts them back
towards, and which zones they border -- weather leans on its neighbours, so an
adjacency graph is what lets a front travel.

Which zone a place belongs to is asked of the table, by **directory**:

    string query_zone(string dir)

The handler hands it a game-relative directory such as `areas/<area>/rooms`, and
the table walks up until one of its declared areas answers. Declaring
`areas/<area>` therefore covers everything beneath it -- its rooms, its npcs, any
sub-area -- so a zone is named once per area, not once per corner of it.

That directory is the same whether the place is a room or a location: a room is a
`.c` under `/games/<game>/areas/...`, and the location it became is a clone whose
own file name says nothing, so the handler reads the save path instead and
reduces both to one key. A game that never converts a room keeps working, and a
game with no locations at all can still declare zones.

Without a table of its own a game shares the lib one.

## Applying a change

Adding a zone reaches a running mud on the next boot, because the handler
notices zones the table has and its own state does not. **Editing** an existing
one does not: the handler keeps its own copy, drifting from the averages it read
when it first saw the zone. Removing one does not either -- its state simply
stays. To make the handler take the table as it now stands:

    exec return load_object("/games/<game>/handlers/weather")->reset_zones();

That discards the current weather everywhere and starts again from the declared
averages, so it is a deliberate act, not something to run casually. Note the
explicit path: a command runs from a file that belongs to no game, so asking for
the handler by name would resolve the lib one.

The table and the handler are independent. A game can override either without
the other: its own clock on the shared climate, or its own climate on the shared
clock.
