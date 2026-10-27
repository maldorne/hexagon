# Chatter

A monster says, emotes or does things on its own now and then: its **chats**
while at peace, its **attack chats** (`achat`) while fighting. The code is
`/lib/monster/chatter.c`, inherited by `/lib/monster.c`, whose heart_beat rolls
it.

## When a monster chats

On every heart_beat (`HEART_BEAT_TIME`, 2 seconds, in `<mud/mudos.h>`) the
monster calls `chatter(chance, lines)` with one of its two sets:

- fighting (anybody in its attacker list or chased): the attack chats;
- otherwise, unless `query_resting()` says it is off work: the chats. Chats are
  written for the working day, so somebody resting at home keeps quiet;
- hidden (`query_hide_shadow()`): nothing.

It speaks when `random(1000) < chance`: the chance is **per thousand per beat**,
not a percentage.

| chance | per beat | a chat every (on average) |
|---|---|---|
| 10 | 1% | 200 s |
| 30 | 3% | 67 s |
| 60 | 6% | 33 s |
| 100 | 10% | 20 s |

Then it picks one line by weight: a line of weight 2 is said twice as often as
one of weight 1. A line said one time in N needs about N times the interval
above to be heard, so a hint meant for players needs a fair chance or a large
weight.

A monster's heart_beat stops when nobody living shares its place and it is at
full health with nothing pending (`check_heart_beat` in `monster.c`), and starts again when somebody
comes in. So a monster alone never chats, and the wait starts when you arrive.

## Lines

Each line is a string, read by its first character:

| starts with | what happens |
|---|---|
| `'` | the monster says the rest (`do_say`) |
| `:` | the room sees `<Name> <rest>`, an emote; `$…$` tokens are expanded |
| `@` | the room sees the rest as it is |
| `#` | the monster calls the function named by the rest on itself |
| anything else | queued as a command the monster types (`queue_action`) |

A plain sentence with no prefix is a command, so `Buenos días.` is an unknown
verb; the line to say it is `'Buenos días.`.

A line can also be an array of strings, a **story**: once drawn, the monster
says one of them every beat, without rolling, until it reaches the end.

### Tokens in emotes

In a `:` line, `$<who><what>$` is replaced. `<who>` is `m` (the monster), `l`
(another living in its place, or one of its attackers) or `o` (any other object
in its place). `<what>` is `name`, `cname`, `gender`, `poss`, `obj`, `gtitle`,
`pronoun`, `race`, `guild`, `group`, `racegroup` or `class`. So
`:mira a $lcname$ de arriba abajo.` names whoever it looks at.

## In a template

An NPC template gives both sets as a chance and a flat weight/line list:

```json
"chat":   { "chance": 30, "lines": [ 1, ":bosteza.", 2, "'Buen día." ] },
"a_chat": { "chance": 50, "lines": [ 1, "'¡A mí la guardia!" ] }
```

They are the type's words, not the person's: `apply_template` (`/lib/npc.c`)
clears and reloads them from the template every time the NPC is materialized,
so a changed template reaches everyone already alive. A type with no `chat`
key says nothing.

## API

All in `chatter.c`; each `chat` function has its `achat` twin
(`load_a_chat`, `set_achat_chance`, ...).

- `load_chat(chance, ({ weight, line, ... }))`: adds the lines and sets the
  chance.
- `clear_chat()`: forgets every line, before loading a new set over an old one.
- `add_chat_string(weight, line)`, `remove_chat_string(line)`: one line, or a
  flat list of them.
- `set_chat_chance(n)`, `query_chat_chance()`.
- `query_chat_string()`: `({ total weight, ({ weight, line, ... }) })`. The
  first number is the sum of the weights, not the chance.
- `chatter(chance, set)`: one roll, as the heart_beat does it; with a chance of
  1000 it always speaks, which is how to hear a monster's lines when testing.
