/* Taniwha was here 09/94
 * extended the code to allow more types
 * so clothing like shirts can be worn with armour
 *
 * Wear.c used for wearable objects.. or rather for players to be able
 * to wear armour and other stuff.
 * Will make it possible for items to be worn even if they're not armour.
 * And also make armour heldable, not only wearable.
 * It is completely rewritten by Baldrick, april '94
 * (With a lot help from Aragorn)
 *
 * we will have different types of worn objects.
 * Body armour, Body clothing, boots/shoes and head will be one only.
 * Rings will be multipple.  Maybe add an amount_heads ? :=)
 * It won't be any problem to add new types later.
 *
 * max dex bon added, skill malus added, neverbot
 */

#include <common/properties.h>
#include <translations/armour.h>
#include <translations/combat.h> // ac types for clothing
#include <language.h>

#define MAXTYPES 20

private mixed * worn_objects;
private int * armour_types;
private int * worn_ac;
private int max_dex_bon;
private int skill_malus;

int query_skill_malus() { return skill_malus; }

int unwear_ob(object ob);
int do_unwear_ob(object on);
int wear_ob(object ob);
int do_wear_ob(object ob);
void reset_types();
object *query_worn_ob(varargs int type);
int query_max_dex_bon() { return max_dex_bon; }
int set_max_dex_bon(int i) { return max_dex_bon = i; }
int * query_total_worn_ac() { return worn_ac; }
int query_worn_ac(int type) { return worn_ac[type]; }

private void _add_worn_ac(object ob)
{
  int i;

  // ignore items with no ac
  if (undefinedp(ob->query_ac()))
    return;

  for (i = 0; i < sizeof(AC_TYPES); i++)
    worn_ac[AC_TYPES[i]] += ob->query_ac() + ob->query_ac_against(AC_TYPES[i]);
}

private void _remove_worn_ac(object ob)
{
  int i;

  // ignore items with no ac
  if (undefinedp(ob->query_ac()))
    return;

  for (i = 0; i < sizeof(AC_TYPES); i++)
    worn_ac[AC_TYPES[i]] -= (ob->query_ac() + ob->query_ac_against(AC_TYPES[i]));
}

void create()
{  
  worn_objects = ({ });
  // allocate with zero value
  worn_ac = allocate_int(sizeof(AC_TYPES));
  
  // a negative number means no max
  max_dex_bon = -1;
  reset_types();
}

void wear_commands()
{
  add_private_action("do_wear",   _LANG_WEAR_VERBS);
  add_private_action("do_unwear", _LANG_UNWEAR_VERBS);
}

object * query_worn_ob(varargs int type)
{
  int i;
  object * ret;

  ret = ({ });

  // out of range there are no items
  if (type < -1 || type > 14)
    return ({ });

  // if no type passed, return everything
  if (!type)
    return worn_objects;

  if (type == -1)
    type = 0; // to view items without type

  for (i = 0; i < sizeof(worn_objects); i++)
    if (worn_objects[i] && worn_objects[i]->query_armour_type()==type)
      ret += ({ worn_objects[i] });

  return ret;
}

void reset_types()
{
  int e;
  armour_types = allocate(MAXTYPES);

  for (e = 0; e < MAXTYPES; e++)
    armour_types[e] = 0;
}

int unwear_ob(object ob)
{
  if (!ob)
    return 0;

  if (this_object()->query_timed_property_exists(PASSED_OUT_PROP))
  {
    notify_fail(this_object()->query_timed_property(PASSED_OUT_PROP));
    return 0;
  }

  if (member_array(ob, worn_objects) == -1)
  {
    notify_fail(_LANG_UNWEAR_NOT_WEARING);
    return 0;
  }

  tell_object(this_object(), _LANG_UNWEAR_START);
  return do_unwear_ob(ob);
}

