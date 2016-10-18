/* 
 * Funciones de ciudadania extraidas, Folken 4/2003
 * Nuevo alineamiento, Folken 27/4/03
 * Eliminado it_them
 * Mejorada la herencia en general, Folken 6/03
 * Nuevo sistema de combate para CcMud, Folken 6/03
 *
 * Traida a este archivo la logica necesaria para recuperar vida, haciendola comun
 *   a players y npcs (antiguos monster::heart_beat y player::heart_beat, 
 *   ahora living::living_heart_beat()), Folken 04/09
 *
 * Añadido do_death para los npcs o players de quest, Folken 03/09
 */

// #include <living.h>
// #include <properties.h>
// #include <room.h> 
// #include <attacks.h>
// #include <drinks.h>
// #include <quests.h>

inherit alignment "/lib/core/basic/alignment";
// inherit "/global/living/death";
// inherit "/global/living/force";
// inherit "/global/living/money";
// inherit "/global/living/effects";
inherit equip     "/lib/living/equip";
inherit gender    "/lib/core/basic/gender";
inherit stats     "/lib/living/stats";
// inherit "/global/living/combat.c";
inherit health    "/lib/living/health";
inherit handle    "/lib/living/handle";

inherit queue     "/lib/living/queue.c";
inherit movement  "/lib/living/movement.c";

// inherit "/global/living/groups_obs.c";
// inherit "/global/living/mount.c";
// inherit "/global/living/drunk.c";
// inherit "/global/living/spells.c";
// inherit "/global/living/consent.c";
// inherit "/global/living/visited.c";

void create()
{
  alignment::create();
  equip::create();
  gender::create();
  stats::create();
  health::create();
  handle::create();
  queue::create();

  // from here we inherit object.c, were the call to
  // setup is, so it must be the last create call
  movement::create();

  // every living has enable_commands activated,
  // so it will respond to living()
  enable_commands();

  add_action("do_equip", "equipar");
  add_action("do_equip", "equiparse");
  add_action("do_hold", "empunyar");
  add_action("do_hold", "empuñar");
  add_action("do_hold", "sostener");
  add_action("do_unhold", "desempunyar");
  add_action("do_unhold", "desempuñar");
  add_action("do_unhold", "soltar");
  add_action("do_wear", "ponerse");
  add_action("do_wear", "ponerme");
  add_action("do_wear", "vestir");
  add_action("do_unwear", "desvestir");
  add_action("do_unwear", "quitarse");
  add_action("do_unwear", "quitarme");

  // combat_commands();    
  handle_commands();
  // groups_commands();
  // spell_commands();
  // consent_commands();
}

int cannot_get_stuff() { return 1; }

void dest_me() 
{
  object *obs;
  int i;

  obs = deep_inventory(this_object());

  for (i = 0; i < sizeof(obs); i++)
    if(obs[i])
      obs[i]->dest_me();

  movement::dest_me();
}

void start_player()
{
  // groups_obs::start_player();
}

mixed * stats() 
{
  return alignment::stats() +
         equip::stats() +
         gender::stats() + 
         stats::stats() + 
         health::stats() + 
         handle::stats() + 
         queue::stats() + 
         movement::stats();
         // death::stats() + 
         // combat::stats() +
         // mount::stats() +
         // groups_obs::stats() +
         // spells::stats() + 
         // drunk::stats() + 
         // consent::stats() + 
         // visited::stats();
}

