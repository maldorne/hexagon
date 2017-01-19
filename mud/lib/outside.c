// #define TESTING 1
// Revisado para CcMud, neverbot 6/03
// Eliminado el string sobre la luna, ahora hay tres y ocupa mucho.

#include <areas/weather.h>

inherit room        "/lib/room";
inherit extra_look  "/lib/core/basic/extra_look";

/* ok this is the out side room standard, It includes weather and
 * all that jazz
 */

// * The night long is for rooms where you want the describe to change at
// * night.
static string night_long;
// * This is the string it will display instead of it is dark.
static string dark_long;
// static string light_change;
// static int current_day;

void set_dark_long(string s) { dark_long = s; }
string query_dark_long() { return dark_long; }
void set_night_long(string s) { night_long = s; }
string query_night_long() { return night_long; }

// * It will call this function on the room when it changes from dark to
// * light.  ie...  changes from day to night.
/*
void set_light_change(string s) { light_change = s; }
string query_light_change() { return light_change; }
*/

void create() 
{
  // dark_long = "Está oscuro.\n";
  dark_long = "";
  room::create();
  extra_look::create();
  add_property("location", "outside");
} /* create() */

string short(int dark) 
{
  if (dark)
    return query_dark_mess(dark);
  else
    return ::short();
} /* short() */

string long(string str, int dark) 
{
  string s, ret; 
  // string aux;

  ret = "";
  // aux = "";
  /*
  if ((int)WEATHER->query_day() != current_day){
    if (light_change)
        call_other(this_object(), light_change, current_day);
  }
  */
  
  if (this_player()) 
    dark = (int)this_player()->check_dark(query_light());
  
  if (!str)
  {
    // aux = "Es de día.\n";
    // Coordenadas eliminadas, neverbot
    // if ( pointerp(co_ord) && !WEATHER->query_day())
    
    /* night... */
    if (!WEATHER->query_day())
    {
      /*
      aux = "Es de noche.\n";
      s = (string) WEATHER->moon_string();
      if (!s){
         aux =  "Es de noche y la luna aún no ha salido.\n";
      }
      else{
        if ((int)WEATHER->cloud_index() > 20)
            aux = "Es de noche y la "+s+" está oculta tras las nubes.\n";

        else if (!dark || dark > 3) // if its that dark it ain't lit up
            aux = "La luz de la "+s+" inunda el ambiente.\n";
      }
      */

      switch(dark) /* O.K. how much can we see at night */
      {
        default: /* can see anyway */
           if ( night_long )
             ret += night_long;
           else
             ret += sprintf("\n   %-=*s\n", (this_player()?this_player()->query_cols():79), "   "+query_long());
           break;
        case 1..3:  /* too dark */
           	if (dark_long && (dark_long != "")) 
              return ret + dark_long + ::query_dark_mess(dark) + ((query_dirs_string()!="")?".\n":"");
	        return ret + ::query_dark_mess(dark)+ ((query_dirs_string()!="")?".\n":"");
        case 4..6:  /* too bright */
           return ret + ::query_dark_mess(dark)+ ".\nA pesar de esta luz parece ser de noche.\n";
      }
    } 
    /* day ... */
    else
    {
      switch(dark) /* O.K. how much can we see daytime */
      {
        default: /* can see anyway */
            ret += sprintf("\n   %-=*s\n", (this_player()?this_player()->query_cols():79), "   "+query_long());
        break;
        case 1..3:  /* too dark */
            return ::query_dark_mess(dark)+"\nA pesar de esta oscuridad parece ser de día.\n";
        case 4..6:  /* too bright */
            return ::query_dark_mess(dark)+".\n";
      }
    } /* if we got here we can see */

    s = calc_extra_look();

    if (s && s != "")
      ret += s;
    
    ret += (string)WEATHER->weather_string(this_object());
    
    if (!exit_string) 
      query_dirs_string();

    if (exit_string)
	    ret += exit_string+"\n";
    
    /* Return the long + the contents of the room. */
    return ret + query_contents("");
  }
  str = expand_alias(str);
  return items[str];
} /* long() */

/* percentage system */
int query_light() 
{
  int i;
#ifdef TESTING
  i = 100;
#else
  i = (int)WEATHER->query_darkness(this_object());
#endif

  return ::query_light()*i/100;
} /* query_light() */

int query_outside()
{
  return 1;
}

/*
mixed *query_init_data(){
   return extra_look::query_init_data()+room::query_init_data();
}
*/

mixed * stats()
{
	return room::stats() + extra_look::stats();
}
