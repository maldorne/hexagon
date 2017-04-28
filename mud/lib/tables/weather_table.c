// New weather system for CcMud, neverbot 22/10/03

// REMEMBER: If you have a weather_table file in 
// /game/tables/weather_table.c
// this file will be ignored by the system

#define BASE "ciudad capital"
#define CIUDAD_CAPITAL "ciudad capital"
#define ZONE_2 "whatever zone you like"
#define ZONE_3 "whatever zone you like too"
// more zones...

/* 
 * Zones included in this file:
 *
 * ([ zone : ({ raining, wind, temperature, 
 *              raining average, wind average, temperature average, 
 *              exits }) ])
 * exits is an array with the name of adjacent zones
 *
 * 0 is the best weather, 100 is the worst (more rain, wind, cold)
 *
 * The values defined here for rain, wind and tem must be the same
 * as the average values (which will be the initial ones and will be 
 * modified by the weather handler)
 *
 * IMPORTANT: adding a new zone the weather will auto update. If what
 *            we want to do is to modify an ALREADY EXISTING zone (remove, 
 *            change exits, change average values), we MUST update the weather
 *            system as a whole, call to the reset_zones() function in the 
 *            handler object:
 *            exec "/lib/handlers/weather.c"->reset_zones()
 *
 */          

private mapping zones;

void create()
{
  zones = ([ 
        BASE : ({ 30, 30, 30, 30, 30, 30, 
          ({ ZONE_2, }), 
          }),
        ZONE_2 : ({ 40, 40, 40, 40, 40, 40, 
          ({ BASE, ZONE_3, }), 
          }),
        ZONE_3 : ({ 40, 40, 40, 40, 40, 40, 
          ({ ZONE_2, }),
          }),
        ]);
}

string query_zone(string path)
{
  string where;
  string * words;
  string zone;

  where = path;
  words = explode(where, "/");
  zone = "";

  // only directories
  where = "/" + implode(words[0..sizeof(words)-2], "/");

  while ((zone == "") && (sizeof(words = explode(where, "/")) > 1))
  {
    switch(where)
    {
      case "/game/areas/ciudad-capital":
      case "/game/areas/guilds/some-guild-inside-the-city":
        zone = CIUDAD_CAPITAL;
        break;
        
      case "/game/areas/ciudad-capital/some-place-outside":
      case "/game/areas/zone_2":
        zone = ZONE_2;
        break;

      case "/game/areas/zone_3":
        zone = ZONE_3;
        break;      
    } // end switch

    // up one level
    // ie: from /game/areas/ciudad-capital/whatever 
    //     to /game/areas/ciudad-capital
    where = "/" + implode(words[0..sizeof(words)-2], "/");

  } // end while

  if (zone != "")
    return zone;
  else
    return BASE;
}

// To ask for the complete data from the weather handler
mixed query_zones()
{ 
  call_out("dest_me", 1);
  return zones; 
}

