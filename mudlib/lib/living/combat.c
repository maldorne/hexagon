/* Ok, this time it's this code I will walk through...
 * Baldrick, april '94
 * Removed the deathstuff and pu tit in death.c.
 * looks a lot better that way.
 * Hope it will work, Baldrick, june '96
 *
 * Añadido armed_combat, neverbot 6/03
 *
 */

#include <common/properties.h>
#include <user/xp.h>
#include <user/hud.h>
#include <living/combat.h>
#include <living/consents.h>
#include <language.h>

inherit unarmed_combat "/lib/living/unarmed_combat.c";
inherit armed_combat   "/lib/living/armed_combat.c";

static object attackee;
static object *attacker_list, *call_outed, protector;

static object concentrate;
static int dodging;
static int combat_mode;
static int combat_role;

static object last_moving_env; // object with the last enviroment
static int combat_counter; // heart_beat counter
static int is_quiet;  // not moving or fighting
static int is_moving; // recently moved


int query_dodging() { return dodging; }
int query_combat_mode() { return combat_mode; }
void set_combat_mode(int i) { combat_mode = i; }
string query_combat_mode_string() { return COMBAT_MODE_STRINGS[combat_mode];}
int query_combat_role() { return combat_role; }
void set_combat_role(int i) { combat_role = i; }
string query_combat_role_string() { return COMBAT_ROLE_STRINGS[combat_role];}
int query_my_att_level() { return (int)this_object()->query_level(); }
int set_concentrate(object ob)
{
  if ( member_array(ob, attacker_list) == -1 ) 
    return 0;
  concentrate = ob;
  return 1;
}
object query_concentrate() { return concentrate; }
int query_concentrate_valid() { return 0; }

// Radix, added for unprotect add_action
void remove_protector(object ob) { protector = nil; }
object set_protector(object ob) { return (protector = ob); }
object query_protector() { return protector; }

int query_is_quiet() { return is_quiet; }
int query_is_moving() { return is_moving; }


void combat_commands() 
{
  add_private_action("do_protect", "proteger");
  add_private_action("do_unprotect", "desproteger");
  add_private_action("do_combat_role", "actitud");
  add_private_action("do_combat_mode", "combate");

  armed_combat_commands();
  unarmed_combat_commands();
}

void create() 
{
  attacker_list = ({ });
  call_outed = ({ });
  protector = nil;
  attackee = nil;
  concentrate = nil;
  dodging = 0;
  combat_mode = MORTAL_COMBAT;
  combat_role = DPS_ROLE;
  unarmed_combat::create();
  armed_combat::create();

  combat_counter  = 1; // start even
  last_moving_env = nil;
  is_moving       = 0;
  is_quiet        = 1;
}

void attack();

void heart_beat()
{
  // Added a combat_counter, so the combat aren't that quick.
  if (combat_counter % 2 == 0)
  {
    attack();
    this_object()->do_active_effects(attackee);

    if (combat_counter % 4 == 0)
    {
      if (last_moving_env != environment(this_object()))
      {
        last_moving_env = environment(this_object());
        is_moving = 1;
      }
      else
        is_moving = 0;
    }

    if (combat_counter >= 100)
     combat_counter = 1; 
    
    if (!this_object()->query_is_fighting() && !is_moving)
      is_quiet = 1;
    else
      is_quiet = 0;
  } 

  combat_counter++;

  if (sizeof(attacker_list) && 
      this_object()->query_wimpy() && 
     (this_object()->query_hp() < this_object()->query_max_hp() * this_object()->query_wimpy() / 100))
  {
    this_object()->run_away();
  }
}

object query_attackee() { return attackee; }

int filter_hidden(object ob)
{
  if (ob->query_hide_shadow()) 
    return 0;
  return 1;
}

int query_fighting()
{
  int i;

  attacker_list -= ({ nil });
  call_outed -= ({ nil });
  for (i=0;i<sizeof(attacker_list);i++)
  {
    if (interactive(attacker_list[i]))
      return 4;
    if (attacker_list[i]->query_aggressive())
      return 1;
    else
      return 2; 
  }

  for (i=0;i<sizeof(call_outed);i++)
  {
    if (interactive(call_outed[i]))
      return 4;
    if (call_outed[i]->query_aggressive())
      return 1;
    else
      return 2; 
  }
  return 0;
}

