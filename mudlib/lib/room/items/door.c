/*
 * Nuevas puertas para CcMud, neverbot 6/03
 * Modificacion de la idea original de Iolo@Rl
 *
 * query_hidden_object() devuelve 1, asi nos saltamos
 *  los find_match, neverbot 2/06
 * Solucionado error que cargaba otras rooms al destruir una
 *  con puertas (añadido flag a query_other_side_door para
 *  evitar el repop), neverbot 12/06
 */

#include <room/room.h>

inherit "/lib/core/object.c";

string dest,		// la direccion de la puerta
       dir_other_side,	// Direccion desde la que se viene por la otra puerta (norte -> sur)
       s_dest;		// direccion destino reducida (norte -> n)

int status,    	// 0 - cerrada, 1 - abierta
    init_status,	// estado inicial (0 - cerrada, 1 - abierta)
    breakable,		// se puede romper
    lockable,		// se puede cerrar con llave
    lock_status,  // 0 - cerrada con llave, 1 - sin cerrar con llave (puede o no estar abierta!!)
    max_hps, hps;	// Estado de la puerta (y del otro lado)

int gender,       // Genero de la puerta 
    number,       // Numero de la puerta (0 == singular, 1 == plural)
    reset_msg;		// Ignoramos el mensaje por genero y numero y utilizamos
    			      // un mensaje del tipo 'abres la puerta hacia xxx'

string * keys;		// Array con los nombres de las llaves que pueden
         			//  abrir/cerrar la puerta

object query_other_side_door(int flag);

// Añadido para regenerar el estado de las puertas si hacemos un 
//  update, por ejemplo (en otro caso al updatear la puerta de nuestra
//  room vuelve al estado original, pero no la otra).
// neverbot, 7/03
void dest_me()
{
  object other_door;
  if (environment(this_object()))
  {
    other_door = query_other_side_door(0);
    if (other_door)
      other_door->repop();
  }
  ::dest_me();
}

void create()
{
	dest = "";
	s_dest = "";
	dir_other_side = "";
	status = 1;
	init_status = 1;
	breakable = 1;
	max_hps = 1000;
	hps = 1000;
  lockable = 0;
  lock_status = 0;
  keys = ({ });
  gender = 1; // male
  number = 0; // single
  reset_msg = 0;
	::create();
}

void setup() {
  set_name("");
  reset_get();
  reset_drop();
  set_long("Es una puerta.\n");
}

int query_hidden_object() { return 1; }

// Añadida para tener un short si miramos las puertas
/* Eliminada, si tiene short aparece en la room!!!
string short(int dark){
  if ((this_player()->query_verb() == "mirar") || 
      (this_player()->query_verb() == "ojear"))
      return "Puerta hacia " + dest;
  else
    return ::short(dark);
}
*/

// Nuevo funcionamiento para limpiar de codigo room.c, ahora todo
//  lo relacionado con abrir y/o cerrar va aqui
void init(){
  ::init();
  add_action("open","abrir");  
  add_action("close","cerrar");
}

int query_status() { return status; }
void set_status(int i) { 
  if (i == 0){
    status = 0; 
  }
  else {
    status = 1;
    lock_status = 1;
  }
  // Obligamos a que la proxima vez que se pida el exit_string en
  //  la room haya que recalcularlo (hemos cambiado la puerta de cerrada
  //  a abierta)
  environment(this_object())->reset_short_exit_string();
  environment(this_object())->query_dirs_string();
}

int query_init_status() { return init_status; }
void set_init_status(int i) { 
  if (i == 0) 
    init_status = 0; 
  else init_status = 1;
  set_status(init_status);
}

int breakable() { return breakable; }
void set_breakable(int i) {
  if (i == 0) 
    breakable = 0;
  else breakable = 1;
}

int query_lockable() { return lockable; }
void set_lockable(int i) {
  if (i == 0){
    lockable = 0;
    lock_status = 0;
  }
  else {
    lockable = 1;
    lock_status = 1;
  }
}

int query_lock_status() { return lock_status; }
void set_lock_status(int i) {
  if (i == 0){
    lock_status = 0;
  }
  else lock_status = 1;
}

int query_door() { return 1; }
int is_open() { return status; }

