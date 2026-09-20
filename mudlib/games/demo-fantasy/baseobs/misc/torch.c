// A torch that burns for a while and then goes out. It has to be held to be
// lit, and what is left of its fuel travels in the player's save.

inherit "/lib/item.c";

#include <language.h>

#define BRIGHTNESS 50
#define FUEL 2000

int amount_of_fuel;
static int is_lighted;
// the call_out that burns the torch out, kept so putting it out can stop it
static int fuel_handle;

void create()
{
  amount_of_fuel = FUEL;
  is_lighted = 0;
  ::create();
}

void setup()
{
  set_name(_LANG_BASEOBS_TORCH);
  set_short(capitalize(_LANG_BASEOBS_TORCH));

  set_main_plural(capitalize(pluralize(_LANG_BASEOBS_TORCH)));
  add_plural(pluralize(_LANG_BASEOBS_TORCH));

  set_long(_LANG_BASEOBS_TORCH_LONG);

  // in spanish
  set_gender(2);

  set_holdable(1);
  set_hands_needed(1);

  set_value(50);
  set_weight(50);
}

// what is left of it, so a nearly spent torch is not sold as new
string query_long()
{
  int percentage;

  percentage = amount_of_fuel * 100 / FUEL;

  if (amount_of_fuel >= FUEL)
    return ::query_long() + _LANG_BASEOBS_TORCH_AS_NEW;
  if (percentage <= 0)
    return ::query_long() + _LANG_BASEOBS_TORCH_BURNT_OUT;

  return ::query_long() + _LANG_BASEOBS_TORCH_LEFT(percentage);
}

string short(varargs int dark)
{
  if (is_lighted)
    return ::short(dark) + _LANG_BASEOBS_TORCH_LIT;
  return ::short(dark);
}

string pretty_plural()
{
  if (is_lighted)
    return ::pretty_plural() + _LANG_BASEOBS_TORCH_LIT_PLURAL;
  return ::pretty_plural();
}

void set_fuel(int f) { amount_of_fuel = f; }
int query_fuel() { return amount_of_fuel; }

void init()
{
  ::init();
  add_action("do_light", _LANG_BASEOBS_TORCH_VERBS_LIGHT);
  add_action("do_extinguish", _LANG_BASEOBS_TORCH_VERBS_EXTINGUISH);
}

private void tell_carrier(string mine, string others)
{
  object who;

  who = environment(this_object());

  if (!who || !who->query_player())
    return;

  tell_object(who, mine);

  if (environment(who))
    tell_room(environment(who), others, ({ who }));
}

int do_light(string str)
{
  if (str != query_name())
  {
    notify_fail(_LANG_BASEOBS_TORCH_WHAT);
    return 0;
  }

  if (!query_in_use())
  {
    notify_fail(_LANG_BASEOBS_TORCH_HOLD_FIRST);
    return 0;
  }

  if (is_lighted)
  {
    notify_fail(_LANG_BASEOBS_TORCH_ALREADY_LIT);
    return 0;
  }

  if (amount_of_fuel <= 0)
  {
    notify_fail(_LANG_BASEOBS_TORCH_BURNT_OUT);
    return 0;
  }

  is_lighted = 1;
  fuel_handle = call_out("out_of_fuel", amount_of_fuel);
  set_light(BRIGHTNESS);

  tell_carrier(_LANG_BASEOBS_TORCH_YOU_LIGHT,
               _LANG_BASEOBS_TORCH_SOMEBODY_LIGHTS(environment(this_object())));
  return 1;
}

// putting it out keeps whatever time the call_out had left
private void put_out()
{
  amount_of_fuel = remove_call_out(fuel_handle);
  is_lighted = 0;
  set_light(0);
}

int do_extinguish(string str)
{
  if (str != query_name())
  {
    notify_fail(_LANG_BASEOBS_TORCH_WHAT);
    return 0;
  }

  if (!is_lighted)
  {
    notify_fail(_LANG_BASEOBS_TORCH_ALREADY_OUT);
    return 0;
  }

  put_out();

  tell_carrier(_LANG_BASEOBS_TORCH_YOU_EXTINGUISH,
               _LANG_BASEOBS_TORCH_SOMEBODY_EXTINGUISHES(environment(this_object())));
  return 1;
}

void out_of_fuel()
{
  amount_of_fuel = 0;
  is_lighted = 0;
  set_light(0);

  tell_carrier(_LANG_BASEOBS_TORCH_GOES_OUT,
               _LANG_BASEOBS_TORCH_SOMEBODYS_GOES_OUT(environment(this_object())));
}

// stopping holding it puts it out
int set_in_use(int i)
{
  if (i == 0 && is_lighted)
  {
    put_out();
    tell_carrier(_LANG_BASEOBS_TORCH_YOU_EXTINGUISH,
                 _LANG_BASEOBS_TORCH_SOMEBODY_EXTINGUISHES(environment(this_object())));
  }
  return ::set_in_use(i);
}

void dest_me()
{
  if (is_lighted)
    put_out();
  ::dest_me();
}

mapping query_auto_load_attributes()
{
  return ::query_auto_load_attributes() +
    ([ "amount_of_fuel" : amount_of_fuel, ]);
}

void init_auto_load_attributes(mapping args)
{
  if (!undefinedp(args["amount_of_fuel"]))
    amount_of_fuel = args["amount_of_fuel"];
  ::init_auto_load_attributes(args);
}
