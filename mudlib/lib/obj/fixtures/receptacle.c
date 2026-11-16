// A fixed object that receives an item from a player: a pedestal that takes an
// idol, a desk opened with its key, an altar that takes a sacrifice. It is the
// fixture counterpart of the NPC component /lib/npc/components/receive.c and
// shares its vocabulary; the item is looked for in what the player carries, so
// no fixture repeats the loop that finds it, checks it and destroys it.
//
// The fixture describes itself and calls set_receptacle in its setup() with a
// mapping, every attribute optional:
//   "items"     - what is accepted: full file paths with .c (compared exactly)
//                 or ids of the item; nothing listed accepts any item
//   "verbs"     - the verbs that hand an item over: "<verb> <item> [en <fixture>]"
//   "carried"   - 1: the item is a key the player carries and the syntax is
//                 only "<verb> <fixture>" ("open chest")
//   "consume"   - 1: the item is destroyed
//   "once"      - "player" takes it once from each player (kept as a property
//                 of the player), "fixture" once from anybody
//   "requires_quest" - "<game>:<quest id>": only who is doing that quest
//   "message"   - told to the giver when the item is taken
//   "room_message" - told to the others in the room when it is taken
//   "refusal"   - told to the giver when they carry nothing that is accepted
//   "refusal_room" - told to the room along with the refusal (the refusal is
//                 then written, and the command is handled)
//   "spent"     - told to the giver when "once" is used up
//   "denied"    - told to the giver when the quest is not the one required;
//                 the item is not taken
//   effects, done together in this order:
//   "property"  - set on the giver: a name, or ({ name, value })
//   "give"      - files of items cloned for the giver
//   "quest"     - ([ "id": "<game>:<quest id>", "action": "accept" or
//                 "complete" (default accept), "source": who gives it ])
//   "reveal"    - the sealed exit of the place that opens; "reveal_time" seals
//                 it again after that many seconds
//   "pass"      - the giver alone is taken through that sealed exit, which
//                 stays sealed ("pass_leave" and "pass_arrive" are the
//                 messages of force_exit)
//   Texts may carry $player$ (the giver) and $item$.
//
// For the odd cases an inheritor redefines can_receive (a condition), received
// (what happens after the item is taken), or one of the finders: is_wanted,
// find_item, is_target and parse_receive.

#include <language.h>
#include <living/quests.h>
#include <translations/filler.h>

inherit "/lib/obj/fixtures/fixture.c";

#define ONCE_PLAYER "player"
#define ONCE_FIXTURE "fixture"
#define QUEST_COMPLETE "complete"
#define RECEPTACLE_PROP "receptacle:"

private mapping receptacle;
// the fixture has already taken something once, for the "fixture" kind of "once"
private int taken;
// the fixture phrase of the command being handled
static string receive_target;

void create()
{
  receptacle = ([ ]);
  taken = 0;
  receive_target = "";
  ::create();
}

/**
 * Sets what the fixture receives and what happens (see the attributes in the
 * header) and registers its verbs.
 *
 * @param  config  the attributes
 */
void set_receptacle(mapping config)
{
  receptacle = ([ ]) + config;
  if (pointerp(receptacle["verbs"]) && sizeof(receptacle["verbs"]))
    add_fixture_action("do_receive", receptacle["verbs"]);
}

mapping query_receptacle() { return ([ ]) + receptacle; }

/**
 * The fixture phrase of the command being handled ("cabeza 1" in "put tube in
 * head 1"), for can_receive and received.
 */
string query_receive_target() { return receive_target; }

// ************************************************************
//  Hooks for the inheritors
// ************************************************************

/**
 * Whether the receiving goes on. Called with the item found, or nil when the
 * player carries nothing that is accepted, before anything else is checked. A
 * fixture that stops it tells the player itself.
 *
 * @param  item  the item, nil for none
 * @param  who   the giver
 * @return 1 to go on
 */
int can_receive(object item, object who) { return 1; }

/**
 * Called when the item has been taken and the effects done, for the puzzles
 * that keep their own logic. The item is already destroyed when "consume" is
 * set.
 *
 * @param  item    the item
 * @param  who     the giver
 * @param  target  the fixture phrase the player typed
 */
void received(object item, object who, string target) { }

// ************************************************************
//  Finding the item
// ************************************************************