// Con el nuevo sistema de mensajes personalizados, hay algunas direcciones
// de uso comun que no funcionan todo lo bien que deberian, nos encargamos de 
// ellas aqui, neverbot 10/03
void set_dest(string str) { 

        // Forzamos a que con estas direcciones siempre se resetee
        // el mensaje personalizado de apertura, cierre, etc
        // De este modo nos saldra 'abres la puerta hacia xxx'
	if ((str == "dentro") || (str == "fuera") ||
	    (str == "arriba") || (str == "abajo")){
	   reset_msg = 1;
	}

	// Hacemos que se puedan abrir o cerrar las puertas con sus
	// versiones reducidas (norte -> n, etc)
    s_dest = EXIT_HAND->query_reduced_exit_name(str);
	dest = str; 
}
string query_short_dest() { return s_dest; }
string query_dest() { return dest; }
void set_dir_other_side(string str) { dir_other_side = str; }
string query_dir_other_side() { return dir_other_side; }
string * query_keys() { return keys; }
void set_keys(string * list) { 
   keys = list; 
   set_lockable(1);
}
void add_key(string str) { 
  keys += ({ str }); 
  set_lockable(1);
}

int query_known_exit(string name){
  switch(name){
    case "abajo":
    case "arriba":
    case "dentro":
    case "fuera":
    case "norte":
    case "sur":
    case "este":
    case "oeste":
    case "sudoeste":
    case "sudeste":
    case "noreste":
    case "noroeste":
      return 1;
    default:
      return 0;
  }
}

void open_msg(string door, object ob, int flag);
void close_msg(string door, object ob, int flag);

int open(varargs string str) 
{
  object other_door;
  int find, i, aux;
  object * obs;
  string key;
  string ex;

  if (this_player()->query_dead())
  {
      notify_fail("Estando muerto tienes poco control sobre el mundo material.\n");
      return 0;
  }

  // Si no estamos abriendo ESTA puerta
  if (!str || (str == "") || ((str != dest) && (str != s_dest)))
    return 0;

  // Si ya esta abierta no la podemos abrir
  if (this_object()->query_status() == 1) 
    return 0;
  else {

    // Si se puede cerrar con llave y ESTÁ cerrada con llave:
    if ((lockable) && (lock_status == 0)){
      find = -1;
      obs = deep_inventory(this_player());
      for(i = 0; i < sizeof(obs); i++){
        if (obs[i]->query_key()){
          key = file_name(obs[i]);
          // Esto no deberia darse nunca...
          aux = strsrch(key, "#");
          if (aux != -1)
             key = extract(key, 0, aux - 1);
          if (member_array(key, keys) != -1){
             find = i;
             break;
          }
        }
      }
      if (find == -1){
      	if (reset_msg){
          notify_fail("La puerta hacia " + dest  + " está cerrada con llave.\n");
          return 0;
        }
        if (query_known_exit(dest)){
          notify_fail("La puerta " + dest  + " está cerrada con llave.\n");
          return 0;
        }
        ex = "";
        if (gender == 1) { // male
          ex += (!number)?("El "):("Los ");
        } else {
          ex += (!number)?("La "):("Las ");
        }
        ex += dest;
        if (gender == 1) { // male
          ex += (!number)?(" está cerrado con llave"):(" están cerrados con llave");
        } else {
          ex += (!number)?(" está cerrada con llave"):(" están cerradas con llave");
        }
        ex += ".\n";
        notify_fail(ex);
        return 0;

      } else {
        tell_object(this_player(), "Utilizas tu " + obs[i]->short() + ".\n");
      }
    }

    set_status(1);
    
    open_msg(dest, this_player(), 0); 
    // tell_object(this_player(), "Abres la puerta "+dest+".\n");
    // tell_room(environment(this_object()), this_player()->query_cap_name() +
    //           " abre la puerta "+dest+".\n", ({ this_player() }));
    
    other_door = query_other_side_door(1);    
    if (other_door && !other_door->is_open()) {
      other_door->set_status(1);
      open_msg(dir_other_side, other_door, 1);
      // tell_room(environment(other_door), 
      //   "Alguien abre la puerta "+dir_other_side+" desde el otro lado.\n");
    }
    return 1; 
  }
}

