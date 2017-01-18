#define CREATOR "admin"
/**
 * Workroom para inmortales sin experiencia. Aqui intentare que 
 * podais encontrar las cosas mas basicas que se pueden hacer con una
 * room.  Tambien incluye algunas cosas un poco mas avanzadas.
 * Fairiel : 7 agosto, 2000 
 * Puertas añadidas por Folken, 25 octubre 2003
 */

/**
 * Para hacer una room, siempre tienes que heredar de otro archivo.
 * Segun el tipo de room que quieras, deberas heredar de un archivo
 * o de otro.Los archivos de los cuales puedes heredar se encuentran
 * en /std/ , y son:
 * room.c        -> para hacer habitaciones de interior.
 * outside.c     -> para hacer habitaciones en el exterior.
 * forest.c      -> para hacer habitaciones que se encuentran en un bosque
 *                  o similar (pantano, etc). 
 * underground.c -> para hacer habitaciones que se encuentren en
 *                  la suboscuridad.
 * uwater.c      -> para hacer habitaciones que se encuentran debajo del
 *                  agua.
 */
inherit "/lib/room.c";

/**
 * Para añadir puertas simples, basta con que el add_exit indique en su tipo
 * de salida que ésta debe ser "door" (ver mas abajo los add_exit).
 * Si queremos que las puertas hagan algo especial, por ejemplo que originalmente
 * aparezcan cerradas, debemos definirlas aqui para poder trabajar con ellas
 * mas adelante.
 */
static object door1, door2, trampilla;

void setup()
{
  set_short("Habitación de Trabajo de "+capitalize(CREATOR));
  
  set_long("Ésta es la habitación donde realizas aquellas actividades de "+
     "creación basicas de un inmortal. Puedes ver un viejo sillón al "+
     "fondo de la habitación junto a una enorme mesa. Una pequeña "+
     "lámpara proporciona luz a la habitación, momento en el cuál te "+
     "das cuenta de que tal vez necesite una limpieza.\n");
  
  /**
   * Selecciona el nivel de luz en la habitacion. ("help light") 
   * para mas detalles.
   */
  set_light(80);
  
  /**
   * add_item() es usado para describir todos los objetos que aparecen en
   * tu descripcion.
   */
  add_item("habitacion","La habitación desprende un olor que te envuelve "+
     "y te hace sentir extraño. Los muros parecen estar recien pintados.\n");
  
  add_item("pared","Los cuatro muros de color blanco estan sin decorar "+
     "todavía.\n");
  add_item("pintura","Hace pocas horas que los muros han sido pintados de "+
     "blanco por lo que es mejor no arrimarse a ellos.\n");
  
  /** Puedes darle a varios objetos la misma descripcion */
  add_item(({"mesa","sillon"}),"Medio tapado con un trapo para que "+
     "no se manche mientras se pintaba la habitación, da una sensación "+
     "de soledad y misterio.\n");
  
  add_item(({"lampara","lampara pequeña"}),
     "En el centro de la habitación hay una lámpara que cuelga. Esta prende "+
     "continuamente aceite con lo que consigue emitir un radio de luz "+
     "considerable.\n");
  
  /** Añade caracteristicas reales a tu habitacion */
  add_smell(({"habitacion","aire"}),"Puedes oler la pintura recien "+
      "usada en las pareces.\n");
  
  add_feel("mesa","Sientes la suavidad del fantástico roble con el que "+
     "han hecho la mesa.\n");
  
  add_sound("habitacion","El único sonido que llega hasta tus oidos es "+
      "el latido de tu corazón.\n");
  
  // En el futuro, tus habitaciones deben de poseer descripciones completas
  // sin que puedan quedarse a medias... 
  
  /**
   * Para incluir objetos en las habitaciones se usara la funcion add_clone(), 
   * para mas detalles man add_clone..
   * Estos objetos pueden ser armas, npcs o cualquier otro objeto.
   * En general se encontraran en ficheros diferentes a los que hacemos una
   * llamada con la funcion add_clone(dir,num).
   */
  add_clone("/lib/obj/coders/button.c", 1);
  
  /**
   * Aqui se describe las salidas que pudiera tener tu habitcion
   * add_exit(direccion, destino, tipo)
   * direccion: Sera lo que aparezca en el rotulo de direcciones
   * destino: Direccion donde te movera esa salida
   * tipo: El tipo de salida tiene que ver con una escalera, puerta, etc..
   * Leer /doc/roomgen/exit_type_help para mas informacion, o mirad
   * man add_exits. Aqui vienen muy bien detalladas.
   * Las salidas comunes (norte, sur, etc) deben ponerse en castellano.
   *
   * Si usamos puertas y queremos hacer con ellas algo diferente a su 
   * funcionamiento por defecto, almacenamos en un objeto lo que nos
   * devuelve la funcion add_exit y, si es correcto, llamamos a sus
   * funciones (ver /std/room/door.c para mas informacion).
   */
  
  door1 = add_exit("comun","/home/common","door");
  if (door1){
    door1->set_init_status(0);
    // Asi nos dara el mensaje de 'abres la puerta hacia xxx'
    door1->reset_message();
    door1->set_dir_other_side(CREATOR);
  }

  // Esto es necesario para añadir la salida de common a tu habitacion
  door2 = "/home/common"->add_exit( CREATOR,"/home/"+CREATOR+"/workroom","door");
  if (door2){
    door2->set_init_status(0);
    // Asi nos dara el mensaje de 'abres la puerta hacia xxx'
    door2->reset_message();
    door2->set_dir_other_side("comun");
  }    
  "/home/common"->renew_exits();

  // Asi es como añadimos una puerta 'por defecto'
  add_exit("este", "/home/"+CREATOR+"/workroom2.c", "door");
}

