/* Edited by Lummen and Antiron 15-7-97 */

#include "../path.h"
#include <language.h>

inherit "/lib/monster.c";

void setup()
{
  set_name(_LANG_NPCS_HEALER_NAME);
  set_short(_LANG_NPCS_HEALER_SHORT);
  add_alias(_LANG_NPCS_HEALER_ALIASES);
  
  set_main_plural(_LANG_NPCS_HEALER_PLURAL);
  add_plural(_LANG_NPCS_HEALER_PLURAL_ALIASES);
  
  set_long(_LANG_NPCS_HEALER_LONG);
  
  set_gender(1);
  
  // the race object
  set_race_ob(OBJ + "races/human.c");
  set_class_ob("/lib/obj/classes/scholar.c");
  
  set_random_stats(6, 16);
  set_level(16);
  set_gender(1);
  set_real_align(-100);
  set_max_hp(1000);
  set_hp(1000);

  load_chat(50, ({
    1, "'Si deseas saber como volver a tu forma mortal escribe 'info'.",
    1, ":realiza una oracion al dios Lummen.",
    1, "'Debes ser un buen devoto."
  }));

  adjust_money(5+random(5), "gold");
  add_clone(BASEOBS + "weapons/mace", 1);
  add_clone(BASEOBS + "armours/cape", 1);
  add_clone(BASEOBS + "armours/chainmail", 1);

  init_equip();
}

void init()
{
  add_action("do_raise", _LANG_NPCS_HEALER_RAISE_COMMANDS);
  add_action("do_info", _LANG_NPCS_HEALER_INFO_COMMANDS);

  // add_attack_spell(40, "pupa", ({"/std/spells/cleric/cause_lt", "cast_spell", 0}));

  ::init();
} 

int do_info(string str)
{
  do_say(_LANG_NPCS_HEALER_INFO);
  return 1;
}

/*
int do_raise(string str)
{
  string nombre;
  object *quien, pc;

  if(!str) return 0;
  
  nombre=this_player()->query_name();
  if (str=="me" || str=="mi" || str==nombre)
    pc=this_player();
  else
  {
   quien=find_match(str,this_object());
   if(!sizeof(quien)) return 0;
   pc=quien[0];
   if(!find_living(str)) return 0;
  }
  
  if (pc->query_timed_property("NO_RESUCITAR")) {
  tell_object(pc, "Thran te dice: Intentas tomarme el pelo?, tu resucitaste " + 
    "hace poco... tendras que esperar mas tiempo.\n");
  return 1; 
  }
  if (pc->query_dead()==0)
{
    if(pc==this_player())
      tell_object(pc, "Porque me preguntas si quieres resucitar si no lo " + 
          "necesitas ?.\n");
    else
     tell_object(pc, "Porque me preguntas si quieres resucitarle si no lo " + 
        "necesita ?.\n");
    return 1;
  }
  else if(priest)
   {
     tell_object(pc, "\nEl clerigo levanta las manos invocando los poderes de los Dioses" + 
    ", y apareces de nuevo en tu forma mortal!.\n");
     tell_room(environment(pc), pc->query_cap_name()+" aparece en su forma mortal.\n", pc);
     pc->remove_ghost();
     pc->adjust_max_lives(1);
     pc->add_timed_property("NO_RESUCITAR", 1, 100);
    }
  return 1;
}
*/