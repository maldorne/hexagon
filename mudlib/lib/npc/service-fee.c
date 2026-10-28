// service-fee.c -- taking the price of a service an NPC sells, for the NPC
// components that charge (raise, heal-service).
//
// Prices are whole amounts in the base coin. A purse is the money object a
// living carries; the payment is made in the coins it has and the change given
// back, as the money handler works it out.

#include <basic/money.h>
#include <translations/money.h>

/**
 * A price in words, in the coins of the money handler.
 *
 * @param amount the price in base coins
 * @return the price as text
 */
string price_words(int amount)
{
  return (string)MONEY_HAND->money_value_string(amount);
}

/**
 * The purse of a holder, nil when it has none.
 *
 * @param holder a living, or a corpse
 * @return the money object it carries
 */
private object _purse_of(object holder)
{
  return holder ? present(MONEY_NAME, holder) : nil;
}

/**
 * How much money, in base coins, a holder carries.
 *
 * @param holder a living, or a corpse
 * @return the value of its purse, 0 when it has none
 */
int query_purse_value(object holder)
{
  object purse;

  purse = _purse_of(holder);
  return purse ? (int)purse->query_value() : 0;
}

/**
 * Who would pay `amount`: the player when their purse covers it; otherwise,
 * when `from_body`, the body they left behind if it lies in the same place and
 * its purse covers it (a death leaves the player's money on the corpse, so a
 * ghost carries none).
 *
 * @param who the player asking for the service
 * @param amount the price in base coins
 * @param from_body 1 to allow paying from the player's own corpse
 * @return the object whose purse pays, nil when nobody can
 */
object find_payer(object who, int amount, int from_body)
{
  object purse, corpse;

  purse = _purse_of(who);
  if (purse && (int)purse->query_value() >= amount)
    return who;

  if (!from_body)
    return nil;

  corpse = (object)who->query_my_corpse();
  if (!corpse || environment(corpse) != environment(who))
    return nil;

  purse = _purse_of(corpse);
  if (purse && (int)purse->query_value() >= amount)
    return corpse;

  return nil;
}

/**
 * Takes `amount` from the purse of `payer`, as found by find_payer.
 *
 * @param payer the living or corpse that pays
 * @param amount the price in base coins
 * @return 1 when it was paid, 0 when the purse could not make the payment
 */
int take_payment(object payer, int amount)
{
  object purse;
  mixed * paid;
  int i;

  purse = _purse_of(payer);
  if (!purse)
    return 0;

  paid = (mixed *)MONEY_HAND->make_payment(BASE_COIN, amount, purse);
  if (!paid)
    return 0;

  // what is handed over, then the change that comes back
  for (i = 0; i < sizeof(paid[0]); i += 2)
    purse->adjust_money(-paid[0][i + 1], paid[0][i]);
  for (i = 0; i < sizeof(paid[1]); i += 2)
    purse->adjust_money(paid[1][i + 1], paid[1][i]);

  return 1;
}

// Whether `path` is one of the `allowed` objects.
private int _is_listed(string path, string * allowed)
{
  if (!path || !strlen(path))
    return 0;
  return member_array(path, allowed) != -1;
}

/**
 * Whether `who` meets every rule of an "only" attribute: a mapping whose keys
 * all have to hold ("guild" and "race" lists of objects, "align_min" the lowest
 * real alignment, "property" a property the player must have). An empty
 * mapping lets anybody through.
 *
 * @param who the player asking for the service
 * @param only the rules
 * @return 1 when the player is served, 0 when left out
 */
int meets_only(object who, mapping only)
{
  if (pointerp(only["guild"]) && !_is_listed(who->query_guild_ob(), only["guild"]))
    return 0;
  if (pointerp(only["race"]) && !_is_listed(who->query_race_ob(), only["race"]))
    return 0;
  if (intp(only["align_min"]) && who->query_real_align() < only["align_min"])
    return 0;
  if (stringp(only["property"]) && !who->query_property(only["property"]))
    return 0;
  return 1;
}
