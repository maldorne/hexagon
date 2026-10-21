# balance

Dumps the numbers that balance a game to CSV, and serves a page that charts
them from the mud's own web server.

## Command

Admin level, `balance`:

```
balance [<game>]    the game of the room you are in by default
```

It evaluates, with the game's own code on a test monster (every stat at 13,
levels 1 to 20, power 1 to 3), the game's races, classes, guilds, jobs, the
spells and skills that damage or heal (`query_balance` of each effect, see
`lib/obj/skills/skills.md`) and the baselines they are measured against (hit
points, regeneration). A game adds columns to the baselines with
`query_balance_baselines(level)` in its master. The work is split in steps
(call_outs) and the command says when it is done.

## Where the data goes

One folder per game, `/games/<game>/docs/balance/`: `races.csv`,
`classes.csv`, `guilds.csv`, `jobs.csv`, `effects.csv`, `skills.csv`,
`baselines.csv`, `stat_bonuses.csv`. A regenerated dump overwrites them.

## Viewer

The package adds these routes to the mud's HTTP server (the `binary_port` of
the config), through `query_http_routes()` in its master:

```
/balance                    the games that have balance data
/balance/<game>/            the viewer page (www/balance.html)
/balance/<game>/files.json  names of the game's csv files
/balance/<game>/<name>.csv  one csv file
```

The page reads the CSVs in the browser; any other tool can read them too.

## Files

- `master.c`: the package object, and its HTTP routes.
- `src/balance.c`: the dump.
- `cmds/admin/balance.c`: the command.
- `net/balance.c`: the HTTP handler.
- `www/balance.html`: the viewer.
