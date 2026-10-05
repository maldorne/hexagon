// Blacksmith component. Marks a location as a smithy: it has a forge, an anvil
// and the tools to work metal, and it is where the repair skill can be used.
//
// The forge is lit while somebody works it: whoever holds a job at this place
// and is standing in it. Without a job declared here the forge is always lit,
// so a smithy that nobody staffs is still a smithy.

#include <room/location.h>
#include <areas/vacancy.h>
#include <language.h>

inherit component "/lib/location/component.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_BLACKSMITH);
}

void init() {}
void dest_me() {}

// Whether the forge is burning: true when no job is held here, or when one of
// the people holding a job here is standing at it.
int query_forge_lit()
{
  object loc, area, holder;
  mapping * jobs;
  int i;

  loc = query_my_location();
  if (!loc)
    return 0;

  area = (object)loc->query_area();
  if (!area)
    return 1;

  jobs = (mapping *)area->query_vacancies_at((string)loc->query_file_name());
  if (!sizeof(jobs))
    return 1;

  for (i = 0; i < sizeof(jobs); i++)
  {
    holder = (object)area->query_vacancy_holder(jobs[i]);
    if (holder && environment(holder) == loc)
      return 1;
  }

  return 0;
}

string query_info()
{
  return query_forge_lit() ? "forge lit" : "forge cold";
}

mapping query_hooks()
{
  return ([ "extra_look": HOOK_PRIORITY_ATMOSPHERE ]);
}

// The forge is part of the place: under the description, a line saying whether
// it is burning or cold.
string hook_extra_look(mixed * args)
{
  return query_forge_lit() ? _LANG_BLACKSMITH_FORGE_LIT
                           : _LANG_BLACKSMITH_FORGE_COLD;
}
