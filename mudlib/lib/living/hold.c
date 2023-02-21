/* 
 * This equip code was started on by Dank.
 * It is completely rewritten by Baldrick, april '94
 *
 * Amputated and cursed hands removed. Will be added later.
 * Dank used only tho hands, This will be "unlimited" hands.
 * so it won't be any "left" and "right" hand here.
 * Right now I am afraid shields won't work.
 * Will add that in version 2...:=)
 *
 * neverbot @CcMud Apr'03, stats added
 */

#include <item/armour.h>
#include <living/combat.h> // ac types for weapons
#include <language.h>

private mixed * held_ob;
private int free_hands_left;
private int * held_ac;

// prototypes:
void reset_hands();
object *query_held_ob() { return held_ob; }
int query_held(object ob);
// int is_shield(mixed ob);
// int is_weapon(mixed ob);
int _find_empty_slot(object ob, mixed *h);
mixed *query_weapons_wielded();
mixed *query_shields_held();
int query_free_hands_left();
int unhold_ob(object ob);
int hold_ob(object ob);
int shield_bonus();
int query_dual_wield_penalty();
int * query_total_held_ac() { return held_ac; }
int query_held_ac(int type) { return held_ac[type]; }

private void _add_held_ac(object ob)
{
  int i;

  // ignore items with no ac
  if (undefinedp(ob->query_ac()))
    return;

  for (i = 0; i < sizeof(AC_TYPES); i++)
    held_ac[AC_TYPES[i]] += ob->query_ac() + ob->query_ac_against(AC_TYPES[i]);
}

private void _remove_held_ac(object ob)
{
  int i;

  // ignore items with no ac
  if (undefinedp(ob->query_ac()))
    return;

  for (i = 0; i < sizeof(AC_TYPES); i++)
    held_ac[AC_TYPES[i]] -= (ob->query_ac() + ob->query_ac_against(AC_TYPES[i]));
}

void create() 
{
  held_ob = ({ });
  free_hands_left = 0;
  
  // not on creation, we don't have yet
  // the race object
  // reset_hands();

  // allocate with zero value
  held_ac = allocate_int(sizeof(AC_TYPES));
}

void hold_commands() 
{
  add_private_action("do_hold",   _LANG_HOLD_VERBS);
  add_private_action("do_unhold", _LANG_UNHOLD_VERBS);
}

// Support functions for the rest of the player object.
// int is_shield(mixed ob) 
// {
//     return (objectp(ob) && (int)ob->query_shield());
// }

// This function is just used to filter out weapons from the 
// array of held objects.
// int is_weapon(mixed ob) 
// {
//     return (objectp(ob) && (int)ob->query_weapon());
// }

// This function returns an array containing all the weapons
// that are currently wielded. If no weapons wielded, an empty
// array is returned. -A
mixed * query_weapons_wielded() 
{
  // return filter_array(held_ob, "is_weapon", this_object());
  mixed * result;
  int i;

  result = ({ });

  for (i = 0; i < sizeof(held_ob); i++)
    if (held_ob[i] && held_ob[i]->query_weapon())
      result += ({ held_ob[i] });

  return result;
}

mixed * query_shields_held() 
{
  // return filter_array(held_ob, "is_shield", this_object());
  mixed * result;
  int i;

  result = ({ });

  held_ob -= ({ nil });

  for (i = 0; i < sizeof(held_ob); i++)
    if (held_ob[i]->query_shield())
      result += ({ held_ob[i] });

  return result;
}

int query_limbs()
{
  object ob;
  string race_path;

  // free_hands_left = query_limbs(); 

  // catch added to avoid errors
  if (!catch(race_path = this_object()->query_race_ob()) && race_path)
    if (ob = load_object(race_path))  
      return ob->query_limbs();

  // By default we could use 2... 
  // Let's use 0 to avoid race errors
  return 0;
}

void reset_hands()
{
  int i;

  held_ob -= ({ nil });

  // Unhold everything
  for (i = 0; i < sizeof(held_ob); i++)
    this_object()->unhold_ob(held_ob[i]);

  free_hands_left = query_limbs();

  // Si no tiene raza, o su raza no tiene brazos (ej: animales)
  // Le asignamos un brazo para que pueda realizar ataques.
  if (free_hands_left == 0)
    free_hands_left = 1;

  held_ob = allocate(free_hands_left);
}

