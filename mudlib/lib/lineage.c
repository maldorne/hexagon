// New subrace system, neverbot 6/03
// Subraces renamed lineages: a variety of a base race, a people or a kind of
// the same body.

#include <living/races.h>

inherit STD_RACE;

string base_race;

void set_base_race(string race){ base_race = race; }
string query_base_race(){ return base_race; }

int query_lineage(){ return 1; }

// The base race with the lineage beside it, "Human (Name)". A lineage whose
// name already says its race overrides it.
string query_sheet_name()
{
  object base;

  base = base_race ? load_object(base_race) : nil;
  if (!base)
    return ::query_sheet_name();
  return base->query_short() + " (" + capitalize(query_name()) + ")";
}

void start_player(object ob) 
{ 
  ::start_player(ob);
  ob->add_alias(lower_case(base_race->query_name()));
}
