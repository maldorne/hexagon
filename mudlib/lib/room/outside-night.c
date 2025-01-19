
// refactor from /lib/outside.c, to be used both from rooms and location
// components, neverbot 01/2025

// * The night long is for rooms where you want the describe to change at
// * night.

static string night_long;

void set_night_long(string s) { night_long = s; }
string query_night_long() { return night_long; }

void create()
{
}

mixed * stats()
{
  return ({ 
    ({ "Night long (nosave)", night_long, }) 
         });
}
