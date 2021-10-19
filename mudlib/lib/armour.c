/* 
 * Revision de la armadura básica para CcMud, Folken 19/06/03
 * Añadidos sistemas de equipos de multiples piezas, Folken 07/10
 * 
 */

inherit "/lib/item.c";

#include <basic/move.h>
#include <item/armour.h>
#include <living/combat.h>
#include <basic/condition.h>

#include <translations/races.h>

#define COST_TO_FIX 5
#define SIZE 8

int enchant;

/* Valores de /table/armour_table */
static int basic_cost, // Coste de una armadura basica de este tipo
    ench_basic_cost,   // Coste de una armadura de este tipo con enchant

    armour_ac,     // AC que proporciona la armadura
    max_dex_bon,   // Bonificador maximo de destreza que se admite con esta
                   //   armadura
    skill_malus,   // Penalizador a las habilidades
    spell_failure, // Probabilidad de fallo de conjuro arcano
    
    bon_slash,     // Bonificador a la AC contra armas cortantes
    bon_blunt,     // Bonificador a la AC contra armas de golpe
    bon_pierc,     // Bonificador a la AC contra armas punzantes
    armour_type,   // Tipo de armadura
    material;      // Material del que esta fabricado

static string localization; // Zona del cuerpo que protege
static string armour_name;  // Nombre del tipo de armadura
string body_type;           // Tipo de cuerpo para el que vale la armadura

// Variables para el sistema de conjuntos de armaduras
static private string main_piece;     // Path de la pieza de equipo principal de este conjunto de armaduras
static private string piece_set_name; // Nombre del conjunto de todas las piezas (p ej: armadura completa, armadura de placas, etc)
static private string * piece_list;   // Lista de paths de todas las piezas que componen el conjunto
static private object * piece_object_list;  
                    // Lista de objetos que componen el conjunto 
                    // (solo se rellena cuando nos podemos el conjunto completo, y se vacia en cuanto
                    //  nos quitamos cualquiera de las piezas)

void set_value(int i);

void create()
{
  localization = "nada";
  armour_name = "armadura";
  enchant = 0;
  main_piece = "";
  piece_set_name = "";
  piece_list = ({ });
  piece_object_list = ({ });
  body_type = _LANG_RACES_HUMANOID_BODY;

  ::create();
} 

// here is the new setup routine, will make the whole a lot cleaner..
void set_base_armour(string lookup)
{
  mixed * data;
  
  if (!strlen(lookup))
    return;
  
  armour_name = lookup;

  data = table("armour_table")->lookup_armour_data(lookup);

  basic_cost = data[0]; // Atencion!! Ahora el valor es en monedas de cobre!! (basicas)
  ench_basic_cost = data[1];
  set_weight(data[2]);
  set_size(data[3]);
  armour_ac = data[4];
  max_dex_bon = data[5];
  skill_malus = data[6];
  spell_failure = data[7];

  bon_slash = data[8];
  bon_blunt = data[9];
  bon_pierc = data[10];

  armour_type = data [11];
  material = data [12];
  
  localization = data[13]; // Zona del cuerpo que protege
  body_type = _LANG_RACES_HUMANOID_BODY; // Las armaduras por defecto son para cuerpos humanoides

  // Ok, this slot is holdable or not, if it's not holdable, it's wearable
  set_wearable(1);
  set_value(basic_cost);
}

/* This just returns the basearmour name -- Hamlet */
string query_armour_name() {  return armour_name; }

int query_ac() { return (armour_ac + enchant); }
int query_armour_ac() { return query_ac(); }

int query_armour_type() { return armour_type; }
int query_material() { return material; }
string query_localization() { return localization; }
int query_enchant(){ return enchant; }
int query_armour() { return 1; } 

string query_armour_type_name()
{
  return table("armour_table")->query_armour_type_name(armour_type);
}
string query_material_name()
{
  return table("material_table")->query_material_name(material);
}

