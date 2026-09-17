// Calendar handler, in addition to the weather handler
//   neverbot 04/2009

#include <areas/weather.h>
#include <areas/calendar.h>
#include <language.h>

inherit "/lib/core/object.c";

private mixed * name_list;
private string * adjective_list;

void create()
{
  // names and gender of that name (only makes sense in
  //  languages with gender, obviously)
  name_list = _LANG_CALENDAR_YEAR_NAMES;

  // adjective list, masculine and feminine pairs
  adjective_list = _LANG_CALENDAR_YEAR_ADJECTIVES;

  ::create();
}

mixed * query_name_list() { return name_list; }
string * query_adjective_list() { return adjective_list; }

string query_week_day_string(int num_day)
{
  return _LANG_CALENDAR_WEEK_DAYS[num_day % 7];
}
