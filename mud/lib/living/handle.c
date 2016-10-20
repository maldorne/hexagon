/*
 * has all the give/get stuff.
 * Fixed so that the objects are unworn/unheld..
 * Baldrick..
 * Revisado para Cc, neverbot 4/03
 * Añadido consentimiento para dar objetos, neverbot 05/09
 */

#include <basic/move.h>
#include <common/properties.h>
#include <basic/money.h>
#include <user/player.h>
#include <living/consents.h>
#include <living/equip.h>

#define TIME_DIV 10

void handle_commands() 
{
  // add_action("take", ({ "get", "take", }));
  add_action("take", "coger");

  // add_action("give", "put");
  add_action("give", ({ "poner", "dar", }));

  // add_action("drop_ob", "drop");
  add_action("drop_ob", "dejar");
}

void create()
{
}

/* this will work as give and put...
 */
int give(string str, varargs string verb, object *bing, string bing2, int blue) 
{
  string person, thing, sh, con;
  int i, j, tot, max;

  object *obs, *per, *ret, *fail;

  if (this_object()->query_timed_property_exists(PASSED_OUT_PROP))
  {
    notify_fail(this_object()->query_timed_property(PASSED_OUT_PROP));
    return 0;
  }

  if (!strlen(str) && !verb)  
  {
    notify_fail("Sintaxis: "+query_verb()+" <objeto> <a/en> <objeto>\n");
    return 0;
  }
  
  if (query_verb() == "poner")
    con = " en ";
  else
    con = " a ";
  if (bing) 
  {
    obs = bing;
    thing = bing2;
    j = blue;
  } 
  else 
  {
    if (sscanf(str, "%s a %s", thing, person) != 2)
      if (sscanf(str, "%s en %s", thing, person) != 2) 
      {
        notify_fail("Sintaxis: "+query_verb()+" <objeto> <a/en> <objeto>\n");
        return 0;
      }
      obs = find_match(person, ({ this_object(), environment() }), 1);
      if (!sizeof(obs)) 
      {
        // notify_fail("No puedo encontrar "+person+" para "+query_verb()+"le nada.\n");
        notify_fail("No se puede encontrar "+person+".\n");
        return 0;
      }
    per = obs;
  }
  
  /* should check to see that we can give things to this person/thing... */
  if (!per) 
    per = ({ });

  for (; j < sizeof(per); j++) 
  {
    if (per[j] == this_object())
      continue;

  if (per[j]->query_player() || per[j]->query_npc())
    if (!per[j]->query_consent(GIVE_CONS))
    {
      tell_object(this_object(), per[j]->query_cap_name() + 
        " no quiere que le den nada.\n");
      tell_object(per[j], this_object()->query_cap_name() + 
        " ha intentado darte algo, pero no te interesa aceptar cosas.\n" +
        "\t(escribe 'consentir dar sí' si quieres cambiarlo).\n");
      continue;
    }

    obs = find_match(thing, this_object(), 1);
    obs -= ({ per[j] });

    if (!sizeof(obs)) 
    {
      write("Nada que "+query_verb()+con+per[j]->short()+".\n");
      continue;
    }

    ret = ({ });
    fail = ({ });

    for ( i = 0; i < sizeof(obs);i++)
    {
      if (!obs[i]->query_property(CURSED_PROP))
      {
        if (obs[i]->query_in_use() && obs[i]->query_holdable() )
          this_object()->unhold_ob(obs[i]);

        if ( obs[i]->query_in_use() && obs[i]->query_wearable() )
          this_object()->unwear_ob(obs[i]);
      }
      
      if ( obs[i]->query_in_use() || obs[i]->query_property(CURSED_PROP) )
      {
        fail += ({ obs[i] });
      }
      else if ( ((int)obs[i]->move(per[j])) == MOVE_OK )
      {
        if (!interactive(per[j]) && interactive(this_object())) 
        {
          obs[i]->add_static_property(PC_GAVE_NPC_PROPERTY, 1);
        }
        ret += ({ obs[i] });
        tot += (int)obs[i]->query_weight();
        
      } /* if (((.. */
      else
        fail += ({ obs[i] });
    }

    if (sizeof(ret)) 
    {   
      sh = query_multiple_short(ret, 1); // flag 1 para usar pretty_short 
      tell_object(this_object(), ((query_verb()=="dar")?"Das ":"Pones ")+sh+con+
        (string)per[j]->short()+".\n");
      tell_room(environment(this_object()), capitalize((string)this_object()->short())+
        " "+((query_verb()=="dar")?"da ":"pone ")+
        sh+con+(string)per[j]->short()+".\n", ({ per[j] }) );
      tell_object(per[j], capitalize((string)this_object()->short())+
        " te da "+sh+".\n");

      max = (int)per[j]->query_max_weight();

      if (max > 0)
      {
        if (tot > max) 
          tot = max;
        max = (tot * 100)/max;

        switch(max)
        {
          case 0..25:     
            break;
          case 95..100:
            tell_room(environment(),
              per[j]->short()+" se tambalea al recibir tanto peso.\n", ({per[j] }) );
            tell_object(per[j], 
              "Te hundes bajo un peso que apenas puedes cargar.\n");
            break;
          default:
            tell_room(environment(), per[j]->short()+ ({
              " está incomodado por la carga adicional.\n",
              " sucumbe momentáneamente bajo el peso de la carga.\n",
              " se tambalea mientras "+per[j]->query_pronoun()+" coge la carga.\n"
              })[(max/25)-1], ({ per[j] }) );
            tell_object(per[j], ({
              "Durante unos momentos te incomoda la carga adicional.\n",
              "Sucumbes momentáneamente bajo el peso de la carga.\n",
              "Te tambaleas al coger la carga. A duras penas puedes con ella.\n"
              })[(max/25)-1]);
        }

        this_object()->add_timed_property(PASSED_OUT_PROP, 
          "Todavía estas intentando coger la carga.\n", max/TIME_DIV);
      }
    }

    if (sizeof(fail))
      write("No puedes "+query_verb()+" "+query_multiple_short(fail, 1)+
        con+per[j]->short()+".\n");
  }
  return 1;
}