/*
 * Funcion que muestra el mensaje de apertura de puerta
 *  Si flag == 0:
 *   - Abres la puerta door + 
 *   - ob abre la puerta door
 *  Si flag == 1:
 *   - Alguien abre la puerta door desde el otro lado
 */
void open_msg(string door, object ob, int flag){

  string ex1, ex2;

  if (flag == 0){
    if (reset_msg){
      tell_object(ob, "Abres la puerta hacia "+door+".\n");
      tell_room(environment(ob), ob->query_cap_name() +
              " abre la puerta hacia "+door+".\n", ({ ob }));
      return;
    }
  	
    if (query_known_exit(door)){
      tell_object(ob, "Abres la puerta "+door+".\n");
      tell_room(environment(ob), ob->query_cap_name() +
              " abre la puerta "+door+".\n", ({ ob }));
      return;
    }

    ex1 = "Abres ";
    ex2 = ob->query_cap_name() + " abre ";

    if (gender == 1) { // male
        ex1 += (!number)?("el "):("los ");
        ex2 += (!number)?("el "):("los ");
    } else {
        ex1 += (!number)?("la "):("las ");
        ex2 += (!number)?("la "):("las ");
    }
    ex1 += door + ".\n";
    ex2 += door + ".\n";
    
    tell_object(ob, ex1);
    tell_room(environment(ob), ex2, ({ ob }) );

  }
  else{
    if (reset_msg){
      tell_room(environment(ob), 
        "Alguien abre la puerta hacia "+door+" desde el otro lado.\n");
      return;
    }
    if (query_known_exit(door)){
      tell_room(environment(ob), 
        "Alguien abre la puerta "+door+" desde el otro lado.\n");
      return;
    }

    ex1 = "Alguien abre ";
    if (gender == 1) { // male
          ex1 += (!number)?("el "):("los ");
    } else {
          ex1 += (!number)?("la "):("las ");
    }
    ex1 += door + " desde el otro lado.\n";
    
    tell_room(environment(ob), ex1);
  }
}

void close_msg(string door, object ob, int flag){
  string ex1, ex2;

  if (flag == 0){
    if (reset_msg){
      tell_object(ob, "Cierras la puerta hacia "+door+".\n");
      tell_room(environment(ob), ob->query_cap_name() +
              " cierra la puerta hacia "+door+".\n", ({ ob }));
      return;
    }
    if (query_known_exit(door)){
      tell_object(ob, "Cierras la puerta "+door+".\n");
      tell_room(environment(ob), ob->query_cap_name() +
              " cierra la puerta "+door+".\n", ({ ob }));
      return;
    }

    ex1 = "Cierras ";
    ex2 = ob->query_cap_name() + " cierra ";

    if (gender == 1) { // male
          ex1 += (!number)?("el "):("los ");
          ex2 += (!number)?("el "):("los ");
    } else {
          ex1 += (!number)?("la "):("las ");
          ex2 += (!number)?("la "):("las ");
    }
    ex1 += door + ".\n";
    ex2 += door + ".\n";
    
    tell_object(ob, ex1);
    tell_room(environment(ob), ex2, ({ ob }) );

  }
  else{
    if (reset_msg){
      tell_room(environment(ob), 
        "Alguien cierra la puerta hacia "+door+" desde el otro lado.\n");
      return;
    }
    if (query_known_exit(door)){
      tell_room(environment(ob), 
        "Alguien cierra la puerta "+door+" desde el otro lado.\n");
      return;
    }

    ex1 = "Alguien cierra ";
    if (gender == 1) { // male
          ex1 += (!number)?("el "):("los ");
    } else {
          ex1 += (!number)?("la "):("las ");
    }
    ex1 += door + " desde el otro lado.\n";
    
    tell_room(environment(ob), ex1);
  }
}

