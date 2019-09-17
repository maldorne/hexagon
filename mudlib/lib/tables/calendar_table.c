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
                 });

  // adjective list (length always odd, masculine and feminine)
  adjective_list = ({ "cornudo", "cornuda",
        "llameante", "llameante",
        "encabritado", "encabritada",
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
