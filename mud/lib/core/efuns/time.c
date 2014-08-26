
// uptime - return the number of seconds elapsed since the last driver reboot
// int uptime();
// This function returns the number of seconds since the last driver reboot.

// TODO uptime

int uptime()
{
  return 1000;
}


// localtime() converts a time value (as returned by time()) into an array
// of values which represents the time locally.  In the past time() was used
// to get the time in GMT (UTC), and then local definitions were used to
// determine the local offset from GMT.  This roundabout approach is no
// longer necessary.  localtime() returns the seconds, minutes and hours,
// the day, month and year, day of the week, day of the year,
// the name of the local timezone and how far the MUD is from GMT.  This
// information is retrieved directly from the operating system and made
// available to the driver without the use of MUD-specific configuration
// files.

// localtime() returns an array containing the values specified above.
// The index for each value is defined symbolically in localtime.h.  The
// following table summarizes the array returned by localtime().

//         int     LT_SEC          Seconds after the minute (0..59)
//         int     LT_MIN          Minutes after the hour (0..59)
//         int     LT_HOUR         Hour since midnight (0..23)
//         int     LT_MDAY         Day of the month (1..31)
//         int     LT_MON          Months since January (0..11)
//         int     LT_YEAR         Year (guarenteed to be >= 1900)
//         int     LT_WDAY         Days since Sunday (0..6)
//         int     LT_YDAY         Days since January 1 (0..365)
//         int     LT_GMTOFF       Seconds after GMT (UTC)
//         string  LT_ZONE         Timezone name

// TODO localtime
mixed * localtime(int timestamp)
{
  mixed * result;
  result = allocate(10);

  return result;
}



/* Intentaremos hacer una simul_efun para 'capar' el
 * funcionamiento del ctime() y que siempre nos devuelva
 * informacion en castellano, con posibles modos de horario
 * (mundo real y/o hora del mud), Folken 4/2003
 * Incluida hora/fecha del mud, Folken 6/03
 *  (tira del /obj/handlers/weather.c, da igual el time que pasemos
 *   como parametro).
 * Incluida hora en formato ingles, para compatibilidad con ftpd,
 *   Folken 12/04
 * Incluido formato que devuelve solo la hora del dia, Folken 1/06
 */

#include <weather.h>

#define LT_WDAY 6
#define LT_MDAY 3
#define LT_MON 4
#define LT_YEAR 5
#define LT_HOUR 2
#define LT_MIN 1
#define LT_SEC 0

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

string convert_english_day(int num){
  switch(num){
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

string convert_month(int num){
 switch(num){
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

string convert_english_month(int num){
 switch(num){
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
	/* we assume it is 4 characters long 'ddmm'*/
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
} /* convert_birthday() */

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
 * Diferentes modos:
 *  - sin flag o flag = 0: hora reducida para logs:
 *    "Lun 21 Abr 2003 15:58:0"
 *  - flag = 1: hora generica del mundo real (para el who, p.ej.):
 *    "Lunes 21 de Abril de 2003"
 *  - flag = 2: Fecha con 6 caracteres: ddmmaa
 *  - flag = 3: Hora y Fecha del mud (segun weather.c)
 *  Añadido para acortar el 'ls -la', Folken 23/11/03
 *  - flag = 4: ddmmaa hh:mm:ss
 *  Añadido para el comando time, Folken 09/12/04 (mundo real)
 *  - flag = 5:  "Lunes 21 de Abril de 2003 - hh:mm:ss"
 *  Añadido para compatibilidad con ftp, Folken 10/12/04
 *  - flag = 6:  "Fri Dec 10 hh:mm:ss aaaa"
 */
string ctime(int time, varargs int flag)
{
  mixed * datos;
  string result;

  result = "";
  datos = localtime(time);

  if (!flag || (flag == 0)){
    result = convert_day(datos[LT_WDAY])[0..2];
    result += " " + repair_number(datos[LT_MDAY]) + " " +
              convert_month(datos[LT_MON])[0..2]; 
    result += " " + datos[LT_YEAR] + " ";
    result += repair_number(datos[LT_HOUR]) + ":" + 
              repair_number(datos[LT_MIN]) + ":" +
              repair_number(datos[LT_SEC]);
  }
  else if (flag == 1){
    result = convert_day(datos[LT_WDAY]);
    result += " " + repair_number(datos[LT_MDAY]) + " de " +
              convert_month(datos[LT_MON]);
    result += " de " + datos[LT_YEAR];
  }
  else if (flag == 2){
    result = "" + repair_number(datos[LT_MDAY]) +
             repair_number(datos[LT_MON]+1);
    result += ("" + (string)datos[LT_YEAR])[2..3];
  }
  else if (flag == 3){
    result = WEATHER->date_string();
  }
  else if (flag == 4){
    result = "" + repair_number(datos[LT_MDAY]) + "/" +
             repair_number(datos[LT_MON]+1) + "/";
    result += ("" + (string)datos[LT_YEAR])[2..3];
    result += " " + repair_number(datos[LT_HOUR]) + ":" + 
              repair_number(datos[LT_MIN]) + ":" +
              repair_number(datos[LT_SEC]);
  }
  else if (flag == 5){
    result = convert_day(datos[LT_WDAY]);
    result += " " + repair_number(datos[LT_MDAY]) + " de " +
              convert_month(datos[LT_MON]);
    result += " de " + datos[LT_YEAR];
    result += " - " + repair_number(datos[LT_HOUR]) + ":" + 
              repair_number(datos[LT_MIN]) + ":" +
              repair_number(datos[LT_SEC]);
  }
  else if (flag == 6){
    result = convert_english_day(datos[LT_WDAY])[0..2] + " " +
             convert_english_month(datos[LT_MON]);
    result += " " + repair_number(datos[LT_MDAY]) + " " +
                    repair_number(datos[LT_HOUR]) + ":" + 
                    repair_number(datos[LT_MIN]) + ":" +
                    repair_number(datos[LT_SEC]) + " " + 
                    repair_number(datos[LT_YEAR]);
  }
  else if (flag == 7){
    result = repair_number(datos[LT_HOUR]) + ":" + 
             repair_number(datos[LT_MIN]) + ":" +
             repair_number(datos[LT_SEC]);
  }
  return result;
} /* ctime() */

