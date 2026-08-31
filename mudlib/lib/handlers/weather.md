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

## Beyond the clock

Zone data comes from the weather table, resolved the same way, so a game wanting
its own climate adds `/games/<game>/tables/weather.c` as well. The handler and
the table are independent: either can be overridden without the other.
