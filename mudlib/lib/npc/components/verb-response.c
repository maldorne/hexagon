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
//               "name": only with the NPC's name ("<verb> <npc>")
//   "effect"  - file of the effect cast on the asker
//   "uses"    - how many times it answers in a period (default 0, no limit)
//   "period"  - seconds before the uses start again (default 1800)
//   "only"    - who is served; a mapping whose keys all have to hold:
//                 "guild" - list of guild objects, "race" - list of race
//                 objects, "align_min" - the lowest real alignment,
//                 "property" - a property the asker must have set
//   "said", "room" - the line for the asker and for the rest of the room when
//               it answers; "failed" - for the asker when the effect cannot be
//               cast; "spent" - when no use is left; "refusal" - for somebody
//               "only" leaves out; "busy" - while it fights (a stock line when
//               absent). $npc$ and $player$ are the names; an empty line says
//               nothing

#include <language.h>

inherit component "/lib/npc/component.c";

#define WORDS_ANY "any"
#define WORDS_NONE "none"
#define WORDS_NAME "name"
#define DEFAULT_PERIOD 1800

private string * response_verbs;
private string response_words, response_effect;
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
  return 1;
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
  if (response_words == WORDS_NAME &&
      (!stringp(str) || !me->id(lower_case(str))))
    return 0;

  if (is_fighting_here(me))
  {
    tell_object(who, say_text(response_busy, who));
    return 1;
  }

  if (!is_served(who))
  {
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
  return 1;
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() + ([
    "verbs"   : response_verbs,
    "words"   : response_words,
    "effect"  : response_effect,
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
    ({ "Uses", response_uses + " per " + response_period + "s" }),
    ({ "Used", used }),
  });
}
