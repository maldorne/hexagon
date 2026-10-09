// crafts.c -- which objects make things to order.
//
// The crafts command and the room listing both need to know, for anything in a
// room, which object holds its crafter code: the component an npc carries, or
// the thing itself when it inherits /lib/crafts/crafter.c.

inherit "/lib/core/object.c";

object crafter_of(object ob)
{
  object component;

  if (!ob)
    return nil;

  component = ob->query_component_by_type("crafter");
  if (component)
    return component;

  if (ob->query_crafter())
    return ob;

  return nil;
}
