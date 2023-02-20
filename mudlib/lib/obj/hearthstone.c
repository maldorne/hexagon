/*
 * Piedra de Hogar para Ciudad Capital...
 * Obviamente si, es el mismo concepto de World of Warcraft
 *
 * Folken 17/04/09
 */

inherit "/lib/item.c";

#define NUM_SECONDS 10
#define LOCK_HEARTHSTONE "hearthstone_lock_property"
#define LOCK_HEARTHSTONE_INVOKE "hearthstone_invoke_lock_property"
// 2 segundos = 1 hb, 1 hora = 3600 segundos = 1800 hbs
#define LOCK_TIME 1800

static int acting;
string destination_path;
string destination_name;

void set_destination_path(string n) { destination_path = n; }
string query_destination_path() { return destination_path; }
void set_destination_name(string n) { destination_name = n; }
string query_destination_name() { return destination_name; }

void create()
{
  acting = 0;
  destination_path = "";
  destination_name = "";
  ::create();
  set_name("piedra de hogar");
  set_short("%^MAGENTA%^Piedra de Hogar%^RESET%^");
  add_alias("piedra");
  add_alias("hogar");
  set_main_plural("%^MAGENTA%^Piedras de Hogar%^RESET%^");
  add_plural("piedras");
  set_long("Es una piedra pequeña, de un color púrpura, con una extraña runa que no sabes descifrar "+
    "tallada en uno de sus lados. Un aura blanquecina la rodea sin llegar a emitir luz, "+
    "dándote la sensación de que es un objeto hermoso. Sin duda parece que posee poderes más "+
    "allá de tu comprensión. Escribe 'ayuda piedra' para ver lo que puedes hacer con ella.\n");

  reset_drop();

  set_gender(2);
  set_weight(1);
}

string long(string s, int dark)
{
  return ::long(s, dark) +
    (strlen(destination_name)?"Tu piedra está marcada en: "+destination_name+".\n":"Tu piedra no está marcada aún.\n");
}

void init()
{
  add_action("do_travel", "transportar");
  add_action("do_mark", "marcar");

  // Ahora la ayuda sale del sistema genérico de ayuda
  // add_action("do_help", "ayuda");
  add_action("do_invoke", "invocar");
  ::init();
}

int do_travel(string str)
{
  if (acting)
  {
    notify_fail("La piedra está actuando, espera un poco.\n");
    return 0;
  }

  if (this_player()->query_dead())
  {
    notify_fail("Estando muerto no puedes hacer eso.\n");
    return 0;
  }

  if (this_object()->query_timed_property(LOCK_HEARTHSTONE))
  {
    notify_fail("La piedra aún no ha acumulado suficiente energía desde la última vez.\n");
    return 0;
  }

  if (!stringp(destination_path) || !load_object(destination_path))
  {
    string race;
    race = this_player()->query_race_ob();

    if (!stringp(race))
    {
      notify_fail("Tu piedra ha olvidado el camino a casa... "+
        "tendrás que marcar un nuevo lugar antes de intentarlo.\n");
      return 0;
    }
    else
    {
      tell_object(this_player(), "Tu piedra ha olvidado el camino a casa... "+
        "pero aún recuerda su lugar de origen inicial.\n");
      destination_path = load_object(race)->query_init_room();
    }
  }

  if (this_player()->query_fighting())
  {
    notify_fail("Debes terminar primero tus combates.\n");
    return 0;
  }

  tell_object(this_player(), "La piedra de hogar comprende tus deseos y sus poderes se activan.\n");
  tell_room(environment(this_player()), "La piedra de hogar de "+this_player()->query_cap_name()+
    " comienza a brillar con tonos púrpura.\n", this_player());

  acting = 1;

  call_out("continue_travel", 1, this_player(), NUM_SECONDS, environment(this_player()));
  return 1;
}

int continue_travel(object player, int count, object where)
{
  if (player->query_fighting())
  {
  tell_object(player, "Debes terminar primero tus combates.\n "+
    "Escribe '%^BOLD%^detener combates%^RESET%^' para terminarlos lo antes posible.\n");
  acting = 0;
  return 1;
  }

  if (environment(player) != where)
  {
  tell_object(player,"Si no paras de moverte será imposible que la piedra acumule energía "+
    "suficiente para transportarte.\n");
  acting = 0;
  return 1;
  }

  if (player->query_dead())
  {
    tell_object(player, "Al morir tu piedra deja escapar la energía acumulada...\n");
    return 1;
  }

  if (count <= 0)
  {
    object dest;
    dest = load_object(destination_path);

    if (!dest)
    {
      tell_object(player, "Algo ha ido mal.\n");
      acting = 0;
      return 1;
    }

    // No transportamos monturas, si no aparecerías con tu caballo en medio de
    // una taberna...
    if (player->query_riding())
      player->destruct_ride_shadow();

    player->move(dest);
    add_timed_property(LOCK_HEARTHSTONE, 1, LOCK_TIME);
    // Permitimos invocar a la montura tras el viaje
    remove_timed_property(LOCK_HEARTHSTONE_INVOKE);
    tell_object(player, "\nQuedas cegado por un potente haz de luz púrpura que desprende la piedra, "+
      "mientras notas cómo se calienta al tacto.\n\n"+
      "Notas como el calor de la piedra desaparece y al difuminarse el haz de luz puedes "+
      "distinguir dónde te encuentras.\n\n");
    player->do_look();
    tell_room(dest, player->query_cap_name() +
      " aparece repentinamente de entre un extraño humo púrpura.\n", player);
    acting = 0;
    return 1;
  }
  else
  {
    // tell_object(player, "La piedra de hogar crepita mientras acumula energía.\n");
    tell_object(player, query_short() +
      ": [%^MAGENTA%^"+sprintf("%*' '-s", NUM_SECONDS, sprintf("%*'*'s", NUM_SECONDS - count,"")) +
      "%^RESET%^]\n");
  }

  call_out("continue_travel", 1, player, count-1, environment(player));
  return 1;

}

