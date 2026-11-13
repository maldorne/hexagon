// receive.c -- an NPC that accepts an item from a player and reacts, as an NPC
// component.
//
// It hooks the ordinary give command (give / dar): an item that arrives in the
// NPC's hands from a player is looked at once the command is done. The verbs of
// the component are only more ways to type the same hand-over ("<verb> <item>
// to <npc>"), which move the item to the NPC and let the same hook take over.
// An item that is not wanted stays with the NPC as with any other, unless the
// component has a refusal, in which case it is handed back.
//
// Attributes, every one optional:
//   "items"     - what is accepted: full file paths with .c (compared exactly)
//                 or ids of the item; nothing listed accepts any item
//   "verbs"     - extra verbs that hand an item over ("<verb> <item> a <npc>")
//   "message"   - told to the room when the item is taken
//   "refusal"   - told to the giver when the item is not wanted, and the item
//                 goes back to them; without it an unwanted item is just kept
//   "spent"     - told to the giver when "once" is used up (a stock line when
//                 absent); the item goes back to them
//   "consume"   - 1: the item is destroyed
//   "once"      - "player" the NPC takes it once from each player (kept as a
//                 property of the player), "npc" once from anybody
//   "delay"     - seconds between taking the item and the effects (default 0)
//   "then"      - told to the room when the delay is over, before the effects
//   effects, done together in this order:
//   "give"      - files of items cloned for the giver
//   "quest"     - ([ "id": "<game>:<quest id>", "action": "accept" or
//                 "complete" (default accept), "everybody": 1 for every player
//                 present and not only the giver, "source": who gives it ])
//   "talks"     - target (template id) passed to the quests handler's talked()
//                 for the giver, like verb-response
//   "transform" - the NPC turns into another: a template id, or the full path
//                 with .c of a monster; its place in the census is kept
//   "appears"   - with "transform", told to the room when the new one stands
//                 there
//   "attack"    - with "transform", who the new NPC attacks at once: "giver"
//                 or "everybody" (everything alive that stands there)
//   "leave"     - the NPC goes away and leaves the census for good; the text is
//                 told to the room (empty for none). Not with "transform"
//   Texts may carry $npc$, $player$ and, in the hand-over lines, $item$.

#include <language.h>
#include <areas/area.h>
#include <basic/move.h>
#include <common/properties.h>
#include <living/consents.h>
#include <living/quests.h>
#include <translations/filler.h>

inherit component "/lib/npc/component.c";

#define ONCE_PLAYER "player"
#define ONCE_NPC "npc"
#define QUEST_ACCEPT "accept"
#define QUEST_COMPLETE "complete"
#define ATTACK_GIVER "giver"
#define ATTACK_EVERYBODY "everybody"
#define RECEIVE_PROP "receive:"

private string * receive_items, * receive_verbs, * receive_give;
private string receive_message, receive_refusal, receive_spent, receive_then,
               receive_once, receive_talks, receive_transform, receive_appears,
               receive_attack, receive_leave;
private mapping receive_quest;
private int receive_consume, receive_delay;
void do_effects(object who);

// the NPC has already taken something once, for the "npc" kind of "once"
private int received;
// an item is taken and its effects have not been done yet, not saved
static int working;

void create()
{
  component::create();
  receive_items = ({ });
  receive_verbs = ({ });
  receive_give = ({ });
  receive_message = "";
  receive_refusal = "";
  receive_spent = _LANG_RECEIVE_SPENT;
  receive_then = "";
  receive_once = "";
  receive_talks = "";
  receive_transform = "";
  receive_appears = "";
  receive_attack = "";
  receive_leave = nil;
  receive_quest = ([ ]);
  receive_consume = 0;
  receive_delay = 0;
  received = 0;
  working = 0;
}

string query_info()
{
  return (sizeof(receive_items) ? implode(receive_items, ", ") : "anything") +
         (receive_consume ? " (consumed)" : "") +
         (strlen(receive_once) ? ", once " + receive_once : "");
}

/**
 * Binds the component to its NPC, which has to let people give it things: the
 * give command refuses whoever has not consented.
 *
 * @param  npc  the NPC
 */
