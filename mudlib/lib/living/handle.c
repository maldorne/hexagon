/*
 * has all the give/get stuff.
 * Fixed so that the objects are unworn/unheld..
 * Baldrick..
 * Reviewed for CcMud, neverbot 4/03
 * added consent to receive items, neverbot 05/09
 */

#include <language.h>
#include <basic/move.h>
#include <common/properties.h>
#include <basic/money.h>
#include <living/consents.h>
#include <translations/money.h>

#define TIME_DIV 10

void handle_commands()
{
  add_private_action("do_take", _LANG_HANDLE_GET_VERBS);
  add_private_action("do_give", _LANG_HANDLE_GIVE_VERBS);
  add_private_action("do_drop", _LANG_HANDLE_DROP_VERBS);
}

void create()
{
}

// this will work as give and put...
int do_give(string str, varargs string verb, object *bing, string bing2, int blue)
{
  string person, thing, sh, con;
  int i, j, tot, max;
  object *obs, *per, *ret, *fail;

  i = 0; j = 0;

  // Added to get rid of one more auto_load bug..
  // Baldrick, nov '94
  if (this_object()->query_property(LOADING_PROP))
  {
    notify_fail(_LANG_HANDLE_WAIT_LOADING);
    return 0;
  }

  if (this_object()->query_timed_property_exists(PASSED_OUT_PROP))
  {
    notify_fail(this_object()->query_timed_property(PASSED_OUT_PROP));
    return 0;
  }


  if (!strlen(str))
  {
    notify_fail(_LANG_HANDLE_GIVE_SYNTAX);
    return 0;
  }

  if (query_verb() == _LANG_HANDLE_PUT_VERB)
    con = " " + _LANG_HANDLE_PUT_PREP + " ";
  else
    con = " " + _LANG_HANDLE_GIVE_PREP + " ";

  if (bing)
  {
    obs = bing;
    thing = bing2;
    j = blue;
  }
  else
  {
    if (sscanf(str, "%s" + con + "%s", thing, person) != 2)
    {
      notify_fail(_LANG_HANDLE_GIVE_SYNTAX);
      return 0;
    }

    obs = find_match(person, ({ this_object(), environment() }), 1);

    if (!sizeof(obs))
    {
      notify_fail(_LANG_HANDLE_GIVE_CANNOT_FIND);
      return 0;
    }

    per = obs;
  }

  // should check to see that we can give things to this person/thing...
  if (!per)
    per = ({ });

  for (; j < sizeof(per); j++)
  {
    if (per[j] == this_object())
      continue;

    if (per[j]->query_player() || per[j]->query_npc())
    {
      if (!per[j]->query_consent(GIVE_CONS))
      {
        tell_object(this_object(), per[j]->query_cap_name() + 
          _LANG_HANDLE_REFUSE);
        tell_object(per[j], this_object()->query_cap_name() + 
          _LANG_HANDLE_REFUSE_GIVE);
        continue;
      }
    }

    obs = find_match(thing, this_object(), 1);
    obs -= ({ per[j] });

    if (!sizeof(obs))
    {
      write(_LANG_HANDLE_NO_PER);
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
      sh = query_multiple_short(ret, 1); // flag 1 to use pretty_short
      tell_object(this_object(), _LANG_HANDLE_GIVE_ME + " " + 
        sh + con + (string)per[j]->short() + ".\n");
      tell_room(environment(this_object()), capitalize((string)this_object()->short()) +
        " " + _LANG_HANDLE_GIVE_ROOM + " " + 
        sh + con + (string)per[j]->short() + ".\n", ({ per[j] }) );
      tell_object(per[j], capitalize((string)this_object()->short()) + " " +
        _LANG_HANDLE_GIVE_THEY + " " + sh + ".\n");

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
            tell_room(environment(), per[j]->short() + 
              _LANG_HANDLE_WEIGHT_HEAVY_THEM, ({per[j] }));
            tell_object(per[j], _LANG_HANDLE_WEIGHT_HEAVIER_ME);
            break;
          default:
            tell_room(environment(), per[j]->short()+ ({
                _LANG_HANDLE_WEIGHT_LIGHT_THEM,
                _LANG_HANDLE_WEIGHT_MEDIUM_THEM,
                _LANG_HANDLE_WEIGHT_HEAVY_THEM
              })[(max/25)-1], ({ per[j] }) );
            tell_object(per[j], ({
                _LANG_HANDLE_WEIGHT_LIGHT_ME,
                _LANG_HANDLE_WEIGHT_MEDIUM_ME,
                _LANG_HANDLE_WEIGHT_HEAVY_ME
              })[(max/25)-1]);
        }

        this_object()->add_timed_property(PASSED_OUT_PROP, 
          _LANG_HANDLE_PASSED_OUT_MSG, max/TIME_DIV);
      }
    }

    if (sizeof(fail))
      write(_LANG_HANDLE_CANNOT + " " + query_verb() + " " + 
        query_multiple_short(fail, 1) + con + per[j]->short() + ".\n");
  }

  return 1;
}

