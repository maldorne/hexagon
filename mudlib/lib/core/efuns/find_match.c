
/*
 * If anyone can tell me what this does...
 * I would be most apprecative, Pinkfish... Yes yes ok i did write it.
 *
 * muhahaha filtering out parse stuff...Raskolnikov Nov 96
 * If you're going to fix this, might as get rid of the
 *   "him", "her", and "it" shit too - Radix
 *   Flode - 240897
 * Made find_match varargs with an optional no_hidden -flag
 *   If the flag is set, hidden players, hidden npc's and basically
 *   any object returning 1 on query_hidden are filtered out.
 *
 * Checks about it, him, her, them removed, neverbot 4/03
 * The hidden flag now ignores invisible coders too, neverbot 2/06
 * It also ignores objects with query_hidden_object (like doors)
 *
 * some errors fixed, add matches with numbers, like 'get sword 2'
 *   neverbot 09/2021
 */

int not_hidden(object ob, varargs mixed extra...)
{
  return (objectp(ob) &&
         !ob->query_hide_shadow() &&
         !ob->query_invis() &&
         !ob->query_hidden_object() &&
         !(ob->is_money() && ob->query_number_coins() == 0) );
}

static mixed find_match(string str, mixed ob, varargs int no_hidden)
{
  object * list;
  string * id_list;
  string * id_list_plurals;
  object * result;
  string * bits;
  int i, j;
  string aux;

  list = ({ });
  id_list = ({ });
  id_list_plurals = ({ });
  result = ({ });

  if (!ob || intp(ob))
    return ({ });

  if (!strlen(str))
    return ({ });

  if (stringp(ob))
  {
    ob->dummy();
    ob = find_object(ob);
  }

  if (!pointerp(ob))
  {
    list = (object *)ob->find_inv_match(str);
    if (!list || !sizeof(list))
      return ({ });
  }
  else
  {
    for (i = 0; i < sizeof(ob); i++)
      list += (object *)ob[i]->find_inv_match(str);
    list -= ({ nil });

    if (!sizeof(list))
      return ({ });
  }

  if (no_hidden)
    list = filter_array(list, "not_hidden", this_object());

  bits = explode(implode(explode(str, " " + _LANG_AND + " "), ","), ",");

  for (j = 0; j < sizeof(bits); j++)
  {
    string what;
    int num;

    aux = trim(bits[j]);
    num = 0;

    // if we are looking for "sword 2"
    if (sscanf(aux, "%s %d", what, num) >= 2)
      aux = what;

    if (this_player())
      aux = this_player()->expand_nickname(aux);

    for (i = 0; i < sizeof(list); i++)
    {
      id_list = ({ list[i]->query_name(), list[i]->query_short(), }) +
                  (list[i]->query_alias() ? list[i]->query_alias() : ({ }));

      id_list_plurals = ({ list[i]->query_main_plural(), }) +
                          (list[i]->query_plurals() ? list[i]->query_plurals() : ({ }));

      if ((member_array(aux, id_list) != -1) && (--num <= 0))
      {
        result += ({ list[i] });
        break;
      }
      else if (member_array(aux, id_list_plurals) != -1)
      {  
        result += ({ list[i] });
      }
      else if (list[i]->query_parse_id( ({ 0, aux }) ))
      {
        result += ({ list[i] });
        break;
      }
    }
  }

  return result;
}

// objective search functions, originally in the spell system

static mixed find_unique_match(string find, mixed inx, varargs int no_ghost)
{
  mixed *olist;
  int i, j;
  olist = find_match(find, inx, 1);

  if (no_ghost)
    for (i = sizeof(olist) - 1; i >= 0; i--)
      if (olist[i]->query_dead())
        olist = delete(olist, i, 1);

  for (i = 0; i < sizeof(olist); i++)
    for (j = sizeof(olist)-1; j > i; j--)
      if (olist[j] == olist[i])
        olist = delete(olist,j,1);

  return olist;
}

static mixed find_one_match(string find, mixed inx, varargs int no_ghost)
{
  mixed *olist;
  int i;
  olist = find_match(find, inx, 1);

  if (no_ghost)
    for(i = sizeof(olist) - 1; i >= 0; i--)
      if (olist[i]->query_dead())
        olist = delete(olist, i, 1);

  if ( sizeof(olist) < 1 )
    return 0;
  else
    return olist[0];
}
