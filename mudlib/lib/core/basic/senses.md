# Senses

Anything can smell, sound, taste or feel of something: a room, a location, an
item, an npc, a prop. The data lives on every object (`/lib/core/basic/senses.c`,
inherited by `object.c`); four player commands ask for it.

## Giving something a sense

    set_smell("Huele a humo de leña y a hierbas.\n");      // the object itself
    add_smell(({ "aire", "callejón" }), "Huele a humedad."); // a word of a place

`set_smell`, `set_sound`, `set_taste` and `set_feel` give what the object itself
is like. `add_smell`, `add_sound`, `add_taste` and `add_feel` give a text to one
word or a list of them, for the parts of a place that are not objects (the air,
the flowers); `remove_*` takes a word away. `query_sense(sense, word)` reads
one, with `sense` being `"smell"`, `"sound"`, `"taste"` or `"feel"`.

## The commands

`oler` / `smell`, `escuchar` / `listen`, `saborear` / `taste` and `tocar` /
`feel`:

- with nothing after them, they answer what the place itself is like;
- with a word, they look for an object here or carried that answers to it (an
  item, an npc, a prop) and give its sense, first for that word and then for
  the object as a whole;
- if nothing here answers to the word, they ask the place for it.

An object or room with a verb of its own for the same thing (a pendant that
does something when touched) still wins: actions are tried before commands.

## Locations and props

A location keeps the senses of the room it is converted from in its `.o` and
puts them back on every load (`set_location_senses`). A prop type declares what
it smells or sounds like in the props table (`PROP_TYPE_SENSES`), and an
instance can change it with `overrides.senses`.
