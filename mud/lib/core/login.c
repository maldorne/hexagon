
// old new_login.c object, now this is the only way to log in
// added some things from the old login.c
//   neverbot 21/01/03
// changed login system to include user accounts
//   neverbot 12/2010
// ported to dgd, neverbot 03/2014

inherit "/lib/core/object";

#include <kernel.h>
#include <mud/config.h>
#include <mud/secure.h>
#include <mud/access.h>
#include <mud/version.h>
#include <mud/patch_version.h>
#include <user/login.h>
#include <user/user.h>
#include <areas/common.h>
#include <user/player.h>

static object _player;

static int validated;
static object account;
static string password, account_name;
// static int cols;
static int gender;
static int coder;
// static int app_coder;
static int no_times;
static int invis_wish;
static object new_copy, other_copy;

// static string player_ob;

// prototypes
nomask void logon(object pl);
nomask void show_options();
nomask void logon_player_name(string str, int flag);
nomask void logon_player_name_2(string str, int flag);
nomask void logon_account(string str);
nomask void begin(int new_player); 
nomask void begin2(int new_player); 
nomask void time_out();
nomask void disconnect(varargs int silence);

void create_account(string str);
void create_account2(string str);
void create_account3(string str);
void create_account4();



void create() 
{
  ::create();

  validated = FALSE;

  no_times = 0;
  coder = 0;
  password = "";
  gender = 1;
  invis_wish = -1;
  seteuid(ROOT);
} 

nomask int test_user(string str) 
{
  string tmp;
  object user;
  int result;

  if (!stringp(str) || sscanf(str, "%s ",tmp))
    return 0;

  user = clone_object("/lib/user.c");
  user->set_name(str);
  result = user->restore_me();

  user->dest_me();
  return result;
}

nomask void time_out() 
{
  if (validated == TRUE)
    return;

  if (!_player)
    return;

  write("Se terminó el tiempo.\n");
  destruct(_player);
  destruct(this_object());
}

// nomask void quit()
// {
//   destruct(_player);
//   destruct(this_object());
// }

nomask void logon(object pl) 
{
  int tim;

  _player = pl;

  write("Versión de LPmud: " + version() + "\n" +
        "Versión de Mudlib: " + MUDLIB_VERSION + "\n\n"/* + 
        "Último parche: " + PATCH_VERSION + "\n\n"*/);

  call_out("time_out", LOGIN_TIMEOUT);

  cat(WELCOME_MESSAGE);

  // TODO check shut object to avoid login near a shutdown
  // if (find_object("/obj/shut")) 
  // {
  //   tim = (int)"/obj/shut"->query_time_to_crash();
  //   if ((tim > 0) && (tim < 120))
  //   {
  //     write("Queda muy poco para el reinicio de "+mud_name()+", inténtalo dentro de unos minutos.\n");
  //     dest_me();
  //     return;
  //   }
  //   else 
  //   { 
  //     int mins;
  //     mins = (tim/60);
  //     write("El juego se reiniciará en "+mins+" minuto"+((tim<2)?"":"s")+".\n");
  //   }
  // }

  // added from old login.c, neverbot 21/01/03
  if (uptime() < MIN_UPTIME_LOGIN_TIME) 
  {
    write("El mundo aún se está formando, inténtalo de nuevo en unos minutos.\n");
    disconnect(1);
    return;
  }

  if (sizeof(users()) > MAX_PLAYERS) 
  {
    write("Hay demasiados jugadores conectados, inténtalo dentro de unos minutos.\n");
    disconnect(1);
    return;
  }

  show_options();

  write("Introduce la opción elegida: ");
  input_to("logon_option");
}

nomask void show_finger(string who)
{
  write("Hacemos un finger\n");

  /*  
  string ret = "/secure/finger.c"->finger_info(who, this_object());

  if (!ret || (ret == ""))
  {
    write("\nNo hay ningún jugador con el nombre '"+who+"'.\n\n");
    return;
  }
  ret = "\n--------------------------------------------------------------------------\n"+
        ret +
        "--------------------------------------------------------------------------\n\n";

  write( "/global/events.c"->fix_string(ret) );
  */
}

nomask void show_who()
{
  write("Hacemos un who\n");
  /*
  object who = load_object("/cmds/player/who.c");
  string ret = "";

  if (!who)
  {
    write("Ha habido un error con el comando.\n");
    return;
  }

  ret = who->who_string(79, 0, 0) + "\n\n";
  write( "/global/events.c"->fix_string(ret) );
  */
}