// The free_hand_left is a quickie, easier to have an int than go through
// calc_free_hands all the time.. 
int query_free_hands_left()
{
  return free_hands_left;
}

int unhold_ob(object ob)
{
  int slot;
  
  if ((slot = member_array(ob, held_ob)) == -1)     
  { 
    notify_fail(_LANG_HOLD_NOT_HOLDING);
    return 0;
  }

  // Taniwha 1996
  if (ob->set_in_use(0) == 0)    
  {
    held_ob[slot] = nil;

    // Not really needed but good for speed.
    // sanity check should be run against held_ob array every so often
    free_hands_left += (int)ob->query_hands_needed();

    // new ac system, neverbot 06/03
    _remove_held_ac(ob);

    // update the max dex bon
    // (only with weapons, not shields)
    if (ob->query_weapon())
      if (ob->query_max_dex_bon() >= 0)
        this_object()->recalc_max_dex_bon();

    tell_object(this_object(), _LANG_UNHOLD_MSG);
  }
  else
  { 
    tell_object(this_object(), _LANG_CANNOT_UNHOLD_MSG);
  }

  return 1;
}

int hold_ob(object ob)
{
  int weap_hands; // hands the weapon needs.
  int slot;
  object gobj;

  // Check to make sure weapon is an allowed weapon

  // Taniwha 1995, make it work ...
  // if (ob->query_weapon())    
  // {
  //   gobj = this_object()->query_guild_ob();

  //   if ( gobj && !gobj->query_legal_weapon(ob->query_weapon_name()))
  //   {
  //     tell_object(this_object(),"Careces de la habilidad necesaria "+
  //                 "para empuñar este arma.\n");
  //     return 1;
  //   }
  // }
  
  // if (ob->query_shield())    
  // {
  //   gobj = this_object()->query_guild_ob();

  //   if ( gobj && !gobj->query_legal_armour(ob->query_shield_name()))
  //   {
  //     tell_object(this_object(),"Careces de la habilidad necesaria "+
  //                 "para llevar este equipo.\n");
  //     return 1;
  //   }
  // }

  weap_hands = (int)ob->query_hands_needed();

  if (this_object()->query_free_hands_left() < weap_hands) 
  {
    notify_fail(_LANG_HOLD_HANDS_NEEDED);
    return 0;
  }

  // The _find_empty_slot can check for and output things like
  // this. -A
  if ((slot = _find_empty_slot(ob, held_ob)) == -1)
  {
    notify_fail(_LANG_HOLD_CANNOT_HOLD);
    return 0;
  }
  
  // Taniwha 1996
  if (!ob->set_in_use(1)) 
    return 0;
  
  held_ob[slot] = ob;

  // new ac system, neverbot 06/03
  _add_held_ac(ob);
  
  // Actualizamos el bonificador maximo de destreza
  // (solo con las armas, los escudos no lo modifican)
  if (ob->query_weapon())
  {
    if (this_object()->query_max_dex_bon() < 0)
       this_object()->set_max_dex_bon(ob->query_max_dex_bon());
    else
       if ((ob->query_max_dex_bon() != -1) && 
           (ob->query_max_dex_bon() < this_object()->query_max_dex_bon()))
              this_object()->set_max_dex_bon(ob->query_max_dex_bon());

    // Nuevo sistema de aprendizaje de maestrias
    // Aprendemos la maestria si no la tenemos
    if (member_array(query_weapons_wielded()[0]->query_weapon_family(),
             keys(this_object()->query_known_weapon_masteries())) == -1 )
    {
      this_object()->add_weapon_mastery(query_weapons_wielded()[0]->query_weapon_family());
    }

    // Siempre practicamos con el arma principal que estamos usando
    this_object()->set_weapon_mastery(query_weapons_wielded()[0]->query_weapon_family());
  }
  
  free_hands_left -= weap_hands; 

  tell_object(this_object(), _LANG_HOLD_MSG);  
  return 1;
}

// Returns a penalty to hit for using two weapons at once
// based on guild object.
// Wonderflug, oct 95
int query_dual_wield_penalty()
{
  object gob;

  if ( sizeof(held_ob) <= 1 || !held_ob[0]->query_weapon() || !held_ob[1]->query_weapon() )
    return 0;

  if ( !this_object()->query_class_ob() ||
     !objectp(gob = load_object(this_object()->query_class_ob())) )
    return 5;
  else
    return gob->query_dual_wield_penalty(this_object(), held_ob[0], held_ob[1] );
}

