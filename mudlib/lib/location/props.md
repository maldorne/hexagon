# Props — string composition reference

How every player-visible string tied to a prop gets built, from
catalogue template to final output.

## 1. Catalogue pieces (per type)

Verbatim in `include/translations/props.<lang>.h`, one section per
type:

```c
_LANG_PROP_CHAIR_ID / _ID_ALIAS_N       "chair" / "seat"
_LANG_PROP_CHAIR_NOUN / _NOUN_PLURAL    "chair" / "chairs"
_LANG_PROP_CHAIR_SHORT / _LONG          "a $material_phrase$ chair"
                                        "A sturdy $material_phrase$ chair, sized for one."
_LANG_PROP_CHAIR_SIT_VERBS              ({ "sit" })
_LANG_PROP_CHAIR_SIT_ME / _OTHERS       "You sit on the chair."
                                        "$mcname$ sits on the chair."
_LANG_PROP_CHAIR_SUFFIX_TIPPED          " (tipped over)"     // compact (section line)
_LANG_PROP_CHAIR_LONG_TIPPED            " It lies on its side."  // long (look prop)
```

Types wire the strings into `PROP_TYPE_*` slots inside
`lib/tables/props_table.c`.

## 2. Runtime tokens

| Token | Resolved by | Substituted where |
|---|---|---|
| `$material_phrase$` | `_apply_material_tokens` (handler) | `query_type_long` / `query_type_short` |
| `$material_name$` | idem | idem |
| `$colour$` `$symbol$` | `_apply_override_tokens` (handler) | `query_type_long` / `query_type_short`; an unset one disappears |
| `$mcname$` `$mname$` | `handler("souls")->parse_string` | `_render_msg` (component) |
| `$mposs$` `$mpronoun$` `$mobj$` `$mvocal$` `$mnumeral$` | souls parser | `_render_msg` |
| `$lastarg$` | pre-substituted, capitalised if sentence-initial | `_render_msg` |
| `%s` (legacy) | `sprintf(template, arg)` fallback | `_render_msg` |

`$material_phrase$` is the adjectival form (`"wooden"`, `"stone"`);
`$material_name$` is the bare noun (`"wood"`, `"stone"`). Instance
material comes from `overrides.material`, falling back to
`PROP_TYPE_DEFAULT_MATERIAL`.

## 3. Language-ordering templates

One macro per language, variables in scope at the call site:

```c
// props.en.h
_LANG_PROPS_NOUN_PHRASE       count_word + " " + material_phrase + " " + noun
_LANG_PROPS_ID_WITH_MATERIAL  material_phrase + " " + id

// props.es.h — same variables, different order
_LANG_PROPS_NOUN_PHRASE       count_word + " " + noun + " " + material_phrase
_LANG_PROPS_ID_WITH_MATERIAL  id + " " + material_phrase
```

All per-language word ordering lives here. `.c` code stays language-
agnostic.

## 4. Composition points

| Output | Entry | Composition |
|---|---|---|
| **Room prose** (`look`) | `hook_long` -> `_compose_prose` | Groups by `(type, material, state)`. Singleton -> `query_type_long` (LONG_KEY + material tokens + state suffixes). Plural -> `_LANG_PROPS_NOUN_PHRASE`-based sentence + one shared state suffix |
| **Section line** (`You see ...`) | `query_props_section_string` | Same grouping. Per group -> `_LANG_PROPS_NOUN_PHRASE` + compact `STATE_SUFFIXES` for singletons. Joiners: `_LANG_PROPS_LIST_SEPARATOR`, `_LANG_PROPS_LIST_AND` |
| **`look <prop>`** | `find_match` -> `component->long(str)` -> `_find_unique_match` -> `query_type_long` | LONG_KEY with material tokens replaced + `LONG_SUFFIXES` for truthy state + `LONG_SUFFIXES_UNSET` for falsy state + `_LANG_PROPS_YOU_CAN` line listing available verbs |
| **Action messages** (e.g. `sit chair`) | `_execute_generic` -> `_render_msg` | Every `MSG_ME` / `MSG_OTHERS` / `BLOCKED_MSG` / `MISSING_MSG` / `ALREADY_SET_MSG` runs through souls parser. `$mcname$` -> actor cap name, `$mposs$` -> gendered possessive, `$lastarg$` -> state value |
| **Parser aliases** | `component->query_alias` + `query_plurals` | Enumerates type's `id_list` (singular) plus `NOUN_PLURAL` (plural) plus every `_LANG_PROPS_ID_WITH_MATERIAL` variant per attached instance |
| **Numeric disambiguation** (`chair 2`) | `location.find_inv_match` inflates the component N times | `find_match`'s `--num` decrement counts down naturally; component's inner matcher strips the trailing number too |

