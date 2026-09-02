// Base citizenship file...
// neverbot 4/2003
// I do not think channels for the citizenship group are needed, nor
// commands of your own, but I leave the code commented just in case
//      neverbot 4/2003

inherit "/lib/core/object.c";

#include <living/family.h>

// mixed *job_commands = ({ });
string init_room;

// The name style for this citizenship's people: the basename of a wordlist
// collection (e.g. "humans.fantasy"), used to give generated citizens names in
// the right style. "" means no generated names -- sentient NPCs then keep
// their template's name.
string name_style;

// The peoples this citizenship is made of: race object paths, one entry per
// share, so listing a race twice makes it twice as likely. A generated citizen
// rolls its race from here rather than inheriting the one its type was
// authored with, which is what lets a single trade -- one farmer, one guard --
// staff a human town and an elf one. Empty leaves the type's own race alone.
string * races;

// The wordlist a family surname is drawn from, kept apart from name_style
// because a culture's houses need not be named like its people. "" means this
// citizenship founds no families.
string surname_style;

// Which parent a marriage joins and whose surname the children take:
// DESCENT_PATRILINEAL, DESCENT_MATRILINEAL, or DESCENT_NONE for a people where
// nobody moves house on marrying.
string descent;

void create()
{
  init_room = "";
  name_style = "";
  surname_style = "";
  descent = DESCENT_PATRILINEAL;
  races = ({ });
  ::create();
}

string query_name_style() { return name_style; }
void set_name_style(string str) { name_style = str ? str : ""; }

string query_surname_style() { return surname_style; }
void set_surname_style(string str) { surname_style = str ? str : ""; }

string query_descent()
{
  return (descent && strlen(descent)) ? descent : DESCENT_PATRILINEAL;
}

void set_descent(string str)
{
  if (str == DESCENT_PATRILINEAL || str == DESCENT_MATRILINEAL ||
      str == DESCENT_NONE)
    descent = str;
}

string * query_races() { return races ? races : ({ }); }
void set_races(string * list) { races = list ? list : ({ }); }

// One of this citizenship's peoples, drawn by share. "" when it declares none.
string query_random_race()
{
  return sizeof(query_races()) ? races[random(sizeof(races))] : "";
}

int query_legal_race(string race) { return(1); }
int query_legal_player(object player) { return 1; }

// A citizenship is a lean social object (like /lib/race.c): it only identifies
// the citizenship (its name). How citizenships relate -- parent, allies,
// enemies, security -- lives in the diplomacy handler, keyed by name.

void set_init_room(string str) { init_room = str; }
string query_init_room() { return init_room; }

// int query_channel() { return 0; }
/*
int add_job_command(string name, int add_if_player) 
{
   if (member_array(name, job_commands) != -1)
      return 0;
   job_commands += ({ name, ({ add_if_player }) });
   return 1;
}
*/
// Mask this and put your shadows in here, scope to this though!
void start_player(object pl) 
{
/*
   int i;
   if (!pointerp(job_commands))
      job_commands = ({ });
   for (i=0;i<sizeof(job_commands);i+=2)
      if (job_commands[i+1][0] || !interactive(pl))
         pl->add_known_command(job_commands[i]);
*/         
}

void on_death(object player, varargs object killer)
{
}

void on_kill(object player, object victim)
{
}

int player_quit(object player)
{
    return 1;
}