int close(varargs string str) 
{ 
  object other_door;
  string * list;
  int i, find, aux;
  string key;
  object * obs;
  string ex;
  
  if (this_player()->query_dead())
  {
      notify_fail("Estando muerto tienes poco control sobre el mundo material.\n");
      return 0;
  }

  if (!str || (str == "") || (str == " "))
    return 0;

  list = explode(str, " ");
  str = list[0];
  
  // Si no estamos cerrando ESTA puerta
  if ((str != dest) && (str != s_dest))
    return 0;

  if (this_object()->is_broken()){
        if (reset_msg){
          notify_fail("La puerta hacia " + dest  + " está rota, no puedes cerrarla.\n");
          return 0;
        }
        if (query_known_exit(dest)){
          notify_fail("La puerta " + dest  + " está rota, no puedes cerrarla.\n");
          return 0;
        }
        ex = "";
        if (gender == 1) { // male
          ex += (!number)?("El "):("Los ");
        } else {
          ex += (!number)?("La "):("Las ");
        }
        ex += dest;
        if (gender == 1) { // male
          ex += (!number)?(" está roto, no puedes cerrarlo"):(" están rotos, no puedes cerrarlos");
        } else {
          ex += (!number)?(" está rota, no puedes cerrarla"):(" están rotas, no puedes cerrarlas");
        }
        ex += ".\n";
        notify_fail(ex);
        return 0;
  }

  if ((sizeof(list) >= 3) && (list[1] == "con") && (list[2] == "llave")){

    // Si se puede cerrar con llave y ESTÁ cerrada con llave:
    if ((lockable) && (lock_status == 0)){
        if (reset_msg){
          notify_fail("La puerta hacia " + dest  + " ya está cerrada con llave.\n");
          return 0;
        }
        if (query_known_exit(dest)){
          notify_fail("La puerta " + dest  + " ya está cerrada con llave.\n");
          return 0;
        }
        ex = "";
        if (gender == 1) { // male
          ex += (!number)?("El "):("Los ");
        } else {
          ex += (!number)?("La "):("Las ");
        }
        ex += dest;
        if (gender == 1) { // male
          ex += (!number)?(" ya está cerrado"):(" ya están cerrados");
        } else {
          ex += (!number)?(" ya está cerrada"):(" ya están cerradas");
        }
        ex += " con llave.\n";
        notify_fail(ex);
        return 0;

    }

    if ((lockable) && (lock_status == 1)){
      find = -1;
      obs = all_inventory(this_player());
      for(i = 0; i < sizeof(obs); i++){
        if (obs[i]->query_key()){
          key = file_name(obs[i]);
          // Esto no deberia darse nunca...
          aux = strsrch(key, "#");
          if (aux != -1)
             key = extract(key, 0, aux - 1);
          if (member_array(key, keys) != -1){
             find = i;
             break;
          }
        }
      }
      if (find == -1){
        // notify_fail("No tienes la llave necesaria para cerrar la puerta " + dest + ".\n");
        notify_fail("No tienes la llave necesaria.\n");
        return 0;
      } else {
        tell_object(this_player(), "Utilizas tu " + obs[i]->short() + ".\n");
        lock_status = 0;
      }
    }
  }

  if (!query_status()){
    other_door = query_other_side_door(1);
    if (lockable && (lock_status == 0)){
      if (reset_msg){
        tell_room(environment(this_object()),
           "Oyes un chasquido en la cerradura de la puerta hacia "+dest+".\n");
      }
      else
      if (query_known_exit(dest)){
        tell_room(environment(this_object()),
           "Oyes un chasquido en la cerradura de la puerta "+dest+".\n");
      }
      else {
        ex = "Oyes un chasquido en la cerradura ";
        if (gender == 1) { // male
          ex += (!number)?("del "):("de los ");
        } else {
          ex += (!number)?("de la "):("de las ");
        }
        ex += dest + ".\n";
        tell_room(environment(this_object()), ex);
      }
      if (other_door) {
         other_door->set_lock_status(0);
         if (reset_msg){
           tell_room(environment(other_door),
              "Oyes un chasquido en la cerradura de la puerta hacia "+dir_other_side+".\n");
         }
         else
         if (query_known_exit(dir_other_side)){
           tell_room(environment(other_door),
              "Oyes un chasquido en la cerradura de la puerta "+dir_other_side+".\n");
         }
         else {
           ex = "Oyes un chasquido en la cerradura ";
           if (gender == 1) { // male
             ex += (!number)?("del "):("de los ");
           } else {
             ex += (!number)?("de la "):("de las ");
           }
           ex += dir_other_side + ".\n";
           tell_room(environment(other_door), ex);
         }
      }
    } else {
      if (reset_msg){
        tell_object(this_player(), "La puerta hacia " + dest + " ya está cerrada.\n");
      }
      else
      if (query_known_exit(dest)){
        tell_object(this_player(), "La puerta " + dest + " ya está cerrada.\n");
      }
      else {
        ex = "";
        if (gender == 1) { // male
          ex += (!number)?("El "):("Los ");
        } else {
          ex += (!number)?("La "):("Las ");
        }
        ex += dest + " ya está"   +((!number)?(""):("n"));
        if (gender == 1) { // male
          ex += (!number)?(" cerrado.\n"):(" cerrados.\n");
        } else {
          ex += (!number)?(" cerrada.\n"):(" cerradas.\n");
        }
        tell_object(this_player(), ex);
      }
    }
    return 1;
  }
  else {
    
    set_status(0);
    close_msg(dest, this_player(), 0);
    // tell_object(this_player(), "Cierras la puerta "+dest+".\n");
    // tell_room(environment(this_object()), this_player()->query_cap_name() +
    //           " cierra la puerta "+dest+".\n", ({ this_player() }));
    other_door = query_other_side_door(1);
    if (other_door && other_door->is_open()) {
      other_door->set_status(0);
      close_msg(dir_other_side, other_door, 1);
      // tell_room(environment(other_door),
      //    "Alguien cierra la puerta "+dir_other_side+" desde el otro lado.\n");
      if (lockable && lock_status == 0){
         other_door->set_lock_status(0);
         if (reset_msg){
           tell_room(environment(other_door),
              "Oyes un chasquido en la cerradura de la puerta hacia "+dir_other_side+".\n");
         }
         else
         if (query_known_exit(dir_other_side)){
           tell_room(environment(other_door),
              "Oyes un chasquido en la cerradura de la puerta "+dir_other_side+".\n");
         }
         else {
           ex = "Oyes un chasquido en la cerradura ";
           if (gender == 1) { // male
             ex += (!number)?("del "):("de los ");
           } else {
             ex += (!number)?("de la "):("de las ");
           }
           ex += dir_other_side + ".\n";
           tell_room(environment(other_door), ex);
         }
      }
    }
    return 1;
  }
}

