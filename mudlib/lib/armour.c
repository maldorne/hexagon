/* 
 * basic armour review for CcMud, neverbot 19/06/03
 * armour sets with multiple pieces, neverbot 07/10
 * 
 */

inherit "/lib/item.c";

#include <basic/move.h>
#include <item/item.h>
#include <translations/armour.h>
#include <living/combat.h>
#include <basic/condition.h>

#include <translations/races.h>
#include <translations/common.h>
#include <translations/combat.h> // ac types
#include <language.h>

int enchant;

/* Valores de /table/armour_table */
static int basic_cost, // cost of a basic armour of this type 
    ench_basic_cost,   // cost of a basic armour of this type with enchant

    armour_ac,         // ac given by the armour
    max_dex_bon,       // max dex bon when wearing this armour
    skill_malus,       // skill malus when wearing this armour
    spell_failure,     // spell failure chances when wearing this armour
    
    armour_type,       // armour type
    material,          // made of
    default_plural;    // should be treated as plural by default (shoes, gloves, trousers, etc)

static int * ac_bonus_against; // ac bonus against different types of weapons (slashing, blunt, piercing...)

static string localization; // body area this armour protects
static string armour_name;  // armour type (name)
string body_type;           // body type this armour fits on

// for the armour sets
static private string main_piece;          // path of the main set piece
static private string piece_set_name;      // name of the set
static private string * piece_list;        // path list of every piece
static private object * piece_object_list; // list of cloned objects
                                           //  - will only be set when we wear the full set
                                           //  - will be empty as soon as we unwear at least one piece

void set_value(int i);

void create()
{
  localization = _LANG_RACES_LOCATIONS_NOTHING;
  armour_name = DEFAULT_ARMOUR_NAME;
  enchant = 0;
  main_piece = "";
  piece_set_name = "";
  piece_list = ({ });
  piece_object_list = ({ });
  body_type = _LANG_RACES_HUMANOID_BODY;
  default_plural = 0;

  // allocate with zero value
  ac_bonus_against = allocate_int(sizeof(AC_TYPES));

  ::create();
} 

// here is the new setup routine, will make the whole a lot cleaner..
void set_base_armour(string lookup)
{
  mixed * data;
  
  if (!strlen(lookup))
    return;
  
  armour_name = lookup;

  data = table("armours")->lookup_armour_data(lookup);

  basic_cost = data[0]; // the value is in basic currency
  ench_basic_cost = data[1];
  set_weight(data[2]);
  set_size(data[3]);
  armour_ac = data[4];
  max_dex_bon = data[5];
  skill_malus = data[6];
  spell_failure = data[7];

  ac_bonus_against[SLASHING] = data[8];
  ac_bonus_against[BLUNT] = data[9];
  ac_bonus_against[PIERCING] = data[10];

  armour_type = data [11];
  material = data [12];
  
  localization = data[13]; // body localization
  body_type = _LANG_RACES_HUMANOID_BODY; // by default humanoid bodies

  // maybe not used in english, useful for other languages
  default_plural = data[14];

  // Ok, this slot is holdable or not, if it's not holdable, it's wearable
  set_wearable(1);
  set_value(basic_cost);
}

int query_armour() { return 1; } 
/* This just returns the basearmour name -- Hamlet */
string query_armour_name() { return armour_name; }
int query_armour_type() { return armour_type; }
int query_material() { return material; }
string query_localization() { return localization; }
int query_enchant() { return enchant; }

int query_ac() { return (armour_ac + enchant); }
int query_armour_ac() { return query_ac(); }
int query_ac_against(int type) { return query_ac() + ac_bonus_against[type]; }

int query_ac_bon_against(int type) { return ac_bonus_against[type]; }
void set_ac_bon_against(int type, int value) { ac_bonus_against[type] = value; }

string query_armour_type_name()
{
  return table("armours")->query_armour_type_name(armour_type);
}
string query_material_name()
{
  return table("materials")->query_material_name(material);
}

void set_material(int i) { material = i; }

void set_body_type(string str) { body_type = str; }
string query_body_type() { return body_type;  }

int query_max_dex_bon() { return max_dex_bon; }
void set_max_dex_bon(int bon){ max_dex_bon = bon; }
int query_skill_malus(){ return skill_malus; }
int query_spell_failure(){ return spell_failure; }

int query_default_plural() { return default_plural; }
void set_default_plural(int value) { default_plural = (value ? 1 : 0); }

// now basic_cost is in basic currency
void set_value(int basic_cost)
{
  /* Make this simpler later.. 
   * And similar to weapon.c (not made either..:=)
   */
  if (enchant <= 0)
      ::set_value(basic_cost + (MAX_COND - cond) * COST_MULTIPLIER_TO_FIX_ARMOUR);
  else
      ::set_value(ench_basic_cost * enchant + (MAX_COND - cond) * COST_MULTIPLIER_TO_FIX_ARMOUR);
}

/* all this code should be changed.. 
 * It can positively be simplified.
 * Can't be easier than this ? :=)
 * New code will be added later 
 */
