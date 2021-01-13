//Created by Lummen 27-7-97

inherit "/std/storeroom";
#include "../path.h"
#include <language.h>

setup()
{
  set_short("El almacen");
  set_long("Aqui e donde se guardan los objetos.\n");
  add_exit(DIR_SOUTH, ROOMS + "BT", "door");
  set_light(80);
}

void reset()
{
  call_out("af_reset", 0);
}

void af_reset()
{
   int i, j;
   object ob, torch;
        
   i=sizeof(find_match("cota de mallas", this_object()));
   for (;i<3;i++)
   {
      ob=new(BASEOBS+"chainmail");
      ob->move(this_object());
      ob->set_value(85);
   }
   i=sizeof(find_match("armadura de cuero", this_object()));
   for (;i<2;i++)
   {
      ob=new(BASEOBS+"leather");
      ob->move(this_object());
      ob->set_value(20);
   }
   i=sizeof(find_match("espada corta", this_object()));
   for (;i<2;i++)
   {
      ob=new(BASEOBS+"short_sword");
      ob->move(this_object());
      ob->set_value(10);
   }
   i=sizeof(find_match("escudo pequenyo", this_object()));
   for (;i<2;i++)
   {
      ob=new(BASEOBS+"sma_shield");
      ob->move(this_object());
      ob->set_value(5);
   }
   i=sizeof(find_match("daga", this_object()));
   for (;i<2;i++)
   {
      ob=new(BASEOBS+"dagger");
      ob->move(this_object());
      ob->set_value(8);
   }
   i=sizeof(find_match("espada larga", this_object()));
   for (;i<5;i++)
   {
      ob=new(BASEOBS+"long_sword");
      ob->move(this_object());
      ob->set_value(8);
   }
}
