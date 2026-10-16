// The temple's behaviour, shared by the temple room (/lib/room/temple/temple-room.c)
// and the temple location component (/lib/location/components/temple.c). A
// temple is given to one deity; there a player may worship it, once and
// forever, and its followers make it offerings. Praying is the pray skill's
// (/lib/obj/skills/pray.c), which works in a temple of one's own deity.
//
// Commands (Spanish and English): adorar / worship (take the temple's deity,
// after a yes/no confirmation), ofrecer / offer <item | corpse | n coins>. A
// corpse may be offered where it lies, in the temple; everything else has to
// be carried.
//
// Who may worship and what a deity takes are the deity's (/lib/deity.c); the
// accounts are kept by the game's deities handler. See temple.md.
//
// Whoever inherits it says where the temple stands (query_temple_place), whether
// it is closed (query_temple_closed) and puts up its sign (create_temple_sign).

#include <deities.h>
#include <language.h>
#include <translations/language.h>

// the help document the temple sign shows, read every time the sign is put up
#define TEMPLE_SIGN_DOC "help/world/temples"

// the deity the temple is given to, its full path with ".c"; "" for none
string temple_deity;
static object temple_sign;

/** Marks the place as a temple; the conversion and the pray skill look for it. */
int query_temple() { return 1; }

/** The room or location the temple stands in. */
object query_temple_place() { return this_object(); }

/** Gives the temple to a deity: its full path with ".c", or "" for none. */
void set_temple_deity(string path) { temple_deity = path ? path : ""; }

string query_temple_deity() { return temple_deity ? temple_deity : ""; }

/** Whether nobody officiates here now; a room temple never closes. */
int query_temple_closed() { return 0; }

/**
 * Puts up the sign with the instructions, read now from the help document so
 * changing it changes every temple sign.
 *
 * @param where the room or location that holds the sign
 * @return the sign
 */
object create_temple_sign(object where)
{
  if (temple_sign)
    temple_sign->dest_me();

  temple_sign = where->add_sign(_LANG_TEMPLE_SIGN_LONG,
    handler("help")->query_document_text(doc(TEMPLE_SIGN_DOC)),
    nil, nil, nil, STD_LANG);
  return temple_sign;
}

/** Takes the sign down again, when the temple leaves its place. */
void take_down_temple_sign()
{
  if (temple_sign)
    temple_sign->dest_me();
  temple_sign = nil;
}

void init()
{
  string * verbs;
  int i;

  verbs = _LANG_TEMPLE_CMDS_WORSHIP;
  for (i = 0; i < sizeof(verbs); i++)
    add_action("do_worship", verbs[i]);

  verbs = _LANG_TEMPLE_CMDS_OFFER;
  for (i = 0; i < sizeof(verbs); i++)
    add_action("do_offer", verbs[i]);
}

/** The deity object of the temple, nil when it has none. */
object query_temple_god()
{
  string path;

  path = query_temple_deity();
  return strlen(path) ? load_object(path) : nil;
}

// Why a follower may not use the temple, by what they worship: nil when it is
// their own deity's.
private string follower_refusal(object me, object god)
{
  mixed mine;
  object own;

  mine = me->query_deity_ob();
  if (!stringp(mine) || !strlen(mine))
    return god->fill_text(_LANG_TEMPLE_NOT_FOLLOWER);

  if (mine == query_temple_deity())
    return nil;

  own = load_object(mine);
  return own ? own->fill_text(_LANG_TEMPLE_OTHER_GOD) : _LANG_TEMPLE_NO_GOD;
}

// Why the temple cannot be used at all now, nil when it can.
private string temple_refusal(object god)
{
  if (!god)
    return _LANG_TEMPLE_NO_GOD;
  if (query_temple_closed())
    return god->fill_text(_LANG_TEMPLE_CLOSED_NOW);
  return nil;
}

// Why the player may not worship the temple's deity, nil when they may.
private string worship_refusal(object me, object god)
{
  mixed mine;
  object own;
  string refusal;

  refusal = temple_refusal(god);
  if (refusal)
    return refusal;

  // a deity is chosen once and forever
  mine = me->query_deity_ob();
  if (stringp(mine) && strlen(mine))
  {
    if (mine == query_temple_deity())
      return god->fill_text(_LANG_TEMPLE_ALREADY_YOURS);

    own = load_object(mine);
    return own ? own->fill_text(_LANG_TEMPLE_OTHER_GOD) : _LANG_TEMPLE_NO_GOD;
  }

  return god->query_worship_refusal(me);
}

/** Asks to worship the temple's deity: forever, so it is confirmed first. */
int do_worship(string str)
{
  object me, god;
  string refusal;

  me = this_player();
  if (!me)
    return 0;

  god = query_temple_god();
  refusal = worship_refusal(me, god);
  if (refusal)
  {
    write(refusal);
    return 1;
  }

  write(god->fill_text(_LANG_TEMPLE_WORSHIP_CONFIRM));
  input_to("confirm_worship", 0, me, query_temple_deity());
  return 1;
}

/**
 * The answer to the confirmation. Everything is asked again: the player may
 * have walked out, or the temple changed, while they made up their mind.
 */
