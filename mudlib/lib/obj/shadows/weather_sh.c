/* ******************************************************
 *
 *     Shadow para el clima (cuando hace mucho frio)
 *          neverbot 04/2009
 *
 * ****************************************************** */

#include <areas/weather.h>

object player;

int query_weather_shadow(){ return 1; }
string extra_look() { return "Está helado de frío.\n"; }

void destruct_weather_shadow()
{
  // tell_object(player,"[WEATHER_SH] shadow destruida.\n");
  player->remove_extra_look(this_object());
  destruct(this_object());
}

void setup_shadow(object ob) 
{
  shadow(ob, 1);
  player = ob;
  player->add_extra_look(this_object());
}

/*
int move( mixed dest, mixed messout, mixed messin ){
  tell_object(find_living("folken"), "move!!!\n");
  return my_player->move(dest, messout, messin);
}
*/

int move_living(string dir, mixed dest, varargs mixed message, mixed enter) 
{
  int ret;

  if (random(4) == 1)
  {
    notify_fail("El frío prácticamente te impide caminar.\n");
    return 0;
  }
  
  // Movemos al player  
  ret = player->move_living(dir, dest, message, enter);

  // Si ya no hace frio la destruimos
  if (handler(WEATHER_HANDLER)->query_actual_data(environment(player))[2] <= 80)
  {
    tell_object(player, "Notas cómo tus músculos vuelven a entrar en calor al moverte.\n");
    destruct_weather_shadow();
  }

  return ret;
}

string wibble_speech(string str)
{
  string * list;
  int i;
 
  if (!str || str == "") 
    return str;
  
  list = explode(str," ");
  
  // La primera palabra en un tell sera la persona a quien 
  // vamos a hacer el, no la modificamos
  for (i = 1; i < sizeof(list); i++)
  {
    list[i] = replace(list[i], ({
        "r", "rer",
        "s", "ss",
        "la", "lala",
        "es", "eses",
        }));
    
    // 1 de cada 4 palabras tartamudeamos
    if ((random(4) == 1) && (list[i][0..0] != "%"))
      list[i] = list[i][0..2] + "-" + list[i][0..2] + "-" + list[i];
  }
  
  str = implode(list," ");

  return str;
}

void do_loud_say(string arg)
{
  player->do_loud_say(wibble_speech(arg));
}
 
void do_say(string arg)
{
  player->do_say(wibble_speech(arg));
}

void do_tell(string arg)
{
  player->do_tell(wibble_speech(arg));
}

void do_shout(string arg)
{
  player->do_shout(wibble_speech(arg));
}

void do_whisper(string arg)
{
  player->do_whisper(wibble_speech(arg));
}