## 5. State -> strings

The instance's `state` mapping mutates through action `SET_STATE` /
`CLEAR_STATE`. Suffixes read that state:

```
state = { tipped: 1, occupied: "test" }

-> Section line appends _LANG_PROP_CHAIR_SUFFIX_TIPPED   " (tipped over)"
-> Section line appends _LANG_PROP_CHAIR_SUFFIX_OCCUPIED " (test is sitting on it)"  // %s legacy
-> look prop appends    _LANG_PROP_CHAIR_LONG_TIPPED     " It lies on its side."
-> look prop appends    _LANG_PROP_CHAIR_LONG_OCCUPIED   " test is sitting on it."   // %s legacy
```

State suffixes are compact by design; if omitted for a plural group,
prose falls back to the noun phrase only.

## 6. Per-instance overrides

Room authors override any catalogue field on a per-instance basis:

```c
overrides = ([
  "short":       "a golden chair",              // wins over SHORT_KEY
  "description": "A throne forged of solid gold.",  // wins over LONG_KEY
  "material":    "gold",                        // feeds $material_*$ tokens
  "props":       ({ "nailed" }),                // boolean flags that BLOCKED_BY checks
  "inscription": "Here lies nobody.",           // what a show_text action shows
  "language":    "elf",                         // the inscription's language
  "colour":      "red",                         // feeds $colour$, and the list of props
  "symbol":      "a falcon",                    // feeds $symbol$
  "ids":         ({ "stones", "heaps" }),       // more words it answers to
  "plural":      "heaps of rubble",             // a group sharing the short
])
```

Every object in a room is its own instance: eight chairs are eight `chair`
instances, never one instance whose short says "eight chairs", so that each
one can be looked at and used apart. The room's list and prose count them
("You see eight wooden chairs"), `look chair 3` picks one and `look chairs`
describes them all, as looking at a plural does with carried objects. An
instance answers to its type's plural noun, its own `plural` and the plural of
every word it answers to. Instances that
share a `short` group under it, and a group of more than one is named by the
count and their `plural` ("three heaps of rubble"); without a `plural` it
keeps the first instance's short.

A `short` override also names the instance in the room's list of props and
heads `look <prop>`. The first type of the catalogue, `scenery`, is built on
that: it has no actions and no words of its own, so every instance says what
it is with `short`, `ids` and `description` (a heap of rubble, broken tiles),
and the conversion never infers it. In `props set` a list is written
`({ stones, heaps })`.

Three more types are plain things to look at: `milestone`, `arch` and `pergola`, with no
actions of their own. A milestone may carry an `inscription` override, and then
the generic `read` shows it; without one it is only looked at. An arch takes its
description from the instance when it is a particular one.

A colour is also named in the room's list of props ("You see a red stone
rune") and in the header of `look <prop>`, through `_LANG_PROPS_NOUN_WITH_COLOUR`;
instances of different colours are listed apart. A type can add a sentence for
an optional detail with `PROP_TYPE_OVERRIDE_SUFFIXES` (`override_key -> text`),
appended to the long only when the instance sets that override: the rune's
`symbol` gives "It depicts $symbol$." An instance with its own `description`
keeps it whole, without those sentences, but the state suffixes still follow it,
so a described prop still says whether it is lit or tipped over.

An action whose spec carries `PROP_SPEC_SHOW_TEXT` (the `read` of a `statue`,
a `ruined_statue` or the plaque of a `painting`; every other type without a
`read` of its own is given a generic one when the catalogue loads, so any prop
with an `inscription` can be read, `scenery` included) prints `MSG_ME`, then the override it names, read the
way any written text is read (`read_message` on the player): in a frame, in the
instance's `language` (the common tongue when unset), and garbled for a reader
who does not know it.

