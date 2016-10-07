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

// inherit "/global/living/alignment";
// inherit "/global/living/death";
// inherit "/global/living/force";
// inherit "/global/living/money";
// inherit "/global/living/effects";
// inherit "/global/living/equip";
// inherit "/global/living/gender";
// inherit "/global/living/stats";
// inherit "/global/living/combat.c";
// inherit "/global/living/health";
// inherit "/global/living/handle.c";

// from here we inherit object.c
inherit container "/lib/core/basic/container";
inherit queue     "/lib/living/queue.c";

// inherit "/global/living/groups_obs.c";
// inherit "/global/living/mount.c";
// inherit "/global/living/drunk.c";
// inherit "/global/living/spells.c";
// inherit "/global/living/consent.c";
// inherit "/global/living/visited.c";

void create()
{
  queue::create();

  // from here we inherit object.c, were the call to
  // setup is, so it must be the last create call
  container::create();

  // every living has enable_commands activated,
  // so it will respond to living()
  enable_commands();
}

mixed * stats() 
{
  return container::stats() + 
         queue::stats();
         // stats::stats() + 
         // equip::stats() + 
         // alignment::stats() + 
         // death::stats() + 
         // health::stats() + 
         // combat::stats() +
         // gender::stats() +
         // mount::stats() +
         // groups_obs::stats() +
         // spells::stats() + 
         // drunk::stats() + 
         // consent::stats() + 
         // visited::stats();
}

