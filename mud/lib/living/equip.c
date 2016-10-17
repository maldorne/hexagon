/*
 * Archivo que une wear.c y hold.c
 * Tambien incluye el antiguo /global/living/armour.c, ahora eliminado (anteriormente 
 *  hold y wear se unian en armour.c)
 *
 * Tambien se incluye aqui la funcion query_living_contents, del antiguo
 *  /global/living/carrying.c, ahora eliminado.
 *
 * Nuevo sistema de combate para CcMud
 * Folken@Cc, 06/03
 *
 * Autoequiparse extraido, ahora en consent.c, Folken 08/09
 */

#include <user/player.h>
#include <item/money.h>
#include <living/equip.h>

inherit wear "/lib/living/wear";
inherit hold "/lib/living/hold";

void create()
{
  wear::create();
  hold::create();
}

void recalc_max_dex_bon()
{
  int max;
  object * obs;
  int i;

  max = 100;

  // Creamos una lista con todos los objetos empuñados y vestidos
  obs = this_object()->query_weapons_wielded() - ({ 0 });
  obs += this_object()->query_worn_ob() - ({ 0 });
  
  for (i = 0; i < sizeof(obs); i++)
    // Nos aseguramos de que el objeto tendra la funcion query_max_dex_bon
    //  (todas las armaduras y armas la tienen).
    if ((obs[i]->query_armour() || obs[i]->query_weapon()) && 
       (obs[i]->query_max_dex_bon() != -1) &&
       (obs[i]->query_max_dex_bon() < max))
      max = obs[i]->query_max_dex_bon();

  if (max != 100)
    this_object()->set_max_dex_bon(max);
  else
    this_object()->set_max_dex_bon(-1);
}

mapping query_total_equip_ac()
{ 
  // return query_total_worn_ac() + query_total_held_ac(); 
  // Tenemos que sumar los mappings:
  int i;
  mapping ret;
  string * types;
  
  ret = ([ ]);
  types = keys(query_total_worn_ac());

  for (i = 0; i < sizeof(types); i++){
    ret[types[i]] = query_total_worn_ac()[types[i]] + 
                    query_total_held_ac()[types[i]];
  }
  return ret;
}

int query_equip_ac(string tipo)
{
  return query_held_ac(tipo) + query_worn_ac(tipo);
}

// do_equip() by Aragorn
//
// Has to be slightly intelligent as the NPCs depend on this for
//
// Here is how it works (should work :-)
//
// 1. A weapon
// 2. A piece of holdable armour (holdable non-weapon with ac)
// 3. Anything holdable, starting with weapons, then armour, then the rest
// 4. Wearable armour (wearable objects with ac)
// 5. Wearable objects
//
// This means we're going to have a _lot_ of arrays here to do a basic
// job. Luckily equip is only called every blue moon. :-)

/* Ok, Aragorn is finished, time for me to clean it up and fix the bugs,
 * he forgot to put query_wearable & query_holdable in here, so everything is..
 * He had holdable, but not wearable..
 */
