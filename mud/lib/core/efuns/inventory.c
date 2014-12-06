
// Non-dgd efuns
// neverbot, 03/2014

object * all_inventory(object ob)
{
  return ob->query_inventory();
}

// TODO deep_inventory

object * deep_inventory(object ob)
{
  object * result;
  object * pending;
  int i;

  pending = all_inventory(ob);

  // recursively check the inventory of all
  // the items
  while(sizeof(pending) > 0)
  {
    object current;
    current = pending[0];

    result += ({ current });
    pending -= ({ current });
    pending += all_inventory(current);
  }

  return result;
}

mixed query_strange_inventory(mixed arr) 
{
  mixed inv;
  object ob;
  mixed desc;
  int i,k;

  inv = ({ });
  for (k = 0; k < sizeof(arr); k++) 
  {
    ob = arr[k];

    if (!(desc = (string)ob->short()) || (desc == ""))
      continue;

    if ((i = member_array(desc, inv)) >= 0)
      inv[i+1] += ({ ob });
    else
      inv += ({ desc, ({ ob }) });
  }

  return inv;
}

