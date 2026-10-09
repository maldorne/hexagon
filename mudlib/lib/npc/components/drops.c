// drops.c -- what an NPC leaves behind when it dies, as an NPC component.
//
// Things it does not carry in life (so it never wears or wields them) but that
// turn up when it falls: cloned onto the body, to end up in the corpse with the
// rest of what it carries, or onto the ground where it died. An optional line
// tells the room about it.
//
// Attributes:
//   "items"   - the files to clone, one each
//   "where"   - "body" (the default) or "room"
//   "message" - what the room sees as they turn up (optional)

inherit component "/lib/npc/component.c";

#define DROPS_ON_BODY "body"
#define DROPS_ON_ROOM "room"

private string * drop_items;
private string drop_where;
private string drop_message;

void create()
{
  component::create();
  drop_items = ({ });
  drop_where = DROPS_ON_BODY;
  drop_message = nil;
}

// The NPC is dying: its body is still in the room.
void died(mixed * args)
{
  object npc, env, ob;
  int i;

  npc = query_owner();
  if (!npc || !(env = environment(npc)))
    return;

  if (drop_message)
    tell_room(env, drop_message);

  for (i = 0; i < sizeof(drop_items); i++)
  {
    ob = clone_object(drop_items[i]);
    if (!ob)
      continue;

    // a body that cannot take it lets it fall where it stands
    if (drop_where == DROPS_ON_ROOM || ob->move(npc))
      ob->move(env);
  }
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "items" : drop_items, "where" : drop_where ]) +
         (drop_message ? ([ "message" : drop_message ]) : ([ ]));
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;
  if (pointerp(args["items"]))
    drop_items = args["items"];
  if (args["where"] == DROPS_ON_ROOM)
    drop_where = DROPS_ON_ROOM;
  if (stringp(args["message"]))
    drop_message = args["message"];
}

mixed * stats()
{
  return component::stats() + ({ ({ "Drops", drop_items }),
                                 ({ "Where", drop_where }) });
}
