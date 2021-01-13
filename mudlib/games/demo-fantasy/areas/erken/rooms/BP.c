inherit "std/pub";
#include "pub.h"
object board, arak;
void setup()
{
   add_menu_item("Licor 'Sangre Orca'", ALCOHOL, 20, 2, 300, 20,
      "bebes de un trago un baso de licor", "se bebe un basito de licor");
   add_menu_item("Cerveza Negra", ALCOHOL, 30, 3, 50, 50,
      "bebes toda la Cerveza Negra", "se bebe una Cerveza Negra de un trago");
   add_menu_item("Whisky Maltes", ALCOHOL, 100, 6, 100, 100,
      "empieza a arder un poco la garganta", "se bebe de un tiro un Whisky" );
   add_menu_item("Aguardiente", ALCOHOL, 150, 15, 200, 200,
      "bebes un vaso de Aguardiente", "saborea un vaso de Aguardiente");
   add_menu_item("Tripas", FOOD, 50, 10, 40, 0, "comes asquerosas tripas de al"
      "gun tipo de animal. Ummh, no tiene mal sabor", "se zampa tripas");
   add_menu_item("Costillas", FOOD, 30, 5, 40, 0, "comes unas costillas asadas"
      " de algun tipo de animal", "se zampa una racion de costillas ");
   add_menu_alias("licor", "Licor 'Sangre Orca'");
   add_menu_alias("cerveza negra", "Cerveza Negra");
   add_menu_alias("whisky", "Whisky Maltes");
   add_menu_alias("maltes", "Whisky Maltes");
   add_menu_alias("whisky maltes", "Whisky Maltes");
   add_menu_alias("aguardiente", "Aguardiente");
   add_menu_alias("tripas", "Tripas");
   add_menu_alias("costillas", "Costillas");
   
   set_short("%^RED%^El Cordero Degollado%^RESET%^");
   set_long("Encima de la puerta de entrada hay un cartel colgando. "
            "La anarquia es el plato del dia, los numerosos goblins borrachos"
            " suelen montar grandes bacanales y disturbios. Suele ser un lugar"
            " bastante sucio y peligroso. A pesar de ello la comida parece"
            " buena.\n"
            "Puedes ver un gran cartel donde se expone el Menu.\n");
   
   add_exit(DIR_OUT, ROOMS + "B5");
   set_light(40);
   add_item("cartel", "En el puedes leer 'El Cordero Degollado'.\n");
   add_property("no_undead", 1);
   set_open_condition("BARMAN");
}
 
int BARMAN()
{
  object *lista;
  int i;
  if (!arak) return 0;
  lista=arak->query_attacker_list();
  for(i=0;i<sizeof(lista);i++)
   if (environment(lista[i])==this_object()) return 0;
  return 1;
}         
    
void reset()
{
   if (!arak)
   {
      arak=new("/d/newbie/npc/arak.c");
      arak->move(this_object());
   }
   if (!board)
   {
      board=clone_object("/obj/misc/board");
      board->set_datafile("playerinfo");
      board->move(this_object());
   }
 ::reset();
}
 
void dest_me()
{
 if (arak) arak->dest_me();
 if (board) board->dest_me();
 ::dest_me();
}
 
