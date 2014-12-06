
// Non-dgd stuff
// neverbot, 12/2014

private static object _environment;
private static object * _inventory;

void create()
{
  _environment = nil;
  _inventory = ({ });
}

nomask object * query_inventory()
{
  return _inventory;
}

nomask object query_environment()
{
  return _environment;
}

// can add object ob to our inventory?
int test_add(object ob)
{
  return 1;
}

// can remove object ob from our inventory?
int test_remove(object ob)
{
  // if item not in inventory
  if (member_array(ob, _inventory) == -1)
    return -1;

  return 1;
}

static nomask int add_to_inventory(object ob)
{
  if (member_array(ob, _inventory) != -1)
    return -1;

  _inventory += ({ ob });

  return 1;
}

static nomask int remove_from_inventory(object ob)
{
  // if item not in inventory
  if (member_array(ob, _inventory) == -1)
    return -1;

  _inventory -= ({ ob });

  return 1;
}

static nomask int update_environment(object destination)
{
  _environment = destination;
  return 1;
}
