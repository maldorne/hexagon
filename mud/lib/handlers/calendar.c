// Handler para el calendario, como apoyo al handler de clima
//   neverbot 04/2009

// Contiene funciones para saber el dia de la semana de una fecha concreta,
// asi como para dar nombres al año en el que nos encontramos.

#include <areas/weather.h>
#include <areas/calendar.h>

inherit "/lib/core/object.c";

// Lista de años ya nombrados (lo unico que se guarda al salvar el handler)
mapping year_list;

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

/* 
 * We'll try to do a simul_efun to wrap the ctime and make it
 * always return information in spanish, with different modes
 * for real-world time or mud time, neverbot 4/2003
 *
 * Added date and time inside the mud, Folken 6/03
 *  (takes info from the weather handler, ignoring the time
 *   passed as parameter)
 *
 * Added english formatted date, for ftpd compatibility, 
 *   neverbot 12/04
 * 
 * Added format with only the time, neverbot 1/06
 */

string convert_day(int num)
{
  switch(num)
  {
    case 0: return "Domingo";
    case 1: return "Lunes";
    case 2: return "Martes";
    case 3: return "Miércoles";
    case 4: return "Jueves";
    case 5: return "Viernes";
    default:
    case 6: return "Sábado";
  }
}

string convert_english_day(int num)
{
  switch(num)
  {
   case 0: return "Sunday";
   case 1: return "Monday";
   case 2: return "Tuesday";
   case 3: return "Wednesday";
   case 4: return "Thursday";
   case 5: return "Friday";
   default:
   case 6: return "Saturday";
  }
}

string convert_month(int num)
{
 switch(num)
 {
  case 0: return "Enero";
  case 1: return "Febrero";
  case 2: return "Marzo";
  case 3: return "Abril";
  case 4: return "Mayo";
  case 5: return "Junio";
  case 6: return "Julio";
  case 7: return "Agosto";
  case 8: return "Septiembre";
  case 9: return "Octubre";
  case 10: return "Noviembre";
  default:
  case 11: return "Diciembre";
  }
}

string convert_english_month(int num)
{
 switch(num)
 {
  case 0: return "Jan";
  case 1: return "Feb";
  case 2: return "Mar";
  case 3: return "Apr";
  case 4: return "May";
  case 5: return "Jun";
  case 6: return "Jul";
  case 7: return "Aug";
  case 8: return "Sep";
  case 9: return "Oct";
  case 10: return "Nov";
  default:
  case 11: return "Dec";
  }
}

string convert_birthday(string str) 
{
  // we assume it is 4 characters long 'ddmm'
  int day, month, tot;
  string retval;

  sscanf(str, "%d", tot);
  day = tot / 100;
  month = tot % 100;
  /* 
  switch(day) {
  case 11:
  retval = "11th"; break;
  case 12:
  retval = "12th"; break;
  case 13:
  retval = "13th"; break;
  default:
  switch(day%10) {
  case 1:
  retval = day+"st"; break;
  case 2:
  retval = day+"nd"; break;
  case 3:
  retval = day+"rd"; break;
  default:
  retval = day+"th";
  }
  }
  */
  /*
  day += " de ";
  */
  retval = day + " de ";
  return retval + convert_month(month-1);
} 

int valid_birthday(string str) 
{
#define LENGTHS ({ 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 })

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
  
  return day <= LENGTHS[month];
} /* valid_birthday() */

string repair_number(int num)
{
  string res;

  res = "";

  if (num < 10)
   res = "0" + num;
  else
   res = "" + num;
  return res;
}

/*
 * Different modes:
 *  - no flag or flag = 0: reduced datetime for logging:
 *    "Lun 21 Abr 2003 15:58:0"
 *  - flag = 1: generic real-world time (i.e. for the who command):
 *    "Lunes 21 de Abril de 2003"
 *  - flag = 2: date with 6 characters: ddmmaa
 *  - flag = 3: mud datetime (from weather.c)
 *
 *  Added to make the 'ls -la' shorter, neverbot 23/11/03
 *  - flag = 4: dd/mm/yy hh:mm:ss
 *
 *  Added real-world datetime for the "time" command
 *    neverbot 09/12/04
 *  - flag = 5:  "Lunes 21 de Abril de 2003 - hh:mm:ss"
 *
 *  Added for the sake of ftpd compatibility, neverbot 10/12/04
 *  - flag = 6:  "Fri Dec 10 hh:mm:ss aaaa"
 */
string ctime(int time, varargs int flag)
{
  mixed * datos;
  string result;

  result = "";
  datos = localtime(time);

  if (!flag || (flag == 0))
  {
    result = convert_day(datos[LT_WDAY])[0..2];
    result += " " + repair_number(datos[LT_MDAY]) + " " +
              convert_month(datos[LT_MON])[0..2]; 
    result += " " + datos[LT_YEAR] + " ";
    result += repair_number(datos[LT_HOUR]) + ":" + 
              repair_number(datos[LT_MIN]) + ":" +
              repair_number(datos[LT_SEC]);
  }
  else if (flag == 1)
  {
    result = convert_day(datos[LT_WDAY]);
    result += " " + repair_number(datos[LT_MDAY]) + " de " +
              convert_month(datos[LT_MON]);
    result += " de " + datos[LT_YEAR];
  }
  else if (flag == 2)
  {
    result = "" + repair_number(datos[LT_MDAY]) +
             repair_number(datos[LT_MON]+1);
    result += ("" + (string)datos[LT_YEAR])[2..3];
  }
  else if (flag == 3)
  {
    result = handler(WEATHER_HANDLER)->date_string();
  }
  else if (flag == 4)
  {
    result = "" + repair_number(datos[LT_MDAY]) + "/" +
             repair_number(datos[LT_MON]+1) + "/";
    result += ("" + (string)datos[LT_YEAR])[2..3];
    result += " " + repair_number(datos[LT_HOUR]) + ":" + 
              repair_number(datos[LT_MIN]) + ":" +
              repair_number(datos[LT_SEC]);
  }
  else if (flag == 5)
  {
    result = convert_day(datos[LT_WDAY]);
    result += " " + repair_number(datos[LT_MDAY]) + " de " +
              convert_month(datos[LT_MON]);
    result += " de " + datos[LT_YEAR];
    result += " - " + repair_number(datos[LT_HOUR]) + ":" + 
              repair_number(datos[LT_MIN]) + ":" +
              repair_number(datos[LT_SEC]);
  }
  else if (flag == 6)
  {
    result = convert_english_day(datos[LT_WDAY])[0..2] + " " +
             convert_english_month(datos[LT_MON]);
    result += " " + repair_number(datos[LT_MDAY]) + " " +
                    repair_number(datos[LT_HOUR]) + ":" + 
                    repair_number(datos[LT_MIN]) + ":" +
                    repair_number(datos[LT_SEC]) + " " + 
                    repair_number(datos[LT_YEAR]);
  }
  else if (flag == 7)
  {
    result = repair_number(datos[LT_HOUR]) + ":" + 
             repair_number(datos[LT_MIN]) + ":" +
             repair_number(datos[LT_SEC]);
  }
  return result;
} 

