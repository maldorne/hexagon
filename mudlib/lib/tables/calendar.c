// Calendar handler, in addition to the weather handler
//   neverbot 04/2009

#include <areas/weather.h>
#include <areas/calendar.h>

inherit "/lib/core/object.c";

private mixed * name_list;
private string * adjective_list;

void create()
{
  // names and gender of that name (only makes sense in
  //  languages with gender, obviously)
  name_list = ({ "rata", 2,
                 "dragón", 1,
                 "caballo", 1,
                 "cerdo", 1,
                 "grulla", 2,
                 "fénix", 1,
                 "tigre", 1,
                 "león", 1,
                 "zorro", 1,
                 "naga", 2,
                 "lagarto", 1,
                 "serpiente", 2,
                 "gato", 1,
                 "perro", 1,
                 "águila", 2,
                 "buitre", 1,
                 "unicornio", 1,
                 "gorrión", 1,
                 "grajo", 1,
                 "buey", 1,
                 "araña", 2,
                 "ratón", 1,
                 "vaca", 2,
                 "paloma", 2,
                 "cobra", 2,
                 "cuervo", 1,
                 "halcón", 1,
                 });

  // adjective list, masculine and feminine pairs
  adjective_list = ({ "cornudo", "cornuda",
        "llameante", "llameante",
        "encabritado", "encabritada",
        "orgulloso", "orgullosa",
        "inmortal", "inmortal",
        "naciente", "naciente",
        "imperial", "imperial",
        "callejero", "callejera",
        "durmiente", "durmiente",
        "sigiloso", "sigilosa",
        "mentiroso", "mentirosa",
        "ardiente", "ardiente",
        "infectado", "infectada",
        "real", "real",
        "oriental", "oriental",
        "occidental", "occidental",
        "rojo", "roja",
        "negro", "negra",
        "pestilente", "pestilente",
        "furioso", "furiosa",
        "aullador", "aulladora",
        "traidor", "traidora",
    });

  ::create();
}

mixed * query_name_list() { return name_list; }
string * query_adjective_list() { return adjective_list; }

string query_week_day_string(int num_day)
{
  switch(num_day % 7)
  {
    case 0: return "lunes";
    case 1: return "martes";
    case 2: return "miércoles";
    case 3: return "jueves";
    case 4: return "viernes";
    case 5: return "sábado";
    default:
    case 6: return "domingo";
  }
}
