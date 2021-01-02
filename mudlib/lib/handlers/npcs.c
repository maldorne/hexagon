
#include <common/properties.h>

// Taniwha, util routine for "membership"
int check_isin(mapping map, object ob)
{
  mixed list;
  int i;
  list = map["race"];
  if (pointerp(list) && 
    ((member_array(ob->query_race_name(),list) != -1) || 
     (member_array(ob->query_base_race_name(),list) != -1)) )
    return 1;
  if (stringp(list) && (string)list == (string)ob->query_race_name()) return 1;
  if (stringp(list) && (string)list == (string)ob->query_base_race_name()) return 1;
  list = map["guild"];
  if (pointerp(list) && member_array(ob->query_guild_name(),list) != -1) return 1;
  if (stringp(list) && (string)list == (string)ob->query_guild_name()) return 1;
  list = map["group"];
  if (pointerp(list) && member_array(ob->query_group_name(),list) != -1) return 1;
  if (stringp(list) && (string)list == (string)ob->query_group_name()) return 1;
  list = map["race_group"];
  if (pointerp(list) && member_array(ob->query_race_group_name(),list) != -1) return 1;
  if (stringp(list) && (string)list == (string)ob->query_race_group_name()) return 1;
  list = map["player"];
  if (pointerp(list) && member_array(ob->query_name(),list) != -1) return 1;
  if (stringp(list) && (string)list == (string)ob->query_name()) return 1;
  list = map["deity"];
  if (pointerp(list) && member_array(ob->query_deity_name(),list) != -1) return 1;
  if (stringp(list) && (string)list == (string)ob->query_deity_name()) return 1;
  list = map["city"];
  if (pointerp(list) && member_array(ob->query_city_name(),list) != -1) return 1;
  if (stringp(list) && (string)list == (string)ob->query_city_name()) return 1;

  list = map["property"];
  if (pointerp(list))
    for (i = 0; i < sizeof(list); i++)
    {
      if (stringp(list[i])&& ob->query_property(list[i])) return 1;
    }
  if (stringp(list) && ob->query_property(list)) return 1;
  return 0;
}

// Retocada por Folken, 6/03
int do_aggressive_check(object ob, int aggressive, object me, int minplayer,
            mapping hated, mapping loved)
{
  if (!ob) return 0;
  if (ob->query_invis()) return 0;
  if (!ob->query_alive()) return 0;
  if (ob->query_level() < minplayer) return 0;
	
  if (ob->query_timed_property(PASSED_OUT_PROP)) return 0;
  
  switch (aggressive) 
  {
   case 0:
     if (!interactive(ob)) return 0;
     if (ob->query_hidden()) return 0;
     if (mappingp(hated) && check_isin(hated,ob)) return 1;
     return 0;
   break;

   case 1: 
     if (mappingp(loved) && check_isin(loved, ob)) return 0;
     if (!interactive(ob)) return 0;
     if (ob->query_hidden()) return 0;
     return 1;
   break;
   
   case 2: 
    if (mappingp(loved) && check_isin(loved,ob)) return 0;
    if (ob->query_hidden()) return 0;
    return 1;
   break;

   case 3: 
     if (mappingp(loved) && check_isin(loved,ob)) return 0;
     if (!interactive(ob)) return 0;
     return 1;
   break;
   
   case 4: 
    if (mappingp(loved) && check_isin(loved,ob)) return 0;
    return 1; 
   break;
   }
}

