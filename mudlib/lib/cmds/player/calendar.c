
#include <mud/cmd.h>
#include <areas/weather.h>
#include <areas/calendar.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_CALENDAR_ALIAS);
  set_usage(_LANG_CMD_CALENDAR_SYNTAX);
  set_help(_LANG_CMD_CALENDAR_HELP);
}

static int cmd(string str, object me, string verb)
{
  string ret;
  string line;
  int cols;
  int i, count_week, count_month;
  int * date_data, global_day, week_day_start;

  ret = "";
  line = "";
  cols = (int) this_user()->query_cols();

  if (strlen(str))
  {
    if (member_array(str, _LANG_CMD_CALENDAR_ARG_YEARS) != -1)
    {
      // date_data = ({ hour_of_day, day_of_year + 1, month + 1, season + 1, year, day_of_month, });
      int present_year;
      present_year = handler(WEATHER_HANDLER)->query_date_data()[4];

      ret += _LANG_CMD_CALENDAR_ERA_HEADER;

      for (i = 0; i <= present_year; i++)
        ret += "\t" + handler(CALENDAR_HANDLER)->query_year_name(i) + "\n";

      me->more_string(ret, _LANG_CMD_CALENDAR_TITLE);
      return 1;
    }
    else
    {
      notify_fail(_LANG_CMD_CALENDAR_USAGE);
      return 0;
    }
  }

  // date_data = ({ hour_of_day, day_of_year + 1, month + 1, season + 1, year, day_of_month, });
  date_data = handler(WEATHER_HANDLER)->query_date_data();

  global_day = handler(CALENDAR_HANDLER)->query_global_day();
  // Weekday on which the current month started.
  week_day_start = handler(CALENDAR_HANDLER)->query_week_day(global_day - date_data[5]) - 1;

  ret +=  sprintf("%|*s\n", cols, ctime(time(), 3));
  ret += "\n" + sprintf("%|*s\n", cols, capitalize(handler(WEATHER_HANDLER)->month_string())) + "\n";

  ret += sprintf("%|*s\n", cols, _LANG_CMD_CALENDAR_WEEK_HEADER);

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
    for (i = count_week; i < 8; i++)
      line += "    ";

    ret += sprintf("%|*s\n", cols, line);
  }

  ret += "\n";

  tell_object(me, ret);

  return 1;
}
