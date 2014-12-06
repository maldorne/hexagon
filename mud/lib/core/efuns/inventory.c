
// Non-dgd efuns
// neverbot, 03/2014

// TODO all_inventory

object * all_inventory(mixed obs)
{
  if (pointerp(obs))
    return ({ });
  else
    return ({ });
}

// TODO deep_inventory

object * deep_inventory(mixed obs)
{
  return all_inventory(obs);
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

