// Edited by Lummen and Antiron 21-9-97
inherit "/obj/monster";

void setup()
{
  set_name("viejo");
  set_short("Viejo");
  set_long("Es un viejo montaraz del lugar que se encuentra descansando en "
    "la ciudad temporalmente.\n");
  set_race("humano");
  set_main_plural("Viejos");
  add_plural("viejos");
  set_level(10);
  set_gender(1);
  set_wimpy(0);
  set_random_stats(3,16);
  set_al(-100);
  adjust_money(random(10), "copper");
  load_chat(50,
  ({
    1, "'En mis viejos tiempos era uno de los mejores escaladores de la "
       "region. Es una lastima que se pierda una aficion tan excitante.",
    1, "'La verdadera aventura es la de escalar los grandes Mallorns.",
    1, "@bebe de su jarra de cerveza.",
    1, "'La verdadera aventura es la de escalar los grandes Mallorns.",
    1, "Viejo te dice: tienes grandes dotes fisicas para ser un excelente "
       "escalador. Si quieres aprender solo tienes que decirmelo.",
  }));
  add_clone("/baseobs/armours/cloak",1);
  init_equip();
} 
