/*
 * This file merges wear.c and hold.c
 * 
 * old /global/living/armour.c now here too (long time ago hold and wear were 
 * inherited in armour.c)
 *
 * query_living_contents added here from /global/living/carrying.c, 
 * now removed
 *
 * new combat system for CcMud
 * neverbot@Cc, 06/03
 *
 * autoequip removed from here, now in consent.c, neverbot 08/09
 */

#include <basic/money.h>
#include <common/properties.h>
#include <translations/equip.h>
#include <living/combat.h> // ac types
#include <language.h>

inherit wear "/lib/living/wear";
inherit hold "/lib/living/hold";

void create()
{
  wear::create();
  hold::create();
}

void equip_commands()
{
  add_private_action("do_equip", _LANG_EQUIP_VERBS);
}

void recalc_max_dex_bon()
{
  int max;
  object * obs;
  int i;

  max = 100;

  // create a list with every wielded and worn object
  obs = this_object()->query_weapons_wielded() - ({ nil });
  obs += this_object()->query_worn_ob() - ({ nil });

  for (i = 0; i < sizeof(obs); i++)
    // be sure the object has the query_max_dex_bon function
    //  (every armour and weapon have it)
    if ((obs[i]->query_armour() || obs[i]->query_weapon()) &&
       (obs[i]->query_max_dex_bon() != -1) &&
       (obs[i]->query_max_dex_bon() < max))
      max = obs[i]->query_max_dex_bon();

  if (max != 100)
    this_object()->set_max_dex_bon(max);
  else
    this_object()->set_max_dex_bon(-1);
}

int * query_total_equip_ac()
{
  int i;
  int * ret, * worn, * held;

  worn = query_total_worn_ac();
  held = query_total_held_ac();

  ret = allocate_int(sizeof(AC_TYPES));

  for (i = 0; i < sizeof(AC_TYPES); i++)
  {
    ret[i] = worn[i] + held[i];
  }

  return ret;
}

int query_equip_ac(int tipo) { return query_held_ac(tipo) + query_worn_ac(tipo); }

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

// Ok, Aragorn is finished, time for me to clean it up and fix the bugs,
// he forgot to put query_wearable & query_holdable in here, so everything is..
// He had holdable, but not wearable..

int do_equip(string str)
{
  object *obs, *holds, *wears;
  int i, j;

  // Assum going to tell us something about the autoequip
  if (strlen(str))
  {
     // Now in configuration
     // if (str == "off") ...
     tell_object(this_object(), _LANG_EQUIP_SYNTAX);
     return 1;
  }

  if (this_object()->query_timed_property_exists(PASSED_OUT_PROP))
  {
    notify_fail(this_object()->query_timed_property(PASSED_OUT_PROP));
    return 0;
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
      // else if (holds[i]->query_ac_type()) // neverbot
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
//  old /global/living/carrying.c
// *************************************************************

int filter_short(object ob, varargs mixed args...)
{
  return (strlen(ob->query_short()));
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
  object * wpn, * held, * worn, * carry, money;
  string s, ret, * strs;
  int i, j, col;
  // types of existing armours 
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

  if (!pointerp(wpn))  wpn  = ({ }); else wpn  -= ({ nil });
  if (!pointerp(held)) held = ({ }); else held -= ({ nil });
  if (!pointerp(worn)) worn = ({ }); else worn -= ({ nil });

  held -= wpn;
  // use the sort function of the armour table
  worn = sort_array(worn, "sort_armours", table("armours"));
  money = present(MONEY_NAME, this_object()); // money
  carry = all_inventory(this_object()) - wpn - held - worn - ({ money }); // rest of items
  carry = filter(carry, "filter_short");      // remove hidden items, remain only those responding
                                              // to query_short

  if (sizeof(wpn))
    strs += ({ sprintf( "%-11s", _LANG_EQUIP_WIELDING), capitalize(query_multiple_short(wpn, 1)) + "."});

  // Guess first weapon is in the right/main hand and second in the left/seconday hand:
  // if (sizeof(wpn)){
  //   if (sizeof(wpn) == 2){
  //      strs+=({_LANG_EQUIP_WIELDING, capitalize(wpn[0]->pretty_short())+
  //                       " (right hand).\n             "+
  //                       capitalize(wpn[1]->pretty_short())+
  //                       " (left hand)."});
  //   }
  //   else{
  //     strs += ({_LANG_EQUIP_WIELDING,  query_multiple_short(wpn, 1) + "."});
  //   }
  // }

  if (sizeof(held))
    strs += ({ sprintf( "%-11s", _LANG_EQUIP_HOLDING), capitalize(query_multiple_short(held, 1)) + "."});

  if (sizeof(worn))
  {
    // remove items with type 0
    wpn   = this_object()->query_worn_ob(-1);
    ret   = query_multiple_short(wpn, 1);
    worn -= wpn;

    if (ret != "")
      ret += ".";

    strs += ({ sprintf( "%-11s", _LANG_EQUIP_WEARING), ret });

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
          strs += ({ sprintf( "%-11s", _LANG_EQUIP_TIED), query_multiple_short(wpn, 1) + ".", });
      }
    }
  }

  if (sizeof(carry) && (self != 2))
    strs += ({ sprintf( "%-11s", _LANG_EQUIP_CARRYING), query_multiple_short(carry, 1) + "." });

  for (i = 0; i < sizeof(strs); i++)
  {
    s += strs[i] + ": " + strs[++i] + "\n";
    // s += sprintf("%11-=s: %-=*s", strs[i], col,
    //           capitalize(strs[++i])) + "\n";
  }

  if (self == 1)
  {
    if (!sizeof(strs))
      s = _LANG_EQUIP_EMPTY_HANDS + "\n";

    ret = (money ? money->short(0) : "");

    if (!strlen(ret))
      s += _LANG_EQUIP_NO_MONEY;
    else
      s += sprintf("%-*s", this_user()->query_cols(), _LANG_EQUIP_MONEY);
  }
  else
    if (money)
    {
      switch(money->query_number_coins())
      {
        case 0..5:     s += _LANG_EQUIP_OTHER_MONEY_1; break;
        case 6..50:    s += _LANG_EQUIP_OTHER_MONEY_2; break;
        case 51..300:  s += _LANG_EQUIP_OTHER_MONEY_3; break;
        default:       s += _LANG_EQUIP_OTHER_MONEY_4;
      }
    }

  // avoid extra line when empty inventory
  if (s != "")
    s += "\n";
  return s;
}

mixed stats()
{
  return wear::stats() + hold::stats();
}