// The text without the filler word that may open it ("the chest").
private string without_article(string text)
{
  string * words;

  words = explode(text, " ") - ({ "" });
  if (sizeof(words) > 1 && member_array(words[0], _LANG_FILLER_WORDS) != -1)
    words = words[1..];

  return implode(words, " ");
}

/**
 * Whether the fixture accepts this item: it is listed by its file or by an
 * id, or nothing is listed.
 *
 * @param  item  the item
 * @return 1 if it is accepted
 */
int is_wanted(object item)
{
  string * items;
  int i;

  items = pointerp(receptacle["items"]) ? receptacle["items"] : ({ });
  if (!sizeof(items))
    return 1;

  for (i = 0; i < sizeof(items); i++)
    if (items[i][0] == '/')
    {
      if (base_name(item) + ".c" == items[i])
        return 1;
    }
    else if (item->id(items[i]))
      return 1;

  return 0;
}

/**
 * The accepted item the player carries that the words name; any accepted one
 * when the words are empty.
 *
 * @param  who    the giver
 * @param  thing  what the player typed for the item
 * @return the item, nil for none
 */
object find_item(object who, string thing)
{
  object * inv;
  int i;

  inv = all_inventory(who);
  for (i = 0; i < sizeof(inv); i++)
    if (is_wanted(inv[i]) &&
        (!strlen(thing) || inv[i]->id(thing) ||
         inv[i]->id(without_article(thing))))
      return inv[i];

  return nil;
}

/**
 * Whether the words name this fixture. Empty words do when the item is typed
 * alone ("put idol").
 *
 * @param  target  what the player typed for the fixture
 * @return 1 if it is this fixture
 */
int is_target(string target)
{
  if (!strlen(target))
    return !receptacle["carried"];

  return id(target) || id(without_article(target));
}

/**
 * Splits what follows the verb into ({ item words, fixture words }). A
 * failure is told with notify_fail and answers nil.
 *
 * @param  str  what follows the verb
 * @return the two phrases, nil on failure
 */
mixed * parse_receive(string str)
{
  string * words, * preps;
  int i;

  if (!str || !strlen(str))
  {
    notify_fail(_LANG_RECEPTACLE_WHAT_OPEN + capitalize(query_verb()) +
                _LANG_RECEPTACLE_WHAT_CLOSE);
    return nil;
  }

  if (receptacle["carried"])
    return ({ "", str });

  words = explode(str, " ") - ({ "" });
  preps = _LANG_RECEPTACLE_PREPS;
  for (i = 1; i < sizeof(words) - 1; i++)
    if (member_array(words[i], preps) != -1)
      return ({ implode(words[.. i - 1], " "), implode(words[i + 1 ..], " ") });

  return ({ implode(words, " "), "" });
}

// ************************************************************
//  Receiving
// ************************************************************

// A text with the names in it, ending in a new line; nothing for an empty one.
private string say_text(string text, object who, object item)
{
  if (!stringp(text) || !strlen(text))
    return "";

  return replace_string(replace_string(text,
           "$player$", who ? (string)who->query_cap_name() : ""),
           "$item$", item ? (string)item->short() : "");
}

// The property of a player that tells they were already served here.
private string once_property()
{
  if (stringp(receptacle["property"]))
    return receptacle["property"];
  if (pointerp(receptacle["property"]) && sizeof(receptacle["property"]))
    return receptacle["property"][0];

  return RECEPTACLE_PROP + base_name(this_object());
}

// Whether this fixture has used up its "once" for this player.
private int is_spent(object who)
{
  if (receptacle["once"] == ONCE_FIXTURE)
    return taken;

  return receptacle["once"] == ONCE_PLAYER && who->query_property(once_property());
}

// Whether the player is doing the quest the fixture requires, if any.
private int meets_quest(object who)
{
  string * parts;

  if (!stringp(receptacle["requires_quest"]))
    return 1;

  // quests are recorded under their full id, "<game>:<name>"
  parts = explode(receptacle["requires_quest"], ":");
  if (sizeof(parts) < 2)
    return 1;

  return who->is_doing_quest(parts[0], receptacle["requires_quest"]);
}