void confirm_worship(string answer, object me, string path)
{
  object god, place;
  string refusal;

  if (!me)
    return;

  answer = answer ? lower_case(trim(answer)) : "";
  if (member_array(answer, _LANG_TEMPLE_YES) == -1)
  {
    tell_object(me, _LANG_TEMPLE_WORSHIP_CANCELLED);
    return;
  }

  place = query_temple_place();
  god = query_temple_god();
  if (!place || environment(me) != place || path != query_temple_deity())
  {
    tell_object(me, _LANG_TEMPLE_WORSHIP_CANCELLED);
    return;
  }

  refusal = worship_refusal(me, god);
  if (refusal)
  {
    tell_object(me, refusal);
    return;
  }

  me->set_deity_ob(path);
  if (me->query_deity_ob() != path)
  {
    tell_object(me, god->fill_text(_LANG_TEMPLE_WORSHIP_FAILED));
    return;
  }

  handler("deities", place)->record_worship(me, path);
  log_file("deities", "[" + ctime(time(), 4) + "] " + me->query_name() +
           " worships " + path + "\n");

  tell_object(me, god->fill_text(_LANG_TEMPLE_WORSHIP_DONE));
  tell_room(place, replace_string(god->fill_text(_LANG_TEMPLE_WORSHIP_ROOM),
            "$who$", me->query_cap_name()), ({ me }));
  me->save();
}

// How many base coins some words name: "<n>" or "<n> coins" are base coins,
// "<n> <coin type>" that many of the type. 0 when they name none.
private int coins_named(string str)
{
  string type;
  object money;
  mixed * values;
  int n;

  if (sscanf(str, "%d %s", n, type) == 2)
  {
    type = lower_case(trim(type));
    if (member_array(type, _LANG_TEMPLE_COIN_WORDS) != -1)
      return n;

    money = handler("money", query_temple_place());
    values = money ? money->query_values() : ({ });
    if (member_array(type, values) != -1)
      return n * (int)money->query_value(type);

    return 0;
  }

  if (sscanf(str, "%d", n) == 1 && "" + n == trim(str))
    return n;

  return 0;
}

// An offering of coins: the whole amount, or nothing.
private int offer_coins(object me, object god, int amount)
{
  mapping rule;
  object place;
  int worth;

  rule = god->query_coin_rule();
  if (!rule)
  {
    write(god->fill_text(_LANG_TEMPLE_OFFER_NO_COINS));
    return 1;
  }

  if (amount <= 0)
  {
    write(_LANG_TEMPLE_OFFER_USAGE);
    return 1;
  }

  if ((int)me->query_value() < amount)
  {
    write(_LANG_TEMPLE_OFFER_NO_MONEY);
    return 1;
  }

  place = query_temple_place();
  me->pay_money(handler("money", place)->create_money_array(amount));

  worth = god->query_offering_worth(rule, amount);
  handler("deities", place)->record_offering(me, query_temple_deity(), worth);

  write(god->query_offering_reply(rule));
  tell_room(place, replace_string(_LANG_TEMPLE_OFFER_COINS_ROOM, "$who$",
            me->query_cap_name()), ({ me }));
  return 1;
}

// What the words name: something carried, or a corpse lying in the temple.
private object offering_named(string str, object me)
{
  mixed found;
  int i;

  found = find_match(str, me);
  if (arrayp(found) && sizeof(found))
    return found[0];

  found = query_temple_place() ? find_match(str, query_temple_place()) : nil;
  if (arrayp(found))
    for (i = 0; i < sizeof(found); i++)
      if (objectp(found[i]) && found[i]->query_corpse())
        return found[i];

  return nil;
}

/** Makes an offering to the temple's deity: an item carried, a corpse, or coins. */
int do_offer(string str)
{
  object me, god, item, place;
  object * inv;
  mapping rule;
  string refusal, short;
  int worth, i;

  me = this_player();
  if (!me)
    return 0;

  if (!str || !strlen(trim(str)))
  {
    notify_fail(_LANG_TEMPLE_OFFER_USAGE);
    return 0;
  }

  god = query_temple_god();
  refusal = temple_refusal(god);
  if (!refusal)
    refusal = follower_refusal(me, god);
  if (refusal)
  {
    write(refusal);
    return 1;
  }

  if (coins_named(str) > 0)
    return offer_coins(me, god, coins_named(str));

  item = offering_named(str, me);
  if (!item)
  {
    write(_LANG_TEMPLE_OFFER_NOTHING);
    return 1;
  }

  if (item->query_property("money"))
  {
    write(_LANG_TEMPLE_OFFER_USAGE);
    return 1;
  }

  if (living(item) || item->query_in_use())
  {
    write(_LANG_TEMPLE_OFFER_IN_USE);
    return 1;
  }

  // what the deity does not take stays with the player
  rule = god->query_offering_rule(item);
  if (!rule)
  {
    write(god->fill_text(environment(item) == me ? _LANG_TEMPLE_OFFER_REFUSED :
                         _LANG_TEMPLE_OFFER_REFUSED_HERE));
    return 1;
  }

  place = query_temple_place();

  // what a corpse still holds is not part of the offering: it stays behind
  if (item->query_corpse())
  {
    inv = all_inventory(item);
    for (i = 0; i < sizeof(inv); i++)
      inv[i]->move(place);
  }

  worth = god->query_offering_worth(rule, god->value_of(item));
  short = item->query_short();
  handler("deities", place)->record_offering(me, query_temple_deity(), worth);
  log_file("deities", "[" + ctime(time(), 4) + "] " + me->query_name() +
           " offers " + base_name(item) + " (" + worth + ") to " +
           query_temple_deity() + "\n");
  item->dest_me();

  write(god->query_offering_reply(rule));
  tell_room(place, replace_string(replace_string(_LANG_TEMPLE_OFFER_ITEM_ROOM,
            "$who$", me->query_cap_name()), "$item$", short ? short : ""),
            ({ me }));
  return 1;
}

mixed * stats()
{
  return ({
    ({ "Temple Deity", strlen(query_temple_deity()) ? query_temple_deity() : "none" }),
  });
}