nomask void show_options()
{
  // new options without login, neverbot 2/06
  string ret;
  ret = "Opciones: %^BOLD%^GREEN%^<email o nombre de personaje>%^RESET%^ para comenzar a jugar.\n"+
        "          %^BOLD%^crear%^RESET%^ para crear una nueva cuenta.\n"+
        "          %^BOLD%^invitado%^RESET%^ para probar el juego temporalmente.\n"+
        "          %^BOLD%^finger <nombre>%^RESET%^ para obtener datos sobre otro jugador.\n"+
        "          %^BOLD%^who%^RESET%^ para ver la lista de jugadores conectados.\n\n";

  write( ret );
}

nomask void logon_option(string str) 
{
  string * list;
  int tmp;

  if (!strlen(str)) 
  {
    write("¡Vuelve dentro de poco!\n");
    destruct(_player);
    destruct(this_object());
    return;
  }

  str = lower_case(str);

#ifdef PLAYER_LOCKOUT
  // This is a part of the lockout..
  if ((str == "invitado") || (str == "guest"))
  {
    write("El acceso a jugadores no está permitido en estos momentos.\n");
    destruct(_player);
    destruct(this_object());
    return;
  }

  if (!read_file("/home/"+str+"/workroom.c")) 
  {
    write("El acceso a jugadores no está permitido en estos momentos");
    destruct(_player);
    destruct(this_object());
  }
#endif

  // new options, neverbot 02/2006
  list = explode(str, " ");
  if ((sizeof(list) >= 2) && (list[0] == "finger"))
  {
    show_finger(list[1]);

    show_options();
    write("Introduce la opción elegida: ");
    input_to("logon_option");
    return;
  }

  if (str == "who")
  {
    show_who();

    show_options();
    write("Introduce la opción elegida: ");
    input_to("logon_option");
    return;
  }

  if (str == "crear")
  {
    string ret;
    ret = "Si ya tienes otra cuenta en "+mud_name()+", recomendamos encarecidamente que crees tus " +
      "distintos personajes dentro de la misma cuenta, ya que esto tiene muchas ventajas a la " +
      "hora de compartir objetos, recursos, etc.\n";
    ret = sprintf("\n%-*s\n", _player->query_cols(), ret);
    write( ret );   
    
    write("Introduce el nombre que tendrá tu cuenta: ");
    input_to("create_account");
    return;
  }

  // from the old login.c, neverbot 21/01/03
  // More of Hamlet's cute crap.
  if(str[0] == '#') 
  {
    invis_wish = 0;
    str = str[1..];
  }
  else if(str[0] == '*') 
  {
    invis_wish = 1;
    str = str[1..];
  }
  else if(str[0] == '@') 
  {
    invis_wish = 2;
    str = str[1..];
  }

  /*
  TODO bastards.c
  // query_player_ob() should only be passed 1 as its 2nd argument HERE!
  //   It's a big mistake to pass a second arg at all anywhere else. - Hamlet
  player_ob = (string)"/secure/bastards"->query_player_ob(str, 1);
  // changed next line to be stringp - Hamlet 
  if (!stringp(player_ob)) 
  {
    write("Lo sentimos, pero ese nombre no está permitido.\n");
    destruct(_player);
    destruct(this_object());
    return ;
  }
  */

  // A hack while login_handler is being looked at -- Hamlet
  if(sizeof(users()) > MAX_PLAYERS - CODER_SLOTS) 
  {
    write("Hay demasiados jugadores conectados, inténtalo en unos minutos.\n");
    destruct(_player);
    destruct(this_object());
    return;
  }
  // End hack

  // TODO guest login
  // if ((str == "invitado") || (str == "guest"))
  //   return guest_login();

  // now we accept emails as accounts names
  // if (strlen(str) > MAX_LEN) 
  // {
  //   write("El nombre es demasiado largo, el máximo son 11 caracteres.\n");
  //   write("Introduce el nombre de la cuenta: ");
  //   input_to("logon_option");
  //   return;
  // }

  // added by neverbot 21/01/03
  if (strlen(str) < MIN_LEN) 
  {
    write("El nombre es demasiado corto, el mínimo son 3 caracteres.\n");
    write("Introduce el nombre de la cuenta: ");
    input_to("logon_option");
    return;
  }

  if (((tmp = SECURE->valid_user_name(str)) != -1) && !SECURE->valid_email(str)) 
  {
    write("Caracter inválido '"+str[tmp..tmp]+"' en ("+str+").\n\n");
    write("Introduce el nombre de la cuenta: ");
    input_to("logon_option");
    return;
  }

  if ((file_size("/save/accounts/"+str[0..0]+"/"+str+".o") == -1) &&
      (file_size("/save/players/"+str[0..0]+"/"+str+".o") == -1))
  {
    write("No existe ninguna cuenta o personaje con ese nombre.\n");

    show_options();

    write("Introduce la opción elegida: ");
    input_to("logon_option");
    return;
  }   

  // Si hemos introducido el nombre de un personaje
  if (file_size("/save/players/"+str[0..0]+"/"+str+".o") > 0)
  {
    // flag para indicar que estamos haciendo login directamente con el nombre 
    // del personaje
    logon_player_name(str, 0); 
  }
  // hemos introducido el nombre de cuenta
  else 
  {
    if (!_player->set_account(str))
    {
      write("No existe una cuenta con ese nombre.\n");
      write("Introduce el nombre de la cuenta: ");
      input_to("logon_option");
      return;
    }

    account = _player->query_account();
  
    write("Introduce la contraseña asociada a tu cuenta: ");
    input_to("logon_account", 1);
  }
}