int query_protect_valid(object attacker, object protectee){
  int base_roll;

  // Wonderflug, dec 95, can't protect while berserk
  if ( this_object()->query_property(FLIPPED_PROP) )
    return 0;

  base_roll = random(100);   // this to be replaced by a guild obj call
  if ( member_array( attacker, attacker_list ) == -1 )
    base_roll -= 20;

  return ( base_roll >= 30 );
}

void attack() 
{
  int i, dam, his_att_lvl, olav, att_level;
  string his_name;
  object ob, *ww;
  int bare_hands;
  int my_xp, my_xp_percentage;
  object * away_attackers;

  i = 0;
  away_attackers = ({ });
  bare_hands = 0;
  my_xp = 0; 
  my_xp_percentage = 0; 

  if (!attacker_list)
    attacker_list = ({ });

  while (i < sizeof(attacker_list)) 
  {
    if (this_object()->query_timed_property_exists(PASSED_OUT_PROP))
    {
      tell_object(this_object(), _LANG_COMBAT_PASSED_OUT_MSG);
      return;
    }

    if (!attacker_list[i]) // dw have they died unexpectedly? 
      attacker_list = delete(attacker_list, i, 1);
    else
    {
      // dw I will put all the ones not in the correct environ in a new list.
      if (environment(attacker_list[i]) != environment(this_object())) 
      {
        // tell_object(this_object(),"Estás persiguiendo a "+attacker_list[i]->short()+".\n");
        away_attackers += ({ attacker_list[i] });

        if (!call_outed)
          call_outed = ({ attacker_list[i] });
        else
          call_outed += ({ attacker_list[i] });
        attacker_list = delete(attacker_list, i, 1);
      } 
      else
        i++;
    }
  }

  if (sizeof(away_attackers) > 0)
    tell_object(this_object(), _LANG_COMBAT_CHASING +
                query_multiple_short(away_attackers) + ".\n");

  // dw check the not correct environ list to see if any one has come back.  
  i = 0;
  while (i < sizeof(call_outed))
  {
    if (!call_outed[i])
      call_outed = delete(call_outed,i,1);
    else if (environment(this_object()) == environment(call_outed[i])) 
    {
      attacker_list += ({ call_outed[i] });
      call_outed = delete(call_outed,i,1);
    } 
    else
      i++;
  }
  
  // dw not hitting anyone... do spell effects anyway... 
  if (!sizeof(attacker_list)) 
  {
    attackee = nil;
    return;
  }

  // dw choose a random person from the attacker list; they're in correct env.
  if (concentrate)
  {
    if (member_array( concentrate, attacker_list ) == -1)
    {
      // attackee = attacker_list[random(sizeof(attacker_list))];
      // Aggro, neverbot 01/2013
      attackee = this_object()->query_main_aggro_doer();
      concentrate = nil;
    }
    else
    {
      attackee = concentrate;
      if ( !this_object()->query_concentrate_valid(
        concentrate, attacker_list) )
        concentrate = nil;
    }
  }
  else
  {
    // attackee = attacker_list[random(sizeof(attacker_list))];
    // Aggro, neverbot 01/2013
    attackee = this_object()->query_main_aggro_doer();
  }

  // dw Check to see if they are being protected.
  if (protector && member_array(protector, attacker_list) != -1) 
  {
    tell_object(this_object(), protector->query_cap_name()+
      " se vuelve contra ti y deja de protegerte.\n");
    tell_object(protector, "Comienzas a pelear con "+
      this_object()->query_cap_name()+" y dejas de proteger"+
      ((this_object()->query_gender()==2)?"la":"le")+".\n");
    tell_room(environment(this_object()),protector->query_cap_name()+
      " se vuelve contra "+
      this_object()->query_cap_name()+" y comienza a atacar"+
      ((this_object()->query_gender()==2)?"la":"le")+".\n",
      ({ protector, this_object() }));

    protector = nil;
  }

  if ((ob = (object)attackee->query_protector()) &&
    environment(ob) == environment(attackee) &&
    ob->query_protect_valid(this_object(), attackee) )
  {
    if (!ob->query_dead())
    {
      tell_object(attackee, ob->query_cap_name()+
        " te protege valientemente.\n");
      tell_object(ob,"Proteges valientemente a "+attackee->query_cap_name()+".\n");
      tell_room(environment(this_object()),ob->query_cap_name()+
        " protege valientemente a "+
        attackee->query_cap_name()+".\n", ({ attackee, ob }));
      attackee = ob;
    }
  }

  his_att_lvl = maxi(({ (int)attackee->query_level(), 1 }) );
  his_name = (string)attackee->query_name();

  attackee->attack_by(this_object());

  dam = 0;
  ww = (object *)this_object()->query_weapons_wielded();

  if (!sizeof(ww))
  {
    for (olav = 0; olav < this_object()->query_free_hands_left(); olav++)
      dam = this_object()->unarmed_attack(attackee, this_object());
    bare_hands = 1;
  }
  else
  {
    for (olav = 0; olav < sizeof(ww); olav++)
      dam += (int)ww[olav]->weapon_attack(attackee, this_object());
    this_object()->reset_num_unarmed_hits();
    bare_hands = 0;
  }

  if (!attackee) 
  {
    tell_object(this_object(), _LANG_COMBAT_KILL_NO_ATTACKEE_MSG_ME);
    tell_room(environment(this_object()), _LANG_COMBAT_KILL_NO_ATTACKEE_MSG_ROOM,
              ({ this_object() }));
  }

  att_level = (int)this_object()->query_level();
  att_level = att_level ? att_level : 1;

  if (map_sizeof(this_object()->query_xp_types()) == 0)
    my_xp_percentage = 100;
  else if (bare_hands)
  {
    // tell_object(this_object(), "[Debug unarmed_combat_xp]: damage: " + dam + " total: " + my_xp + " final: ");
    my_xp_percentage = this_object()->query_xp_types()[UNARMED_COMBAT_XP];
    // tell_object(this_object(), "" + my_xp + "(" + my_xp_percentage + ")% \n");
  }
  else
  {
    // tell_object(this_object(), "[Debug armed_combat_xp]: damage: " + dam + " total: " + my_xp + " final: ");
    my_xp_percentage = this_object()->query_xp_types()[ARMED_COMBAT_XP];
    // tell_object(this_object(), "" + my_xp + "(" + my_xp_percentage + ")%\n");
  }

  // only adjust xp with targets of similar or higher level
  if (his_att_lvl + 2 >= att_level)
  {
    // xp types, neverbot 07/04
    my_xp = dam * (his_att_lvl/att_level);
    my_xp = (my_xp * my_xp_percentage ) / 100;

    this_object()->adjust_xp(my_xp);
  }

  event(environment(this_object()), "fight_in_progress", attackee);
}