void set_enchant(int i) 
{
  if (i > 6)
    i = 6;
  if (i < -3)
    i = -3;
  enchant = i;
  set_value(0);
}

void set_main_piece(string path) 
{ 
  object main;
  // main_piece = path; 
  if (main = load_object(path))
    main_piece = base_name(main);
}

void set_pieces(string name, string * list)
{
  int i;
  object ob;
  
  piece_set_name = name;
  // piece_list = list;

  // store in piece_list the file names 
  // do not store items which do not load
  for (i = 0; i < sizeof(list); i++)
  {
    ob = load_object(list[i]);
    
    if (ob)
      piece_list += ({ base_name(ob) });
  }
  
  // if current item file name is not in the list, we add it to avoid errors
  name = base_name(this_object());
  
  if (member_array(name, piece_list) == -1)
    piece_list += ({ name });
}

string * query_piece_list() { return piece_list; }
string query_main_piece() 
{
  if (main_piece && (main_piece != "")) 
    return main_piece; 
  
  if (sizeof(piece_list))
    return base_name(this_object());
  
  return "";
}
int is_main_piece()
{
  if (sizeof(piece_list))
    return 1;
  else
    return 0;
}

string query_piece_set_name() { return piece_set_name; }
int query_pieces()
{
  return ( (main_piece != "") || (sizeof(piece_list) > 0) );
}

void set_piece_object_list(object * list) 
{ 
  if (!list)
    piece_object_list = ({ });
  else
    piece_object_list = list; 
  /*
  int i;
  piece_object_list = ({ });
  
  for (i = 0; i < sizeof(list); i++)
    piece_object_list += ({ list [i] });
  */
}
object * query_piece_object_list() { return piece_object_list; }

// these functions will be called when the full set is worn/unworn
// they must be overwritten in the main set piece (indicated by set_main_piece 
// in every other piece)
int complete_set_on(object player) 
{ 
  return 1; 
}
int complete_set_off(object player) 
{ 
  return 1; 
}

// description modified if it is a set piece
// string long(string s, int dark)
string query_long()
{
  object main_piece;
  string * pieces;
  int i;
  string ret;

  if (!query_pieces())
    return ::query_long();
    
  if (!(main_piece = load_object(query_main_piece())))
    return ::query_long();
  
  if (!is_main_piece())
    return ::query_long() + _LANG_DESC_IS_SET_PIECE;
  
  pieces = main_piece->query_piece_list();
  
  ret = ::query_long();
  ret += _LANG_DESC_IS_MAIN_SET_PIECE;

  for (i = 0; i < sizeof(pieces); i++)
  {
    // reuse variable
    main_piece = load_object(pieces[i]);
    if (!main_piece)
      continue;
    ret += main_piece->query_short();
    if (i < sizeof(pieces) - 2)
      ret += ", ";
    else if (i == sizeof(pieces) - 2)
      ret += " " + _LANG_AND + " ";
    else 
      ret += ".\n";
  }
  
  return ret;
}

mapping query_auto_load_attributes() 
{
  return ([ 
      "::" : ::query_auto_load_attributes(),
    ]) + 
    ((body_type != _LANG_RACES_HUMANOID_BODY) ? ([ "body type" : body_type ]) : ([ ])) + 
    ((enchant != 0) ? ([ "enchant" : enchant ]) : ([ ]));
}

void init_auto_load_attributes(mapping attribute_map)
{
  if (!undefinedp(attribute_map["enchant"]))
    enchant = attribute_map["enchant"];
  else
    enchant = 0;

  if (!undefinedp(attribute_map["body type"]))
    body_type = attribute_map["body type"];
  else
    body_type = _LANG_RACES_HUMANOID_BODY;

  if (!undefinedp(attribute_map["::"]))
    ::init_auto_load_attributes(attribute_map["::"]);
}

mixed * stats() 
{
  mixed * ret;
  ret = ::stats() + ({ 
        ({ "Enchant", enchant, }),
        ({ "Basic Cost (nosave)", basic_cost, }),
        ({ "Ench Basic Cost (nosave)", ench_basic_cost, }),
        ({ "Max Dex Bon (nosave)", max_dex_bon, }),
        ({ "Skill Malus (nosave)", skill_malus, }),
        ({ "Spell Failure (nosave)", spell_failure, }),
        ({ "Ac Bonus Against (nosave)", ac_bonus_against, }),
        ({ "Armour Type (nosave)", armour_type, }),
        ({ "Material (nosave)", material, }),
        ({ "Localization (nosave)", localization, }),
        ({ "Armour Name (nosave)", armour_name, }),
        ({ "Body Type", body_type, }),
        ({ "Main Piece (nosave)", main_piece, }),
        ({ "Piece List (nosave)", piece_list, }),
        ({ "Piece Set Name (nosave)", piece_set_name, }),
        ({ "Piece Object List (nosave)", piece_object_list, }),
          }); 
  return ret; 
}
