// drops.c -- what an NPC leaves behind when it dies, as an NPC component.
//
// Things it does not carry in life (so it never wears or wields them) but that
// turn up when it falls: cloned onto the body, to end up in the corpse with the
// rest of what it carries, or onto the ground where it died. An optional line
// tells the room about it.
//
// Attributes:
//   "items"   - the files to clone, one each
//   "random"  - ([ file : weight ]): one more of them, drawn at random, a
//               file of weight 3 turning up three times as often as one of 1
//   "where"   - "body" (the default) or "room"
//   "message" - what the room sees as they turn up (optional)

inherit component "/lib/npc/component.c";

#define DROPS_ON_BODY "body"
#define DROPS_ON_ROOM "room"

private string * drop_items;
private mapping drop_random;
private string drop_where;
private string drop_message;

void create()
{
  component::create();
  drop_items = ({ });
  drop_random = ([ ]);
  drop_where = DROPS_ON_BODY;
  drop_message = nil;
}

/**
 * One file of the random list, drawn by weight; nil when the list is empty.
 */
private string _draw_random()
{
  string * files;
  int i, total, roll;

  if (!drop_random || !map_sizeof(drop_random))
    return nil;

  files = map_indices(drop_random);
  total = 0;
  for (i = 0; i < sizeof(files); i++)
    if (intp(drop_random[files[i]]) && drop_random[files[i]] > 0)
      total += drop_random[files[i]];
  if (!total)
    return nil;

  roll = random(total);
  for (i = 0; i < sizeof(files); i++)
  {
    if (!intp(drop_random[files[i]]) || drop_random[files[i]] <= 0)
      continue;
    roll -= drop_random[files[i]];
    if (roll < 0)
      return files[i];
  }

  return nil;
}

// The NPC is dying: its body is still in the room.
void died(object killer)
{
  object npc, env, ob;
  string * files;
  string drawn;
  int i;

  npc = query_entity();
  if (!npc || !(env = environment(npc)))
    return;

  files = ({ }) + drop_items;
  drawn = _draw_random();
  if (drawn)
    files += ({ drawn });
  if (!sizeof(files))
    return;

  if (drop_message)
    tell_room(env, drop_message);

  for (i = 0; i < sizeof(files); i++)
  {
    ob = clone_object(files[i]);
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
         ([ "items" : drop_items, "random" : drop_random,
            "where" : drop_where ]) +
         (drop_message ? ([ "message" : drop_message ]) : ([ ]));
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;
  if (pointerp(args["items"]))
    drop_items = args["items"];
  if (mappingp(args["random"]))
    drop_random = args["random"];
  if (args["where"] == DROPS_ON_ROOM)
    drop_where = DROPS_ON_ROOM;
  if (stringp(args["message"]))
    drop_message = args["message"];
}

mixed * stats()
{
  return component::stats() + ({ ({ "Drops", drop_items }),
                                 ({ "Random drops", drop_random }),
                                 ({ "Where", drop_where }) });
}