void attack_by(object ob) 
{
  if (ob == this_object())
    return;

  if (this_object()->query_hidden())
    return;

  if (!attacker_list)
    attacker_list = ({ });

  if (!sizeof(attacker_list)) 
  {
    dodging = (int)this_object()->query_level();
  }

  if (member_array(ob, attacker_list) == -1 &&
      member_array(ob, call_outed) == -1) 
  {
    string color, name;
    name = ob->query_short();

    // change player appereance from /table/hud_table.c, neverbot 01/2020
    color = (string)HUD->query_color(this_object(), ob);

    name = strlen(color) ? color + name  + "%^RESET%^" : name;

    tell_object(this_object(), _LANG_COMBAT_BEING_ATTACKED);

    attacker_list += ({ ob });
  }

  if (!query_heart_beat())
    set_heart_beat(1);
}

// Hmm, this is called by the attacker, object ob is the victim 
void attack_ob(object ob) 
{
  if (ob == this_object())
    return;
  if (ob->query_dead())
    return;
    
  // Should use the one used by death.c before.
  if (!attacker_list)
    attacker_list = ({ });

  if (!sizeof(attacker_list)) 
  {
    // This will be changed later.. 
    dodging = (int)this_object()->query_level();
  }

  if (member_array(ob, attacker_list) == -1 &&
    member_array(ob, call_outed) == -1) 
  {
    attacker_list += ({ ob });
  }

  if (ob)
    ob->attack_by(this_object());
}

