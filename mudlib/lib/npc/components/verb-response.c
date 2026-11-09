// verb-response.c -- an NPC that answers a verb, as an NPC component: it says a
// line and, when it has one, casts an effect on whoever asked.
//
// The effect is a spell or skill the NPC knows, cast through cast_effect_on, so
// it makes the same refusals a player's cast does (gp, guild, lockouts). With
// no effect the NPC only says its line. It serves nobody while it fights
// somebody in its place.
//
// Attributes, every one optional but "verbs":
//   "verbs"   - the verbs it answers
//   "words"   - "any" (default): with or without words; "none": only with none,
//               the verb with words is left to whatever else answers it;
//               "name": only when the words identify this NPC: the filler
//               words ("al", "to", "the"...) are dropped wherever they stand,
//               the words of "object" are taken out, and what is left has to
//               be one of the NPC's ids ("<verb> <npc>", "<verb> to the <npc>",
//               "<verb> <object> to the <npc>"); otherwise the verb is left
//               to whatever else answers it
//   "object"  - with "words": "name", what the player names besides the NPC:
//               a string, or a list of alternatives; each is one or more
//               words that must be in the text ("ask the ape FOR A BOOK")
//   "effect"  - file of the effect cast on the asker
//   "uses"    - how many times it answers in a period (default 0, no limit)
//   "period"  - seconds before the uses start again (default 1800)
//   "only"    - who is served; a mapping whose keys all have to hold:
//                 "guild" - list of guild objects, "race" - list of race
//                 objects, "align_min" - the lowest real alignment,
//                 "property" - a property the asker must have set, or
//                 ({ name, value }) to have it set to that value
//   "said", "room" - the line for the asker and for the rest of the room when
//               it answers; "failed" - for the asker when the effect cannot be
//               cast; "spent" - when no use is left; "refusal" - for somebody
//               "only" leaves out; "busy" - while it fights (a stock line when
//               absent). $npc$ and $player$ are the names; an empty line says
//               nothing, except an empty "refusal": the NPC then does not
//               answer somebody "only" leaves out, and the verb is left to
//               whatever else answers it
//   "talks"   - target (template id) passed to the quests handler's talked()
//               each time it answers, so a quest can count the answer as an
//               objective of kind OBJECTIVE_TALK

#include <language.h>
#include <translations/filler.h>
#include <living/quests.h>

inherit component "/lib/npc/component.c";

#define WORDS_ANY "any"
#define WORDS_NONE "none"
#define WORDS_NAME "name"
#define DEFAULT_PERIOD 1800

private string * response_verbs;
private string response_words, response_effect, response_talks;
private string * response_object;
private int response_uses, response_period;
private mapping response_only;
private string response_said, response_room, response_failed, response_spent,
               response_refusal, response_busy;
// the answers given in the current period, and when it began
static int used, period_start;

void create()
{
  component::create();
  response_verbs = ({ });
  response_words = WORDS_ANY;
  response_effect = nil;
  response_talks = "";
  response_object = ({ });
  response_uses = 0;
  response_period = DEFAULT_PERIOD;
  response_only = ([ ]);
  response_said = "";
  response_room = "";
  response_failed = "";
  response_spent = "";
  response_refusal = "";
  response_busy = _LANG_VERB_RESPONSE_BUSY;
  used = 0;
  period_start = 0;
}

string query_info()
{
  return implode(response_verbs, ", ") +
         (response_effect ? " -> " + response_effect : "") +
         (response_uses ? " (" + used + "/" + response_uses + ")" : "");
}

mapping query_component_actions()
{
  mapping out;
  int i;

  out = ([ ]);
  for (i = 0; i < sizeof(response_verbs); i++)
    out[response_verbs[i]] = "do_respond";

  return out;
}

// A line with the names in it, nothing for an empty one.
private string say_text(string text, object who)
{
  if (!strlen(text))
    return "";

  return replace(text, ({ "$npc$", query_entity()->query_cap_name(),
                          "$player$", who->query_cap_name() })) + "\n";
}

// Whether `path` is one of the `allowed` objects.
private int is_listed(string path, string * allowed)
{
  if (!path || !strlen(path))
    return 0;
  return member_array(path, allowed) != -1;
}

// Whether `who` meets every rule of the "only" attribute.
private int is_served(object who)
{
  if (pointerp(response_only["guild"]) &&
      !is_listed(who->query_guild_ob(), response_only["guild"]))
    return 0;
  if (pointerp(response_only["race"]) &&
      !is_listed(who->query_race_ob(), response_only["race"]))
    return 0;
  if (intp(response_only["align_min"]) &&
      who->query_real_align() < response_only["align_min"])
    return 0;
  if (stringp(response_only["property"]) &&
      !who->query_property(response_only["property"]))
    return 0;
  if (pointerp(response_only["property"]) &&
      sizeof(response_only["property"]) == 2 &&
      who->query_property(response_only["property"][0]) !=
        response_only["property"][1])
    return 0;
  return 1;
}

