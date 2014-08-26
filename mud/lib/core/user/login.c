
// Antiguo objeto new_login.c, ahora el unico modo de hacer login
// Añadidas tambien algunas cosas del antiguo login.c
//   neverbot 21/01/03
// Cambiado el sistema de login al incluir cuentas de usuario,
//   neverbot 12/2010
// Ported to dgd, neverbot 03/2014

#include <config.h>
#include <login.h>
#include <user.h>

static int timestamp;
static int validated;
// static object account;
// static string password;
// static string account_name;
// static int cols;
// static int gender;
// static int creator;
// static int app_creator;
// static int no_times;
// static int invis_wish;
// static string player_ob;


void create() 
{
  ::create();

  timestamp = 0;
  validated = FALSE;

  // cols = 79;
  // account = nil;
  // initial_player = "";
  // set_name("logon");
  // gender = -1;
  // invis_wish = -1;
  // seteuid("Root");
  // player_ob = "/global/player";
} 

// Function prototypes
nomask void show_options();
nomask void disconnect(varargs int silence);
nomask void time_out();
nomask void logon();
nomask void logon_options();

nomask void disconnect(varargs int silence)
{
  if (!silence)
    write("¡Vuelve dentro de poco!\n");  
}

nomask void time_out() 
{
  if (validated == TRUE)
    return;

  write("Se terminó el tiempo.\n");
  destruct(this_object());
}

nomask void quit()
{
  // TODO muchas cosas aqui
  destruct(this_object());
}

nomask void logon() 
{
  timestamp = time();

  call_out("time_out", LOGIN_TIMEOUT);

  // TODO check shut object to avoid login near a shutdown

  cat(WELCOME_MESSAGE);

  // Añadido del antiguo login.c, neverbot 21/01/03
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

  logon_options();

  write("Introduce la opción elegida: ");
  input_to("logon_option");
}

nomask void logon_options()
{
  // new options before login, neverbot 2/06
  cat(OPTIONS_MESSAGE);
}


nomask void logon_option(string str) 
{
  write("Hemos hecho login!\n");
  validated = TRUE;
}

