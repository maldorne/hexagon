
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
 */

// static mixed *rest;

// string *parse_blocks = ({
//   "thing",
//   "things",
// });

int not_hidden(object ob, varargs mixed extra...) 
{
  return (ob && 
         !ob->query_hide_shadow() && 
         !ob->query_invis() && 
         !ob->query_hidden_object() && 
         !(ob->is_money() && ob->query_number_coins() == 0) ); 
}

/*
static int local_atoi(string str)
{
  int x;

  if (!stringp(str)) 
    return 0;
  else sscanf(str, "%d", x);
    return x;
}

static int remove_ints(string s)
{
  return (local_atoi(s) == 0);
}
*/

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
    list -= ({ 0 });

    if (!sizeof(list))
      return ({ });
  }

  if (no_hidden)
    list = filter(list, "not_hidden", this_object());    

  bits = explode(implode(explode(str, " y "), ","), ",");
  
  for (j = 0; j < sizeof(bits); j++) 
  {
    aux = bits[j];

    if (this_player())
      aux = this_player()->expand_nickname(bits[j]);

    for (i = 0; i < sizeof(list); i++)
    {
      id_list = ({ list[i]->query_name(), list[i]->query_short(), }) + 
                  (list[i]->query_alias() ? list[i]->query_alias() : ({ }));

      id_list_plurals = ({ list[i]->query_main_plural(), }) +
                          (list[i]->query_plurals() ? list[i]->query_plurals() : ({ }));

      if (member_array(aux, id_list) != -1)
        result += ({ list[i] });

      else if (member_array(aux, id_list_plurals) != -1)
        result += ({ list[i] });

      else if (list[i]->query_parse_id( ({ 0, aux}) ))
        result += ({ list[i] });
    } 
  }

  return result;    

  /* 
  mixed *arr, test, *ret;
  int i, bing, j;
  // int num, top, bot;
  string nick, *bits;
  // string type; 
  mapping rabbit;

  string * id_list_common;
  string * id_list_common_plurals; 

  id_list_common = this_object()->parse_command_id_list();
  id_list_common_plurals = this_object()->parse_command_plural_id_list();

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
    ob = (mixed)ob->find_inv_match(str);
    if (!ob)
      return ({ });
  }
  else 
  {
    arr = ({ });
    for (i = 0; i < sizeof(ob); i++)
      if ((test = (object *)ob[i]->find_inv_match(str)))
        arr += test;
    ob = arr;
  }
  
  if (no_hidden)
    ob = filter(ob, "not_hidden", this_object());

  bits = explode(implode(explode(str, " y "), ","), ",");
  ret = ({ });
  
  for (j = 0; j < sizeof(bits); j++) 
  {











    str = bits[j];
    nick = (string)this_object()->expand_nickname(str);

    if (!nick && this_player())
      nick = (string)this_player()->expand_nickname(str);

    if (nick && nick!="")
      str = nick;

    test = explode(str, " ");
    
    if (!sizeof(test)) 
      test = ({ "" });
      
    sscanf(test[sizeof(test)-1], "%d", bing);

    test = ({ });
    rest = ({ });
    
    // Elementos por partes (2/3 de algo) eliminados
    // if (sscanf(str, "%d/%d de %s", top, bot, str) != 3)
    //     top = bot = 1;
    // if (top <= 0)
    //     top = 1;
    // if (bot < top)
    //     bot = top;
    
    // block parse stuff
    // if (member_array(str, parse_blocks) == -1) 
    // {

    parse_command(str, ob, "%i", test);

    if (!test || !sizeof(test))
      continue;

    // Atencion: comprobacion añadida por Folken, 05/2009
    // parse_command (efun) no responde como deberia... por ejemplo
    // un parse_command("espada larga", lista_objetos, ...) donde en la lista
    // haya espadas largas y cortas, las devuelve todas. Probablemente por dentro
    // separe el primer parametro con un explode("espada larga", " ") y compruebe
    // palabra por palabra sobre la lista de objetos.
    
    // El siguiente if elimina de los resultados devueltos por parse_command aquellos
    // que no responden al string str (bien sea por name, main_plural, alias o plurals)
    
    if ((str != "todo") && (str != "all") && !test[1]->is_money() && !test[1]->is_room_item())
    {
      int p; 
      string * id_list;
      string * id_list_plurals;
      object * filtered = ({ });

      //for(p = 0; p<sizeof(test);p++)
      //    tell_object(find_living("folken"), "test["+p+"] = "+(objectp(test[p])?test[p]->short():test[p])+"\n");

      nick = implode(filter(explode(str, " "), "remove_ints"), " ");
      // tell_object(find_living("folken"), "nick = "+nick+"\n");

      for(p = 1; p < sizeof(test); p++)
      {  
        id_list = ({ test[p]->query_name(), test[p]->query_short(), });
        id_list_plurals = ({ test[p]->query_main_plural(),  });

        if (sizeof(test[p]->query_alias()))
          id_list += test[p]->query_alias();

        if (sizeof(test[p]->query_plurals()))
          id_list_plurals += test[p]->query_plurals();

        if ((member_array(nick, id_list) == -1) && 
            (member_array(nick, id_list_common) == -1) &&
            (member_array(nick, id_list_plurals) == -1) && 
            (member_array(nick, id_list_common_plurals) == -1))

          test[0]--;
        else
          filtered += ({ test[p], });
      }
      
      test = ({ (test[0]<0?0:test[0]) }) + filtered;
      
      // for(p = 0; p<sizeof(test);p++)
      //  tell_object(find_living("folken"), "test["+p+"] = "+(objectp(test[p])?test[p]->short():test[p])+"\n");
    }

    // {
    // int p; 
    // for(p = 0; p<sizeof(test);p++)
    // tell_object(find_living("folken"), "test["+p+"] = "+(objectp(test[p])?test[p]->short():test[p])+"\n");
    // }

    if (test[0] == 1 && bing)
      test[0] = -bing;

    rabbit = ([ ]);

    for (i = 1; i < sizeof(test); i++)
      rabbit[test[i]] = 1;

    // {
    // int p; 
    // for(p = 0; p<sizeof(keys(rabbit));p++)
    // tell_object(find_living("folken"), "keys(rabbit)["+p+"] = "+keys(rabbit)[p]->short()+"\n");
    // }

    arr = map_array(keys(rabbit), 
            "query_simul_efun_id",
            this_object(), 
            ({ test[0], str }));

    arr += rest;
    ret += arr;

    // }
  }
  
  return ret - ({ 0 });
  */
} 

// auxiliar functions
/*
object query_simul_efun_id(object ob, mixed *arr) 
{ 
  mixed ret;
  ret = (mixed)ob->query_parse_id(arr);

  if (!pointerp(ret))
    return ret;

  rest += ret[1..sizeof(ret)];
  return ret[0];
} 
*/

/*
object query_frac_simul_efun_id(object ob, mixed *arr) 
{
  mixed ret;
  ret = (mixed)ob->query_frac_parse_id(arr);
  if (!pointerp(ret))
      return ret;
  rest += ret[1..sizeof(ret)];
  return ret[0];
} 
*/

// objective search functions, originally in the spell system

mixed find_unique_match(string find, mixed inx, varargs int no_ghost) 
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

mixed find_one_match(string find, mixed inx, varargs int no_ghost) 
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