// si flag == 0, hacemos login directamente con el nombre del player
// si flag == 1, hacemos login con el nombre de cuenta
nomask void logon_player_name(string str, int flag) 
{
  string * list;
  string chosen;
  int value;

  chosen = "";

  // login with the account name
  if (flag == 1)
  {
    // creating a new player in the account
    if (str == "nuevo")
    {
      write("Introduce el nombre de tu nuevo personaje: ");
      input_to("create_player", 1);
      return;
    }

    list = account->query_player_list();
      
    // existant character name
    if ((value = member_array(str, list)) != -1)
      chosen = list[value];
    else 
    {
      value = atoi(str);
      // list goes from 1 to n, the array from 0 to n-1
      value--;
  
      if ((value >= 0) && (value < sizeof(list)))
        chosen = list[value];
    }
  
    // none of the account characters
    if (chosen == "")
    {
      write("No conozco a ese personaje, vuelve a introducir el nombre o número: ");
      input_to("logon_player_name", 1, 1);
      return;
    }
  }
  // login with the character name
  else
  {
    chosen = str;
  }
  
  name = chosen;
  _player->set_name(name);
  
  // try to restore the character
  if (!_player->restore_me())
  {
    write("Error, no se ha podido recuperar el personaje.\n");
    destruct(_player);
    destruct(this_object());
    return;
  }
  
  // login with the character name
  if (flag == 0)
  {
    if (!account)
    {
      _player->set_account(_player->query_account_name());
      account = _player->query_account();
    }

    password = account->query_password();
    
    write("Introduce tu contraseña: ");
    input_to("logon_player_name_2", 1, 0);
    return;
  }
  else
    logon_player_name_2("", 1);
}

// si flag == 0, hacemos login directamente con el nombre del player
// si flag == 1, hacemos login con el nombre de cuenta
nomask void logon_player_name_2(string str, int flag)
{
  // estamos haciendo login con el nombre del player,
  // aun tenemos que comprobar la contraseña
  if (flag == 0)
  {
    write("\n");

    if (str == "") 
    {
      write("¡Vuelve dentro de poco!\n");
      destruct(_player);
      destruct(this_object());
      return ;
    }

    str = crypt(str, password);

    if (password && str != password) 
    {
      if (++no_times > MAX_RETRIES) 
      {
          write("Demasiados reintentos...\n"+
            "No estarás haciendo nada malo, ¿verdad?\n");
          destruct(_player);
          destruct(this_object());
          return ;
      }

      write("No se corresponde con tu contraseña.\n");
      write("Introduce tu contraseña de nuevo: ");
      input_to("logon_account",1);
      return ;
    }
  }

  other_copy = find_player(name);

  if (other_copy == this_player())
  {
    begin(0);
    return;
  }

  if (other_copy)
  {
    write("Ya estás jugando, ¿tiro la otra copia del personaje? (s/n): ");
    input_to("try_throw_out");
    return;
  }
  
  begin(0);

} /* logon_player_name() */