int query_max_hps() { return max_hps; }
void set_max_hps(int i) {
  object other_door;

  max_hps = i;
  other_door = query_other_side_door(1);
  
  if (other_door && 
     (other_door->query_max_hps() != this_object()->query_max_hps()) )  
    other_door->set_max_hps(i);

  this_object()->set_hps(i);
}

int query_hps() { return hps; }
void adjust_hps(int i) { 
  this_object()->set_hps(this_object()->query_hps() + i);
}
void set_hps(int i) { 
 object other_door;

 if (this_object()->query_hps() < 0) 
    return;
 if (i <= max_hps) 
   hps = i; 
 else 
   return;
 
 other_door = query_other_side_door(1);
 if (other_door && 
    (other_door->query_hps() != this_object()->query_hps()) )
   other_door->set_hps(i);   
 
 if (this_object()->query_hps() < 0) 
   this_object()->do_break();
}

void do_damage(int i) { 
  if ((i > 0) && 
      (!this_object()->is_open()) && 
      this_object()->breakable()) 
    this_object()->adjust_hps(-i);
}

void do_break() {

 string ex;

 if (reset_msg){
    tell_room(environment(this_object()),"La puerta hacia "+dest+" se rompe en mil pedazos.\n");
 }
 if (query_known_exit(dest)){
   tell_room(environment(this_object()),"La puerta "+dest+" se rompe en mil pedazos.\n");
 }
 else {
   ex = "";
   if (gender == 1) { // male
     ex += (!number)?("El "):("Los ");
   } else {
     ex += (!number)?("La "):("Las ");
   }
   ex += dest + " se rompe"+((!number)?(""):("n"))+" en mil pedazos.\n";
   tell_room(environment(this_object()), ex);
 }

 this_object()->open();
}

int is_broken() { 
  if (this_object()->query_hps() < 0) 
    return 1; 
  else 
    return 0; 
}

