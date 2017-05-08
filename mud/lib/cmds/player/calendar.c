
#include <mud/cmd.h>
#include <areas/weather.h>
#include <areas/calendar.h>

inherit CMD_BASE;

static int cmd(string str, object me, string verb)
{
  // tell_object(me, ctime(time(), 5)+" en el mundo real.\n");
  // tell_object(me,ctime(time()-3600)+" Canarias.\n");
  // tell_object(me,ctime(time()-14400)+" America Central.\n");
  // tell_object(me, ctime(time(), 3)+".\n");
  string ret;
  string line;
  int cols;
  int i, count_week, count_month;
	int * date_data, global_day, week_day_start;

  ret = "";
  line = "";
  cols = me->query_cols();
	
	if (strlen(str))
	{
		if (str == "años" || str == "anyos")
		{
			// date_data = ({ hora_del_dia, dia_del_anyo + 1, mes + 1, estacion + 1, anyo, dia_del_mes, });
			int present_year;
      present_year = handler(WEATHER_HANDLER)->query_date_data()[4];
			
			ret += "Calendario de la presente era:\n\n";

			for (i = 0; i <= present_year; i++ )
				ret += "\t" + handler(CALENDAR_HANDLER)->query_year_name(i) + "\n";
			
			me->more_string(ret, "[Calendario]");
		}
		else
		{
			notify_fail("Sintaxis: calendario [años]\n");
			return 0;
		}
		
		return 1;
	}

	// date_data = ({ hora_del_dia, dia_del_anyo + 1, mes + 1, estacion + 1, anyo, dia_del_mes, });
  date_data = handler(WEATHER_HANDLER)->query_date_data();

  global_day = handler(CALENDAR_HANDLER)->query_global_day();
  // Dia de la semana en que comenzaba este mes
  week_day_start = handler(CALENDAR_HANDLER)->query_week_day(global_day - date_data[5]) - 1;

  ret +=  sprintf("%|*s\n", cols, ctime(time(), 3));
  ret += "\n" + sprintf("%|*s\n", cols, capitalize(handler(WEATHER_HANDLER)->month_string())) + "\n";
  
  ret += sprintf("%|*s\n", cols, "  L   M   X   J   V   S   D\n");
  
  count_week = 1;
  count_month = 1;

  for (i = 0; count_month <= handler(WEATHER_HANDLER)->month_days(); i++)
  {
    if (i < week_day_start)
    {
      count_week++;
      line += "    ";
      continue;
    }
    
    if (count_month == date_data[5]+1)
      line += sprintf("[%2d]", count_month);
    else
      line += sprintf(" %2d ", count_month);
    
    count_week++;
    count_month++;
    
    if (count_week%8 == 0)
    {
      ret += sprintf("%|*s\n", cols, line);
      line = "";
      count_week = 1;
    }
  }
  
  if (line != "")
  {
    for( i = count_week; i < 8 ; i++)
      line += "    ";

    ret += sprintf("%|*s\n", cols, line);
  }
  
  ret += "\n";
  
  tell_object(me, ret);

  return 1;
}
