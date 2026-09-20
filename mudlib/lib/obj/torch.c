// A torch that burns for a while and then goes out. It has to be wielded to be
// lit, it hits like a club, it burns whoever it hits while it is lit, and what
// is left of its fuel travels in the player's save.

inherit "/lib/weapon.c";

#include <translations/weapon.h>
#include <translations/combat.h>
#include <language.h>

#define BRIGHTNESS 50
#define FUEL 2000
#define TORCH_VALUE 50
// the key the fire attack is indexed by, never shown to a player
#define TORCH_ATTACK "torch"

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
  set_base_weapon(CLUB);

  set_name(_LANG_TORCH_NAME);
  set_short(capitalize(_LANG_TORCH_NAME));

  set_main_plural(capitalize(pluralize(_LANG_TORCH_NAME)));
  add_plural(pluralize(_LANG_TORCH_NAME));

  set_long(_LANG_TORCH_LONG);

  // in spanish
  set_gender(2);

  set_hands_needed(1);

  set_value(TORCH_VALUE);
  set_weight(50);
  set_size(1);
}

// being a weapon, the value would otherwise follow its condition, and a torch
// is sold for the same price however burnt it is
int query_value() { return TORCH_VALUE; }

// what is left of it, so a nearly spent torch is not sold as new
string query_long()
{
  int percentage;

  percentage = amount_of_fuel * 100 / FUEL;

  if (amount_of_fuel >= FUEL)
    return ::query_long() + _LANG_TORCH_AS_NEW;
  if (percentage <= 0)
    return ::query_long() + _LANG_TORCH_BURNT_OUT;

  return ::query_long() + _LANG_TORCH_LEFT;
}

string short(varargs int dark)
{
  if (is_lighted)
    return ::short(dark) + _LANG_TORCH_LIT;
  return ::short(dark);
}

string pretty_plural()
{
  if (is_lighted)
    return ::pretty_plural() + _LANG_TORCH_LIT_PLURAL;
  return ::pretty_plural();
}

void set_fuel(int f) { amount_of_fuel = f; }
int query_fuel() { return amount_of_fuel; }

void init()
{
  ::init();
  add_action("do_light", _LANG_TORCH_VERBS_LIGHT);
  add_action("do_extinguish", _LANG_TORCH_VERBS_EXTINGUISH);
}

private void tell_lighting()
{
  object who;

  who = environment(this_object());

  if (!who || !who->query_player())
    return;

  tell_object(who, _LANG_TORCH_YOU_LIGHT);

  if (environment(who))
    tell_room(environment(who), _LANG_TORCH_SOMEBODY_LIGHTS, ({ who }));
}

private void tell_extinguishing()
{
  object who;

  who = environment(this_object());

  if (!who || !who->query_player())
    return;

  tell_object(who, _LANG_TORCH_YOU_EXTINGUISH);

  if (environment(who))
    tell_room(environment(who), _LANG_TORCH_SOMEBODY_EXTINGUISHES, ({ who }));
}

private void tell_going_out()
{
  object who;

  who = environment(this_object());

  if (!who || !who->query_player())
    return;

  tell_object(who, _LANG_TORCH_GOES_OUT);

  if (environment(who))
    tell_room(environment(who), _LANG_TORCH_SOMEBODYS_GOES_OUT, ({ who }));
}

int do_light(string str)
{
  if (str != query_name())
  {
    notify_fail(_LANG_TORCH_WHAT);
    return 0;
  }

  if (!query_in_use())
  {
    notify_fail(_LANG_TORCH_HOLD_FIRST);
    return 0;
  }

  if (is_lighted)
  {
    notify_fail(_LANG_TORCH_ALREADY_LIT);
    return 0;
  }

  if (amount_of_fuel <= 0)
  {
    notify_fail(_LANG_TORCH_BURNT_OUT);
    return 0;
  }

  is_lighted = 1;
  fuel_handle = call_out("out_of_fuel", amount_of_fuel);
  set_light(BRIGHTNESS);

  // a lit torch burns what it hits
  add_attack(TORCH_ATTACK, FIRE, 1, 2, 0);

  tell_lighting();
  return 1;
}

// putting it out keeps whatever time the call_out had left
private void put_out()
{
  amount_of_fuel = remove_call_out(fuel_handle);
  is_lighted = 0;
  set_light(0);
  remove_attack(TORCH_ATTACK);
}

int do_extinguish(string str)
{
  if (str != query_name())
  {
    notify_fail(_LANG_TORCH_WHAT);
    return 0;
  }

  if (!is_lighted)
  {
    notify_fail(_LANG_TORCH_ALREADY_OUT);
    return 0;
  }

  put_out();
  tell_extinguishing();
  return 1;
}

void out_of_fuel()
{
  tell_going_out();

  amount_of_fuel = 0;
  is_lighted = 0;
  set_light(0);
  remove_attack(TORCH_ATTACK);
}

// stopping wielding it puts it out
int set_in_use(int i)
{
  if (i == 0 && is_lighted)
  {
    put_out();
    tell_extinguishing();
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