int do_unwear_ob(object ob)
{
  if (!ob)
    return 0;

  if (ob->set_in_use(0) == 0)
  {
    worn_objects -= ({ ob });
    armour_types[ob->query_armour_type()]--;
    // tell_object(this_object(), _LANG_UNWEAR_END);

    // new ac system, neverbot 06/03
    _remove_worn_ac(ob);

    // update max dex bon
    if (ob->query_max_dex_bon() >= 0)
      this_object()->recalc_max_dex_bon();

    // update skill malus
    skill_malus -= ob->query_skill_malus();
    // always negative or zero
    if (skill_malus > 0)
      skill_malus = 0;

    if (ob->query_pieces())
    {
      string main_piece;
      object main_object;

      main_piece = ob->query_main_piece();
      main_object = nil;

      // look for the main piece
      if (base_name(ob) == main_piece)
        main_object = ob; // the piece we are unwearing is the main piece
      else
      {
        // the main piece is another one
        // look through the wear
        int i;
        for (i = 0; i < sizeof(worn_objects); i++)
        {
          // tell_object(find_living("neverbot"), "-- looking for main: " + file_name(worn_objects[i]) + "\n");
          if (base_name(worn_objects[i]) == main_piece)
          {
            main_object = worn_objects[i];
            break;
          }
        }
      }

      // if the main piece exists and we are wearing it
      // (and only if we are wearing the full set)
      if (main_object && sizeof(main_object->query_piece_object_list()))
      {
        // tell_object(find_living("neverbot"), "-- main is: " + file_name(main_object) + "\n");
        main_object->set_piece_object_list( 0 );
        tell_object(this_object(), _LANG_UNWEAR_FULL_SET);
        main_object->complete_set_off(this_object());
      }
    }
  }
  else
    tell_object(this_object(), _LANG_CANNOT_UNWEAR_MSG);

  return 1;
}

int wear_ob(object ob)
{
  int a_type;
  // string *exp;
  string thisone;
  int aux, i;
  mixed local;

  /* Maybe this check is/can be another place ? */
  /* Made a query_holdable in living.c.. */
  if (this_object()->query_property(LOADING_PROP))
  {
    notify_fail(_LANG_WEAR_STILL_LOADING);
    return 0;
  }

  if (this_object()->query_timed_property_exists(PASSED_OUT_PROP))
  {
    notify_fail(this_object()->query_timed_property(PASSED_OUT_PROP));
    return 0;
  }

  // localization system adapted to CcMud, neverbot 4/03
  local = this_object()->obtain_location();

  if (local && local[2] && (local[2] != ob->query_body_type())) 
  {
    notify_fail(_LANG_WEAR_WRONG_BODY_TYPE);
    return 0;
  }

  // if (this_object()->query_guild_ob() &&
  //    !this_object()->query_guild_ob()->
  //     query_legal_armour(ob->query_armour_name()))
  // {
  //   tell_object(this_object(), "cannot wear.\n");
  //   return 1;
  // }

  notify_fail(_LANG_WEAR_SAME_TYPE);

  thisone = base_name(ob);

  // only one copy of each object (i.e. can't wear the same ring in each hand)
  for (i = 0; i < sizeof(worn_objects); i++)
  {
    if (thisone == base_name(worn_objects[i]))
        return 0;
  }

  // max 4 amulet/necklace (type 5)
  // max 2 rings, so in case they are magic, only one type of magic per hand (type 7)
  // max two pendants (type 12)
  // max 4 tied objects (bagpack, quiver, etc) (type 13)
  a_type = (int)ob->query_armour_type();
  aux = 1;
  switch(a_type)
  {
    case 5:  if (armour_types[a_type] >= 4) aux = 0; break;
    case 7:  if (armour_types[a_type] >= 2) aux = 0; break;
    case 12: if (armour_types[a_type] >= 2) aux = 0; break;
    case 13: if (armour_types[a_type] >= 4) aux = 0; break;
    default: if (armour_types[a_type])      aux = 0; break;
  }

  if (!aux)
      return 0;

  if (!(aux = ob->query_size()))
  {
    notify_fail(_LANG_WEAR_NO_SIZE);
    return 0;
  }

  if (ob->set_in_use(1))
  {
    worn_objects += ({ ob });
    armour_types[a_type]++;
    // aux = aux / 5;
    tell_object(this_object(), _LANG_WEAR_START);

    // new ac system, neverbot 06/03
    _add_worn_ac(ob);

    // update max dex bonus
    if (max_dex_bon < 0)
       max_dex_bon = ob->query_max_dex_bon();
    else
       if ((ob->query_max_dex_bon() != -1) && (ob->query_max_dex_bon() < max_dex_bon))
         max_dex_bon = ob->query_max_dex_bon();

    // update skill malus
    skill_malus += ob->query_skill_malus();
    // always negative or zero
    if (skill_malus > 0)
      skill_malus = 0;

    // sets of items, neverbot
    // TO DO
    // this code is not optimal, it traverses two (short) lists and can be improved
    if (ob->query_pieces())
    {
      object main;
      object * set;
      string * pieces;
      string * wearing;
      int found;
      int position, central_piece;

      main = nil;
      set = ({ });
      pieces = ({ });
      wearing = ({ });
      found = 0;

      if (ob->query_main_piece() && (main = load_object(ob->query_main_piece())) )
      {
        // store the file names of worn items
        for (i = 0; i < sizeof(worn_objects); i++)
        {
          wearing += ({ base_name(worn_objects[i]) });
          // tell_object(find_living("neverbot"), "-- wearing: " + base_name(worn_objects[i]) + "\n");
        }

        pieces = main->query_piece_list();

        for (i = 0; i < sizeof(pieces); i++)
        {
          // tell_object(find_living("neverbot"), "-- comparing: " + pieces[i] + "\n");

          if ((position = member_array(pieces[i], wearing)) != -1)
          {
            // how many pieces of the set we are wearing
            found += 1;
            // tell_object(find_living("neverbot"), "-- add to the set: " + file_name(worn_objects[position]) + "\n");

            // store real objects (not file names) of the set
            set += ({ worn_objects[position] });

            // index of the main piece if the set
            if (worn_objects[position]->is_main_piece())
              central_piece = sizeof(set) - 1;
          }
          else
            // not present, stop searching
            break;
        }

        if ( (found > 0) && (found == sizeof(pieces)) )
        {
          set[central_piece]->set_piece_object_list(set);
          tell_object(this_object(), _LANG_WEAR_FULL_SET);
          set[central_piece]->complete_set_on(this_object());
        }
      }
    }

    // only for players
    if (this_object()->query_player())
      // not in the first minute of the connection (usually they equip)
      if (time() - this_object()->query_last_log_on() > 60)
        if (this_object()->query_time_remaining(PASSED_OUT_PROP) < aux/6)
            this_object()->add_timed_property(PASSED_OUT_PROP, _LANG_WEAR_PASSED_OUT_MSG, aux/6);

    return 1;
  }

  notify_fail(_LANG_CANNOT_WEAR_MSG);
  return 0;
} /* wear_ob */

