/* 
 * Nuevo sistema de estilos, Folken para Ciudad Capital, 27/4/2003
 * ([ nombre_estilo : ({ num_dados, tipo_dados, lista_mensajes }) ])
 *
 * La lista de mensajes es a su vez otra lista de arrays
 *   lista_inicial
 *   lista_media
 *   lista_experto
 *   lista_mas_experto
 *   ... (el numero puede ser cualquiera > 1)
 *
 * Se utilizara cada una dependiendo del dominio de la tecnica por parte
 *  del atacante (por ejemplo si son tres, al tener 30%, 60% y 90%, el 
 *  porcentaje al que se mostrara cada mensaje se calcula solo)
 *
 * Cada una de esas listas tiene 3 elementos, mensaje para el atacante, 
 *  mensaje para el defensor y mensaje para la room
 *
 * Nota: En el primero (pelea) los mensajes no cuentan. Por ser el estilo mas
 *  utilizado los mensajes estan en /std/living/unarmed_combat.c, para cargar
 *  menos la maquina con la busqueda de los mensajes
 */

// save file
// #define SAVEFILE "/save/unarmed_combat_handler"

mapping unarmed_combat_styles;

#define DEFAULT_NUM_DICE 1
#define DEFAULT_DICE_SIZE 3

void create()
{
  seteuid("Room");
  unarmed_combat_styles = ([ 
    "pelea" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
         ({
            ({ // Mensajes hasta el 33%
               ({ "golpeas", "das un puñetazo", "magullas", }), 
               ({ "te golpea", "te da un puñetazo", "te magulla", }), 
               ({ "golpea", "da un puñetazo", "magulla", }), 
            }),
            ({ // Mensajes hasta el 66%
                ({ "golpeas", "das un puñetazo", "magullas", "golpeas con fuerza", }), 
                ({ "te golpea", "te da un puñetazo", "te magulla", "te golpea con fuerza", }), 
                ({ "golpea", "da un puñetazo", "magulla", "golpea con fuerza", }), 
            }),
            ({ // Mensajes hasta el 66%
                ({ "golpeas", "das un puñetazo", "magullas", "golpeas con fuerza", "golpeas con furia y rabia", }), 
                ({ "te golpea", "te da un puñetazo", "te magulla", "te golpea con fuerza", "te golpea con furia y rabia", }), 
                ({ "golpea", "da un puñetazo", "magulla", "golpea con fuerza", "golpea con furia y rabia", }), 
            }),
         }),
       }),
    "test" : ({ 1, 4, 
         ({
           ({ "haces daño", "te hace daño", "hace daño", }),
           ({ "haces mucho daño", "te hace mucho daño", "hace mucho daño", }),
         }),
       }),

    // *****************************
    //   Para npcs
    // *****************************

    "colmillos" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
       ({ ({ "muerdes con tus colmillos", "te muerde con sus colmillos", "muerde con sus colmillos", }), }),
                }),       
    "garras" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
       ({ ({ "arañas con tus garras", "te araña con sus garras", "araña con sus garras", }), }),
                }),       
    "pico" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
       ({ ({ "golpeas con el pico", "te golpea con el pico", "golpea con el pico", }), }),
                }),       
    "cuernos" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
       ({ ({ "corneas", "te cornea", "cornea", }), }),
                }),       
    "pezuñas" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
       ({ ({ "golpeas con las pezuñas", "te golpea con sus pezuñas", "golpea con sus pezuñas", }), }),
                }),       
    "dientes" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
       ({ ({ "muerdes", "te muerde", "muerde", }), }),
                }),       
    "zarpas" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
       ({ ({ "das un zarpazo", "te da un zarpazo", "da un zarpazo", }), }),
                }),       
    "cola" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
       ({ ({ "das un coletazo", "te da un coletazo", "da un coletazo", }), }),
                }),       
    "patas" : ({ DEFAULT_NUM_DICE, DEFAULT_DICE_SIZE, 
       ({ ({ "golpeas con tus patas", "te golpea con sus patas", "golpea con sus patas", }), }),
                }),       

   ]);
   
   // restore_object(SAVEFILE);
}

int query_valid_attack(string attack, string style)
{
  if(member_array(attack, unarmed_combat_styles[style][2])>-1)
    return 1;
  return 0;
}

string *query_unarmed_styles()
{
  return keys(unarmed_combat_styles);
}

int style_exists(string name)
{
    return (member_array(name, keys(unarmed_combat_styles)) != -1);
}

mapping query_all_uc_info(){
  return unarmed_combat_styles;
}

// Funciones añadidas:

// Cuando el personaje ataca por primera vez, o cuando cambia su estilo de
//  lucha, cambiamos el numero y tipo de dados que utilizara a la hora de calcular
//  el daño que hace, Folken 4/03
int set_damage_dice(string style, object player)
{
  // Si el estilo no existe 
  if (!unarmed_combat_styles[style])
     return 0;
  player->set_damage_dice(unarmed_combat_styles[style][0], 
                          unarmed_combat_styles[style][1]);
  return 1;
}

// Funcion que devuelve los mensajes personalizados
mixed * query_messages(string style, int ability, object att)
{
	int divs;
	// int rand;
	int list;

  list = 0;

	// Si no existe el estilo devolvemos la lista vacia
	if (!unarmed_combat_styles[style])
	   return ({ });
	   
	// divs almacena cuantas listas de mensajes hay
	divs = sizeof(unarmed_combat_styles[style][2]);

	// Obtenemos cual es la lista que tendremos que utilizar segun
	//  nuestra habilidad de entre todas la que hay
	list = ability/(100/divs);
	
	// Al llegar al 100% podemos salirnos del array y tener problemas
	if (list >= divs)
	   list = divs - 1;

    // Para darle aun mas realismo, tendremos un random para ver
    //  que lista usamos de todas las que tengamos posibles.
    // rand = random(list + 1);

    // Odio los arrays de tantas dimensiones!!!! Folken 4/03	
	return unarmed_combat_styles[style][2][list];
}


/* Comentadas, Folken 6/03. Este archivo ya no funciona como handler, sino como tabla comun.
void add_uc_style(string name, int num_dice, int dam_die, string *moves){
  unarmed_combat_styles+=([ name : ({ num_dice, dam_die, moves }) ]);
  save_object(SAVEFILE);
}

void remove_uc_style(string name){
  map_delete(unarmed_combat_styles, name);
  save_object(SAVEFILE);
}
*/

/* This has to be redone, to take into consideration the different bonuses and
 * such.
 *
 * Con el nuevo sistema se actualizan solos los dados que utilizamos a la hora de
 *  combatir, con lo que esta funcion (que los calcula de nuevo y carga mucho a la
 *  hora de buscar repetidas veces en los mappings) ya no es necesaria, Folken 04/03
 */
/*
int unarmed_damage(string style, int num, int type){
  int res;
  // Si el estilo no existe 
  if (!unarmed_combat_styles[style])
     return 0;

  // Si por defecto vamos a tirar mas dados (y de tipo mas alto que los correspondientes
  //  al estilo de lucha que estamos usando, utilizaremos lo de por defecto.
  if((num > unarmed_combat_styles[style][0]) && 
     (type >= unarmed_combat_styles[style][1]))
    res = roll(num, type);
  else
    res = roll(unarmed_combat_styles[style][0], unarmed_combat_styles[style][1]);
  return res;
}
*/
