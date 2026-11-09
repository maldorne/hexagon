// transform.c -- an NPC that turns into another one, as an NPC component.
//
// When its trigger happens the NPC goes and another one, made from a template,
// takes its place in the same location and carries on the fight against
// whoever was fighting the original. The original leaves a corpse or not.
//
// Attributes:
//   "trigger"    - when it happens:
//                    "attacked"  the first blow it takes
//                    "defeated"  a blow leaves it with "hp_percent" of its
//                                hit points or less, still alive
//                    "killed"    it dies
//   "template"   - template id of the NPC that appears (areas/<area>/npcs/<type>)
//   "corpse"     - 1 (default) the original leaves a corpse, 0 its body is
//                  gone, with everything it carried
//   "attack"     - 1 (default) the new NPC fights whoever fought the original,
//                  0 it does not
//   "hp_percent" - the threshold of "defeated" (default 20)
//   "message"    - told to the room when the original goes
//   "appears"    - told to the room when the new one stands there
//                  (both texts may carry $name$, the original's short)
//
// "attacked" and "defeated" are judged after each blow, so a spell that does
// not go through the fight events does not trigger them.

#include <areas/area.h>

inherit component "/lib/npc/component.c";

#define DEFAULT_HP_PERCENT 20

private string trigger_kind, template_id, room_message, appears_message;
private int leaves_corpse, hp_percent, attacks;
// the attackers the original had when it died, not saved
static object * last_attackers;
// the transformation is already under way
static int pending;

void create()
{
  component::create();
  trigger_kind = "killed";
  template_id = "";
  room_message = "";
  appears_message = "";
  leaves_corpse = 1;
  hp_percent = DEFAULT_HP_PERCENT;
  attacks = 1;
  last_attackers = ({ });
  pending = 0;
}

string query_info()
{
  return trigger_kind + " -> " + template_id + (leaves_corpse ? "" : ", no corpse");
}

/**
 * Tells the room a text of the template, with the original's name in it.
 *
 * @param  text  the text, empty for none
 */
private void tell_here(string text)
{
  object npc;

  npc = query_entity();
  if (!npc || !environment(npc) || !strlen(text))
    return;

  tell_room(environment(npc), replace_string(text, "$name$", npc->query_short()) + "\n");
}

/**
 * Makes the new NPC from its template and sets it where the original stands,
 * fighting the attackers who are still in the room.
 *
 * @param  attackers  who fought the original
 */
private void raise_new(object * attackers)
{
  object npc, place, area, born;
  string game;
  int i;

  npc = query_entity();
  place = npc ? environment(npc) : nil;
  if (!place || !strlen(template_id))
    return;

  game = game_name(place);
  born = handler("bestiary", place)->spawn_from_template(game, template_id);
  if (!born)
    return;

  area = place->query_area();
  if (area)
    born->set_level((int)area->decide_level(game, template_id));

  born->move(place);
  born->components_placed();

  tell_here(appears_message);

  for (i = 0; attacks && i < sizeof(attackers); i++)
    if (attackers[i] && !attackers[i]->query_dead() &&
        environment(attackers[i]) == place)
      born->attack_ob(attackers[i]);
}

/**
 * Takes away what the original left: with no corpse its body is gone, and so
 * is everything it carried.
 *
 * @param  corpse  the corpse the original left, nil for none
 */
private void clear_corpse(object corpse)
{
  object * inv;
  int i;

  if (!corpse || leaves_corpse)
    return;

  inv = all_inventory(corpse);
  for (i = 0; i < sizeof(inv); i++)
    if (inv[i])
      inv[i]->dest_me();

  corpse->dest_me();
}

// The original dies: the attackers it had are the ones the new NPC meets.
void died(object killer)
{
  object npc;

  npc = query_entity();
  if (!npc || trigger_kind != "killed")
    return;

  last_attackers = npc->query_attacker_list() + ({ killer });
}

// The body is made and the death announced: the new NPC rises.
void second_life(object corpse, object initiator)
{
  if (trigger_kind != "killed" || pending)
    return;

  pending = 1;
  clear_corpse(corpse);
  tell_here(room_message);
  raise_new(last_attackers - ({ nil }));
}

/**
 * The original goes without dying: it leaves its corpse or not, and the census
 * counts it as dead so its kind is replaced like any other.
 */
void change_now()
{
  object npc, corpse;
  object * attackers;
  int i;

  npc = query_entity();
  if (!npc || npc->query_dead())
    return;

  attackers = npc->query_attacker_list() - ({ nil });
  tell_here(room_message);

  corpse = npc->make_corpse(sizeof(attackers) ? attackers[0] : nil);
  clear_corpse(corpse);

  raise_new(attackers);

  for (i = 0; i < sizeof(attackers); i++)
    attackers[i]->stop_fight(npc, 1);
  npc->set_dead(1);
  npc->dest_me();
}

// A blow in the room: the original takes it, or has taken enough of them.
void event_fight_in_progress(object ob, object * fighters)
{
  object npc;

  npc = query_entity();
  if (!npc || pending || npc->query_dead() || !pointerp(fighters) ||
      sizeof(fighters) < 2)
    return;

  if (trigger_kind == "attacked")
  {
    if (fighters[0] != npc)
      return;
  }
  else if (trigger_kind == "defeated")
  {
    if (member_array(npc, fighters) == -1 ||
        npc->query_hp() * 100 > npc->query_max_hp() * hp_percent)
      return;
  }
  else
    return;

  // outside the blow: the fight is still being resolved
  pending = 1;
  call_out("change_now", 0);
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "trigger" : trigger_kind,
            "template" : template_id,
            "corpse" : leaves_corpse,
            "attack" : attacks,
            "hp_percent" : hp_percent,
            "message" : room_message,
            "appears" : appears_message ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;

  if (stringp(args["trigger"]))
    trigger_kind = args["trigger"];
  if (stringp(args["template"]))
    template_id = args["template"];
  if (intp(args["corpse"]))
    leaves_corpse = args["corpse"];
  if (intp(args["attack"]))
    attacks = args["attack"];
  if (intp(args["hp_percent"]))
    hp_percent = args["hp_percent"];
  if (stringp(args["message"]))
    room_message = args["message"];
  if (stringp(args["appears"]))
    appears_message = args["appears"];
}

mixed * stats()
{
  return component::stats() +
         ({ ({ "Trigger", trigger_kind }), ({ "Template", template_id }),
            ({ "Leaves a corpse", leaves_corpse }),
            ({ "Hp percent", hp_percent }) });
}
