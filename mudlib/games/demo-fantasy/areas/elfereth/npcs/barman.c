//Paris. Abr'98

inherit "/obj/monster";

void setup() 
{
  set_name("Feleir");
  set_short("Feleir");
  set_long("El elfo cocinero mas famosos de todos es el que habita esta "
    "posada.\n");
   set_race("Elfo");
   set_race_ob("/std/races/elf");
   add_alias("feleir");
   add_plural("feleires");
   set_level(5+random(3));
   set_wimpy(0);
   set_gender(1);
   set_random_stats(6, 16);
   set_al(-200);
   adjust_money(random(50), "copper");
   load_chat(10,
   ({
     1, "'hmmmmmm.... me encanta la comida natural.",
     1, "'Estas bayas me estan quedando de rechupete.",
   }));
    init_equip();
}
