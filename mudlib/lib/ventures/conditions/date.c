// Open condition: on some days of the year only. Arguments: "day", the day of
// the year it opens (1-365), and optionally "days", how many days in a row it
// stays open (1 when not given). A fair open one day a year is day=200.

#include <language.h>

mixed check_open(object venture, object location, object who, mapping args)
{
  int today, first, length;

  if (!args || undefinedp(args["day"]))
    return 1;

  first = args["day"];
  length = undefinedp(args["days"]) ? 1 : args["days"];
  today = ((int *)handler("weather", location)->query_date_data())[1];

  // a run of days may wrap past the end of the year
  if ((today - first + 365) % 365 < length)
    return 1;

  return _LANG_CONDITION_DATE_CLOSED;
}

string query_description(mapping args)
{
  if (!args || undefinedp(args["day"]))
    return "date (none set: always open)";
  return "open on day " + args["day"] + " of the year" +
         (!undefinedp(args["days"]) && args["days"] > 1
            ? " and the " + (args["days"] - 1) + " after it" : "");
}