// This function coded by Aragorn@NANVAENT for Final Realms
// No guarantee whatsoever... ;-)
int _find_empty_slot(object ob, mixed *h) 
{
  int i, good, start_slot;

  // How many hands do we need?   
  // sent from hold,,
  int nh;

  nh = (int)ob->query_hands_needed();

  // No hands? Not a holdable object.
  // More hands than we have needed? Skip it...
  if (!nh || nh > sizeof(h)) 
    return -1;

  // This is the counter of how many good slots we've got in a row.
  good = 0;

  // Loop through all slots looking for (a) suitable empty slot(s).
  for(i = 0; i < sizeof(h); i++) 
  {
    // if there is an object in this slot, skip this slot plus any
    // extra slots this object controls. 
    if (objectp(h[i]))  
    {
      i += ((int)h[i]->query_hands_needed()-1); // -1 as we skip current slot
      // at the end of the for-loop.
      good = 0;

      continue;
    }
    // Later, differenciate between the various integer values in the 
    // slots for amputated hands, cursed hands etc
    // For now, just assume a !0 slot is a no-no
    else if (h[i]) 
    {
      // No continue here, just subtract one from the good-counter.
      // This allows a three handed monster to wield a twohanded weapon
      // even though the middle hand is gone. This will also have to 
      // depend on the integer given at some point. I.e. can't do it
      // if the hand in between is cursed, but no problem if it's 
      // amputated etc.
      good -= 1;
    } 

    // Now we have to remember what slot we actually started off in
    // so we can return that to the calling function.
    if (!good)
      start_slot = i; 

    // We have a good slot!
    good++;

    // enough good slots?
    if (good == nh) 
      return start_slot;
  }

  return -1;
}

int do_hold(string woo)
{
  object *boo;
  int i;

  if (!strlen(woo))
  {
    notify_fail(_LANG_HOLD_WHAT);
    return 0;
  }

  // AAArrrggghhhh find_match() doesn't work... -Aragorn
  // boo = find_match(woo, this_object());
  boo = all_inventory(this_object());
  
  for (i = 0; i < sizeof(boo); i++)
    if (!boo[i]->id(woo)) 
      boo[i] = nil;
    
  boo -= ({ nil });

  if (!sizeof(boo))
  {
    notify_fail(_LANG_HOLD_NOT_IN_INV);
    return 0;
  }

  // Somewhat nasty but does the trick... -Aragorn
  boo -= (mixed *)this_object()->query_held_ob();

  // Somehow, this is not working...
  if (!sizeof(boo) || boo[0]->query_in_use())
  {
    notify_fail(_LANG_HOLD_ALREADY_HOLDING);
    return 0;
  }

  if (!boo[0]->query_holdable())
  {
    notify_fail(_LANG_HOLD_CANNOT_HOLD);
    return 0;
  }

  // At the moment you can only hold one thing at the time. -Aragorn
  // if (!hold_ob(boo)) return 0;
  return hold_ob(boo[0]);
}

int do_unhold(string woo)
{
  object *boo;
  int i;

  if (!strlen(woo))
  {
    notify_fail(_LANG_UNHOLD_WHAT);
    return 0;
  }

  // AAArrrggghhhh find_match() doesn't work... -Aragorn
  // boo = find_match(woo, this_object());
  boo = all_inventory(this_object());
  
  for (i = 0 ; i < sizeof(boo); i++)
    if (!boo[i]->id(woo)) 
      boo[i] = nil;
    
  boo -= ({ nil });

  // As nasty as it gets but I'm just trying to get it to work 
  // right now. Speedups later.
  // -Aragorn
  boo -= (all_inventory(this_object())-(mixed*)this_object()->query_held_ob());

  if (!sizeof(boo))
  {
    notify_fail(_LANG_HOLD_NOT_IN_INV);
    return 0;
  }
  // At the moment you can only unhold one thing at the time. -Aragorn
  // unhold_ob (boo);
  return unhold_ob(boo[0]);
}

// stats añadido, algunos archivos con stats no llegan a devolver nada
mixed stats() {
  return ({ ({"Held ob", held_ob, }),
            ({"Free Hands Left", free_hands_left, }),
            ({"Held Ac", held_ac, }),
          });
}
