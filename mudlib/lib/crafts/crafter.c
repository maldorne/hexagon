// crafter.c -- what somebody who makes things to order knows and does.
//
// A crafter has recipes: some materials, a price and what comes out of them. A
// player brings the materials, pays, and gets the thing made. The crafts
// command is what finds the crafters in a room and asks them; the crafter owns
// no verbs.
//
// Carried by an npc through its component (/lib/npc/components/crafter.c), and
// by anything else that inherits this directly. Declared, for an npc, in its
// template:
//
//   "crafter" : ([ "craft_recipes": ({ ([ "materials": ([ "<item>": 1 ]),
//                                         "price": 500,
//                                         "result": "<item>" ]) }),
//                  "craft_needs_quests": ({ "<game>:<quest>" }) ])
//
// Item paths are full paths without the .c. A crafter that needs quests only
// deals with whoever has completed all of them: to anybody else it is not a
// crafter at all, with no mark, no hint and nothing to offer.

#include <language.h>

#define RECIPE_MATERIALS "materials"
#define RECIPE_PRICE     "price"
#define RECIPE_RESULT    "result"

private mapping * recipes;
private string * needs_quests;

int query_crafter() { return 1; }

// Who makes the things: the carrier itself, or the npc a component belongs to.
object query_crafter_owner() { return this_object(); }

mapping * query_recipes() { return recipes ? recipes : ({ }); }
void set_recipes(mapping * list) { recipes = list; }

void add_recipe(mapping materials, int price, string result)
{
  if (!recipes)
    recipes = ({ });

  recipes += ({ ([ RECIPE_MATERIALS: materials,
                   RECIPE_PRICE: price,
                   RECIPE_RESULT: result ]) });
}

string * query_needs_quests() { return needs_quests ? needs_quests : ({ }); }
void set_needs_quests(string * list) { needs_quests = list; }

// Whether this crafter deals with somebody: every quest it needs completed.
int deals_with(object who)
{
  string * quests;
  int i;

  if (!who)
    return 0;

  quests = query_needs_quests();
  for (i = 0; i < sizeof(quests); i++)
    if (!who->has_completed_quest(game_name(who), quests[i]))
      return 0;

  return 1;
}

// The recipes somebody may ask for: all of them, or none.
mapping * recipes_for(object who)
{
  return deals_with(who) ? query_recipes() : ({ });
}

// How many of a material somebody carries.
private int carried(object who, string path)
{
  object * inv;
  int i, n;

  inv = all_inventory(who);
  for (i = 0; i < sizeof(inv); i++)
    if (base_name(inv[i]) == path)
      n++;

  return n;
}

// What is missing for somebody to commission a recipe: the materials they lack,
// as path -> how many more, and whether they can pay.
mapping missing_for(object who, mapping recipe)
{
  mapping out, materials;
  string * paths;
  int i, have;

  out = ([ ]);
  materials = recipe[RECIPE_MATERIALS];
  paths = map_indices(materials);

  for (i = 0; i < sizeof(paths); i++)
  {
    have = carried(who, paths[i]);
    if (have < materials[paths[i]])
      out[paths[i]] = materials[paths[i]] - have;
  }

  return out;
}

int can_afford(object who, mapping recipe)
{
  return (int)who->query_value() >= recipe[RECIPE_PRICE];
}

// Make a recipe for somebody: take the materials and the money and hand over
// the result. Returns the new object, or nil when anything is missing.
object craft(object who, mapping recipe)
{
  mapping materials;
  string * paths;
  object * inv;
  object made;
  int i, j, left;

  if (!who || !deals_with(who) || map_sizeof(missing_for(who, recipe)) ||
      !can_afford(who, recipe))
    return nil;

  made = clone_object(recipe[RECIPE_RESULT]);
  if (!made)
    return nil;

  materials = recipe[RECIPE_MATERIALS];
  paths = map_indices(materials);
  inv = all_inventory(who);

  for (i = 0; i < sizeof(paths); i++)
  {
    left = materials[paths[i]];
    for (j = 0; j < sizeof(inv) && left > 0; j++)
      if (inv[j] && base_name(inv[j]) == paths[i])
      {
        inv[j]->dest_me();
        left--;
      }
  }

  if (recipe[RECIPE_PRICE] > 0)
    who->pay_money(handler("money")->create_money_array(recipe[RECIPE_PRICE]));

  if (made->move(who))
    made->move(environment(who));

  return made;
}

// What looking at the crafter adds, for whoever it deals with.
string query_craft_hint(object who)
{
  if (!who || !sizeof(recipes_for(who)))
    return "";

  return _LANG_CRAFTER_HINT;
}

string extra_look() { return query_craft_hint(this_player()); }