/**
 * La funcion init se llama automaticamente cuando se carga
 * en memoria el objeto (room, npc o lo que sea)
 * Aqui es donde tienes que definir las funciones extra.
 * man init para mas ayuda.
 */
void init()
{
  /**
   * Muy importante esta linea. Aqui llamais al init()
   * del archivo del que heredamos, y es importante.
   * No os olvideis de esta linea.
   */
  ::init();
  /**
   * La funcion add_action ata un comando verbal a una 
   * funcion local, definida en la room. El primer
   * parametro es el nombre de la funcion local, 
   * y el segundo el nombre del comando verbal.
   * man add_action para mas ayuda.
   * Se pueden definir tantas funciones locales como se 
   * deseen.
   */
  add_action("limpiar_mesa","limpiar");
  add_action("mover_mesa","mover");
}

/**
 * Funcion local. El int delante indica que hemos de
 * devolver un entero.
 * El string str que se le pasa, es lo que escribimos
 * despues del comando verbal
 */
int limpiar_mesa(string str)
{
  if(str=="mesa")
  {
    /**
     * Jugamos con las propiedades. Le preguntamos si ya 
     * hemos limpiado la mesa
     */
    if( this_object()->query_timed_property("LIMPIA")==0 ){
      /**
       * Cuando limpiamos la mesa, le añadimos
       * una timed property para que le de tiempo
       * a ensuciarse y no estar limpiandola todo 
       * el rato XD joer, que somos inmortales :P
       * Para mas ayuda:
       * man add_timed_property, query_timed_property
       * man add_property, query_property
       * man add_static_property, query_static_property
       */
      this_object()->add_timed_property("LIMPIA",1,1000);
      /**
       * Con esta funcion modificamos la descripcion de un item 
       * ya existente.
       * man modify_item
       */
      modify_item("mesa","La mesa parece haber sido limpiada recientemente.\n");
      tell_object(this_player(),"Empiezas a limpiar la mesa y "+
            "encuentras un papel en ella.\n");
      tell_room(this_object(),this_player()->query_cap_name()+" limpia la mesa "+
          "encontrando en ella un papel.\n",this_player());
      /**
       * Aqui usa una funcion distinta para clonar cosas.
       * Clone object solo clona el objeto.  Para que salga aqui lo
       * tienes que traer con el  ->move(this_object())
       */
      clone_object("/obj/misc/newcreator_paper.c")->move(this_object());
      /**
       * Devolvemos 1 porque la accion ha sido hecha con exito
       */
      return(1);
    }
    /**
     * Lo que sale cuando ya has limpiado la mesa una vez
     */
    tell_object(this_player(),"Ya has limpiado la mesa.\n");
    return(1);
  }
  /**
   * Los mensajes de error siempre se dan con notify_fail
   */
  notify_fail("Nada en esta habitación necesita limpiarse excepto la mesa.\n");
  /**
   * Devolvemos 0 por que no hemos realizado la accion que
   * deseabamos
   */
  return(0);
}

int mover_mesa(string str)
{
  if(str=="mesa")
  {
    tell_object(this_player(),"Mueves la mesa descubriendo una "+
      "trampilla debajo de esta.\n");
    tell_room(this_object(),this_player()->query_cap_name()+" mueve la mesa.\n",
      this_player());
    trampilla = add_exit("trampilla","/room/void.c","door");
    trampilla->set_gender(2);
    /**
     * Como modificamos las salidas en una room ya cargada
     * hay que actualizar las salidas. Se hace con renew_exits()
     * man renew_exits para mas ayuda
     */
    renew_exits();
    /**
     * Introduccion para impedir el paso en salidas.
     * La funcion encargada de esto es modify_exit
     * En este caso, le indicamos que queremos
     * modificar la salida "trampilla" con una funcion
     * "function" que se llama "permiso" y que debemos
     * definir. Si devuelve 0, impide el paso.  En caso
     * contrario deja pasar. La funcion modify_exit
     * tiene muchas mas utilidades. Para verlas con mas 
     * detalle, man modify_exit
     */
    modify_exit("trampilla",({"function","permiso"}));
    return(1);
  }
  notify_fail("¿Mover qué?\n");
  return(0);
}

/**
 * Funcion definida para ver quien puede pasar.
 * Solo si eres inmortal te deja pasar XD
 */
int permiso(){
  if(this_player()->query_creator()){
    tell_object(this_player(),"¡Pasa, Oh inmortal!\n");
    return(1);
  }
  notify_fail("¡¡¡Un mortal no puede entrar aquí!!!\n");
  return(0);
}