// Accepts or completes the quest of the fixture for the giver.
private void do_quest(object who)
{
  mapping quest;

  quest = receptacle["quest"];
  if (!mappingp(quest) || !stringp(quest["id"]))
    return;

  if (quest["action"] == QUEST_COMPLETE)
    handler(QUESTS_HANDLER, who)->complete(who, quest["id"]);
  else
    handler(QUESTS_HANDLER, who)->accept(who, quest["id"],
                                         stringp(quest["source"]) ? quest["source"] : "");
}

/**
 * Seals again an exit the fixture revealed for a while. Public so that
 * call_out reaches it.
 *
 * @param  direction  the exit
 */
void hide_exit(string direction)
{
  if (environment())
    environment()->seal_exit(direction);
}

// The effects of the fixture, in the order of the header.
private void do_effects(object who)
{
  string * files;
  object place, gift;
  mixed prop;
  int i;

  place = environment();

  prop = receptacle["property"];
  if (stringp(prop))
    who->add_property(prop, 1);
  else if (pointerp(prop) && sizeof(prop) == 2)
    who->add_property(prop[0], prop[1]);

  files = pointerp(receptacle["give"]) ? receptacle["give"] : ({ });
  for (i = 0; i < sizeof(files); i++)
  {
    gift = clone_object(files[i]);
    if (gift->move(who) && place)
      gift->move(place);
  }

  do_quest(who);

  if (place && stringp(receptacle["reveal"]))
  {
    place->reveal_exit(receptacle["reveal"]);
    if (intp(receptacle["reveal_time"]) && receptacle["reveal_time"] > 0)
      call_out("hide_exit", receptacle["reveal_time"], receptacle["reveal"]);
  }

  if (place && stringp(receptacle["pass"]))
    place->force_exit(receptacle["pass"], who, receptacle["pass_leave"],
                      receptacle["pass_arrive"]);
}

/**
 * Takes the item from the giver: tells the messages, marks the "once", destroys
 * the item when it is consumed, does the effects and calls received. Public
 * for a fixture that learns of the item by other means than its verbs (an
 * item put on it with the common command).
 *
 * @param  item    the item, which the giver carries
 * @param  who     the giver
 * @param  target  the fixture phrase the player typed
 */
void take_item(object item, object who, string target)
{
  string text;

  text = say_text(receptacle["message"], who, item);
  if (strlen(text))
    tell_object(who, text);

  text = say_text(receptacle["room_message"], who, item);
  if (strlen(text) && environment())
    tell_room(environment(), text, who);

  if (receptacle["once"] == ONCE_FIXTURE)
    taken = 1;
  else if (receptacle["once"] == ONCE_PLAYER)
    who->add_property(once_property(), 1);

  if (receptacle["consume"])
    item->dest_me();

  do_effects(who);
  received(item, who, target);
}

/**
 * The verbs of the fixture: finds the item, runs the conditions and takes it.
 *
 * @param  str  what follows the verb
 * @return 1 if the verb was handled
 */
int do_receive(string str)
{
  mixed * parsed;
  object who, item;
  string text;

  who = this_player();
  if (!who)
    return 0;

  parsed = parse_receive(str);
  if (!parsed)
    return 0;

  if (!is_target(parsed[1]))
  {
    notify_fail(_LANG_RECEPTACLE_WHAT_OPEN + capitalize(query_verb()) +
                _LANG_RECEPTACLE_WHAT_CLOSE);
    return 0;
  }
  receive_target = parsed[1];

  item = find_item(who, parsed[0]);
  if (!can_receive(item, who))
    return 1;

  if (!item)
  {
    text = stringp(receptacle["refusal"]) ? receptacle["refusal"] :
           _LANG_RECEPTACLE_REFUSAL;
    if (!receptacle["carried"] && !stringp(receptacle["refusal_room"]))
    {
      notify_fail(say_text(text, who, nil));
      return 0;
    }

    write(say_text(text, who, nil));
    text = say_text(receptacle["refusal_room"], who, nil);
    if (strlen(text) && environment())
      tell_room(environment(), text, who);
    return 1;
  }

  if (is_spent(who))
  {
    write(stringp(receptacle["spent"]) ? say_text(receptacle["spent"], who, item) :
          _LANG_RECEPTACLE_SPENT);
    return 1;
  }

  if (!meets_quest(who))
  {
    write(stringp(receptacle["denied"]) ? say_text(receptacle["denied"], who, item) :
          _LANG_RECEPTACLE_DENIED);
    return 1;
  }

  take_item(item, who, parsed[1]);
  return 1;
}
