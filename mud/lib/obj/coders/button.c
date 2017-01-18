
inherit "/lib/item.c";

void setup() 
{
  // add_adjective("rojo");
  reset_drop();
  set_weight(10000000);
  set_name("boton");
  set_short("%^BOLD%^%^RED%^Botón rojo%^RESET%^");
  set_long(
    "Este botón destruye todos los objetos que no sean jugadores en la habitación.\n"+
    "Escribe 'pulsar botón' para utilizarlo.\n"+
    "ÚSALO CON EXTREMO CUIDADO.\n");
}

void init() 
{
  ::init();
  add_action("do_pulsar", "pulsar");
}

int do_pulsar(string str) 
{
  object ob, temp;
  int changed;
  changed = 1;

  if (!stringp(str) || ((str != "boton") && (str != "botón")))
  {
    notify_fail("¿Pulsar qué? Quizá quieras 'pulsar botón'.\n");
    return 0;
  }

  tell_object(this_player(), "Pulsas el botón rojo.\n");
  tell_room(environment(this_object()), this_player()->query_cap_name() +
    " pulsa el botón rojo.\n", this_player());

  while (changed) 
  {
    changed = 0;
    ob = first_inventory(environment(this_object()));

    while (ob) 
    {
      if (ob != this_object() && !query_ip_number(ob)) 
      {
        changed = 1;
        temp = ob;
      }
    
      ob = next_inventory(ob);
    
      if (temp) 
      {
        temp->dest_me();
      }
    }
  }
  return 1;
}

