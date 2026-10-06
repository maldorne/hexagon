// Open condition: somebody is at work here. A venture with a job held at its
// location (a shopkeeper, a barman) is open only while one of the people whose
// work is this location stands in it and is not fighting anybody here. A venture
// with no job held at its location is always open.

#include <language.h>

// Whether somebody who works at this location is in it, and free to serve.
// Returns 1, or the reason it is not.
mixed somebody_at_work(object loc)
{
  object * here, * attackers;
  string file;
  mixed work;
  int i, j, busy, found;

  file = (string)loc->query_file_name();
  here = all_inventory(loc);

  for (i = 0; i < sizeof(here); i++)
  {
    if (!here[i]->query_living())
      continue;
    // anything else answers no work, or not a place
    work = here[i]->query_work();
    if (!stringp(work) || work != file)
      continue;

    found = 1;
    attackers = (object *)here[i]->query_attacker_list();
    busy = 0;
    for (j = 0; j < sizeof(attackers); j++)
      if (attackers[j] && environment(attackers[j]) == loc)
        busy = 1;

    if (!busy)
      return 1;
  }

  return found ? _LANG_CONDITION_ATTENDED_BUSY : _LANG_CONDITION_ATTENDED_NOBODY;
}

mixed check_open(object venture, object location, object who, mapping args)
{
  object area;

  area = function_exists("query_area", location)
           ? (object)location->query_area() : nil;

  // nobody is meant to work here, so nobody has to be here
  if (!area || !sizeof((mapping *)area->query_vacancies_at(
                         (string)location->query_file_name())))
    return 1;

  return somebody_at_work(location);
}

string query_description(mapping args)
{
  return "open while somebody who works here is in it";
}
