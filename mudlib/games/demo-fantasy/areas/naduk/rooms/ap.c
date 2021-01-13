//Created by Lummen and Antiron 26-7-97
/* Escalar fixed by Oskuro 2 anyos y medio despues  *
 * Ah, tambien he indentao un poco el tema. 17SEP99 */
inherit "std/pub";
#include "pub.h"
object board,gordo,escalar;
//nombre.tipo.coste.heal.volumen.intox
void setup()
{
   add_menu_item("Cerveza", ALCOHOL, 20, 2, 10, 10,
      "bebes toda la Cerveza", "se bebe una pinta de Cerveza");
   add_menu_item("Cerveza Negra", ALCOHOL, 30, 3, 10, 15,
      "bebes toda la Cerveza Negra", "se bebe una Cerveza Negra de un trago");
   add_menu_item("Whisky Maltes", ALCOHOL, 100, 6, 100, 100,
      "empieza a arder un poco la garganta", "se bebe de un tiro un Whisky" );
   add_menu_item("Aguardiente Enanil", ALCOHOL, 120, 15, 200, 200,
      "bebes un vaso de Aguardiente Enanil", "saborea un vaso de Aguardiente Enanil");
   add_menu_item("Leche", SOFTDRINK, 10, 1, 5, 0, "bebes un refrescante vaso de leche",
      "se bebe un vasito de leche");
   add_menu_item("Tostada", FOOD, 20, 2, 40, 0, "comes unas tostadas con mermelada",
      "se zampa unas tostadas");
   add_menu_item("Tortilla de Patata", FOOD, 80, 15, 100, 0,
      "comes una tortilla de patata con ajo-aceite", "se come una tortilla de patata, Ummm!");
   add_menu_alias("cerveza", "Cerveza");
   add_menu_alias("cerveza negra", "Cerveza Negra");
   add_menu_alias("whisky", "Whisky Maltes");
   add_menu_alias("maltes", "Whisky Maltes");
   add_menu_alias("whisky maltes", "Whisky Maltes");
   add_menu_alias("aguardiente", "Aguardiente Enanil");
   add_menu_alias("aguardiente enanil", "Aguardiente Enanil");
   add_menu_alias("tostada", "Tostada");
   add_menu_alias("leche", "Leche");
   add_menu_alias("tortilla", "Tortilla de Patata");
   add_menu_alias("tortilla de patata", "Tortilla de Patata");
   
   set_short("%^YELLOW%^El Penultimo Hogar%^RESET%^");
   set_long("Encima de la puerta de entrada hay un cartel colgando. Al entrar"
      "ves a un gordo y bajito humano que resulta ser el duenyo del local. Es mas "
      "o menos tranquila, aunque no por ello poco visitada. Es famosa "
      "en toda la region por su excelente tortilla de patata, con un "
      "cremoso y fantastico alioli. El local no es excesivamente grande, "
      "dispone de una docena de mesas y una barra bastante grande y no "
      "siempre demasiado limpia. Al fondo en el lugar mas oscuro hay "
      "una pequenya chimenea que casi nunca esta encendida.\n "
      "Puedes ver un gran cartel donde se expone el Menu.\n");
   
   add_exit(DIR_NORTH, ROOMS + "a4.c", "door");
   set_light(60);
   
   add_item("chimenea",
      "Es una pequenya chimenea que desde hace mucho tiempo no ha sido "
      "encendida.\n");
   add_item("cartel", "En el puedes leer 'El Ultimo Hogar'.\n");
   add_property("no_undead", 1);
   set_open_condition("BARMAN");
}
 
int BARMAN()
{
  object *lista;
  int i;
  if (!escalar) return 0;
  lista=escalar->query_attacker_list();
  for(i=0;i<sizeof(lista);i++)
   if (environment(lista[i])==this_object()) return 0;
  return 1;
}         
    
void reset()
{
   if (!gordo)
   {
      gordo=clone_object("/d/newbie/npc/gordo.c");
      gordo->move(this_object());
   }
   if (!escalar)
   {
      escalar=clone_object("/d/newbie/npc/escalar.c");
      escalar->move(this_object());
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
   if (gordo) gordo->dest_me();
   if (escalar) escalar->dest_me();
   if (board) board->dest_me();
   ::dest_me();
}

void init()
{
   add_action("quest", "aprender");
   add_action("quest", "learn");
   ::init();
}

void quest(string str)
{
   if((str=="escalar") || (str=="a escalar"))
   {
      if (!TP->query_property_exists("general_newbie_quest"))
      {
         if (escalar)
         {
            tell_object(TP, "\nViejo te dice: Veo que sabes apreciar las buenas "
               "sensaciones.\n\n"
               "El viejo montaraz acepta ensenyarte a escalar. Te diriges a las "
               "inmediaciones del bosque junto al misterioso personaje. Una vez "
               "alli comienzas a ser adoctrinado en las mas puras y antiguas artes "
               "de escalada montaraz.\nTras varias horas de aprendizaje consigues "
               "escalar un pequenyo arbol. Tal vez seas lo suficiente habilidoso para "
               "enfrentarte a los majestuosos Mallorns.\n\n"
               "Viejo te dice: Si deseas probar tus habilidades ves al gran Mallorn, "
               "si consigues superar esta prueba seras capaz de salir de aqui en busca "
               "de aventuras. El Mallorn esta en el norte de la region. Buena suerte!\n");
            TP->add_property("general_newbie_quest");
            return 1;
         }
         else
            notify_fail("Quien pretendes que te ensenye?\n");
      }
      notify_fail("Viejo te dice: Quieres aprender mas? Me temo que "
             "te ensenye todo lo que sabia!\n");
   }
   else
      notify_fail("Que quieres aprender?\n");
     
}