void initialize(object npc)
{
  component::initialize(npc);

  if (npc)
    npc->set_consent(GIVE_CONS, 1);
}

mapping query_component_actions()
{
  mapping out;
  int i;

  out = ([ ]);
  for (i = 0; i < sizeof(receive_verbs); i++)
    out[receive_verbs[i]] = "do_hand_over";

  return out;
}

/**
 * A text with the names in it, ending in a new line; nothing for an empty one.
 *
 * @param  text  the text, with $npc$, $player$ and $item$ in it
 * @param  who   the player named by $player$, nil for none
 * @param  item  the item named by $item$, nil for none
 * @return the text ready to tell
 */
private string say_text(string text, object who, object item)
{
  if (!strlen(text))
    return "";

  return replace_string(replace_string(replace_string(text,
           "$npc$", query_entity()->query_cap_name()),
           "$player$", who ? (string)who->query_cap_name() : ""),
           "$item$", item ? (string)item->short() : "") + "\n";
}

/**
 * Tells the room where the NPC stands a text, everyone present included.
 *
 * @param  text  the text, empty for none
 * @param  who   the giver
 * @param  item  the item given, nil for none
 */
private void tell_here(string text, object who, object item)
{
  object npc;

  npc = query_entity();
  if (npc && environment(npc) && strlen(text))
    tell_room(environment(npc), say_text(text, who, item));
}

/**
 * Whether the NPC wants this item: it is listed by its file or by an id, or
 * nothing is listed.
 *
 * @param  item  the item given
 * @return 1 if it is wanted
 */
private int is_wanted(object item)
{
  int i;

  if (!sizeof(receive_items))
    return 1;

  for (i = 0; i < sizeof(receive_items); i++)
    if (receive_items[i][0] == '/')
    {
      if (base_name(item) + ".c" == receive_items[i])
        return 1;
    }
    else if (item->id(receive_items[i]))
      return 1;

  return 0;
}

// The property of a player that tells they were already served by this NPC.
private string once_property()
{
  object npc;

  npc = query_entity();
  return RECEIVE_PROP + (npc->query_npc_source() ? npc->query_npc_source() :
                         base_name(npc));
}

/**
 * Whether this NPC has used up its "once" for this player.
 *
 * @param  who  the giver
 * @return 1 if nothing more is taken from them
 */
private int is_spent(object who)
{
  if (receive_once == ONCE_NPC)
    return received;

  return receive_once == ONCE_PLAYER && who->query_property(once_property());
}

/**
 * Hands an item back to the giver, or leaves it where the NPC stands when the
 * giver is gone.
 *
 * @param  item  the item
 * @param  who   the giver
 */
private void give_back(object item, object who)
{
  object npc;

  npc = query_entity();
  if (!item || !npc || environment(item) != npc)
    return;

  if (who && environment(who) == environment(npc) && !item->move(who))
    return;

  if (environment(npc))
    item->move(environment(npc));
}

/**
 * The words that name a thing or the NPC, without the article that may open
 * them.
 *
 * @param  text  what the player typed
 * @return the text without its first word when that is an article or similar
 */
private string without_article(string text)
{
  string * words;

  words = explode(text, " ") - ({ "" });
  if (sizeof(words) > 1 && member_array(words[0], _LANG_FILLER_WORDS) != -1)
    words = words[1..];

  return implode(words, " ");
}

/**
 * The verbs of the component: "<verb> <item> a <npc>" gives the item to this
 * NPC the way the give command does, and the hook of event_enter takes over.
 *
 * @param  str  what follows the verb
 * @return 1 if the verb was for this NPC
 */