void check_stop_fight(object ob) 
{
  int i;
  if ((i=member_array(ob,call_outed)) != -1)
    call_outed = delete(call_outed,i,1);
}

int stop_fight(object ob, varargs int silent) 
{
  int i;
  
  if ((i = member_array(ob, call_outed)) != -1) 
  {
    if (!silent)
      tell_object(this_object(), _LANG_COMBAT_STOP_CHASING + 
        call_outed[i]->short(0) + ".\n");
    
    call_outed = delete(call_outed, i, 1);
  }
  if ((i = member_array(ob,attacker_list)) != -1)
    attacker_list = delete(attacker_list, i, 1);
}

mixed * query_attacker_list()
{
  attacker_list -= ({ nil });
  return attacker_list;
}

mixed * query_call_outed()
{
  call_outed -= ({ nil });
  return call_outed;
}

int query_is_fighting()
{
  return (sizeof(query_attacker_list()) || sizeof(query_call_outed()));
}

/** added by Raskolnikov Oct 96 **/
mixed * remove_call_outed()
{
  call_outed = ({ });
  return call_outed;
}

/** added by Raskolnikov Oct 96 **/
mixed * reset_attacker_list()
{
  attacker_list = ({ });
  return attacker_list;
}

mixed add_attacker_list(object ob)
{
    if (member_array(ob, call_outed)>-1)
    {
        call_outed-=({ob});
        attacker_list+=({ob});
    }
    else
      attacker_list+=({ob});
    return attacker_list;
}

// dw Sets up protection matrixes.
int do_protect(string str) 
{
  object *obs, *ok, *no_prot; 
  int i;

  if (!strlen(str))
  {
    notify_fail("Sintaxis: proteger <objetivos>\n");
    return 0;
  }
  
  /*
  if (!this_object()->query_class_ob())
  {
    notify_fail("¿Acabas de aparecer en este mundo y ya piensas en proteger a la gente? "
          "Mejor cuida de ti mismo y más adelante ya te dedicarás "
          "a los demás.\n");
    return 0;
  }
  */
  
  obs = find_match(str, environment(this_object()), 1);
  
  // No necesario con el flag de find match
  // obs = filter_array(obs,"filter_hidden");
  if (!sizeof(obs)) 
  {
    notify_fail("¿Proteger a quién?\n");
    return 0;
  }
  
  if (member_array(this_object(), obs))
    obs -= ({ this_object() });
    
  ok = ({ });
  no_prot = ({ });
  
  for (i = 0; i < sizeof(obs); i++)
    if (obs[i]->query_consent(PROTECT_CONS) && obs[i]->set_protector(this_object()))
      ok += ({ obs[i] });
    else
      no_prot += ({ obs[i] });
      
  if (sizeof(no_prot)) 
  {
    tell_object(this_object(), query_multiple_short(no_prot)+ " no quiere"+
      (sizeof(no_prot)==1?"":"n")+" tu protección.\n");
  }
  
  if (sizeof(ok))
  {
    tell_player(this_object(),"Proteges a " + 
    ((sizeof(ok) == 1)?ok[0]->query_short():query_multiple_short(ok)) + ".\n");
    tell_object(this_object(),"Utiliza 'desproteger' para dejar de hacerlo.\n");

    tell_room(environment(this_object()),this_object()->query_cap_name() +
      " protege a " + ((sizeof(ok) == 1)?ok[0]->query_short():query_multiple_short(ok)) +
      ".\n", ok);
  }

  if (sizeof(ok) > 1)
  {
    for (i = 0; i < sizeof(ok); i++)
      tell_player(ok[i], this_object()->query_cap_name()+" os protege a ti y a "+
        query_multiple_short(ok - ({ ok[i] })) +".\n");
  }
  else if (sizeof(ok) == 1)      
    tell_player(ok[0], this_object()->query_cap_name()+" te protege.\n"); 

  return 1;
}

