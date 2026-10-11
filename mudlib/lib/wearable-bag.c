// A bag worn on the body: an armour of the containers' type (a backpack, a
// quiver), which protects nothing, and a container that holds things. Put
// things in it and take them out with the usual verbs (poner X en mochila,
// coger X de mochila); it can be worn or carried in the hand alike.
//
// A bag names its base in setup(), set_base_armour(BAGPACK) or QUIVER, and its
// capacity with set_max_weight().
//
// 10/2026 - Ported to Hexagon, neverbot (from the CcMud std/wearable_bag.c).

#include <basic/condition.h>
#include <translations/armour.h>
#include <language.h>

inherit armour    "/lib/armour.c";
inherit container "/lib/core/basic/container.c";

void create()
{
  // the container's defaults by hand: its create() would run the object's
  // creation, and setup(), a second time
  set_weight_reduction(30);
  set_max_weight(0);
  armour::create();
}

int query_container() { return 1; }

// What it weighs is its own weight and, lightened, what it holds.
int query_weight() { return container::query_weight(); }

string long(varargs string str, int dark)
{
  return armour::long(str, dark) +
         query_contents(capitalize(short(dark)) + _LANG_WEARABLE_BAG_CONTAINS,
                        all_inventory());
}

// It never wears out.
int query_cond() { return MAX_COND; }

// No money in it, nor another bag: what is lost with a bag stays simple.
int test_add(object ob, int flag)
{
  if (ob->is_money() || ob->query_container())
    return 0;

  return 1;
}

int test_remove(object ob, int flag) { return 1; }

// The things it holds go where it goes, and are kept with it.
mapping query_auto_load_attributes()
{
  return ([
      "armour" : armour::query_auto_load_attributes(),
      "inv"    : "/lib/core/basic/auto_load"->create_auto_load(all_inventory(this_object())),
    ]);
}

void init_auto_load_attributes(mapping attribute_map)
{
  if (!undefinedp(attribute_map["armour"]))
    armour::init_auto_load_attributes(attribute_map["armour"]);
  if (!undefinedp(attribute_map["inv"]))
    "/lib/core/basic/auto_load"->load_auto_load(attribute_map["inv"], this_object());
}

mixed * stats()
{
  return armour::stats() + ({
      ({ "Contents Weight (nosave)", query_contents_weight(), }),
      ({ "Max Weight (nosave)", query_max_weight(), }),
      ({ "Weight Reduction (nosave)", query_weight_reduction(), }),
    });
}