int do_hand_over(string str)
{
  object npc, who, * inv;
  string thing, name;
  int i;

  npc = query_entity();
  who = this_player();
  if (!npc || !who || who == npc || !stringp(str))
    return 0;

  if (sscanf(lower_case(trim(str)), "%s " + _LANG_RECEIVE_PREP + " %s",
             thing, name) != 2 ||
      !npc->id(without_article(name)))
    return 0;

  inv = all_inventory(who);
  for (i = 0; i < sizeof(inv); i++)
    if ((inv[i]->id(thing) || inv[i]->id(without_article(thing))) &&
        is_wanted(inv[i]))
      break;

  if (i == sizeof(inv))
  {
    notify_fail(_LANG_RECEIVE_NOT_HAVE);
    return 0;
  }

  if (inv[i]->query_in_use() || inv[i]->move(npc) != MOVE_OK)
  {
    notify_fail(_LANG_RECEIVE_CANNOT);
    return 0;
  }

  // like the give command, so the NPC does not wear it
  inv[i]->add_static_property(PC_GAVE_NPC_PROP, 1);

  tell_object(who, say_text(_LANG_RECEIVE_HAND_OVER_ME, who, inv[i]));
  tell_room(environment(who), say_text(_LANG_RECEIVE_HAND_OVER_ROOM, who, inv[i]),
            ({ who, npc }));
  return 1;
}

/**
 * Something arrived where the NPC stands. An item that a player has just put
 * in its hands is looked at after the move that brought it is over, never
 * inside it.
 *
 * @param  ob     what arrived
 * @param  mess   the arrival text
 * @param  from   where it came from
 * @param  avoid  who must not be told
 */
void event_enter(object ob, varargs string mess, object from, mixed avoid)
{
  object npc;

  npc = query_entity();
  if (!npc || !ob || living(ob) || environment(ob) != npc || !from ||
      !living(from) || !from->query_player())
    return;

  call_out("receive_item", 0, ob, from);
}

/**
 * Takes what a player gave, when it is wanted and the NPC can still take it,
 * and starts the effects.
 *
 * @param  item  the item in the NPC's hands
 * @param  who   the player who gave it
 */
void receive_item(object item, object who)
{
  object npc;

  npc = query_entity();
  if (!npc || !item || environment(item) != npc || working)
    return;

  if (!is_wanted(item))
  {
    if (strlen(receive_refusal))
    {
      tell_here(receive_refusal, who, item);
      give_back(item, who);
    }
    return;
  }

  if (is_spent(who))
  {
    tell_here(receive_spent, who, item);
    give_back(item, who);
    return;
  }

  working = 1;
  tell_here(receive_message, who, item);

  if (receive_consume)
    item->dest_me();

  if (receive_once == ONCE_NPC)
    received = 1;
  else if (receive_once == ONCE_PLAYER && who)
    who->add_property(once_property(), 1);

  if (receive_delay > 0)
    call_out("do_effects", receive_delay, who);
  else
    do_effects(who);
}

/**
 * The players who take part in a quest: the giver, or everybody standing with
 * the NPC.
 *
 * @param  who  the giver
 * @return the players
 */
private object * quest_players(object who)
{
  object * here, * out;
  int i;

  out = ({ });
  if (!receive_quest["everybody"])
  {
    if (who)
      out += ({ who });
    return out;
  }

  here = all_inventory(environment(query_entity()));
  for (i = 0; i < sizeof(here); i++)
    if (here[i]->query_player())
      out += ({ here[i] });

  return out;
}

/**
 * Accepts or completes the quest of the component for every player it names.
 *
 * @param  who  the giver
 */
private void do_quest(object who)
{
  object * players;
  string source;
  int i;

  if (!stringp(receive_quest["id"]))
    return;

  source = stringp(receive_quest["source"]) ? receive_quest["source"] : "";
  players = quest_players(who);
  for (i = 0; i < sizeof(players); i++)
    if (receive_quest["action"] == QUEST_COMPLETE)
      handler(QUESTS_HANDLER, players[i])->complete(players[i], receive_quest["id"]);
    else
      handler(QUESTS_HANDLER, players[i])->accept(players[i], receive_quest["id"],
                                                  source);
}

/**
 * Who the NPC attacks when it turns into another one.
 *
 * @param  who  the giver
 * @return the targets
 */
private object * transform_targets(object who)
{
  object * here, * out;
  int i;

  out = ({ });
  if (receive_attack == ATTACK_GIVER && who)
    out += ({ who });
  if (receive_attack != ATTACK_EVERYBODY)
    return out;

  here = all_inventory(environment(query_entity()));
  for (i = 0; i < sizeof(here); i++)
    if (living(here[i]) && here[i] != query_entity())
      out += ({ here[i] });

  return out;
}