int drop_ob(string str)
{
  int i, num;
  object *ob, *ret, *fail;
  string sh;

  if (this_object()->query_timed_property_exists(PASSED_OUT_PROP))
  {
    notify_fail(this_object()->query_timed_property(PASSED_OUT_PROP));
    return 0;
  }

  if (!strlen(str)) 
  {
    notify_fail("Sintaxis: "+query_verb()+" <objeto(s)>\n");
    return 0;
  }
  
  if (this_object()->query_property(LOADING_PROP))
  {
    notify_fail("Tu equipo todavía se está cargando, así que es un poco difícil "+
      "que puedas dejar nada.\n");
    return 0;
  }

  ob = find_match(str, this_object(), 1);
  if (!sizeof(ob)) 
  {
    notify_fail("No ves ningún "+str+" que dejar.\n");
    return 0;
  }

  ret = ({ });
  fail = ({ });

  for (i = 0; i < sizeof(ob); i++) 
  {
    if (!ob[i]->short())
      continue;

    /* Add by Baldrick, so the players will unhold the object they drop..
     * This method is *NOT* good, but faster than the hard way.. 
     * but we should do it the hard way, later. 
     */
    if (!ob[i]->query_property(CURSED_PROP))
    {
      if (ob[i]->query_in_use() && ob[i]->query_holdable() )
        this_object()->unhold_ob(ob[i]);

      if ( ob[i]->query_in_use() && ob[i]->query_wearable() )
        this_object()->unwear_ob(ob[i]);
    }

    if ( !ob[i]->query_property(CURSED_PROP) && 
         !ob[i]->query_in_use() &&
        ob[i]->move(environment()) == MOVE_OK)
      ret += ({ ob[i] });
    else
      fail += ({ ob[i] });
  }

  if (sizeof(ret)) 
  {
    // hack: si haces un dejar 2 platinos,3 cobres, el primer objeto money ya 
    // lleva la descripcion de todas las monedas que estas dejando (los objetos money
    // se van acumulando en uno solo)
    
    int j, has_money;
    object * new_ret;

    new_ret = ({ });
    has_money = 0;
    
    for (j = 0; j < sizeof(ret); j++)
    {
      // Filtramos los objetos money excepto el primero
      if (ret[j])
        if (ret[j]->is_money())
        {
          if (!has_money)
            new_ret += ({ ret [j] });
          {
            has_money = 1;
          }
        }
        else
          new_ret += ({ ret [j] });
    }
    
    sh = query_multiple_short(new_ret, 1);

    if (!this_object()->query_hidden())
      say(capitalize((string)this_object()->short())+" deja "+sh+".\n");

    write("Dejas "+sh+".\n");
  }

  if (sizeof(fail))
    write("No puedes dejar "+query_multiple_short(fail, 1)+".\n");

  num += sizeof(fail)+sizeof(ret);

  if (!num) 
  {
    notify_fail("No puedes dejar "+str+".\n");
    return 0;
  }
  return 1;
}