// Added by Radix, missing too long...
int do_unprotect(string str)
{
  object tmp;
  
  if (!strlen(str))
  {
    notify_fail("Sintaxis: desproteger <objetivo>\n");
    return(0);
  }
  
  tmp = find_living(str);
  
  if (!tmp)
  {
    notify_fail("¿Desproteger a quién?\n");
    return(0);
  }
  
  if (tmp->query_protector() != this_object())
  {
    write("No estás protegiendo a "+tmp->query_cap_name()+".\n");
    return(1);
  }
  
  tmp->remove_protector(this_object());
  
  if (environment(tmp) == environment(this_object()))
  {
    tell_player(tmp, this_object()->query_cap_name()+" se retira y "+
      "no te protege más.\n");
    tell_room(environment(), this_object()->query_cap_name()+" deja de "+
      "proteger a " + tmp->query_cap_name() + ".\n", ({tmp, this_object() }));
  }
  else
  {
    tell_object(tmp, this_object()->query_cap_name()+" deja de protegerte.\n");
  }
  
  tell_object(this_object(),"Te retiras y dejas de proteger a "+tmp->query_cap_name()+".\n");
  return(1);
}

static string combat_role_message(int new_role)
{
  return "Tu actitud de combate pasa a ser " + COMBAT_ROLE_STRINGS[new_role] + ".\n";
}

// Funcion para cambios forzados del rol de combate (al ejecutar dotes, por ejemplo)
int change_combat_role(int new_role)
{
  if (combat_role != new_role)
  {
    set_combat_role(new_role);
    tell_player(this_object(), combat_role_message(new_role));
  }
  return 1;
}

// Funcion para cambios del rol de combate ejecutados por el player activamente
int do_combat_role(string str)
{
  int i;
  int last_time;

  last_time = this_object()->query_static_property(COMBAT_ROLE_CHANGED_PROP);

  if (!strlen(str))
  {
    tell_object(this_object(), "Tu actitud de combate actual es " + 
      COMBAT_ROLE_STRINGS[query_combat_role()] + ".\n" + 
      "Utiliza 'actitud <modo>' para cambiarla.\n");
    return 1;

  }

  if ((i = member_array(str, COMBAT_ROLE_STRINGS)) == -1)
  {
    notify_fail("Sintaxis: actitud <modo>\n" + 
          "Los modos posibles son: " + query_multiple_short(COMBAT_ROLE_STRINGS) + 
          ".\n");
    return 0;
  }

  // aun no ha cambiado de rol desde que conecto (al conectar por defecto
  // todos los players son DPS_ROLE)
  if (!last_time)
  {
    this_object()->add_static_property(COMBAT_ROLE_CHANGED_PROP, time());
  }  
  // No dejamos cambiar de rol en menos de cinco minutos
  else if (time() - last_time < 300)
  {
    notify_fail("Hace poco que has cambiado tu actitud de combate, debes esperar un poco.\n");
    return 0;
  }

  set_combat_role(i);
  tell_object(this_object(), combat_role_message(i));
  
  return 1;
}

int do_combat_mode(string str)
{
  int i;

  if (!strlen(str))
  {
    tell_object(this_object(), "Tu modo de combate actual es " + 
      COMBAT_MODE_STRINGS[query_combat_mode()] + ".\n" + 
      "Utiliza 'combate <modo>' para cambiarlo.\n");
    return 1;

  }

  if ((i = member_array(str, COMBAT_MODE_STRINGS)) == -1)
  {
    notify_fail("Sintaxis: combate <modo>\n" + 
          "Los modos posibles son: " + query_multiple_short(COMBAT_MODE_STRINGS) + 
          ".\n");
    return 0;
  }

  set_combat_mode(i);
  tell_object(this_object(), "Ok, tu nuevo modo de combate será " + COMBAT_MODE_STRINGS[i] + ".\n");

  return 1;
}

mixed stats() 
{
  mixed *ret;
  ret = ({ 
    ({ "Combat Mode (nosave)",  combat_mode, }), 
    ({ "Combat Role (nosave)",  combat_role, }), 
    ({ "Protector (nosave)",  protector, }), 
      });
  ret += unarmed_combat::stats() + armed_combat::stats();
  
  return (ret);
}
