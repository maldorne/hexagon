// protector.c -- an NPC that defends some people, as an NPC component.
//
// Who it defends is a rule of the NPC, not of the place: wherever it stands,
// when somebody it protects is attacked in its room it steps in with
// 'proteger' (so blows aimed at them may fall on it instead) and attacks the
// attacker. With "on_arrival" it also protects them as they come in, before
// any fight.
//
// A living is protected when it matches any of the lists and passes every
// filter. With no list the component protects nobody.
//
// Attributes:
//   "names"           - names of NPCs or players (query_name)
//   "citizenships"    - citizenship names; those under one in the diplomacy
//                       graph count as of it
//   "guilds"          - guild object paths whose members it protects
//   "races"           - race object paths
//   "min_class_level" - least class level of the protected
//   "max_class_level" - greatest class level of the protected
//   "min_align"       - least real alignment of the protected (0: not evil)
//   "on_arrival"      - 1 to protect them as soon as they come in
//
// Protecting is the NPC typing 'proteger' (the command checks the consent of
// the protected, which every NPC gives by default). Blows from an NPC that is itself protected by the rule are not answered: the
// guards of a city arresting one of its citizens are not an attack to defend.

inherit component "/lib/npc/component.c";

private string * protector_names, * protector_citizenships,
               * protector_guilds, * protector_races;
private int protector_min_class_level, protector_max_class_level,
            protector_on_arrival;
// the least real alignment of the protected, nil when it does not look at it
private mixed protector_min_align;

void create()
{
  component::create();
  protector_names = ({ });
  protector_citizenships = ({ });
  protector_guilds = ({ });
  protector_races = ({ });
  protector_min_class_level = 0;
  protector_max_class_level = 0;
  protector_on_arrival = 0;
  protector_min_align = nil;
}

string query_info()
{
  return implode(protector_names + protector_citizenships + protector_guilds +
                 protector_races, ", ");
}

// Is `who` of the citizenship `name` or of one under it?
private int _is_of_citizenship(object who, string name)
{
  object citizenship;
  string city_ob, current;

  city_ob = who->query_city_ob();
  if (!city_ob || !strlen(city_ob))
    return 0;

  citizenship = load_object(city_ob);
  if (!citizenship)
    return 0;

  current = citizenship->query_name();
  while (current && strlen(current))
  {
    if (current == name)
      return 1;
    current = handler("diplomacy", citizenship)->query_parent(current);
  }

  return 0;
}

/**
 * @param  who  a living
 * @return 1 when this NPC protects them
 */
int is_protected(object who)
{
  object me;
  int i, level, matched;

  me = query_entity();
  if (!who || !me || who == me || !living(who))
    return 0;

  matched = member_array(who->query_name(), protector_names) >= 0 ||
            member_array(who->query_guild_ob(), protector_guilds) >= 0 ||
            member_array(who->query_race_ob(), protector_races) >= 0;

  for (i = 0; !matched && i < sizeof(protector_citizenships); i++)
    matched = _is_of_citizenship(who, protector_citizenships[i]);

  if (!matched)
    return 0;

  level = (int)who->query_class_level();
  if (protector_min_class_level && level < protector_min_class_level)
    return 0;
  if (protector_max_class_level && level > protector_max_class_level)
    return 0;
  if (intp(protector_min_align) &&
      (int)who->query_real_align() < protector_min_align)
    return 0;

  return 1;
}

// Types 'proteger' on them, unless somebody in the room already protects them.
private void _protect(object who)
{
  object me, current;

  me = query_entity();
  if (!me || !who || environment(who) != environment(me))
    return;

  current = who->query_protector();
  if (current && environment(current) == environment(who))
    return;

  me->do_command("proteger " + who->query_name());
}

/**
 * A blow in the room, `fighters` being ({ victim, attacker }). When the victim
 * is protected, the NPC protects them and attacks the attacker.
 */
void event_fight_in_progress(object ob, object * fighters)
{
  object me, victim, attacker;

  me = query_entity();
  if (!me || me->query_dead() || !pointerp(fighters) || sizeof(fighters) < 2)
    return;

  victim = fighters[0];
  attacker = fighters[1];
  if (!victim || !attacker || attacker == me || victim == attacker)
    return;
  if (environment(attacker) != environment(me) || !is_protected(victim))
    return;
  if (!interactive(attacker) && is_protected(attacker))
    return;

  _protect(victim);

  if (member_array(attacker, me->query_attacker_list()) < 0)
    me->attack_ob(attacker);
}

/**
 * Somebody has come in: with "on_arrival", the protected are protected now.
 */
void event_enter(object ob, varargs string mess, object from, mixed avoid)
{
  if (protector_on_arrival && is_protected(ob))
    call_out("protect_present", 1);
}

/**
 * The NPC itself has moved: with "on_arrival", it protects whoever it finds.
 */
void event_moved(object ob, object from, object dest)
{
  if (protector_on_arrival)
    call_out("protect_present", 1);
}

void placed()
{
  if (protector_on_arrival)
    call_out("protect_present", 1);
}

/**
 * Protects everybody in its room that it protects.
 */
void protect_present()
{
  object me;
  object * inv;
  int i;

  me = query_entity();
  if (!me || me->query_dead() || !environment(me))
    return;

  inv = all_inventory(environment(me));
  for (i = 0; i < sizeof(inv); i++)
    if (is_protected(inv[i]))
      _protect(inv[i]);
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() + ([
    "names"           : protector_names,
    "citizenships"    : protector_citizenships,
    "guilds"          : protector_guilds,
    "races"           : protector_races,
    "min_class_level" : protector_min_class_level,
    "max_class_level" : protector_max_class_level,
    "min_align"       : protector_min_align,
    "on_arrival"      : protector_on_arrival,
  ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;

  if (pointerp(args["names"]))
    protector_names = ({ }) + args["names"];
  if (pointerp(args["citizenships"]))
    protector_citizenships = ({ }) + args["citizenships"];
  if (pointerp(args["guilds"]))
    protector_guilds = ({ }) + args["guilds"];
  if (pointerp(args["races"]))
    protector_races = ({ }) + args["races"];
  if (intp(args["min_class_level"]))
    protector_min_class_level = args["min_class_level"];
  if (intp(args["max_class_level"]))
    protector_max_class_level = args["max_class_level"];
  if (intp(args["min_align"]))
    protector_min_align = args["min_align"];
  if (intp(args["on_arrival"]))
    protector_on_arrival = args["on_arrival"];
}

mixed * stats()
{
  return component::stats() + ({
    ({ "Names", implode(protector_names, ", ") }),
    ({ "Citizenships", implode(protector_citizenships, ", ") }),
    ({ "Guilds", implode(protector_guilds, ", ") }),
    ({ "Races", implode(protector_races, ", ") }),
    ({ "Min Class Level", protector_min_class_level }),
    ({ "Max Class Level", protector_max_class_level }),
    ({ "Min Align", intp(protector_min_align) ? "" + protector_min_align : "-" }),
    ({ "On Arrival", protector_on_arrival }),
  });
}