int take(string str, varargs string verb, object *bing, string bing2, int blue) 
{
  object *dest, *obs, *fail, *ret_a;
  mixed *ret;
  string s2, sh, str2, aux;
  int i, num, j, cap, perc, we, tot, max;

  /* Added to get rid of one more auto_load bug..
   * Baldrick, nov '94
   */
  if (this_object()->query_loading() ||
      this_object()->query_property(LOADING_PROP))
  {
    notify_fail("Tienes que esperar a que todo tu equipo esté cargado.\n");
    return 0;
  }

  if (this_object()->query_timed_property_exists(PASSED_OUT_PROP))
  {
    notify_fail(this_object()->query_timed_property(PASSED_OUT_PROP));
    return 0;
  }

  if (this_object()->query_dead())
  {
    notify_fail("Tu estado etéreo tiene poco efecto en el mundo material.\n ");
    return 0;
  }

  if (!strlen(str) && !verb) 
  {
    notify_fail("Sintaxis: "+query_verb()+" <objeto> [de <objeto>]\n");
    return 0;
  }
  
  cap = (int)this_object()->query_max_weight()+1;
  if (verb) 
  {
    dest = bing;
    i = blue;
    str = bing2;
  } 
  else 
  {
    dest = ({ environment() });

    /* Caso 1 */
    if (sscanf(str,"%smonedas de %s de %s",aux,str2,s2) == 3)
    {
      dest = find_match(s2, ({environment(), this_object() }), 1);
      str = aux+"monedas de "+str2;
      if (!sizeof(dest))
      {
        notify_fail("No encuentras ningún "+s2+" por aquí.\n");
        return 0;
      }
    }
    /* Caso 2 */
    else
    {
      if (sscanf(str,"%smonedas de %s",aux,s2) == 2 )  
      {
        if (member_array(s2, MONEY_HAND->query_values()) == -1)
        {
          dest = find_match(s2, ({ environment(), this_object() }), 1);
          str = aux+"monedas";
          if (!sizeof(dest))
          {
            notify_fail("No encuentras ningún "+s2+" por aquí.\n");
            return 0;
          }
        }
      }
      else
      {     
        if (sscanf(str, "%s de %s", str, s2) == 2) 
        {
          /* since we only allow one lvl anyway... (*/
          dest = find_match(s2, ({ environment(), this_object() }), 1);
          if (!sizeof(dest)) 
          {
            notify_fail("No encuentras ningún "+s2+" por aquí.\n");
            return 0;
          }
        }
      }
    }
    i = 0;
  }

  for (;i<sizeof(dest);i++) 
  {
    if (dest[i]->cannot_get_stuff())
      continue;
    
    obs = find_match(str, dest[i], 1);
    ret = ({ ({ }), ({ }), ({ }), ({ }), ({ }), });
    ret_a = ({ });
    fail = ({ });
    tot = 0;

    for (j = 0; j < sizeof(obs); j++)
      if (obs[j]->move(this_object()) == MOVE_OK) 
      {
        if ((perc = (we = (int)obs[j]->query_weight())*100/cap) >= 95)
          ret[4] += ({ obs[j] });
        else
          ret[perc/25] += ({ obs[j] });
        tot += we;
        ret_a += ({ obs[j] });
      } 
      else if (!living(obs[j]))
        fail += ({ obs[j] });

    for (j = 0; j < sizeof(ret); j++)
      if (sizeof(ret[j])) 
      {
        sh = query_multiple_short(ret[j], 1);

        write(({ "Coges", "Coges con un poco de dificultad",
          "Te las arreglas como puedes para coger",
          "Te resulta muy difícil coger",
          "Usas toda tu fuerza y a duras penas logras coger" })[j]+
          " "+sh+" de "+(string)dest[i]->short()+".\n");

        if (!this_object()->query_hidden())
          say((string)this_object()->query_cap_name()+" "+
            ({ "coge", "coge con un poco de dificultad",
            "se las arregla como puede para coger",
            "tiene muchas dificultades para coger",
            "usa toda su fuerza y a duras penas logra coger" })[j]+
            " "+sh+" de "+(string)dest[i]->short()+".\n");

        num += sizeof(ret[j]);
      }

    if (tot) 
    {
    /*
    this_object()->adjust_time_left(-tot/TIME_DIV);
    if (this_object()->query_time_left() < 0) {
      this_object()->set_interupt_command("get_put_interupt", this_object(),
      ({ tot, ret_a, "Coges", "de", dest, i, str }));
    }
    */
      max = (int)this_object()->query_max_weight();
      if (max && tot)
      {
        if (tot > max) 
          tot = max;
        tot = (tot * 100)/max;
        this_object()->add_timed_property(PASSED_OUT_PROP, 
          "Aún estas ocupado con la carga.\n", tot/TIME_DIV);
      }
    }
    
    if (sizeof(fail))
      write("No puedes "+query_verb()+" "+query_multiple_short(fail, 1)+".\n");
    num += sizeof(fail);
  }

  if (!num) 
  {
    notify_fail("Nada que "+query_verb()+".\n");
    return 0;
  }
  
  return 1;
}

