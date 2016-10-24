// Nuevo sistema de subrazas, neverbot 6/03

#include <living/races.h>

inherit STD_RACE;

string base_race;

void set_base_race(string race){ base_race = race; }
string query_base_race(){ return base_race; }

int query_subrace(){ return 1; }

void start_player(object ob) 
{ 
  ::start_player(ob);
  ob->add_alias(lower_case(base_race->query_name()));
}
