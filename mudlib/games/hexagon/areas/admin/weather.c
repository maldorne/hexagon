
// Traducida para CcMud, neverbot 29/6/03
// y arreglada, nunca funcionó ;)
// Algún dia habria que arreglarlo para poder establecer el clima
// con las palancas... paciencia :(

#include <areas/weather.h>
#include <room/room.h>

#include "path.h"

inherit "/lib/room.c";

mixed leverarr;

void reset()
{
  // seteuid("baldrick"); // neverbot
  /*
  leverarr = ({ ({ "blue",
       "The rooms spins and a small bit of printed paper falls from the\n"+
       "Weather controller saying:\n#query_rain" }),
                ({ "white",
       "The ground heaves in a major convulsion and a mound of paper falls onto\n"+
       "You from the ceiling saying:\n#query_cloud" }) });
  */

  leverarr = ({ ({ "lluvia",
       "La habitación gira y un pequeño pedazo de papel sale del "+
       "controlador del tiempo con el siguiente mensaje escrito: "+
       "\n#rain_string" }),
                ({ "nubes",
       "La habitación gira y un pequeño pedazo de papel sale del "+
       "controlador del tiempo con el siguiente mensaje escrito: "+
       "\n#cloud_string" }),
                ({ "estacion",
       "La habitación gira y un pequeño pedazo de papel sale del "+
       "controlador del tiempo con el siguiente mensaje escrito: "+
       "\n#season_string" }),
                ({ "dia",
       "La habitación gira y un pequeño pedazo de papel sale del "+
       "controlador del tiempo con el siguiente mensaje escrito: "+
       "\n#daynight_string" }),
                ({ "temperatura",
       "La habitación gira y un pequeño pedazo de papel sale del "+
       "controlador del tiempo con el siguiente mensaje escrito: "+
       "\n#temperature_string" }),
                ({ "luna",
       "La habitación gira y un pequeño pedazo de papel sale del "+
       "controlador del tiempo con el siguiente mensaje escrito: "+
       "\n#query_moon_string" }),
                ({ "fecha",
       "La habitación gira y un pequeño pedazo de papel sale del "+
       "controlador del tiempo con el siguiente mensaje escrito: "+
       "\n#date_string" }),
              });
}

void setup()
{
  set_short("Habitación de control del clima");
  set_long("Este es el centro de control del clima para todo " + mud_name() +
           ". En el suelo debería estar el controlador en sí mismo, y "+
           "tirados alrededor están un montón de luces parpadeantes, controles y palancas. "+
           "Tienes la extraña sensación de que realmente no sirven para nada, y "+
           "alguien debe estar trabajando aún en ellas para que algún día "+
           "tengan utilidad.\n");

  set_light(60);
  add_item( ({ "luces", "luz" }), "Montones de luces parpadeantes.\n");
  add_item( ({ "controles", "control" }), "Algunos controles sin aparente utilidad.\n");
  add_item( ({ "palancas", "palanca" }), "Palancas multicolor instaladas en el suelo. "+
             "Puedes ver una palancas etiquetadas como lluvia, nubes, estacion, dia, fecha, luna y temperatura.\n");

  add_exit(DIR_SOUTHWEST, ADMIN + "admin3.c", "standard");

  add_property(NO_CLEAN_UP_PROP, 1);

  /*
  add_item("dials","Large dials with all sort of things on them. One you can see says\n"+
           "'Colour of spring', it is currently pointing at purple.\n");
  add_item("levers",
           "The levers are large multicolour protuberances which are scattered around the\n"+
           "room in random profusion.\n");
  */
}

void init()
{
  ::init();
  add_action("do_pull","tirar");
  add_action("do_push","empujar");
}


int do_pull(string str)
{
  int i;
  string type,rand;
  string lever;
  // object weather;

  if (str == "palancas")
    lever = leverarr[random(sizeof(leverarr))];
  else {
    // if (!sscanf(str,"%s lever%s",type,rand)) {
    if (!sscanf(str, "palanca %s", type)) {
      notify_fail("Tienes que tirar de una de las palancas.\n");
      return 0;
    }
    for (i = 0; i < sizeof(leverarr); i++) {
      if (leverarr[i][0] == type) {
        lever = leverarr[i];
        break;
      }
    }
  }

  if (!lever) {
    notify_fail("Tienes que escoger una de las palancas existentes.\n");
    return 0;
  }

  // weather = present("tiempo", this_object());
  // weather = find_object(WEATHER);

  if (!handler(WEATHER_HANDLER)) {
    notify_fail("¡¡Ups, el controlador del tiempo no está en esta habitación!!\n");
    return 0;
  }

  sscanf(lever[1], "%s#%s", type, rand);

  type = sprintf("%*-=s", this_user()->query_cols(), type);

  tell_object(this_player(), type);
  // this_player()->print_object(call_other(handler(WEATHER_HANDLER), rand));
  type = (string)call_other(handler(WEATHER_HANDLER), rand);
  if (!type || (type != ""))
    tell_object(this_player(), capitalize(type) + ".\n");
  else
    tell_object(this_player(), "No hay datos relevantes.\n");
  return 1;
}

int do_push(string str)
{
  string lever;
  int i;
  string type;
  // string bing;

  if (str == "palancas")
    lever = leverarr[random(sizeof(leverarr))];
  else {
    // if (!sscanf(str,"%s lever%s",type,bing)) {
    if (!sscanf(str,"palanca %s", type)) {
      notify_fail("Tienes que empujar una de las palancas.\n");
      return 0;
    }
    for (i = 0; i < sizeof(leverarr); i++)
      if (type == leverarr[i][0]) {
        lever = leverarr[i];
        break;
      }
  }
  if (!lever) {
    notify_fail("Tienes que escoger una de las palancas existentes.\n");
    return 0;
  }

  tell_object(this_player(), "Empujas una y otra vez pero no puedes activar la palanca.\n"+
              "Algún día alguien la activará.\n");
  return 1;
}