int do_drop(string str)
{
  int i, num;
  object * ob, * ret, * fail;
  string sh;

  // Added to get rid of one more auto_load bug..
  // Baldrick, nov '94
  if (this_object()->query_property(LOADING_PROP))
  {
    notify_fail(_LANG_HANDLE_WAIT_LOADING);
    return 0;
  }

  if (this_object()->query_timed_property_exists(PASSED_OUT_PROP))
  {
    notify_fail(this_object()->query_timed_property(PASSED_OUT_PROP));
    return 0;
  }

  if (!strlen(str))
  {
    notify_fail(_LANG_HANDLE_DROP_SYNTAX);
    return 0;
  }

  ob = find_match(str, this_object(), 1);
  if (!sizeof(ob))
  {
    notify_fail(_LANG_HANDLE_DROP_CANNOT_FIND);
    return 0;
  }

  ret = ({ });
  fail = ({ });

  for (i = 0; i < sizeof(ob); i++)
  {
    if (!ob[i]->short())
      continue;

    // Add by Baldrick, so the players will unhold the object they drop..
    // This method is *NOT* good, but faster than the hard way..
    // but we should do it the hard way, later.
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
    // if you do 'drop 2 platinum,3 copper', the first money object 
    // already has the description and value of every coin dropped
    int j, has_money;
    object * new_ret;

    new_ret = ({ });
    has_money = 0;

    for (j = 0; j < sizeof(ret); j++)
    {
      // filter money items but the first one
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
      tell_room(environment(), capitalize((string)this_object()->short()) + 
        " " + _LANG_HANDLE_DROP_THEY + " " + sh + ".\n");

    write(_LANG_HANDLE_DROP_ME + " " + sh + ".\n");
  }

  if (sizeof(fail))
    write(_LANG_HANDLE_CANNOT + " " + query_verb() + " " + 
      query_multiple_short(fail, 1) + ".\n");

  // num += sizeof(fail) + sizeof(ret);

  // if (!num)
  // {
  //   notify_fail("No puedes dejar "+str+".\n");
  //   return 0;
  // }

  return 1;
}

int do_take(string str, varargs string verb, object *bing, string bing2, int blue)
{
  object *dest, *obs, *fail, *ret_a;
  mixed *ret;
  string s2, sh, str2, aux;
  int i, num, j, cap, perc, we, tot, max;

  i = 0; j = 0;

  // Added to get rid of one more auto_load bug..
  // Baldrick, nov '94
  if (this_object()->query_property(LOADING_PROP))
  {
    notify_fail(_LANG_HANDLE_WAIT_LOADING);
    return 0;
  }

  if (this_object()->query_timed_property_exists(PASSED_OUT_PROP))
  {
    notify_fail(this_object()->query_timed_property(PASSED_OUT_PROP));
    return 0;
  }

  if (this_object()->query_dead())
  {
    notify_fail(_LANG_HANDLE_DEAD);
    return 0;
  }

  if (!strlen(str) && !verb)
  {
    notify_fail(_LANG_HANDLE_GET_SYNTAX);
    return 0;
  }

  cap = (int)this_object()->query_max_weight() + 1;

  if (verb)
  {
    dest = bing;
    i = blue;
    str = bing2;
  }
  else
  {
    dest = ({ environment() });

    // this hack only makes sense for spanish language, will
    // never happen in english
    if (sscanf(str, _LANG_HANDLE_GET_COINS_MATCH, aux, s2) == 2 )
    {
      if (member_array(s2, MONEY_HAND->query_values()) == -1)
      {
        dest = find_match(s2, ({ environment(), this_object() }), 1);
        str = aux + pluralize(COIN_BASE_NAME);
        if (!sizeof(dest))
        {
          notify_fail(_LANG_HANDLE_GET_CANNOT_FIND);
          return 0;
        }
      }
    }
    else
    {
      if (sscanf(str, "%s " + _LANG_HANDLE_GET_PREP + " %s", str, s2) == 2)
      {
        // since we only allow one lvl anyway... 
        dest = find_match(s2, ({ environment(), this_object() }), 1);

        if (!sizeof(dest))
        {
          notify_fail(_LANG_HANDLE_GET_CANNOT_FIND);
          return 0;
        }
      }
    }

    i = 0;
  }

  for (; i < sizeof(dest); i++)
  {
    if (dest[i]->cannot_get_stuff())
      continue;

    obs = find_match(str, dest[i], 1);

    ret = ({ ({ }), ({ }), ({ }), ({ }), ({ }), });
    ret_a = ({ });
    fail = ({ });
    tot = 0;

    for (j = 0; j < sizeof(obs); j++)
    {
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
      {
        fail += ({ obs[j] });
      }
    }

    for (j = 0; j < sizeof(ret); j++)
    {
      if (sizeof(ret[j]))
      {
        sh = query_multiple_short(ret[j], 1);

        write(_LANG_HANDLE_GET_MESSAGES[j] + " " + sh + " " + 
          _LANG_HANDLE_GET_PREP + " " + (string)dest[i]->short() + ".\n");

        if (!this_object()->query_hidden())
          tell_room(environment(), (string)this_object()->query_cap_name() + 
            " " + _LANG_HANDLE_GET_MESSAGES_OTHERS[j] + " " + sh + " " + 
            _LANG_HANDLE_GET_PREP + " " + (string)dest[i]->short() + ".\n");

        num += sizeof(ret[j]);
      }
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
          _LANG_HANDLE_STILL, tot/TIME_DIV);
      }
    }

    if (sizeof(fail))
      write(_LANG_HANDLE_FAIL);

    num += sizeof(fail);
  }

  if (!num)
  {
    notify_fail(_LANG_HANDLE_GET_NOTHING);
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
    do_give(0, arg[2], per, thing, j+1);
    break;
  case "get" :
  case "coger":
    do_take(0, arg[2], per, thing, j+1);
    break;
  }
  }
}
*/

mixed * stats()
{
  return ({ });
}

