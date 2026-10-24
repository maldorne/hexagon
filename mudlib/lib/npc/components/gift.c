// gift.c -- an item handed to those who greet the NPC in the right way, as an
// NPC component. Ported from the bow of the masters of the old muds, who gave
// their holy symbol to whoever honoured them.
//
// The NPC asks for the verbs to greet it with ("<verb> <its name>"). Whoever
// greets it is turned away unless they meet every rule, and is given a fresh
// item otherwise, once in a while.
//
// Attributes:
//   "verbs"           - the greeting verbs
//   "item"            - file of the item handed over, without the .c
//   "guild"           - guild object whose members are given it; anybody else
//                       is ignored
//   "min_level"       - class level needed; a lower one is only greeted back
//   "unless_carrying" - files of items (without the .c) that make the gift
//                       pointless when the greeter carries any of them
//   "wait"            - seconds before the same one is given another (default
//                       1800)
//   "ignore", "greet", "again", "given" - what the NPC does to a stranger, to
//                       somebody too young, to somebody served too soon or who
//                       carries one already, and what it does when it gives:
//                       each a line for the greeter, with $npc$ and $player$
//                       for the names; "given_room" is the line for the room
//                       and "greet_room" the one for the room when greeting back

inherit component "/lib/npc/component.c";

#define GIFT_WAIT_DEFAULT 1800
#define GIFT_PROP "gift from "

private string * gift_verbs, * gift_unless;
private string gift_item, gift_guild;
private int gift_min_level, gift_wait;
private string gift_ignore, gift_greet, gift_greet_room, gift_again, gift_given,
               gift_given_room;

void create()
{
  component::create();
  gift_verbs = ({ });
  gift_unless = ({ });
  gift_item = nil;
  gift_guild = nil;
  gift_min_level = 0;
  gift_wait = GIFT_WAIT_DEFAULT;
  gift_ignore = "";
  gift_greet = "";
  gift_greet_room = "";
  gift_again = "";
  gift_given = "";
  gift_given_room = "";
}

string query_info()
{
  return (gift_item ? gift_item : "-") + " from level " + gift_min_level;
}

mapping query_component_actions()
{
  mapping out;
  int i;

  out = ([ ]);
  for (i = 0; i < sizeof(gift_verbs); i++)
    out[gift_verbs[i]] = "do_greet";

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

// Whether the greeter carries one of the items that make the gift pointless.
private int carries_one(object who)
{
  object * items;
  int i;

  items = all_inventory(who);
  for (i = 0; i < sizeof(items); i++)
    if (member_array(base_name(items[i]), gift_unless) != -1)
      return 1;

  return 0;
}

int do_greet(string str)
{
  object me, who, item;
  string prop;

  who = this_player();
  me = query_entity();
  if (!who || !me || !gift_item || !stringp(str) || !me->id(lower_case(str)))
    return 0;

  if (gift_guild && who->query_guild_ob() != gift_guild)
  {
    if (strlen(gift_ignore))
      tell_object(who, say_text(gift_ignore, who));
    return 1;
  }

  if (who->query_level() < gift_min_level)
  {
    tell_object(who, say_text(gift_greet, who));
    if (strlen(gift_greet_room))
      tell_room(environment(who), say_text(gift_greet_room, who), ({ who }));
    return 1;
  }

  prop = GIFT_PROP + me->query_name();
  if (who->query_timed_property(prop) || carries_one(who))
  {
    tell_object(who, say_text(gift_again, who));
    return 1;
  }

  item = clone_object(gift_item + ".c");
  if (!item)
    return 1;

  tell_object(who, say_text(gift_given, who));
  if (strlen(gift_given_room))
    tell_room(environment(who), say_text(gift_given_room, who), ({ who }));

  if (item->move(who))
    item->move(environment(who));
  who->add_timed_property(prop, 1, gift_wait);

  return 1;
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() + ([
    "verbs"           : gift_verbs,
    "item"            : gift_item,
    "guild"           : gift_guild,
    "min_level"       : gift_min_level,
    "unless_carrying" : gift_unless,
    "wait"            : gift_wait,
    "ignore"          : gift_ignore,
    "greet"           : gift_greet,
    "greet_room"      : gift_greet_room,
    "again"           : gift_again,
    "given"           : gift_given,
    "given_room"      : gift_given_room,
  ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;

  if (pointerp(args["verbs"]))
    gift_verbs = ({ }) + args["verbs"];
  if (stringp(args["item"]))
    gift_item = args["item"];
  if (stringp(args["guild"]))
    gift_guild = args["guild"];
  if (intp(args["min_level"]))
    gift_min_level = args["min_level"];
  if (pointerp(args["unless_carrying"]))
    gift_unless = ({ }) + args["unless_carrying"];
  if (intp(args["wait"]))
    gift_wait = args["wait"];
  if (stringp(args["ignore"]))
    gift_ignore = args["ignore"];
  if (stringp(args["greet"]))
    gift_greet = args["greet"];
  if (stringp(args["greet_room"]))
    gift_greet_room = args["greet_room"];
  if (stringp(args["again"]))
    gift_again = args["again"];
  if (stringp(args["given"]))
    gift_given = args["given"];
  if (stringp(args["given_room"]))
    gift_given_room = args["given_room"];
}

mixed * stats()
{
  return component::stats() + ({
    ({ "Item", gift_item }),
    ({ "Guild", gift_guild }),
    ({ "Min Level", gift_min_level }),
    ({ "Wait", gift_wait }),
  });
}