int do_equip(string str)
{
   object *obs, *holds, *wears;
   int i, j;

   // Assum going to tell us something about the autoequip
   if (str && stringp(str)) 
   {
      // Now in consent
      // if (str == "off") {
      //    this_object()->remove_property(AUTOEQUIP_PROP);
      //    tell_object(this_object(),"Equiparte automáticamente al entrar desactivado.\n");
      // } else if (str == "on") {
      //    this_object()->add_property(AUTOEQUIP_PROP, 1);
      //    tell_object(this_object(),"Equiparte automáticamente al entrar activado.\n");
      // } else
      //    tell_object(this_object(),"Sintaxis: equipar [on|off].\n"+
      //                              "         (opcionales: on/off son para equiparte automáticamente al entrar).\n");
      tell_object(this_object(),"Sintaxis: equipar o equiparse.\n"+
                                "Para equiparse automáticamente al entrar, utiliza 'consentir'.\n");
      return 1;
   }

   // Don't blow your top, the empty array is a shared array. No memory wasted.
   holds = wears = ({ });

   // Get the objects from the player, we don't care about what he's got
   // in various containers
   obs = all_inventory(this_object());

   // Split into holdables and wearables
   for (i = 0; i < sizeof(obs); i++)
   {
     if (!interactive(this_object()) && obs[i]->query_static_property(PC_GAVE_NPC_PROPERTY)) 
      continue;

     if ( (obs[i]->query_holdable()) && !obs[i]->query_in_use() )
       holds += ({ obs[i] });

     // Items can be both not_holdable & not_wearable, so we have to 
     // check both.
     if ((obs[i]->query_wearable()) && !obs[i]->query_in_use() )
       wears += ({ obs[i] });
   }

   // First of all, take care of holdables.
   // The 'if' here is to save a few for-loop checks later
   if (sizeof(holds)) 
   {
     object *wpns, *harms;

     wpns = harms = ({ });
     // Split in weapons, holdable armour and other objects
     for (i = 0; i < sizeof(holds); i++)
       if (holds[i]->query_weapon())
         wpns += ({ holds[i] });
       // Weapons can have AC, but they are taken care of by the above 
       // else if (holds[i]->query_ac_type()) // Folken
       else if (holds[i]->query_ac() && !holds[i]->query_armour())
         harms += ({ holds[i] });

     holds = holds - wpns - harms;
     
     // Wield a weapon, remember to check if it's actually wielded
     // as it may be a twohanded weapon (or worse) and the person
     // may only have 1 hand. I.e. a loop...
     for (j = 0; j < sizeof(wpns); j++)
       if (this_object()->hold_ob(wpns[j]))
       {
         wpns -= ({ wpns[j] });
         break;
        }

     // Time for a shield of some sort.
     // Note the flaw in the logic:
     //  Someone as a twohanded weapon, a onehanded weapon and a shield.
     //  Assuming the person has 2 hands the two handed weapon _might_
     //  get wielded. Would 1 weapon and 1 shield be better?
     for (j = 0; j < sizeof(harms); j++)
       if (this_object()->hold_ob(harms[j]))
       {
         harms -= ({ harms[j] });
         break;
       }

     // Now just pile the rest of the objects in, starting with weapons
     // Note that we can't really find out if the hold array is full or
     // not. Need another function in hold.c for that.
     // As the number of objects in someone inv tend to be low, this ain't
     // too bad...
     for (j = 0; j < sizeof(wpns); j++) 
             this_object()->hold_ob(wpns[j]);

     // More holdable armour, same problem as above.
     for (j = 0; j < sizeof(harms); j++) 
             this_object()->hold_ob(harms[j]);

     // holding rest of junk (torches, whatever)
     for (j = 0; j < sizeof(holds); j++) 
             this_object()->hold_ob(holds[j]);

     // End of the line for holdable objects. Either we've got
     // no hands left or no holdable objects left.
   }
   
   // Then put some clothes/armour on the naked critter.
   if (sizeof(wears))
   {
     object *warms;
     warms = ({ });

     // Split in wearable armours and other wearables
     for (i = 0; i < sizeof(wears); i++)
      // if (wears[i]->query_ac_type())
      if (wears[i]->query_ac())
        warms += ({ wears[i] });
     // Subtract one from the other to split the original array
     wears -= warms;
     
     // First of all, we burn through the loop of wearable armour
     for ( j = 0; j < sizeof(warms); j++) 
       this_object()->wear_ob(warms[j]);
    
    // Then anything without ac.
    for ( j = 0; j < sizeof(wears); j++) 
     this_object()->wear_ob(wears[j]);
    // End of line here as well.
    // No more objects to wear or no more slots free.
   }
   // Mission accomplished
   return 1;
}

// *************************************************************
//   Antiguo /global/living/carrying.c
// *************************************************************

private int sort(object s1, object s2)
{
  if (s1->query_armour_type()>s2->query_armour_type()) 
    return 1;
  if (s1->query_armour_type()<s2->query_armour_type()) 
    return -1;
  return 0;
}

private int filtro(object ob)
{
  string str;
  str = ob->query_short();

  return (str && str!="");
}