/**
 * Whether the words of a verb identify this NPC: the filler words are dropped,
 * the words of the object (when one is set) must be there and are taken out,
 * and what is left has to be one of its ids.
 *
 * @param me the NPC
 * @param str what follows the verb
 * @return 1 when the words name this NPC
 */
private int names_npc(object me, string str)
{
  string * words, * wanted;
  int i, j, found;

  if (!stringp(str) || !strlen(str))
    return 0;

  str = lower_case(str);
  if (!sizeof(response_object) && me->id(str))
    return 1;

  words = explode(str, " ") - ({ "" }) - _LANG_FILLER_WORDS;

  if (sizeof(response_object))
  {
    found = 0;
    for (i = 0; i < sizeof(response_object) && !found; i++)
    {
      wanted = explode(lower_case(response_object[i]), " ") - ({ "" });
      found = sizeof(wanted) > 0;
      for (j = 0; j < sizeof(wanted); j++)
        if (member_array(wanted[j], words) == -1)
          found = 0;
      if (found)
        words -= wanted;
    }

    if (!found)
      return 0;
  }

  if (!sizeof(words))
    return 0;

  return me->id(implode(words, " "));
}

// Whether the NPC is fighting somebody in its own place.
private int is_fighting_here(object me)
{
  object * rivals;
  int i;

  rivals = me->query_attacker_list();
  for (i = 0; i < sizeof(rivals); i++)
    if (rivals[i] && living(rivals[i]) &&
        environment(rivals[i]) == environment(me))
      return 1;

  return 0;
}

// Whether a use is left in the current period, starting a new one when due.
private int has_use_left()
{
  if (!response_uses)
    return 1;

  if (time() - period_start >= response_period)
  {
    period_start = time();
    used = 0;
  }

  return used < response_uses;
}

int do_respond(string str)
{
  object me, who, effect;

  me = query_entity();
  who = this_player();
  if (!me || !who || who == me)
    return 0;

  if (stringp(str))
    str = trim(str);
  if (response_words == WORDS_NONE && stringp(str) && strlen(str))
    return 0;
  if (response_words == WORDS_NAME && !names_npc(me, str))
    return 0;

  if (is_fighting_here(me))
  {
    tell_object(who, say_text(response_busy, who));
    return 1;
  }

  if (!is_served(who))
  {
    // no line of refusal: the verb is left to whatever else answers it
    if (!strlen(response_refusal))
      return 0;
    tell_object(who, say_text(response_refusal, who));
    return 1;
  }

  if (!has_use_left())
  {
    tell_object(who, say_text(response_spent, who));
    return 1;
  }

  if (response_effect)
  {
    effect = nil;
    catch(effect = load_object(response_effect));
    if (!effect)
      return 1;
  }

  tell_object(who, say_text(response_said, who));
  if (strlen(response_room))
    tell_room(environment(who), say_text(response_room, who), ({ who }));

  if (effect && !effect->cast_effect_on(({ who }), me))
  {
    tell_object(who, say_text(response_failed, who));
    return 1;
  }

  used++;

  if (strlen(response_talks))
    handler(QUESTS_HANDLER, who)->talked(who, response_talks);

  return 1;
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() + ([
    "verbs"   : response_verbs,
    "words"   : response_words,
    "effect"  : response_effect,
    "object"  : response_object,
    "talks"   : response_talks,
    "uses"    : response_uses,
    "period"  : response_period,
    "only"    : response_only,
    "said"    : response_said,
    "room"    : response_room,
    "failed"  : response_failed,
    "spent"   : response_spent,
    "refusal" : response_refusal,
    "busy"    : response_busy,
  ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;

  if (pointerp(args["verbs"]))
    response_verbs = ({ }) + args["verbs"];
  if (stringp(args["words"]))
    response_words = args["words"];
  if (stringp(args["effect"]))
    response_effect = args["effect"];
  if (stringp(args["object"]))
    response_object = ({ args["object"] });
  else if (pointerp(args["object"]))
    response_object = ({ }) + args["object"];
  if (stringp(args["talks"]))
    response_talks = args["talks"];
  if (intp(args["uses"]))
    response_uses = args["uses"];
  if (intp(args["period"]) && args["period"] > 0)
    response_period = args["period"];
  if (mappingp(args["only"]))
    response_only = ([ ]) + args["only"];
  if (stringp(args["said"]))
    response_said = args["said"];
  if (stringp(args["room"]))
    response_room = args["room"];
  if (stringp(args["failed"]))
    response_failed = args["failed"];
  if (stringp(args["spent"]))
    response_spent = args["spent"];
  if (stringp(args["refusal"]))
    response_refusal = args["refusal"];
  if (stringp(args["busy"]))
    response_busy = args["busy"];
}

mixed * stats()
{
  return component::stats() + ({
    ({ "Verbs", implode(response_verbs, ", ") }),
    ({ "Words", response_words }),
    ({ "Effect", response_effect }),
    ({ "Object", implode(response_object, ", ") }),
    ({ "Talks", response_talks }),
    ({ "Uses", response_uses + " per " + response_period + "s" }),
    ({ "Used", used }),
  });
}
