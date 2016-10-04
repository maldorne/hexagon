
// Radix - fixes from new driver .. void was an unfriendly place
// after we moved to Aurora...
// Sept 28, 1995
// This got COMPLICATED .. argh
// Traducido para CcMud, Folken 7/03
//   Los jugadores ahora van a la ENTRY_POS (definida en player.h)

#define ENTRY_POS "/game/someplace"

#include <areas/common.h>

inherit "/lib/room";


int do_update(string str)
{
   int i;
   object *all = all_inventory(this_object());
   // if(!stringp(str) || str == "here")
   // {
      // load destinations
      CODER_COMMON_ROOM->BAH();
      ENTRY_POS->BAH();

      for(i = 0; i < sizeof(all); i++)
         if(interactive(all[i])) 
         {
            if(!all[i]->query_creator())
            {
               tell_object(all[i], "Sacándote del infierno...\n");
               all[i]->move(ENTRY_POS);
               all[i]->do_look();
            }
            if(all[i]->query_creator())
            {
               tell_object(all[i],"Viajando hasta la habitación común...\n");
               all[i]->move(CODER_COMMON_ROOM);
               all[i]->do_look();
               tell_room(find_object(CODER_COMMON_ROOM), all[i]->query_cap_name()+
                  " llega con cara de cansancio... \n", all[i]);
            }
         }
         else
            all[i]->dest_me();     // dest those NPCs and items
   // }
   return 1; 
}

void setup() {
     set_light(40);
     set_short(mud_name()+": El Vacío");
     set_long("Éste es el lugar al que vienes cuando el lugar en el que "
              "estás desaparece y no tienes ningún lugar al que ir. "
              "La razón por la que estás en él se deberá muy probablemente "
              "a algún fallo involuntario del programador de la zona en la "
              "que estabas.\n");

     add_exit("ciudad", ENTRY_POS, "standard");
}

void init()
{
   add_action("do_update", ({"update", "up"}));
   room::init();
}

