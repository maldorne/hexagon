// vehicle.c -- an item that can be ridden or driven: a cart, a motorbike, a
// boat. It carries the same spec an NPC steed carries in its rideable component
// (the keys of <living/riding.h>, over the row of its kind in the steeds
// table) and answers query_rideable() with it; the riding handler
// (/lib/handlers/riding.c) does the rest. What inherits it gives the name and
// the description and calls set_rideable in setup:
//
//     set_rideable(([ "kind" : "motorbike", "seats" : 2 ]));
//
// A game may fill the hooks the handler calls on a steed: steed_can_board(who,
// seat) (a refusal written for the rider, or nil), steed_boarded(who) and
// steed_dismounted(who).

#include <living/riding.h>

inherit "/lib/item.c";

private mapping spec;

void create()
{
  spec = ([ ]);
  ::create();

  // a vehicle is not carried away
  reset_get();
}

/**
 * Gives the vehicle its spec: the kind and whatever differs from it.
 */
void set_rideable(mapping data) { spec = data; }

/**
 * The spec this vehicle carries on its own, over its kind's row. Nil would
 * mean it cannot be ridden; a vehicle always can.
 */
mapping query_rideable() { return spec; }

void init()
{
  mapping row;

  ::init();

  row = handler("riding", this_object())->query_spec_of(this_object(), spec);
  if (!row)
    return;

  add_action("do_board", row["verbs_board"]);
  add_action("do_dismount", row["verbs_dismount"]);
}

/**
 * The board verb: it answers when what follows names this vehicle.
 */
int do_board(string str)
{
  object who;
  string refusal;

  who = this_player();
  if (!who || environment(who) != environment(this_object()))
    return 0;

  if (!handler("riding", this_object())->query_addressed(this_object(), str))
    return 0;

  refusal = handler("riding", this_object())->query_board_refusal(who, this_object(), -1);
  if (refusal)
  {
    notify_fail(refusal);
    return 0;
  }

  return handler("riding", this_object())->board(who, this_object());
}

/**
 * The dismount verb answers only for the vehicle the rider is on.
 */
int do_dismount(string str)
{
  object who;

  who = this_player();
  if (!who || who->query_mount() != this_object())
    return 0;

  return handler("riding", this_object())->dismount(who);
}

// With somebody on it, the vehicle does not show: its riders do.
string pretty_short(varargs int dark)
{
  if (sizeof(handler("riding", this_object())->query_riders(this_object())))
    return "";

  return ::pretty_short(dark);
}

// Moved away from its riders: they fall, unless they are the ones driving it.
void event_moved(object ob, object from, object dest)
{
  ::event_moved(ob, from, dest);
  handler("riding", this_object())->steed_moved(this_object(), from, dest);
}

void dest_me()
{
  if (environment())
    handler("riding", this_object())->unseat_riders(this_object());

  ::dest_me();
}

mapping query_auto_load_attributes()
{
  return ([ "::" : ::query_auto_load_attributes() ]) +
      (map_sizeof(spec) ? ([ "rideable" : spec ]) : ([ ]));
}

void init_auto_load_attributes(mapping attribute_map)
{
  if (!undefinedp(attribute_map["rideable"]))
    spec = attribute_map["rideable"];
  if (!undefinedp(attribute_map["::"]))
    ::init_auto_load_attributes(attribute_map["::"]);
}
