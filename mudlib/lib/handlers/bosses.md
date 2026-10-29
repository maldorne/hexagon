# Bosses handler

`/lib/handlers/bosses.c` keeps the record of the NPCs that carry the `boss`
component (`/lib/npc/components/boss.c`), saved in `/save/bosses`. One handler
serves every game: a boss is keyed by its template path (the game is read from
it), so the record survives respawns.

- The component registers the boss when it is placed, so `bosses` / `jefes`
  lists it before anybody has fought it.
- The fight clock starts at the first blow involving the boss (`event_fight_in_progress`)
  and restarts after two minutes without blows. On death (`died`) the component
  hands the handler the killer, the NPC's attacker list and the seconds.
- A player killed by the boss is counted through `event_kill` (see
  `lib/core/basic/events.md`).
- Per boss: deaths, players killed, first and last death, the last 20 deaths
  (killer, group, time, seconds) and the shortest fight. Coders of every kind,
  guests and the test characters in `BOSSES_TEST_NAMES` are left out; a death
  with nobody else in it is not recorded.
- `prune()` drops the bosses whose template is gone or no longer names the
  component (`bosses prune`, coders).
- `forget_boss(key)` removes one record.
