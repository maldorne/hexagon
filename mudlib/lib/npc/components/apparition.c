// apparition.c -- an NPC that is only there for a while, as an NPC component.
//
// From the moment it stands in the world it plays a timed script to the room
// and then vanishes. It can also be intangible: nothing hurts it and it never
// fights.
//
// Attributes:
//   "scenes"      - the script, a list of mappings ([ "at": seconds,
//                   "message": text, "item": path ]): at that second the room
//                   is told the text, and the item, if any, is cloned on the
//                   floor. Texts may carry $name$, the apparition's short
//   "vanish"      - seconds after which it goes (default: never)
//   "vanish_message" - told to the room when it goes
//   "intangible"  - 1: no blow reaches it
//   "refusal"     - what whoever attacks an intangible one is told

inherit component "/lib/npc/component.c";

#define INTANGIBLE_SHADOW "/lib/obj/shadows/intangible.c"

private mixed * scenes;
private string vanish_message, refusal;
private int vanish_after, intangible;
// the shadow that makes it intangible, not saved
static object shadow_ob;

void create()
{
  component::create();
  scenes = ({ });
  vanish_message = "";
  refusal = "";
  vanish_after = 0;
  intangible = 0;
  shadow_ob = nil;
}

string query_info()
{
  return sizeof(scenes) + " scenes" + (vanish_after ? ", vanishes at " + vanish_after + "s" : "") +
         (intangible ? ", intangible" : "");
}

// Tells the room a text of the script, with the apparition's name in it.
private void tell_here(string text)
{
  object npc;

  npc = query_entity();
  if (!npc || !environment(npc) || !strlen(text))
    return;

  tell_room(environment(npc), replace_string(text, "$name$", npc->query_short()) + "\n");
}

// The scene `index` of the script plays.
void play_scene(int index)
{
  object npc, item;
  mapping scene;

  npc = query_entity();
  if (!npc || !environment(npc) || index >= sizeof(scenes))
    return;

  scene = scenes[index];
  if (stringp(scene["message"]))
    tell_here(scene["message"]);

  if (stringp(scene["item"]))
  {
    item = clone_object(scene["item"]);
    if (item)
      item->move(environment(npc));
  }
}

// The apparition goes.
void vanish()
{
  object npc;

  npc = query_entity();
  if (!npc)
    return;

  tell_here(vanish_message);
  npc->dest_me();
}

void placed()
{
  object npc;
  int i;

  npc = query_entity();
  if (!npc)
    return;

  if (intangible && !shadow_ob)
  {
    shadow_ob = clone_object(INTANGIBLE_SHADOW);
    shadow_ob->setup_shadow(npc, refusal);
  }

  for (i = 0; i < sizeof(scenes); i++)
    call_out("play_scene", (int)scenes[i]["at"], i);
  if (vanish_after)
    call_out("vanish", vanish_after);
}

void dest_me()
{
  if (shadow_ob)
    destruct_object(shadow_ob);
  component::dest_me();
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "scenes" : scenes,
            "vanish" : vanish_after,
            "vanish_message" : vanish_message,
            "intangible" : intangible,
            "refusal" : refusal ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;

  if (pointerp(args["scenes"]))
    scenes = ({ }) + args["scenes"];
  if (intp(args["vanish"]))
    vanish_after = args["vanish"];
  if (stringp(args["vanish_message"]))
    vanish_message = args["vanish_message"];
  if (intp(args["intangible"]))
    intangible = args["intangible"];
  if (stringp(args["refusal"]))
    refusal = args["refusal"];
}

mixed * stats()
{
  return component::stats() +
         ({ ({ "Scenes", sizeof(scenes) }), ({ "Vanish after", vanish_after }),
            ({ "Intangible", intangible }) });
}
