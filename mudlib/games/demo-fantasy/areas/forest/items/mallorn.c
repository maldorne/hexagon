// Edited by Lummen 21-9-97

#include <language.h>
 
inherit "/lib/item.c"; 
 
void setup()
{
  set_name("mallorn");
  set_short("%^GREEN%^BOLD%^Mallorn%^RESET%^");
  add_alias(_LANG_MALLORN_ALIASES);

  set_main_plural("%^ORANGE%^Mallorns%^RESET%^");
  add_plural(_LANG_MALLORN_PLURALS);

  set_long(_LANG_MALLORN_LONG);
  reset_get();                                   
}
 
/*
void init() {
  ::init();
  add_action ("quest","escalar");                        
  add_action ("quest","subir");                    
}
 
int quest(string str) 
{
  
  if (((str=="mallorn") || (str=="arbol")) 
        && (this_player()->query_property_exists("general_newbie_quest")==1))
  {  
   if(this_player()->query_level()>=5)
   {
    TP->add_timed_property("passed out","Estas trepando el arbol\n",1,8);
    call_out("salida",3);
   }
   else
     write("Te acercas al Mallorn dispuesto a escalarlo, pero por mucho que"
        " lo intentas no logras escalar mas de un metro. Eres consciente de "
        "todavia te hace falta adquirir mas experiencia.\n");
  }
  else
        write("Escalar que?.\n");
  return(1);
}
 
void salida()
{
 write("\nTe acercas al Mallorn decidido a escalarlo. Sin pensarlo dos "
        "veces te encaramas a sus ramas en un abrir y cerrar de ojos. Una "
        "vez alli eres conciente de que has llegado a un nivel de experiencia "
        "aceptable durante el tiempo que has estado aqui.\n");
 TP->add_timed_property("passed out","Estas trepando el arbol\n",1,8);
 tell_room(environment(this_player()),this_player()->query_cap_name()+
           " se encarama al Mallorn decidido a escalarlo.\n",this_player());
 call_out("salida2",3);
}
 
void salida2()
{
 write("Durante tu ensimismamiento la rama en la que estabas comienza a "
        "romperse debido a tu peso.\n");
 TP->add_timed_property("passed out","Estas haciendo equilibrios para no caerte.\n",1,7);
 call_out("salida3",3);
}
 
void salida3()
{
 object *cosas;   //Array de libros a destruir.
 write("CRASSSSHHH...!! Caes junto a la rama al rio, momento en el que te "
        "golpeas la cabeza con una de las rocas del mismo perdiendo el cono"
        "cimiento.\nAl despertar te das cuenta de que la corriente del rio "
        "te ha transportado a ti y la rama hasta un lugar extranyo.\n");
        
      tell_room(environment(this_player()), "Una vez en el, "
        "la rama que le sujetaba se rompe, cayendo ambos al rio para no "
           "volver a verlos nunca mas.\n",this_player());
     if(TP->query_race() == "Hombre-Lagarto")
     {
     this_player()->move("/d/zulk/rooms/camino/a1.c");
     }
     else
     {
     if(TP->query_race() == "Duergar")
     {
     this_player()->move("/d/bendorf/ciudad/plaza.c");
     }
     else
     if(TP->query_race() == "Kuruloo")
     {
     this_player()->move("/d/dendara/rooms/muralla/m_0");
     }
     else
     {
     this_player()->move("/d/anduar/anduar/ciudad/c1.c");
     }
     }
     this_player()->remove_property("escalar");
     TP->remove_property("general_newbie_quest");
     TP->set_guild_ob(0);
     TP->look_me();
     TP->add_timed_property("passed out","Aun estas conmocionado por el viaje.\n",1,4);
     cosas=find_match("libros",this_player());
     
     if(!sizeof(cosas))         /* Si esta vacio. */
        return 1;
     else
     {
      int i;
      
      write("Cuando recuperas la compostura revisas tu inventario y te "
          "das cuenta que algo ha desaparecido.\n");
      for (i=0; i<sizeof(cosas);i++)
          cosas[i]->dest_me();
  }
}
*/