/**
 * The NPC turns into another one, through its transform component (attached
 * now when it has none).
 *
 * @param  who  the giver
 */
private void turn_into_another(object who)
{
  object npc, change;

  npc = query_entity();
  if (!npc->query_component_by_type("transform"))
    npc->add_component("transform", ([ "trigger": "called" ]));

  change = npc->query_component_by_type("transform");
  if (change)
    change->become(receive_transform, "", say_text(receive_appears, who, nil),
                   transform_targets(who), 0);
}

/**
 * The NPC goes away for good: its census row (or its place in the count of
 * its location) goes with it, so it is not made again.
 */
private void go_for_good()
{
  object npc, area;

  npc = query_entity();
  if (strlen(receive_leave))
    tell_room(environment(npc), say_text(receive_leave, nil, nil));

  if (npc->query_persisted())
  {
    area = AREA_HANDLER->query_area((string)npc->query_npc_area_path());
    if (area)
    {
      area->retire_npc((string)npc->query_uuid());
      return;
    }
  }

  npc->set_dead(1);
  npc->dest_me();
}

/**
 * What taking the item does: the line after the delay, the items given, the
 * quest, and last the change of the NPC itself, which ends with it.
 *
 * @param  who  the giver
 */
void do_effects(object who)
{
  object npc, item;
  int i;

  npc = query_entity();
  if (!npc || !environment(npc))
    return;

  tell_here(receive_then, who, nil);

  for (i = 0; i < sizeof(receive_give); i++)
  {
    item = clone_object(receive_give[i]);
    if (!item)
      continue;
    if (!who || environment(who) != environment(npc) || item->move(who))
      item->move(environment(npc));
  }

  do_quest(who);

  if (who && strlen(receive_talks))
    handler(QUESTS_HANDLER, who)->talked(who, receive_talks);

  working = 0;

  if (strlen(receive_transform))
    turn_into_another(who);
  else if (receive_leave)
    go_for_good();
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() + ([
    "items"     : receive_items,
    "verbs"     : receive_verbs,
    "give"      : receive_give,
    "message"   : receive_message,
    "refusal"   : receive_refusal,
    "spent"     : receive_spent,
    "then"      : receive_then,
    "once"      : receive_once,
    "talks"     : receive_talks,
    "transform" : receive_transform,
    "appears"   : receive_appears,
    "attack"    : receive_attack,
    "leave"     : receive_leave,
    "quest"     : receive_quest,
    "consume"   : receive_consume,
    "delay"     : receive_delay,
    "received"  : received,
  ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;

  if (pointerp(args["items"]))
    receive_items = ({ }) + args["items"];
  if (pointerp(args["verbs"]))
    receive_verbs = ({ }) + args["verbs"];
  if (pointerp(args["give"]))
    receive_give = ({ }) + args["give"];
  if (stringp(args["message"]))
    receive_message = args["message"];
  if (stringp(args["refusal"]))
    receive_refusal = args["refusal"];
  if (stringp(args["spent"]))
    receive_spent = args["spent"];
  if (stringp(args["then"]))
    receive_then = args["then"];
  if (stringp(args["once"]))
    receive_once = args["once"];
  if (stringp(args["talks"]))
    receive_talks = args["talks"];
  if (stringp(args["transform"]))
    receive_transform = args["transform"];
  if (stringp(args["appears"]))
    receive_appears = args["appears"];
  if (stringp(args["attack"]))
    receive_attack = args["attack"];
  if (stringp(args["leave"]))
    receive_leave = args["leave"];
  if (mappingp(args["quest"]))
    receive_quest = ([ ]) + args["quest"];
  if (intp(args["consume"]))
    receive_consume = args["consume"];
  if (intp(args["delay"]))
    receive_delay = args["delay"];
  if (intp(args["received"]))
    received = args["received"];
}

mixed * stats()
{
  return component::stats() + ({
    ({ "Items", implode(receive_items, ", ") }),
    ({ "Verbs", implode(receive_verbs, ", ") }),
    ({ "Consume", receive_consume }),
    ({ "Once", receive_once }),
    ({ "Transform", receive_transform }),
    ({ "Leave", receive_leave }),
    ({ "Received", received }),
  });
}
