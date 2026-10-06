// Open condition: between two game hours. Arguments: "from" and "to", hours of
// the day (0-23); it opens at "from" and closes at "to", across midnight when
// "to" is earlier than "from" (a tavern open from 18 to 2).

#include <language.h>

mixed check_open(object venture, object location, object who, mapping args)
{
  int hour, from, to;

  if (!args || undefinedp(args["from"]) || undefinedp(args["to"]))
    return 1;

  from = args["from"];
  to = args["to"];
  hour = ((int *)handler("weather", location)->query_date_data())[0];

  if (from <= to ? (hour >= from && hour < to) : (hour >= from || hour < to))
    return 1;

  return _LANG_CONDITION_HOURS_CLOSED;
}

string query_description(mapping args)
{
  if (!args || undefinedp(args["from"]) || undefinedp(args["to"]))
    return "hours (none set: always open)";
  return "open from " + args["from"] + " to " + args["to"];
}
