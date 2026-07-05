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
| **Room prose** (`look`) | `hook_long` → `_compose_prose` | Groups by `(type, material, state)`. Singleton → `query_type_long` (LONG_KEY + material tokens + state suffixes). Plural → `_LANG_PROPS_NOUN_PHRASE`-based sentence + one shared state suffix |
| **Section line** (`You see ...`) | `query_props_section_string` | Same grouping. Per group → `_LANG_PROPS_NOUN_PHRASE` + compact `STATE_SUFFIXES` for singletons. Joiners: `_LANG_PROPS_LIST_SEPARATOR`, `_LANG_PROPS_LIST_AND` |
| **`look <prop>`** | `find_match` → `component->long(str)` → `_find_unique_match` → `query_type_long` | LONG_KEY with material tokens replaced + `LONG_SUFFIXES` for truthy state + `LONG_SUFFIXES_UNSET` for falsy state + `_LANG_PROPS_YOU_CAN` line listing available verbs |
| **Action messages** (e.g. `sit chair`) | `_execute_generic` → `_render_msg` | Every `MSG_ME` / `MSG_OTHERS` / `BLOCKED_MSG` / `MISSING_MSG` / `ALREADY_SET_MSG` runs through souls parser. `$mcname$` → actor cap name, `$mposs$` → gendered possessive, `$lastarg$` → state value |
| **Parser aliases** | `component->query_alias` + `query_plurals` | Enumerates type's `id_list` (singular) plus `NOUN_PLURAL` (plural) plus every `_LANG_PROPS_ID_WITH_MATERIAL` variant per attached instance |
| **Numeric disambiguation** (`chair 2`) | `location.find_inv_match` inflates the component N times | `find_match`'s `--num` decrement counts down naturally; component's inner matcher strips the trailing number too |

## 5. State → strings

The instance's `state` mapping mutates through action `SET_STATE` /
`CLEAR_STATE`. Suffixes read that state:

```
state = { tipped: 1, occupied: "test" }

→ Section line appends _LANG_PROP_CHAIR_SUFFIX_TIPPED   " (tipped over)"
→ Section line appends _LANG_PROP_CHAIR_SUFFIX_OCCUPIED " (test is sitting on it)"  // %s legacy
→ look prop appends    _LANG_PROP_CHAIR_LONG_TIPPED     " It lies on its side."
→ look prop appends    _LANG_PROP_CHAIR_LONG_OCCUPIED   " test is sitting on it."   // %s legacy
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
])
```

## 7. End-to-end example

Room with two wooden chairs and one metal chair. Player types
`look chair`:

```
find_match("chair")
  ├─ location.find_inv_match strips num=0, no inflation
  ├─ component.query_alias returns
  │     ["chair", "seat", "wooden chair", "metal chair", ...]
  ├─ "chair" matches → component added to result
  │
component.long("chair")
  ├─ _find_unique_match iterates props_instances
  ├─ first match (chair_1, wood) → handler.query_type_long(chair, ovs, state, "chair_1")
  │     ├─ LONG_KEY  "A sturdy $material_phrase$ chair, sized for one."
  │     ├─ material  overrides.material || DEFAULT_MATERIAL = "wood"
  │     ├─ $material_phrase$ = "wooden"
  │     └─ returns "A sturdy wooden chair, sized for one."
  ├─ query_actions_hint → "You can: sit, stand, tip, right"
  └─ writes both to the player
```

For `look chair 2`: `location.find_inv_match` inflates the component
to 2 references, `find_match` counts down `--num` and returns the
same object, `component.long("chair 2")` strips the trailing `2`,
filters matching instances, and picks the 2nd one.

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
