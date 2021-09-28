/* 
 * New combat styles system for CcMud, neverbot 27/4/2003
 * ([ style_name : ({ num_dice, dice_type, message_list }) ])
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
    DEF_UNARMED_STYLE_NAME : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
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

    "colmillos" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
       ({ ({ "muerdes con tus colmillos", "te muerde con sus colmillos", "muerde con sus colmillos", }), }),
                }),       
    "garras" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
       ({ ({ "arañas con tus garras", "te araña con sus garras", "araña con sus garras", }), }),
                }),       
    "pico" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
       ({ ({ "golpeas con el pico", "te golpea con el pico", "golpea con el pico", }), }),
                }),       
    "cuernos" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
       ({ ({ "corneas", "te cornea", "cornea", }), }),
                }),       
    "pezuñas" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
       ({ ({ "golpeas con las pezuñas", "te golpea con sus pezuñas", "golpea con sus pezuñas", }), }),
                }),       
    "dientes" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
       ({ ({ "muerdes", "te muerde", "muerde", }), }),
                }),       
    "zarpas" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
       ({ ({ "das un zarpazo", "te da un zarpazo", "da un zarpazo", }), }),
                }),       
    "cola" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
       ({ ({ "das un coletazo", "te da un coletazo", "da un coletazo", }), }),
                }),       
    "patas" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
       ({ ({ "golpeas con tus patas", "te golpea con sus patas", "golpea con sus patas", }), }),
                }),       

   ]);
   
   // restore_object(SAVEFILE);
}

int query_valid_attack(string attack, string style)
{
  if(member_array(attack, unarmed_combat_styles[style][2])>-1)
    return 1;
  return 0;
}

string * query_unarmed_styles()
{
  return keys(unarmed_combat_styles);
}

int style_exists(string name)
{
    return (member_array(name, keys(unarmed_combat_styles)) != -1);
}

mapping query_all_uc_info(){
  return unarmed_combat_styles;
}

// added functions:

// when the players attacks for the first time, or chages the combat style,
// we change the number and type of dice, neverbot 4/03
int set_damage_dice(string style, object player)
{

    stderr("-----> " + style + to_string(player) + "\n");

  // if it does not exist
  if (!unarmed_combat_styles[style])
     return 0;

  player->set_damage_dice(unarmed_combat_styles[style][0], 
                          unarmed_combat_styles[style][1]);
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
  divs = sizeof(unarmed_combat_styles[style][2]);

  //  get the list we have to use depending of the attacker ability
  list = ability/(100/divs);
  
  // check maximum when we reach 100%
  if (list >= divs)
     list = divs - 1;

  // randomize among every available list?
  // rand = random(list + 1);

  // hate arrays with so many dimensions, neverbot 4/03  
  return unarmed_combat_styles[style][2][list];
}
