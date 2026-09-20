/* 
 * New combat styles system for CcMud, neverbot 27/4/2003
 * ([ style_id : ({ style_name, num_dice, dice_type, message_list }) ])
 *
 * style_id is just a word in english, to store with the player data
 * style_name will be translated 
 *
 * message_list is a list of arrays
 *   initial list
 *   medium list
 *   expert list
 *   even more expert list
 *   ... (any number > 1)
 *
 * The list used will depend of the style ability of the attacker
 * i.e. if we have three lists, the messages will change with ability
 * 33% and 66%
 *
 * Every list has three elements: message for the attacker, for the
 * defender and for the room. If any of this elements is a list, a random
 * message will be taken from them.
 *
 */

#include <translations/combat.h>
#include <living/combat.h>
#include <language.h>

// save file
// #define SAVEFILE "/save/unarmed_combat_handler"

mapping unarmed_combat_styles;

#define DEFAULT_NUM_DICE 1
#define DEFAULT_DICE_SIZE 3

void create()
{
  // seteuid("Room");
  unarmed_combat_styles = ([ 
     DEF_UNARMED_STYLE_NAME : ({ _LANG_UNARMED_DEFAULT_STYLE, DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
         ({
            ({ // messages up to 33%
               _LANG_UNARMED_INITIAL_ATT_MSGS, 
               _LANG_UNARMED_INITIAL_DEF_MSGS, 
               _LANG_UNARMED_INITIAL_ROOM_MSGS, 
            }),
            ({ // messages up to 66%
               _LANG_UNARMED_MEDIUM_ATT_MSGS, 
               _LANG_UNARMED_MEDIUM_DEF_MSGS, 
               _LANG_UNARMED_MEDIUM_ROOM_MSGS, 
            }),
            ({ // messages from 66% to 100%
               _LANG_UNARMED_EXPERT_ATT_MSGS, 
               _LANG_UNARMED_EXPERT_DEF_MSGS, 
               _LANG_UNARMED_EXPERT_ROOM_MSGS, 
            }),
         }),
       }),

    // "test" : ({ 1, 4, 
    //      ({
    //        ({ "haces daño", "te hace daño", "hace daño", }),
    //        ({ "haces mucho daño", "te hace mucho daño", "hace mucho daño", }),
    //      }),
    //    }),

    // *****************************
    //   for npcs
    // *****************************

    "colmillos" : ({ _LANG_UNARMED_FANGS_STYLE, DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE,
       ({ ({ _LANG_UNARMED_FANGS_ATT_MSGS,
              _LANG_UNARMED_FANGS_DEF_MSGS,
              _LANG_UNARMED_FANGS_ROOM_MSGS, }), }),
                }),
    "garras" : ({ _LANG_UNARMED_CLAWS_STYLE, DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE,
       ({ ({ _LANG_UNARMED_CLAWS_ATT_MSGS,
              _LANG_UNARMED_CLAWS_DEF_MSGS,
              _LANG_UNARMED_CLAWS_ROOM_MSGS, }), }),
                }),
    "pico" : ({ _LANG_UNARMED_BEAK_STYLE, DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE,
       ({ ({ _LANG_UNARMED_BEAK_ATT_MSGS,
              _LANG_UNARMED_BEAK_DEF_MSGS,
              _LANG_UNARMED_BEAK_ROOM_MSGS, }), }),
                }),
    "cuernos" : ({ _LANG_UNARMED_HORNS_STYLE, DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE,
       ({ ({ _LANG_UNARMED_HORNS_ATT_MSGS,
              _LANG_UNARMED_HORNS_DEF_MSGS,
              _LANG_UNARMED_HORNS_ROOM_MSGS, }), }),
                }),
    "pezuñas" : ({ _LANG_UNARMED_HOOVES_STYLE, DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE,
       ({ ({ _LANG_UNARMED_HOOVES_ATT_MSGS,
              _LANG_UNARMED_HOOVES_DEF_MSGS,
              _LANG_UNARMED_HOOVES_ROOM_MSGS, }), }),
                }),
    "dientes" : ({ _LANG_UNARMED_TEETH_STYLE, DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE,
       ({ ({ _LANG_UNARMED_TEETH_ATT_MSGS,
              _LANG_UNARMED_TEETH_DEF_MSGS,
              _LANG_UNARMED_TEETH_ROOM_MSGS, }), }),
                }),
    "zarpas" : ({ _LANG_UNARMED_PAWS_STYLE, DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE,
       ({ ({ _LANG_UNARMED_PAWS_ATT_MSGS,
              _LANG_UNARMED_PAWS_DEF_MSGS,
              _LANG_UNARMED_PAWS_ROOM_MSGS, }), }),
                }),
    "cola" : ({ _LANG_UNARMED_TAIL_STYLE, DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE,
       ({ ({ _LANG_UNARMED_TAIL_ATT_MSGS,
              _LANG_UNARMED_TAIL_DEF_MSGS,
              _LANG_UNARMED_TAIL_ROOM_MSGS, }), }),
                }),
    "patas" : ({ _LANG_UNARMED_LEGS_STYLE, DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE,
       ({ ({ _LANG_UNARMED_LEGS_ATT_MSGS,
              _LANG_UNARMED_LEGS_DEF_MSGS,
              _LANG_UNARMED_LEGS_ROOM_MSGS, }), }),
                }),
   ]);
   
   // restore_object(SAVEFILE);
}

int query_valid_attack(string attack, string style)
{
  if (member_array(attack, unarmed_combat_styles[style][3])>-1)
    return 1;
  return 0;
}

string * query_unarmed_styles()
{
  return keys(unarmed_combat_styles);
}

mixed * query_unarmed_style_info(string name)
{
  if (undefinedp(unarmed_combat_styles[name]))
    return ({ });

  return unarmed_combat_styles[name];
}

int style_exists(string name)
{
  return (member_array(name, keys(unarmed_combat_styles)) != -1);
}

mapping query_all_uc_info()
{
  return unarmed_combat_styles;
}

// added functions:

// when the players attacks for the first time, or chages the combat style,
// we change the number and type of dice, neverbot 4/03
int set_damage_dice(string style, object player)
{
  // if it does not exist
  if (!unarmed_combat_styles[style])
     return 0;

  player->set_damage_dice(unarmed_combat_styles[style][1], 
                          unarmed_combat_styles[style][2]);
  return 1;
}

mixed * query_messages(string style, int ability, object att)
{
  int divs;
  // int rand;
  int list;

  list = 0;

  // empty list if the style does not exist
  if (!unarmed_combat_styles[style])
     return ({ });
     
  // divs stores how many message lists there are
  divs = sizeof(unarmed_combat_styles[style][3]);

  //  get the list we have to use depending of the attacker ability
  list = ability/(100/divs);
  
  // check maximum when we reach 100%
  if (list >= divs)
     list = divs - 1;

  // randomize among every available list?
  // rand = random(list + 1);

  // hate arrays with so many dimensions, neverbot 4/03  
  return unarmed_combat_styles[style][3][list];
}