int do_wear(string woo)
{
  object *boo;
  int i;
  int size;

  if (!strlen(woo))
  {
    notify_fail(_LANG_WEAR_WHAT);
    return 0;
  }

  boo = find_match(woo, this_object());

  if (!sizeof(boo))
  {
    notify_fail(_LANG_WEAR_NOT_IN_INV);
    return 0;
  }

  // Somewhat nasty but does the trick... -Aragorn
  /* what does this do ? Baldy..
   * It gives me an error message here, but not in hold.c */
  boo -= (mixed *)this_object()->query_worn_ob();

  if (sizeof(boo))
  {
    if (!boo[0]->query_wearable())
    {
      notify_fail(_LANG_WEAR_NOT_WEARABLE);
      return 0;
    }

    if (boo[0]->query_in_use())
    {
      notify_fail(_LANG_WEAR_ALREADY_WEARING);
      return 0;
    }

    if (size = boo[0]->query_size() == 0)
    {
      notify_fail(_LANG_WEAR_NO_SIZE);
      return 0;
    }

    size = size / 2;
    if (size < 1)
      size = 1;

    return wear_ob(boo[0]);
  }
  return 1;
}

int do_unwear(string woo)
{
  object *boo;
  int i;

  if (!strlen(woo))
  {
    notify_fail(_LANG_UNWEAR_WHAT);
    return 0;
  }

  boo = find_match(woo, this_object());

  // As nasty as it gets but I'm just trying to get it to work
  // right now. Speedups later.
  // -Aragorn
  boo -= (all_inventory(this_object())-(mixed*)this_object()->query_worn_ob());

  if (!sizeof(boo))
  {
    notify_fail(_LANG_WEAR_NOT_IN_INV);
    return 0;
  }

   // At the moment you can only unwear one thing at the time. -Aragorn
   return unwear_ob(boo[0]);
}

mixed stats()
{
  return ({
      ({"Worn Objects (nosave)", worn_objects, }),
      ({"Armour Types (nosave)", armour_types, }),
      ({"Worn Ac (nosave)", worn_ac, }),
      ({"Max Dex Bon (nosave)", max_dex_bon, }),
      ({"Skill Malus (nosave)", skill_malus, }),
        });
}