An altar, a statue or a ruined statue may be dedicated to a deity with a
`deity` override, the deity's full path with `.c`
(`props set altar_1 overrides.deity /games/<game>/obj/deities/<deity>.c`):
there its followers can pray. Their `pray` action carries
`PROP_SPEC_YIELDS_TO_SKILL` (a skill id): for a player who knows that skill the
action does nothing and sets no message, so the skill's answer stands. See
`lib/room/temple/temple.md`.

## Light

A type gives light with `PROP_TYPE_LIGHT` (`state_field -> light`): each
instance adds that light to the location while the field is truthy. The props
component stands in the location's inventory, so it keeps the sum as its own
light (`set_light`) and the location's light follows, recalculated whenever an
instance is added or removed or its state changes. A lit rune gives 20, a
burning fireplace or forge 30.

```
props set ruined_statue_1 overrides.inscription "Here lies nobody."
read plaque
  -> "You push aside the creepers covering the pedestal and uncover the plaque."
  -> "You read:" and the text in a frame
```

An instance without that override gets `MISSING_MSG` when the action has one; no `read`
type has one (statue, ruined statue, rune, painting), so the verb falls through to the
ordinary `read` command ("<X> has nothing written on it.").
The verb is left out of the "You can" line of an instance with no inscription.
A prop whose description mentions writing must carry the `inscription`;; a
`language` the game's languages handler does not register cannot be read by anyone,
so register it there first.

## 7. End-to-end example

Room with two wooden chairs and one metal chair. Player types
`look chair`:

```
find_match("chair")
  ├─ location.find_inv_match strips num=0, no inflation
  ├─ component.query_alias returns
  │     ["chair", "seat", "wooden chair", "metal chair", ...]
  ├─ "chair" matches -> component added to result
  │
component.long("chair")
  ├─ _find_unique_match iterates props_instances
  ├─ first match (chair_1, wood) -> handler.query_type_long(chair, ovs, state, "chair_1")
  │     ├─ LONG_KEY  "A sturdy $material_phrase$ chair, sized for one."
  │     ├─ material  overrides.material || DEFAULT_MATERIAL = "wood"
  │     ├─ $material_phrase$ = "wooden"
  │     └─ returns "A sturdy wooden chair, sized for one."
  ├─ query_actions_hint -> "You can: sit, stand, tip, right"
  └─ writes both to the player
```

For `look chair 2`: `location.find_inv_match` inflates the component
to 2 references, `find_match` counts down `--num` and returns the
same object, `component.long("chair 2")` strips the trailing `2`,
filters matching instances, and picks the 2nd one.

## Verbs, prepositions and missing targets

A prop verb's argument is trimmed of leading filler words
(`_LANG_PROPS_FILLER_WORDS`: `en`, `ante`, `frente`, `a`, `el`... /
`at`, `before`, `front`, `of`, `the`...) before it is matched, so
`rezar ante el altar`, `rezar frente al altar`, `pray before the statue`
and `tumbarse en la cama` all reach the prop. To accept a new
preposition, add it to that list in both languages.

A prop verb typed with no target, when some prop here answers to it, asks
for one: `_LANG_PROPS_VERB_PROMPTS` maps the verb to its question
(`rezar` -> `¿Rezar ante qué?`), and a verb with no entry uses
`_LANG_PROPS_VERB_PROMPT_DEFAULT`. A bare verb where no prop knows it, or a
target that matches nothing, answers `_LANG_PROPS_NO_TARGET`. A bare verb
still resolves first through the state-based auto-target (`stand`).

The read verbs (`_LANG_PROPS_READ_VERBS`) never set a failure message when
nothing matches: the generic `read` command (`lib/cmds/player/read.c`)
answers with its own. A skill-backed verb (`PROP_SPEC_YIELDS_TO_SKILL`)
returns before any of this when the player knows the skill.

## 8. Files

- `lib/location/components/props.c` — component: parser, prose, section, dispatch.
- `lib/location.c::find_inv_match` — numeric-idiom inflation.
- `lib/handlers/props.c` — catalogue loader, `query_type_long`, `query_type_short`, `_apply_material_tokens`, state-suffix helpers.
- `lib/handlers/souls.c::parse_string` — actor/target token parser.
- `lib/tables/props_table.c` — per-type catalogue.
- `lib/tables/materials.c` — material catalogue (`$material_*$` sources).
- `include/room/prop.h` — field / spec / state constants.
- `include/translations/props.<lang>.h` — per-language strings + ordering templates.
- `include/translations/materials.<lang>.h` — per-language material forms.
