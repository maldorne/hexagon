# Crafters

A crafter makes things to order: a player brings the materials a recipe asks
for, pays its price, and gets the result. An alchemist, a smith who forges from
ore, a druid who carves a branch.

## Declaring one

The code is `/lib/crafts/crafter.c`. An NPC carries it through its component
`crafter`, declared in its template with the recipes and, if it is picky, the
quests a player must have completed before it deals with them:

    "components": {
      "crafter": {
        "craft_needs_quests": [ "<game>:<quest>" ],
        "craft_recipes": [
          { "materials": { "/games/<game>/areas/<area>/items/<item>": 1 },
            "price": 500,
            "result": "/games/<game>/areas/<area>/items/<result>" }
        ]
      }
    }

Item paths are full paths without the `.c`. A material is counted by its
blueprint, so any clone of it will do. The price is in the base unit of money
(see the money handler) and may be 0.

Anything else (a monster, an item) can inherit the mixin directly and use
`add_recipe(materials, price, result)` and `set_needs_quests(list)`.

## Who it deals with

A crafter that needs quests is not a crafter at all to anybody who has not
completed every one of them: no mark, no hint, nothing listed. That is how a
first quest introduces somebody who then works to order.

## What the player sees

- The room listing marks a crafter with a cyan `[?]` (quest marks are yellow)
  for whoever it deals with.
- Looking at it adds a line saying it works to order and which command to type.
- `encargos` / `crafts` lists what the crafters in the room make, numbered.
  `info <n>` shows the result, its materials and price, and what the player
  still lacks; `encargar <n>` / `commission <n>` takes the materials and the
  money and hands over the result. With only one recipe the number may be left
  out.

The command finds the crafters through `handler("crafts")->crafter_of(ob)`,
which answers the NPC's component or the object itself.
