// crafter.c -- an npc that makes things to order, as a component.
//
// Everything a crafter knows and does comes from /lib/crafts/crafter.c; this
// file only adds what a component needs on top: being attached to an npc and
// saved with it. Declared in the npc template's components (see the mixin for
// the shape of the recipes).

inherit component "/lib/npc/component.c";
inherit crafter   "/lib/crafts/crafter.c";

void create()
{
  component::create();
}

// Looking at the npc tells whoever it deals with how to ask.
void initialize(object npc)
{
  component::initialize(npc);

  if (npc)
    npc->add_extra_look(this_object());
}

object query_crafter_owner() { return query_owner(); }

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
    ([ "craft_recipes"     : query_recipes(),
       "craft_needs_quests": query_needs_quests() ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);

  if (!args)
    return;

  if (!undefinedp(args["craft_recipes"]))
    set_recipes(args["craft_recipes"]);
  if (!undefinedp(args["craft_needs_quests"]))
    set_needs_quests(args["craft_needs_quests"]);
}