/*
void get_put_interupt(int left, mixed arg) {
  int tot, j, i, weight;
  object *obs, *per;
  string sh, thing, sh2;

  per = arg[4];
  j = arg[5];
  thing = arg[6];
  obs = arg[1];
  if (left > 0) {
  object dest;
  // We have been stopped!  How rude :)
  tot = arg[0] - left; // This is how much they pick up
  if (arg[2] != "coger")
    dest = this_object();
  else
    dest = per[j];

  for (i=0;i<sizeof(obs);i++) {
    weight = (int)obs[i]->query_weight();
    if (tot - weight < 0) {
  if (!i)
    sh = "none of the above";
  else
    sh = query_multiple_short(obs[0..i-1]);
  sh2 = query_multiple_short(obs);
  for (;i<sizeof(obs);i++)
    obs[i]->move(dest);
  write("Your interupted "+arg[2]+"ing "+sh2+" "+arg[3]+" "+
    per[j]->short()+", only "+arg[2]+"ing "+sh+".\n");
  say(this_object()->query_cap_name()+" is interupted "+arg[2]+"ing "+
    sh2+" "+arg[3]+" "+per[j]->short()+", only "+arg[2]+"ing "+sh+".\n");
  return ;
    }
    tot -= weight;
  }
  write("Hmmm...  here,,,?\n");
  return ;
  }
  if (j < sizeof(per)) {
  // Keep going... 
  switch (arg[2]) {
  case "give" :
  case "dar" :
  case "poner":
  case "put" :
    give(0, arg[2], per, thing, j+1);
    break;
  case "get" :
  case "coger":
    take(0, arg[2], per, thing, j+1);
    break;
  }
  }
}
*/

mixed * stats() 
{
  return ({ });
}