void set_material(int i) { material = i; }

void set_body_type(string str) { body_type = str; }
string query_body_type() { return body_type;  }
void set_slashing_bon(int i) { bon_slash = i; }
int query_slashing_bon() { return bon_slash; }
void set_blunt_bon(int i) { bon_blunt = i; }
int query_blunt_bon() { return bon_blunt; }
void set_piercing_bon(int i) { bon_pierc = i; }
int query_piercing_bon() { return bon_pierc; }
int query_max_dex_bon() { return max_dex_bon; }
void set_max_dex_bon(int bon){ max_dex_bon = bon; }
int query_skill_malus(){ return skill_malus; }
int query_spell_failure(){ return spell_failure; }

int query_total_ac_against(int type)
{
  if (type == BLUNT)
    return query_ac() + query_blunt_bon();
  if (type == PIERCING)
    return query_ac() + query_piercing_bon();
  if (type == SLASHING)
    return query_ac() + query_slashing_bon();
  return query_ac();
}


// Cambio importante, ahora basic_cost almacena monedas de cobre, no de oro!!!
void set_value(int basic_cost)
{
  /* Make this simpler later.. 
   * And similar to weapon.c (not made either..:=)
   */
  if (enchant <= 0)
      ::set_value(basic_cost + (MAX_COND - cond) * COST_TO_FIX);
  else
      ::set_value(ench_basic_cost * enchant + (MAX_COND - cond) * COST_TO_FIX);
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

// Cambiado por Folken, 6/03
// Estaria bien comprobar que esto no hace que las armaduras se estropeen
// demasiado rapido ni demasiado lento.
int hit_armour(int dam)
{
  int res;
  res = this_object()->adjust_cond(-1);
  if ((res <= 0) && interactive(environment(this_object())) )
    tell_player(environment(this_object()), "¡Tu "+query_short()+" se rompe en mil pedazos!\n");
    return res;
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

  // Guardamos en piece_list los nombres de archivo tal y como salen de base_name
  // para evitar comparaciones con y sin el .c final
  // No guardamos en la lista los objetos que no cargan
  for (i = 0; i < sizeof(list); i++)
  {
    ob = load_object(list[i]);
    
    if (ob)
      piece_list += ({ base_name(ob) });
  }
  
  // Si el nombre de archivo del objeto actual no esta en la lista de todas las piezas,
  // la añadimos para evitar errores
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

// Estas funciones se llamaran cuando se vista un conjunto de piezas completo
// Deben sobreescribirse en la pieza central del equipo (indicada mediante set_main_piece
//  en todos las demas piezas)
int complete_set_on(object player) 
{ 
  return 1; 
}
int complete_set_off(object player) 
{ 
  return 1; 
}


// Modificacion de la descripcion si son piezas de equipo
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
  {
      return ::query_long() + 
          "   Este objeto forma parte del conjunto: " + main_piece->query_piece_set_name() + ".\n";
  }
  
  pieces = main_piece->query_piece_list();
  
  ret = ::query_long();
  ret += "   Este objeto es la pieza central del conjunto '" + 
    main_piece->query_piece_set_name() + "', compuesto de ";

  for (i = 0; i < sizeof(pieces); i++)
  {
    // Reutilizamos la variable main_piece
    main_piece = load_object(pieces[i]);
    if (!main_piece)
      continue;
    ret += main_piece->query_short();
    if (i < sizeof(pieces) - 2)
      ret += ", ";
    else if (i == sizeof(pieces) - 2)
      ret += " y ";
    else 
      ret += ".\n";
  }
  
  // ret = sprintf("%-=*s", (this_player()?this_player()->query_cols():79), ret);
  
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
        ({ "Bon Slash (nosave)", bon_slash, }),
        ({ "Bon Blunt (nosave)", bon_blunt, }),
        ({ "Bon Pierc (nosave)", bon_pierc, }),
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
