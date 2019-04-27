/*
 * Nueva room de inicio del juego, aqui los jugadores deben escoger su raza
 * antes de empezar a jugar (antes de poder salir de ella).
 * Folken para Cc, 4/2003
 *
 * Descripcion de la room por Sirio && Folken.
 */

inherit "/lib/room";

#include <living/races.h>
#include <user/user.h>

#define PATH "/doc/races/"
// TODO /game races
#define OBS "/lib/obj/races/"
#define LOG "assign_race"

int i;
object ob;
mapping races;
string * race_files;
string tmp;

void create()
{
  races = ([ ]);

  ::create();
  // obtain_data();
}

mapping test() { return races; }

void obtain_data()
{
  i = 0;

  // get every file and directory in /lib/obj/races
  race_files = get_dir(OBS);
  for (i = 0; i < sizeof(race_files); i++)
  {
    if (ob = clone_object(OBS + race_files[i]))
    {
      if (ob->query_is_race_ob() && (ob->query_name() != "desconocido") &&
      (ob->query_playable()))
      {
        // write("Añado "+ob->query_name()+" "+ob->query_long()+"\n");
        races[ob->query_name()] = ob->query_long();
        // write("Tengo "+sizeof(races)+" "+races[ob->query_name()]+"\n");
        // races += ({ ob->query_name() });
        // descripciones += ({ ob->query_desc() });
      }
    }
  }
}

void setup()
{
  int num_races;

  obtain_data();

  num_races = map_sizeof(races);

  set_light((LHUMANH - LHUMANL)/2);
  set_short("Ciudad Capital: %^BOLD%^Luz en las %^BLACK%^Tinieblas%^RESET%^");
  set_long("Te sientes caer a lo largo de una inmaterial oscuridad cuando "+
    "un destello frente a ti aumenta en intensidad, y para tu horror "+
    "ves que te aproximas a él sin posibilidad de escapatoria. Cuál "+
    "es tu sorpresa al descender lentamente hasta una plataforma de "+
    "luz con "+number_as_string(num_races)+" %^BOLD%^cofre"+((num_races==1)?"":"s")+
    "%^RESET%^ colocado"+((num_races==1)?"":"s")+" "+
    "a tu lado como si fueses el centro de toda existencia. "+
    "Del suelo surgen chispas de luz como pequeñas y revoltosas hadas "+
    "y al mirarte tu misma forma insustancial te aterra.\n"+
    "¿Exactamente qué es lo que eres? ¿O más bien.. qué es lo que deseas ser?\n"+
    "%^BOLD%^Escribe <mirar cofres> para tener más información.%^RESET%^\n");

  add_item( "plataforma", "Te encuentras en una plataforma de divina luz que flota en "+
    "mitad de una nada de absoluta oscuridad.\n");
  add_item(({"cofres","cofre","efigie","efigies"}), "Puedes ver a tu alrededor pequeños cofres con las "+
    "efigies de seres de las más diversas razas de "+mud_name()+". Concretamente "+
    "observas dibujos de un "+query_multiple_short(keys(races))+".\n"+
    "%^BOLD%^Puedes mirar a cada uno de ellos por separado "+
    "para observarlos detenidamente (ej: <mirar "+keys(races)[random(map_sizeof(races))]+">).%^RESET%^\n");

  /* No podemos utilizar los add_item con este tipo de construcciones,
  por alguna razon no funciona... habra que investigar
  for(i = 0; i < sizeof(races); i++){
    add_item(races[i], descripciones[i]);
  }
  */
}

void init()
{
  ::init();
  add_action("do_look", "mirar");
  add_action("do_open_chest", "abrir");
}

int do_look(string str)
{
  // Si miramos algo que no son los cofres, hacemos un return 0
  //  para utilizar el comando mirar normal
  if (member_array(lower_case(str), keys(races)) == -1)
  {
    return 0;
  }

  tmp = "Cofre con la efigie de un "+str+".\n";
  tmp += "   "+(string)races[lower_case(str)]; //+"\n";
  tmp += "%^BOLD%^Escribe <abrir cofre "+str+"> si deseas reencarnarte como un miembro "+
    "de esta raza.%^RESET%^\n";

  tell_object(this_player(), tmp);
  return 1;
}

void event_enter(object who, varargs string msg, object from, mixed avoid)
{
  if (living(who) && (sizeof(keys(races)) == 1))
  {
    who->set_race_ob(OBS + keys(races)[0]);

    log_file(LOG, who->query_cap_name() + ": "+
      keys(races)[0] + " (asignada auto por ser unica raza) " + ctime(time(),4) + "\n");
    who->move("/game/areas/start/inicio_2.c");
    who->do_look();
  }
}

// Funcion que nos hace de una raza determinada
// No termina nunca con un 0, puesto que se invoca con el verbo
//  abrir y nos daria tambien los mensajes de las puertas.
int do_open_chest(string str)
{
  string * lista;

  if (!str || (str == ""))
  {
    tell_object(this_player(), "¿Abrir el qué?\n");
    return 1;
  }

  lista = explode(str, " ");

  if (sizeof(lista) < 2)
  {
    tell_object(this_player(), "¿Abrir el qué?\n");
    return 1;
  }

  if ((lista[0] != "cofre") ||
  (member_array(lista[1], keys(races)) == -1))
  {
    tell_object(this_player(), "¿Abrir el qué?\n");
    return 1;
  }

  tmp = "Te acercas al cofre con la efigie dorada de "+
    "un "+lista[1]+" y lo abres con cuidado, notando como comienza "+
    "a atraerte hacia su interior antes de perder la consciencia.\n\n";

  tell_object(this_player(), tmp);

  // Solo cambiamos la raza SI NO TIENE!!!!
  if (this_player()->query_race_ob() &&
  (this_player()->query_race_name() != "desconocido"))
  {
    log_file(LOG, this_player()->query_cap_name() + " estaba en /game/areas/start/inicio.c "+
      "teniendo raza asignada! " + ctime(time(),4) + "\n");
  }
  else
  {
    // Añadidas subraces!!
    load_object(OBS + lista[1]+".c");
    if (sizeof(find_object(OBS + lista[1]+".c")->query_subraces()) > 0)
    {
      this_player()->set_race_ob(OBS + lista[1]);
      log_file(LOG, this_player()->query_cap_name() + ": "+
        lista[1] + " (pendiente de escoger subraza) " + ctime(time(),4) + "\n");
      this_player()->move("/game/areas/start/inicio_2.c");
      this_player()->do_look();
      return 1;
    }

    this_player()->set_race_ob(OBS + lista[1]);
    log_file(LOG, this_player()->query_cap_name() + ": "+
      lista[1] + " " + ctime(time(),4) + "\n");
  }

  // Nuevo sistema de alineamiento, Folken 6/03
  // Aqui solo llegamos si no hay subraza (en caso de subraza el
  // alineamiento se establece en /game/areas/start/inicio_2.c
  this_player()->set_ext_align((this_player()->query_race_ob())->query_ext_align());

  /* Ahora esto se hace en inicio_3.c, Folken 6/03
  tmp = (this_player()->query_race_ob())->query_init_room();
  if (tmp && (tmp != ""))
    this_player()->move(tmp);
  else
    this_player()->move(ENTRY_POS);
  */

  this_player()->move("/game/areas/start/inicio_3.c");
  this_player()->do_look();
  return 1;
}
