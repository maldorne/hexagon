/* Created by Lummen and Antiron 26-7-97 Escalar fixed by Oskuro 2 anyos y 
 * medio despues Ah, tambien he indentao un poco el tema. 17SEP99 */
  
inherit "std/pub";
#include "pub.h"
object board,escalar;
 
//nombre.tipo.coste.heal.volumen.intox
void setup()
{
   add_menu_item("Hidromiel", ALCOHOL, 150, 15, 200, 200,
      "bebes una gran jarra de Hidromiel", "saborea una jarra de Hidromiel");
   add_menu_item("Bayas", FOOD, 50, 10, 40, 0, "comes unas deliciosas bayas "
                 "silvestres.", "se come unas deliciosas bayas silvestres.");
   add_menu_item("Raices", FOOD, 30, 5, 40, 0, "masticas unas deliciosas raices "
                 "tonificantes.", "mastica unas raices tonificantes.");
   add_menu_alias("hidromiel", "Hidromiel");
   add_menu_alias("bayas", "Bayas");
   add_menu_alias("raices", "Raices");
    
    set_short("Arbol de la Recuperacion");
    set_long("El Arbol de la Recuperacion hace las veces de Taberna"
                   " en el poblado de Elfereth. Es un viejo arbol cuyas ramas crecen "
                   "a modo de toldo sobre unas mesas construidas con las mismas "
                   "ramas del arbol, que amablemente se presto para que sus moradores"
                   " tuviesen mas comodidad.\n");
   add_exit(DIR_OUT, ROOMS + "z18", "door");
   set_light(40);
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
   if (!escalar)
   {
      escalar=clone_object("/d/newbie/npc/barman");
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
   if (escalar) escalar->dest_me();
   if (board) board->dest_me();
   ::dest_me();
}
 
