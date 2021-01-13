// Edited by Lummen and Antiron 26-7-97
inherit "/obj/monster";

void setup()
{
  set_name("cebadilla mantecona");
  add_alias("gordo");
  add_alias("hombre");
  add_alias("cebadilla");
  set_short("Cebadilla Mantecona");
  set_long("Es un humano bajito y gordo con cara de bonachon. Su sucio delantal"
    " nos da una idea del trabajo que tiene que soportar en una "
    "jornada completa.\n");
  set_race("humano");
  set_level(5+random(6));
  set_gender(1);
  set_wimpy(0);
  set_random_stats(3,14);
  set_al(-100);
  adjust_money(random(10),"silver");
  load_chat(10,
  ({
    1, "@te saluda cordialmente.",
    1, "'La tortilla de patata es excelente en mi taverna.",
    1, "'No probaras ninguna comida igual en otro lugar.",
  }));
  load_a_chat(30,
  ({
    1, "'Socorro, avisad a los guardias.",
    1, "'Arrggghhh!",
  }));
}
