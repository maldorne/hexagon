
inherit "/obj/monster";

void setup()
{
   set_name("arak");
   add_alias("goblin");
   set_short("Arak");
   set_long("Para ser un goblin tiene una estatura bastante notable. Sus "
      "efluvios corporales le destacan entre la multitud de la taverna.\n");
   set_race_ob("/std/races/goblin");
   set_race("Goblin");
//   set_kill_xp(400);
   set_wimpy(10);
   set_gender(1);
   set_random_stats(6, 18);
   set_al(100);
   adjust_money(random(50), "copper");
   set_level(5+random(3));
   adjust_thac0(-10);
   load_chat(10,
      ({
           1, "@te escupe a los pies.",
           1, "'Uggg, murrr, come tripas en salsa.",
       }));
   add_clone("/baseobs/weapons/club",1);
   add_clone("/baseobs/armours/chainmail",1);
   this_object()->init_equip();

} /* setup() */