nomask void logon_account(string str)
{
  int tmp;
  string * list;

  password = account->query_password();
  write("\n");

  if (!strlen(str)) 
  {
    write("¡Vuelve dentro de poco!\n");
    destruct(_player);
    destruct(this_object());
    return;
  }

  str = crypt(str, password);

  if (password && (str != password)) 
  {
    if (++no_times > MAX_RETRIES) 
    {
      write("Demasiados reintentos...\n"+
            "No estarás haciendo nada malo, ¿verdad?");
      destruct(_player);
      destruct(this_object());
      return;
    }

    write("No se corresponde con tu contraseña.\n");
    write("Introduce tu contraseña de nuevo: ");
    input_to("logon_account",1);
    return;
  }

  // Llegados aqui el password es correcto

  list = account->query_player_list();
  
  write("Tienes disponibles en esta cuenta los siguientes personajes:\n");
  
  for (tmp = 0; tmp < sizeof(list); tmp++)
    write("   " + ((string)(tmp+1)) + ") " + capitalize(list[tmp]) + "\n");

  write("\nEscribe su nombre (o número) para empezar a jugar.\n"+
    "   ('nuevo' para crear un nuevo personaje)\n> \n");

  input_to("logon_player_name", 1, 1); // flag para indicar que venimos del login con cuenta

} /* logon_account() */

nomask void begin(int new_pl) 
{
  string player_ob;

  player_ob = SECURE->query_player_ob(name);

// Solo si estamos utilizando varios servidores y este no es el de desarrollo:
#ifdef MULTIMUD
#ifndef CCMUD_DEVEL   
  if (coder && !SECURE->query_admin(name))
  {
    write("Los programadores no tienen permitido el acceso a este puerto, prueba en el 5000.\n");
    destruct(_player);
    destruct(this_object());
    return;
  }
#endif
#endif

  // if (!find_object(player_ob)) 
  // {
  //   write("Por favor espera... cargando el personaje.\n");
  //   call_out("begin2", 0, new_pl);
  // } 
  // else

  write("Por favor espera... cargando el personaje.\n");
  begin2(new_pl);

} /* begin() */

nomask void begin2(int new_player) 
{
  object room;

  _player->set_name(name);
  _player->set_living_name(name); // both for players and npcs

  // store the user name in the user handler
  find_object(USER_HANDLER)->update_user(_player);

  validated = TRUE;
  write("¡Te has conectado! Bienvenido a "+mud_name()+"\n");

  if (query_property(GUEST_PROP)) 
    _player->add_property(GUEST_PROP, 1);

  // set user privileges
  _player->set_role(_player->query_role_name());

  /* Hamlet stuff */
  if(!coder)
    invis_wish = -1;
  else if( (invis_wish == 2) && !SECURE->query_admin(name) )
    invis_wish = 1;

  // room = load_object(CODER_COMMON_ROOM);
  // _player->move(room);
  _player->move_player_to_start(invis_wish, new_player);

  // Añadido por neverbot, esto deberia andar por aqui
  cat("/doc/news.txt");

  // do not show the first prompt, a command will be issued 
  // in move_player_to_start and after that the prompt will be
  // shown
  _player->set_no_prompt();

  destruct(this_object());

} /* begin2() */

void create_account(string str)
{
  if (!SECURE->valid_email(str)) 
  {
    write("Introduce una dirección de email válida: ");
    input_to("create_account", 1);
    return;
  }

  if (file_size("/save/accounts/"+str[0..0]+"/"+str+".o") > 0)
  {
    write("Lo sentimos, pero esa direción ya está utilizada.\n");
    write("Por favor, introduce otra dirección de email: ");
    input_to("create_account", 1);
    return ;
  }

  account_name = str;

  write("Has escogido como nombre de cuenta: '"+account_name+"', ¿estás seguro? (s/n): ");
  input_to("check_account_name");
}

int check_account_name(string str) 
{
  str = implode(explode(str," "),"");

  if (strlen(str))
  {
    if ((str[0] == 's') || (str[0] == 'S')) 
    {
      write("De acuerdo, introduce la contraseña que estará asociada a tu cuenta: ");  
      input_to("create_account2",1);
      return 1;
    } 
    else if ((str[0] == 'n') || (str[0] == 'N')) 
    {
      write("De acuerdo, inténtalo de nuevo.\n");
      write("Introduce una dirección de email válida: ");
      input_to("create_account",1);
      return 1;
    }
  }

  write("Debes responder sí o no (s/n): ");
  input_to("check_account_name", 1);
  return 1;
} /* check_account_name() */

