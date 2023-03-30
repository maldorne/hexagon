
// calendar handler, helping the weather handler
//   neverbot 04/2009

// functions to know the day of the week in a particular date,
// and to give name to the years

#include <mud/time.h>
#include <translations/time.h>
#include <areas/weather.h>
#include <areas/calendar.h>
#include <language.h>

inherit "/lib/core/object.c";

#define MONTH_LENGTHS ({ 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 })

// Lista de años ya nombrados (lo unico que se guarda al salvar el handler)
mapping year_list;

// implemented in /include/translations/common.*.h
string ordinal(int number);

void create()
{
  year_list = ([ ]);

  ::create();
  restore_object(SAVE_FILENAME, 1);
}

void setup()
{
  // Anticloning
  if (file_name(this_object()) != "/lib/handlers/calendar")
  {
    write("Este objeto no puede ser clonado.\n");
    dest_me();
    return;
  }

  reset_get();
  set_name("calendario");
  set_short("Calendario");
  set_long("¡Éste es el controlador del calendario!\n");
  add_alias("controlador");
  add_plural("controladores");

  // Move esta redefinida, siempre mueve el objeto a HOME
  move("bing");
}

int move(mixed dest, varargs mixed messin, mixed messout)
{
  ::move(HOME, messin, messout);
}

void save_handler()
{
  save_object(SAVE_FILENAME, 1);
}

void dest_me()
{
  save_object(SAVE_FILENAME, 1);
  ::dest_me();
}

mixed * get_year(int num)
{
  mixed * year;
  int rand;
  int gender;
  mixed * name_list;
  string * adjective_list;

  year = allocate(3);
  name_list = table(CALENDAR_TABLE)->query_name_list();
  adjective_list = table(CALENDAR_TABLE)->query_adjective_list();

  if (!undefinedp(year_list[num]))
    return year_list[num];

  // Nombre del año
  rand = random(sizeof(name_list)/2) * 2;
  year[POS_YEAR_NAME] = name_list[rand];

  // Genero del nombre
  gender = name_list[rand+1];
  year[POS_YEAR_GENDER] = gender;

  // Adjetivo dependiendo del genero
  rand = random(sizeof(adjective_list)/2) * 2;
  if (gender == 1)
    year[POS_YEAR_ADJECTIVE] = adjective_list[rand];
  else
    year[POS_YEAR_ADJECTIVE] = adjective_list[rand+1];

  year_list[num] = year;

  save_handler();

  return year;
}

string query_year_name(int num)
{
  mixed * year;
  int gender;

  gender = 0;

  if (!undefinedp(year_list[num]))
    year = year_list[num];
  else
    year = get_year(num);

  if ((year[POS_YEAR_GENDER] == 1) || (year[POS_YEAR_NAME][0..0] == "a") || (year[POS_YEAR_NAME][0..0] == "á"))
    gender = 1;
  else
    gender = 2;

  return "" + num + ", el año de" + ((gender == 1)?"l ":" la ") + capitalize(year[POS_YEAR_NAME]) + " " + year[POS_YEAR_ADJECTIVE];
}

int query_global_day()
{
  // ({ hora_del_dia, dia_del_anyo + 1, mes + 1, estacion + 1, anyo, });
  int * date_data;
  date_data = handler(WEATHER_HANDLER)->query_date_data();

  // return (date_data[4] - 1) * 365 + date_data[1];

  // No restamos uno al numero de años porque suponemos que existio un año cero!!!
  return date_data[4] * 365 + date_data[1];
}

int query_week_day(int num_day)
{
  if (!num_day)
    num_day = query_global_day();
  return (num_day % 7) + 1;
}

string query_week_day_string(varargs int num_day)
{
  if (!num_day)
    num_day = query_global_day();

  return table(CALENDAR_TABLE)->query_week_day_string(num_day);
}

string convert_birthday(string str)
{
  // we assume it is 4 characters long 'ddmm'
  int day, month, tot;
  string retval;

  sscanf(str, "%d", tot);
  day = tot / 100;
  month = tot % 100;

  return _LANG_CALENDAR_BIRTHDAY_STRING;
}

int valid_birthday(string str)
{
  int tot, month, day;

  if(strlen(str) != 4)
    return 0;

  if(!sscanf(str, "%d", tot))
    return 0;

  month = tot % 100;
  day = tot / 100;
  if(month > 12 || month < 1)
    return 0;

  if(day < 1)
    return 0;

  return day <= MONTH_LENGTHS[month];
} /* valid_birthday() */