// Funcion auxiliar para los load_chat o load_a_chat de monster.c
// Si en el mensaje incluimos entre simbolos de dolar ($) determinadas
//  palabras, se sustituyen (ej: $race$ --> ob->query_race() )
string expand_string(object me, string in_str, varargs object on, object attacker)
{
  string *str, ret;
  int i, j, add_dollar;
  int k,l;

  object liv, *obs, ob, *attacker_list;

  str = explode(in_str, "$");
  ret = "";

  // Fix by Wonderflug, only parse every SECOND argument, just tack on the 
  // others.
  for (i = 0; i < sizeof(str); i++)
  {
    if ( !( i % 2) )
      ret += str[i];
    else
    {
      switch (str[i][0])
      {
        case 'm' :
          ob = me;
          break;
        case 'l' :
          if (!ob)
          {
            if (!liv)
            {
              obs = all_inventory(environment(me));
              k = sizeof(obs);
              l = random(k);
              for (j = 0; j < k; j++)
              {
                if (living(obs[l]) && obs[l] != me)
                {
                  liv = obs[l];
                  break;
                }
                l++;
                l %= k;
              }
            }
            if (!liv)  /* No one here to see us blow up anyway ;) */
              ob = me;
            ob = liv;
          }

          if (!ob)
          {
            attacker_list = me->query_attacker_list();
            
            if (!sizeof(attacker_list))
              break;
            
            if (!attacker)
              attacker = attacker_list[random(sizeof(attacker_list))];
            
            ob = attacker;
          }
          
        case 'o' :
          if (!ob)
          {
            if (!on)
            {
              obs = all_inventory(environment(me));
              k = sizeof(obs);
              l = random(k);
              ob = me; /* should all else fail */
              for (j = 0; j < k; j++)
              {
                if ( obs[l] != me)
                {
                  on = obs[l];
                  break;
                }
                l++;
                l %= k;
              }
            }
            else
              ob = on;
          } /* if (!ob) */

          switch (str[i][1..1000])
          {
          case "name" :
            ret += (string)ob->query_name();
            add_dollar = 0;
            break;
          case "cname" :
            ret += (string)ob->query_cap_name();
            add_dollar = 0;
            break;
          case "gender" :
            ret += (string)ob->query_gender_string();
            add_dollar = 0;
            break;
          case "poss" :
            ret += (string)ob->query_possessive();
            add_dollar = 0;
            break;
          case "obj" :
            ret += (string)ob->query_objective();
            add_dollar = 0;
            break;
          case "gtitle" :
            ret += (string)ob->query_gender_title();
            add_dollar = 0;
            break;
          case "pronoun" :
            ret += (string)ob->query_pronoun();
            add_dollar = 0;
            break;
          case "race":
            ret += (string)ob->query_race_name();
            add_dollar =0;
            break;
          case "guild":
            ret += (string)ob->query_guild_name();
            add_dollar =0;
            break;
          case "group":
            ret += (string)ob->query_group_name();
            add_dollar =0;
            break;
          case "racegroup":
            ret += (string)ob->query_race_group_name();
            add_dollar =0;
            break;
          case "class":
            ret += (string)ob->query_class_name();
            add_dollar =0;
            break;

          default :
            if (add_dollar)
              ret += "$";
            ret += str[i];
            add_dollar = 1;
            break;
          } /* switch str[i][1.. */

          ob = nil;
          break;

        default :
          if (add_dollar)
            ret += "$";
          ret += str[i];
          add_dollar = 1;
          ob = nil;
          break;
      }
    }
  }

  if (ret[strlen(ret)-1] == '$')
    ret = ret[0..strlen(ret)-2];
  
  if (!stringp(ret))
    return "Parece confuso por un momento.";
  
  return ret;
}

void effect_heart_beat(object me, object *attacker_list, mixed attack_effects)
{
  int i;
  int k, silent;
  string effect_target;

  i = random(sizeof(attack_effects)/3);
  i *= 3;
  if (random(100) > attack_effects[i] ) 
    return;

  /* Check that we can cast a effect */
  // if ( attack_effects[i+2][2] == 4 && me->check_props(NOFEAT_PROP) )
  //   return;
  if ( me->check_props(NOFEAT_PROP) ) 
    return;

  /* Find a target */
  if (attack_effects[i+2][2] == 1)
  {
    effect_target = "";
    for (k = 0; k < (sizeof(attacker_list) - 1); k++)
      effect_target += attacker_list[k]->query_name() + ",";
    effect_target += attacker_list[sizeof(attacker_list) - 1]->query_name();
  }
  else if ( attack_effects[i+2][2] == 2 )
    effect_target = "todos";
  else if ( attack_effects[i+2][2] == 3 )
    effect_target = me->query_name();
  else effect_target =
    attacker_list[random(sizeof(attacker_list))]->query_name();

  if (sizeof(attack_effects[i+2]) > 3) 
    silent = attack_effects[i+2][3];
  else 
    silent = 0;

  call_other(attack_effects[i+2][0], attack_effects[i+2][1],
    effect_target, me, silent);
  /* end of the attack effect thingie */
}

