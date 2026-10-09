// toll.c -- charging for the way through an exit, as an NPC component.
//
// The NPC keeps one exit of the place it stands in, the way a guard does: the
// exit handler asks it about whoever tries to go that way, and it lets through
// only those who have paid, for a while, and those whose citizenship crosses
// free. `pay` charges the fee; when there is a gate on that exit, the NPC opens
// it for the one who paid. With the NPC gone nobody is charged.
//
// Attributes:
//   "direction" - the exit it keeps
//   "fee"       - the price, in the base coin
//   "duration"  - how long a payment lasts, in heart beats
//   "exempt"    - the names of the citizenships that cross free

#include <basic/money.h>
#include <translations/money.h>
#include <language.h>

inherit component "/lib/npc/component.c";

private string toll_direction;
private int toll_fee;
private int toll_duration;
private string * toll_exempt;

void create()
{
  component::create();
  toll_direction = nil;
  toll_fee = 0;
  toll_duration = 300;
  toll_exempt = ({ });
}

// Keep the exit: the place asks us about whoever goes that way.
void placed()
{
  object npc, env;

  npc = query_owner();
  if (!npc || !toll_direction || !(env = environment(npc)))
    return;

  env->register_guard(npc, toll_direction);
}

// The timed property a payment leaves on the payer, one per toll keeper.
private string paid_property()
{
  return "toll_" + query_owner()->query_name();
}

private int is_exempt(object who)
{
  string city;
  object citizenship;

  city = who->query_city_ob();
  if (!city || !(citizenship = load_object(city)))
    return 0;

  return member_array(citizenship->query_name(), toll_exempt) != -1;
}

// Let `mover` through the kept exit? Only people are charged.
int check(object mover)
{
  if (!mover || !query_owner() || !interactive(mover))
    return 1;

  return mover->query_timed_property(paid_property()) || is_exempt(mover);
}

// What the keeper says to whoever it stops.
string message()
{
  object who;

  who = this_player();
  if (!who)
    return nil;

  return query_owner()->query_cap_name() + ": " + _LANG_TOLL_BLOCKED + "\n";
}

// A closed gate on the kept exit is opened for whoever may go through.
private void open_gate()
{
  object npc, env, gate;

  npc = query_owner();
  env = npc ? environment(npc) : nil;
  gate = env ? env->query_door_ob(toll_direction) : nil;
  if (gate && !gate->is_open())
    npc->queue_action(gate->query_open_verb() + " " + toll_direction);
}

mapping query_component_actions()
{
  mapping out;
  string * verbs;
  int i;

  out = ([ ]);
  verbs = _LANG_TOLL_VERBS;
  for (i = 0; i < sizeof(verbs); i++)
    out[verbs[i]] = "do_pay";

  return out;
}

int do_pay(string str)
{
  object who, npc;

  who = this_player();
  npc = query_owner();
  if (!who || !npc)
    return 0;

  if (is_exempt(who))
  {
    npc->do_say(_LANG_TOLL_EXEMPT);
    return 1;
  }

  if (who->query_timed_property(paid_property()))
  {
    npc->do_say(_LANG_TOLL_ALREADY_PAID);
    open_gate();
    return 1;
  }

  if ((int)who->query_value() < toll_fee)
  {
    npc->do_say(_LANG_TOLL_NO_MONEY);
    return 1;
  }

  who->pay_money(handler("money")->create_money_array(toll_fee));
  npc->adjust_money(toll_fee, BASE_COIN);
  who->add_timed_property(paid_property(), 1, toll_duration);
  npc->do_say(_LANG_TOLL_PAID);
  open_gate();

  return 1;
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "direction" : toll_direction, "fee" : toll_fee,
            "duration" : toll_duration, "exempt" : toll_exempt ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;
  if (stringp(args["direction"]))
    toll_direction = args["direction"];
  if (intp(args["fee"]))
    toll_fee = args["fee"];
  if (intp(args["duration"]) && args["duration"] > 0)
    toll_duration = args["duration"];
  if (pointerp(args["exempt"]))
    toll_exempt = args["exempt"];
}

mixed * stats()
{
  return component::stats() + ({ ({ "Direction", toll_direction }),
                                 ({ "Fee", toll_fee }),
                                 ({ "Exempt", toll_exempt }) });
}