string health_string() {
  switch ( (query_hps() * 100) / query_max_hps()) {
    case 100:    return ("Está en perfecto estado."); break;
    case 80..99: return ("Está un poco estropeada."); break;
    case 60..79: return ("No está en buen estado."); break;
    case 40..59: return ("Parece estar en mal estado."); break;
    case 20..39: return ("Está en mal estado."); break;
    case 0..19:  return ("Casi no se tiene en pie."); break;
    default: return ("Está rota."); break;
  }
  return "";
}

// Si flag==1 cargamos la otra room para asegurarnos de obtener su objeto door
// Si flag==0 no la cargamos (estamos destruyendo cosas)
object query_other_side_door(int flag) {
  
  mixed *exits;
  object other_room, other_door;
  int i;
  
  if (!environment(this_object()))
    return nil;
  
  exits = environment(this_object())->query_dest_dir();
  i = 0;
  while (i < sizeof(exits)) {
    if (exits[i] == dest) {
      other_room = find_object(exits[i+1]);
      if (!other_room && flag) {
        load_object(exits[i+1]);
        other_room = find_object(exits[i+1]);
      }
      i = sizeof(exits);
    }
    i += 2;
  }
   
  if (other_room) {
    other_door = other_room->query_doors(dir_other_side);
  }
  return (other_door);
}
     
void repop() {
  
  string ex;
  set_max_hps(query_max_hps());

  if (!query_status() && (init_status == 1)){
    // tell_room(environment(this_object()), "La puerta "+dest+" se abre lentamente.\n");

    if (reset_msg){
      tell_room(environment(this_object()), "La puerta hacia "+dest+" se abre lentamente.\n");    	
    }
    else if (query_known_exit(dest)){
      tell_room(environment(this_object()), "La puerta "+dest+" se abre lentamente.\n");
    }
    else {
      ex = "";
      if (gender == 1) { // male
        ex += (!number)?("El "):("Los ");
      } else {
        ex += (!number)?("La "):("Las ");
      }
      ex += dest + " se abre"+((!number)?(""):("n"))+" lentamente.\n";
      tell_room(environment(this_object()), ex);
    }
  }
  if (query_status() && (init_status == 0)){
    // tell_room(environment(this_object()),"La puerta "+dest+" se cierra lentamente.\n");

    if (reset_msg){
       tell_room(environment(this_object()),"La puerta hacia "+dest+" se cierra lentamente.\n");
    }
    else if (query_known_exit(dest)){
       tell_room(environment(this_object()),"La puerta "+dest+" se cierra lentamente.\n");
    }
    else {
      ex = "";
      if (gender == 1) { // male
        ex += (!number)?("El "):("Los ");
      } else {
        ex += (!number)?("La "):("Las ");
      }
      ex += dest + " se cierra"+((!number)?(""):("n"))+" lentamente.\n";
      tell_room(environment(this_object()), ex);
    }
  }
    
  set_status(init_status);

  if (!is_open() && query_lockable()) {
    // tell_room(environment(this_object()),"Oyes un chasquido en la cerradura de la puerta "+dest+".\n");
    if (reset_msg){
      tell_room(environment(this_object()),"Oyes un chasquido en la cerradura de la puerta hacia "+dest+".\n");
    }
    else
    if (query_known_exit(dest)){
      tell_room(environment(this_object()),"Oyes un chasquido en la cerradura de la puerta "+dest+".\n");
    }
    else {
      ex = "Oyes un chasquido en la cerradura ";
      if (gender == 1) { // male
        ex += (!number)?("del "):("de los ");
      } else {
        ex += (!number)?("de la "):("de las ");
      }
      ex += dest + ".\n";
      tell_room(environment(this_object()), ex);
    }
    lock_status = init_status;
  }
}  
      
void set_gender(int i) { 
  if (i == 2) 
    // female
    gender = 2;
  else
    // male
    gender = 1; 
}

void set_number(int i) { 
  if (i == 0)
    number = i;
  else
    number = 1; 
}

int query_gender() { return gender; }
int query_number() { return number; }
void reset_message() { reset_msg = 1; }
int query_reset_message() { return reset_msg; }