int do_mark(string str)
{

  if (str != "piedra")
  {
    notify_fail("¿Marcar el qué? Quizá quieras 'marcar piedra'.\n");
    return 0;
  }

  if (acting)
  {
    notify_fail("La piedra está actuando, espera un poco.\n");
    return 0;
  }

  if (this_player()->query_dead())
  {
    notify_fail("Estando muerto no puedes hacer eso.\n");
    return 0;
  }

  if (this_player()->query_fighting())
  {
    notify_fail("Debes terminar primero tus combates.\n");
    return 0;
  }

  if (!environment(this_player())->query_pub())
  {
    notify_fail("Sólo puedes hacer eso en una taberna.\n");
    return 0;
  }

  destination_path = base_name(environment(this_player()));
  destination_name = environment(this_player())->query_short();

  tell_object(this_player(), "De acuerdo, a partir de ahora el destino de tu piedra de hogar será: "+
    environment(this_player())->query_short()+".\n");
  return 1;
}


string get_help(varargs string str)
{
  string ret;

  // if (str != "piedra")
  // {
  //   notify_fail("Prueba 'ayuda piedra' para más información.\n");
  //   return 0;
  // }

  ret = "Puedes utilizar los siguientes comandos:\n";
  ret += "\tmarcar      - en una taberna para que tu piedra de hogar recuerde el lugar.\n";
  ret += "\ttransportar   - para que la piedra te transporte de vuelta al lugar marcado.\n";
  ret += "\tinvocar montura - para traer a la montura a tu lado (sólo al aire libre).\n";

  // tell_object(this_player(), ret + "\n");
  // return 1;
  return ret;
}

int do_invoke(string str)
{
  object ob;

  if (str != "montura")
  {
    notify_fail("¿Invocar el qué? Quizá quieras 'invocar montura'.\n");
    return 0;
  }

  if (!this_player()->query_mount())
  {
    notify_fail("Debes tener una montura para eso.\n");
    return 0;
  }

  if (!environment(this_player())->query_outside())
  {
    notify_fail("Sólo puedes hacer eso al aire libre.\n");
    return 0;
  }

  if (environment(this_player())->query_water_environment())
  {
    notify_fail("Hacer eso en el agua no es muy recomendable.\n");
    return 0;
  }

  if (this_object()->query_timed_property(LOCK_HEARTHSTONE_INVOKE))
  {
    notify_fail("La piedra aún no ha acumulado suficiente energía desde la última vez.\n");
    return 0;
  }

  ob = this_player()->query_mount();

  if (environment(this_player()) == environment(ob))
  {
    notify_fail("Tu montura ya se encuentra aquí.\n");
    return 0;
  }

  // Por si la esta montando otra persona
  ob->unride();
  ob->move(environment(this_player()));

  tell_player(this_player(), "Tu montura aparece a tu lado.\n");
  tell_room(environment(this_player()), "La montura de "+this_player()->query_cap_name()+
    " aparece a su lado.\n", this_player())  ;

  // Con esto impedimos que invoque a la montura hasta la proxima vez que pueda transportarse
  //  (ya que lo logico es transportarse + invocar montura cuando encuentre un lugar apropiado)
  if (query_timed_property(LOCK_HEARTHSTONE))
    this_object()->add_timed_property(LOCK_HEARTHSTONE_INVOKE, 1,
      query_time_remaining(LOCK_HEARTHSTONE));
  else
    this_object()->add_timed_property(LOCK_HEARTHSTONE_INVOKE, 1, LOCK_TIME);

  return 1;
}

mixed * stats()
{
  return ::stats() + ({
      ({ "Destination Path", destination_path, }),
      ({ "Destination Name", destination_name, }),
    });
}

mapping query_auto_load_attributes()
{
  return ([ "::" : ::query_auto_load_attributes() ]) + 
      ((strlen(destination_path)) ? ([ "destination path" : destination_path ]) : ([ ])) + 
      ((strlen(destination_name)) ? ([ "destination name" : destination_name ]) : ([ ]));
}

void init_auto_load_attributes(mapping attribute_map)
{
  if (!undefinedp(attribute_map["destination path"]))
    destination_path = attribute_map["destination path"];
  if (!undefinedp(attribute_map["destination name"]))
    destination_name = attribute_map["destination name"];
  if (!undefinedp(attribute_map["::"]))
    ::init_auto_load_attributes(attribute_map["::"]);
}
