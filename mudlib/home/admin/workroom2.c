#define CREATOR "admin"
/* Workroom adicional para inmortales sin experiencia. Aqui encontraras un
 * perfecto ejemplo para que puedas iniciarte en el mundo del ROL-MUD.
 * Posee las funciones basicas para generar una habitacion. Buena suerte...
 *
 * Lummen : 18 Septiembre, 1997
 */

inherit "/lib/room.c";


object trampilla;

/* Haz un inherit al fichero "/std/outside.c" si quieres que influya el
 * tiempo en tu habitacion, es decir, una habitacion del exterior.
 */

void setup()
{
   set_short("Habitación de Trabajo de "+capitalize(CREATOR));

   set_long("Esta es la habitacion donde realizas aquellas actividades de "+
      "creacion basicas de un inmortal. Puedes ver un viejo sillon al "+
      "fondo de la habitacion junto a una enorme mesa. Una pequeña "+
      "lampara proporciona luz a la habitacion, momento en el cual te "+
      "das cuenta de que tal vez necesite una limpieza.\n");

   // Selecciona el nivel de luz en la habitacion. ("help light") 
   // para mas detalles.
   set_light(80);

   add_exit("oeste", "/home/"+CREATOR+"/workroom.c", "door");
}

//En esta funcion se situaran los add_action donde se realizaran llamadas
//a las funcinoes que describan una cierta accion.
void init()
{
 ::init();
 add_action("limpiar_mesa","limpiar");
 add_action("mover_mesa","mover");
}

int limpiar_mesa(string str)
{
  if(str=="mesa")
  {
   write("Empiezas a limpiar la mesa y encuentras un papel en ella.\n");
   say(this_player()->query_cap_name()+" limpia la mesa encontrando en ella "+
    "un papel.\n");
   clone_object("/obj/misc/newcreator_paper.c")->move(this_object());
   return(1);
  }
  notify_fail("Nada en esta habitacion necesita limpiarse excepto la mesa.\n");
  return(0);
}
int mover_mesa(string str)
{
  if(str=="mesa")
  {
   write("Mueves la mesa descubriendo una trampilla debajo de esta.\n");
   say(this_player()->query_cap_name()+" mueve la mesa.\n");
   trampilla = add_exit("trampilla","/room/void.c","door");
   trampilla->set_gender(2);
   renew_exits();
   return(1);
  }
  notify_fail("¿Mover qué?\n");
  return(0);
}