void create_account2(string str)
{
  if (!strlen(str)) 
  {
    write("¡Vuelve dentro de poco!\n");
    destruct(_player);
    destruct(this_object());
    return ;
  }

  password = crypt(str, password);
  
  write("Repite la contraseña para confirmar que no hay ningún error: ");
  input_to("create_account3",1);  
}

void create_account3(string str)
{
  string tmppasswd;
  
  if (!strlen(str)) 
  {
    write("De acuerdo, contraseña borrada. Volvemos a empezar.\n"+
      "Introduce la contraseña que estará asociada a tu cuenta: ");
    input_to("create_account2",1);
    return;
  }

  tmppasswd = crypt(str, password);
  
  if (password != tmppasswd)
  {
    write("Las contraseñas son distintas. Volvemos a empezar.\n"+
      "Introduce la contraseña que estará asociada a tu cuenta: ");
    input_to("create_account2",1);
    return;   
  }
  
  create_account4();
}

void create_account4()
{
  // Creamos el objeto cuenta
  account = clone_object(ACCOUNT_OB);

  account->set_account_name(account_name);
  account->set_password(password);
  // account->add_player(name);
  account->update_last_connection();
    
  write("\n   %^BOLD%^Nueva cuenta creada con el nombre '"+account_name+"'.%^RESET%^\n\n");

  _player->set_account_ob(account);

  write("Pasamos ahora a crear el primer personaje de la cuenta.\n");
  write("Introduce el nombre de tu nuevo personaje: ");
  input_to("create_player",1);
  return; 
}

void create_player(string str)
{
  int tmp;
  
  if ((tmp = SECURE->valid_user_name(str)) != -1)
  {
    write("Caracter inválido '"+str[tmp..tmp]+"' en ("+str+").\n\n");
    write("Introduce el nombre de tu nuevo personaje: ");
    input_to("create_player",1);
    return;
  }

  if (strlen(str) > MAX_LEN) 
  {
    write("El nombre es demasiado largo, el máximo son 11 caracteres.\n");
    write("Introduce el nombre de tu nuevo personaje: ");
    input_to("create_player",1);
    return;
  }

  if (strlen(str) < MIN_LEN) 
  {
    write("El nombre es demasiado corto, el mínimo son 3 caracteres.\n");
    write("Introduce el nombre de tu nuevo personaje: ");
    input_to("create_player",1);
    return;
  }

  if ((file_size("/save/players/"+str[0..0]+"/"+str+".o") > 0) ||
      (file_size("/save/accounts/"+str[0..0]+"/"+str+".o") > 0))
  {
    write("Lo sentimos, pero ese nombre ya está utilizado.\n");
    write("Por favor, introduce otro nombre: ");
    input_to("create_player",1);
    return ;
  }

  name = str;

  write("Has escogido como nombre: '"+name+"', ¿estás seguro? (s/n): ");
  input_to("check_player_name");
}

int check_player_name(string str) 
{
  str = implode(explode(str," "),"");

  if ((str[0] == 's') || (str[0] == 'S')) 
  {
    write("¿Tu personaje es hombre o mujer? (h/m): ");
    input_to("get_sex");
    return 1;
  }
  else if ((str[0] == 'n') || (str[0] == 'N')) 
  {
    write("De acuerdo, inténtalo de nuevo.\n");
    write("Introduce el nombre de tu personaje: ");
    input_to("create_player");
    return 1;
  }

  write("Debes responder sí o no (s/n): ");
  input_to("check_player_name");
  return 1;

} /* check_player_name() */

void get_sex(string str) 
{
  int i;
  str = lower_case(str);

  if (!strlen(str))
  {
    write("Inténtalo con hombre o mujer (h/m): ");
    input_to("get_sex");
    return;
  }
  
  i = str[0];

  if ((i == 'h') || (i == 'H'))
  {
    _player->set_gender(1);
    gender = 1;
  }
  else if ((i== 'm')  || (i == 'M'))
  {
    _player->set_gender(2);
    gender = 2;
  }
  else 
  {
    write("Inténtalo con hombre o mujer (h/m): ");
    input_to("get_sex");
    return;
  }

  if (!query_property(GUEST_PROP))
  {
    _player->save_me();
    account->add_player(name);
  }

  begin(!query_property(GUEST_PROP));
}

mixed * stats() 
{
  return ({ 
    ({ "Login validated", validated, }),
          });
}


