/* 
 * Reviwe of basic shield for CcMud, neverbot 22/06/03
 */

inherit "/lib/item.c";

#include <basic/condition.h>
#include <item/item.h>
#include <translations/shield.h>
#include <translations/races.h>

int enchant;
static int hands_needed;

// values from shield_table
static int basic_cost,		  // cost of basic item
           ench_basic_cost, // cost of basic item with enchant

           shield_ac,			  // ac given
           skill_malus,		  // skill malus given
           spell_failure,		// spell failure chances when wearing this armour

           material;			  // made of

static string shield_name;

void set_value(int i);

void create() 
{
  hands_needed = 1;
  enchant = 0;
  ::create();
  set_holdable(1);
  add_alias(BASIC_NAME); 
  add_plural(pluralize(BASIC_NAME));
} 

void set_base_shield(string lookup)
{
  int * data;

  if (stringp(lookup))
    shield_name = lookup;

  data = table("shields")->lookup_shield_data(lookup);

  basic_cost = data[0]; // the value is in basic currency
  ench_basic_cost = data [1];
  set_weight(data[2]);
  set_size(data[3]);

  material = data [4];
  shield_ac = data[5];
  spell_failure = data[6];
  skill_malus = data[7];

  if (data[8] == 1)
    hands_needed = 2;
  else
    hands_needed = 1;
  
  set_value(basic_cost);
}

int query_shield() { return 1; } 
string query_shield_name() {  return shield_name; }
string query_shield_type_name(){ return shield_name; }
	
int query_ac() { return (shield_ac + enchant); }
int query_shield_ac() { return query_ac(); }
// specialized shields? not for now
int query_ac_against(int type) { return 0; }

int query_material() { return material; }
int query_enchant() { return enchant; }
int query_hands_needed() { return hands_needed; }

// body localization it protects
string query_localization() { return _LANG_RACES_LOCATIONS_ARM; }

string query_material_name() {
	return table("materials")->query_material_name(material);
}

int query_skill_malus(){ return skill_malus; }
int query_spell_failure(){ return spell_failure; }

// the value is in basic currency
void set_value(int basic_cost)
{
  /* Make this simpler later.. 
   * And similar to weapon.c (not made either..:=)
   */
  if (enchant <= 0)
    ::set_value(basic_cost + (MAX_COND - cond) * COST_MULTIPLIER_TO_FIX_SHIELD);
  else
    ::set_value(ench_basic_cost * enchant + (MAX_COND - cond) * COST_MULTIPLIER_TO_FIX_SHIELD);
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

void dest_me()
{
  enchant = 0;
  // Call equip-> dest equipment here when that's done
  ::dest_me();
}

mixed * stats() 
{
  mixed * ret;
  ret = ::stats() + ({ 
  	    ({ "Enchant", enchant, }),
  	    ({ "Basic Cost (nosave)", basic_cost, }),
  	    ({ "Ench Basic Cost (nosave)", ench_basic_cost, }),
  	    ({ "Skill Malus (nosave)", skill_malus, }),
  	    ({ "Spell Failure (nosave)", spell_failure, }),
  	    ({ "Material (nosave)", material, }),
  	    ({ "Shield Name (nosave)", shield_name, }),
  	    ({ "Shield Ac (nosave)", shield_ac, }),  	    
      }); 
  return ret; 
} 

mapping query_auto_load_attributes() 
{
  if (enchant == 0)
    return ::query_auto_load_attributes();
  else
    return ::query_auto_load_attributes() +
           ([ "enchant" : enchant ]);
}

void init_auto_load_attributes(mapping args) 
{
    if (!undefinedp(args["enchant"]))
      enchant = args["enchant"];
 
    ::init_auto_load_attributes(args);
}