// This function returns a string containing the formatted output
// of someones inventory. Neatly split up into wielded objects,
// held objects, worn objects and carried objects. And money.
// 'Someone' includes players, npc and corpses.
//
// A few extra lines here and we can have info like:
//   - Which hand something is held/wielded in.
//   - If the object is 1 handed, 2 handed, etc etc
//
// 'self' is true if someone is looking at himself. Basically
// changes a few minor things like:
//    - 'You' instead of 'Playername'
//    - Show exact amount of money instead of the usual messages
//     like 'His purse is fit to burst!'
string query_living_contents(int self)
{
  object *wpn, *held, *worn, *carry, money;
  string s, ret, *strs;
  int i, j, col;
  // Tipos de Armaduras existentes
  string * armour_type_locations;

  armour_type_locations = ({ 
          BODY_OBJECT,      
          SHIELD_OBJECT,    
          HEAD_OBJECT,      
          FEET_OBJECT,      
          NECK_OBJECT,      
          SHOULDERS_OBJECT, 
          FINGER_OBJECT,    
          HANDS_OBJECT,     
          ARM_OBJECT,       
          BELT_OBJECT,      
          LEGS_OBJECT,      
          PENDANT_OBJECT,   
          // This one must be the last
          TIED_OBJECT, // Wearable in a non specific position
          // UNIDENTIFIED_OBJECT,  // Unidentified object
       });

  s = "";
  strs  = ({ });
  carry = ({ });

  wpn  = this_object()->query_weapons_wielded();  // Weapons
  held = this_object()->query_held_ob();          // Every held object (includes weapons)
  worn = this_object()->query_worn_ob();          // Worn objects

  if (!pointerp(wpn))  wpn  = ({ }); else wpn  -= ({ 0 });
  if (!pointerp(held)) held = ({ }); else held -= ({ 0 });
  if (!pointerp(worn)) worn = ({ }); else worn -= ({ 0 });

  held -= wpn;
  worn = sort_array(worn, "sort");
  money = present(MONEY_NAME, this_object()); // Dinero
  carry = all_inventory(this_object()) - wpn - held 
          - worn - ({ money });               // Resto de objetos
  carry = filter(carry, "query_short");       // Elimino objetos "ocultos",
                                              // quedandonos solo con los que
                                              // responden a la funcion
                                              // query_short

  if (sizeof(wpn))  
    strs += ({ sprintf( "%-11s", "Empuñando"), capitalize(query_multiple_short(wpn, 1)) + "."});

  // Supondremos que la primera arma esta en la derecha y la segunda
  // en la izquierda:
  // if (sizeof(wpn)){
  //   if (sizeof(wpn) == 2){
  //      strs+=({"Empuñando  ", capitalize(wpn[0]->pretty_short())+
  //                       " (mano derecha).\n             "+
  //                       capitalize(wpn[1]->pretty_short())+
  //                       " (mano izquierda)."});
  //   }
  //   else{
  //     strs += ({"Empuñando  ",  query_multiple_short(wpn, 1) + "."});
  //   }
  // }

  if (sizeof(held)) 
    strs += ({ sprintf( "%-11s", "Sosteniendo"), capitalize(query_multiple_short(held, 1)) + "."});
          
  if (sizeof(worn))
  {
    // Quito los objetos de tipo 0
    wpn   = this_object()->query_worn_ob(-1);
    ret   = query_multiple_short(wpn, 1);
    worn -= wpn;

    if (ret != "") 
      ret += ".";

    strs += ({ sprintf( "%-11s", "Llevando"), ret });

    col = 0;
    wpn = ({ });
    
    for (i = 0; i < sizeof(armour_type_locations); i++)
    {
      wpn = ({ });
      for (j = 0; j < sizeof(worn); j++)
      {
        if (worn[j]->query_armour_type() == i+1)
        {
          wpn += ({ worn[j] });
        }     
      }
      if (sizeof(wpn))
      {
        if (armour_type_locations[i] != TIED_OBJECT)
          strs += ({ sprintf( "%-11s", " * " + armour_type_locations[i] ), query_multiple_short(wpn, 1) + ".", });
        else
          strs += ({ sprintf( "%-11s", "Atados"), query_multiple_short(wpn, 1) + ".", });
      }
    }
  }
  
  if (sizeof(carry) && (self != 2))  
    strs += ({ sprintf( "%-11s", "Cargando"), query_multiple_short(carry, 1) + "." });
            // sprintf(
            //         "%-=*s", 
            //         (this_object()?(this_object()->query_cols()):79), 
            //         query_multiple_short(carry) + "." 
            //        ); 

  // col = this_player()?(this_player()->query_cols()-13):79-13;

  for (i = 0; i < sizeof(strs); i++)
  {
    s += strs[i] + ": " + strs[++i] + "\n";
      // s += sprintf("%11-=s: %-=*s", strs[i], col,
      //           capitalize(strs[++i])) + "\n";
  }

  if (self == 1)
  {
     if (!sizeof(strs))
     {
        s = "Estás con las manos vacías.\n";
     }
     ret = (money?money->short(0):0);

   if (!ret)
       s += "No llevas dinero encima.";
     else
       s += sprintf(
           "%-=*s", 
           (this_player()?(this_player()->query_cols()):79), 
           "Llevas encima " + ret+".");     
  }
  else  
    if (money) 
    {
      switch(money->query_number_coins())
      {
        case 0..5:     s += "No parece llevar dinero encima.";     break;
        case 6..50:    s += "Apenas parece llevar dinero encima."; break;
        case 51..300:  s += "Parece estar cargado de monedas.";       break;
        default:       s += "¡Lleva una bolsa de monedas a punto de reventar!";
      }
    }
  
  // Evitamos un retorno de carro de mas cuando un jugador/npc
  //  no lleva nada en el inventario
  if (s != "") 
    s += "\n";
  return s;
}

mixed stats() 
{
  return wear::stats() + hold::stats();
}
