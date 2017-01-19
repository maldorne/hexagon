/* 
 * your very own personal weather... 
 * Algunos retoques, neverbot@CcMud
 * Renovado para el nuevo sistema climatico v3.0, neverbot@CcMud 11/03
 *
 * Añadida nieve, para el sistema climatico v3.1, neverbot 20/02/04
 *
 * Cambiado el daño por frio extremo, ahora solo hara daño hasta llegar al 20%
 *  de la vida total del player, y despues colocara una shadow que molestara al
 *  jugador pero sin llegar a matarlo. neverbot 04/09
 *
 */

#include <areas/weather.h>
#include <common/properties.h>
// Para los mensajes de daño por frio
#include <living/combat.h>

#define NO_SNOWMAN "there is a snowman"

int wetness;
 
void create() 
{
  wetness = 0;
}

void init() 
{
  // add_action("make","make");
  add_action("make","hacer");
  // add_action("splash","splash");
  add_action("splash","mojar"); // chorrada pero entretiene
}

int query_wetness() { return wetness; }
void adjust_wetness(int arg) 
{ 
    wetness += arg; 
    // Ponemos un maximo y un minimo, neverbot 11/03
    if (wetness > 100)
      wetness = 100;
    if (wetness < 0)
      wetness = 0;
}
void add_wetness(int arg) { adjust_wetness(arg); }
void set_wetness(int i) { wetness = i; }

string weather_extra_look(int self) 
{
  string me;
  string vocal;
  
  vocal = "o";

  if (this_object()->query_gender() == 2)
    vocal = "a";

  /* Ghosts shouldn't be wet, nor getting wet, Baldrick, june '96 */
  if (this_object()->query_dead()) {
      wetness = 0;
      return "";
  } 

  if (!self)
  {
    me = capitalize(this_object()->query_pronoun());
    if (wetness>90)
        return me+" está calad"+vocal+" hasta los huesos.\n";
    if (wetness>70)
        return me+" está chorreando agua por todas partes.\n";
    if (wetness>50)
        return me+" está bastante mojad"+vocal+".\n";
    if (wetness>10)
        return me+" está mojad"+vocal+".\n";
    if (wetness>0)
        return me+" está un poco mojad"+vocal+".\n";
    return "";
  }
  else
  {
    if (wetness>90)
        return "Estás calad"+vocal+" hasta los huesos.\n";
    if (wetness>70)
        return "Estás chorreando agua por todas partes.\n";
    if (wetness>50)
        return "Estás bastante mojad"+vocal+".\n";
    if (wetness>10)
        return "Estás mojad"+vocal+".\n";
    if (wetness>0)
        return "Estás un poco mojad"+vocal+".\n";
    return "";
  }
}

/*     
void event_weather() {
    call_out("check_it",0);
}
void check_it() {
*/

// who: previous_object que llama al evento
// flag: 1 = amanecer
//       2 = anochecer
//       3 = cambio climatico
// values: lista con 6 int que son: 
//         lluvia, viento y temperatura antes del cambio seguidos de
//         lluvia, viento y temperatura nuevos
void event_weather(object who, varargs int flag, int * values) 
{
  int * difs;
  string * frases;
  string ret;

  difs = ({ 0, 0, 0, });
  frases = ({ });

  if (flag == FLAG_AMANECER)
  {
    tell_player(this_object(), "El sol sale por el horizonte indicándote el comienzo de un nuevo día.\n");
    return;
  }
  if (flag == FLAG_ANOCHECER)
  {
    tell_player(this_object(), "El sol se pone lentamente por el horizonte.\n");
    return;
  }
  // Solo deberia informar a npcs de esto
  if (flag == FLAG_CHECK)
  {
    // tell_object(this_object(), "Chimpum.\n");  
    return;
  }
  
  if (sizeof(values) != 6)
    return;

  /*  
  tell_object(this_object(),"[Event weather]: " +
                "Flag: "+flag+" Values: " + 
                values[0] + " " + values[1] + " " + values[2] + " " +
                values[3] + " " + values[4] + " " + values[5] + "\n\n");   
  */
  
  // Calculamos las diferencias entre el clima anterior y el actual
  // (lluvia, viento, temperatura)
  difs[0] = values[0] - values[3];
  difs[1] = values[1] - values[4];
  difs[2] = values[2] - values[5];
  
  // HUMEDAD:
  if (!this_object()->query_dead() && !environment(this_object())->query_water_environment())
  {
    if (values[3] >= 50)
    {
      tell_object(this_object(), "Te mojas un poco.\n");
      adjust_wetness(values[3]/3);
    }
    else
    {
      if (query_wetness() > 0)
      {
        tell_object(this_object(), "Te secas poco a poco.\n");
        adjust_wetness(-values[3]/2);
      }
    }
  }

  // LLUVIA:
  // Si no estaba lloviendo
  if (values[0] < 50){
    if (((values[3] >= 50) && (values[3] < 70)) ||
      ((values[3] >= 70) && (values[5] <= 80)) )
      frases += ({"comienza a llover"});
    else if ((values[3] >= 70) && (values[5] > 85))
      frases += ({"comienza a nevar"});
    else if ((values[3] >= 70) && (values[5] > 80))
      frases += ({"comienza a granizar"});
  }
  // Estaba lloviendo
  if ((values[0] >= 50) && (values[2] <= 80)){
    // Termina el agua
    if (values[3] < 50)
      frases += ({"deja de llover"});
    // Sigue el agua y baja la temperatura
    else if ((values[3] >= 70) && (values[5] > 85))
      frases += ({"deja de llover y comienza a nevar"});
    else if ((values[3] >= 70) && (values[5] > 80))
      frases += ({"deja de llover y comienza a granizar"});
  }
  // Estaba granizando (lluvia y frio)
  if ((values[0] >= 70) && (values[2] > 80) && (values[2] <= 85)){
    // Termina el agua
    if (values[3] < 50) 
      frases += ({"deja de granizar"});
    // Sigue el agua pero sube la temperatura
    else if ((values[3] >= 50) && (values[5] > 85))
      frases += ({"deja de granizar y comienza a nevar"});
    else if ((values[3] >= 50) && (values[5] <= 80))
      frases += ({"deja de granizar pero sigue lloviendo"});
  }
  // Estaba nevando (lluvia y mucho frio)
  if ((values[0] >= 70) && (values[2] > 85)){
    // Termina el agua
    if (values[3] < 50) 
      frases += ({"deja de nevar"});
    // Sigue el agua pero sube la temperatura
    else if ((values[3] >= 50) && (values[5] <= 80))
      frases += ({"deja de nevar pero sigue lloviendo"});
    else if ((values[3] >= 50) && (values[5] <= 85))
      frases += ({"deja de nevar y comienza a granizar"});
  }
    
  // VIENTO:
  if ((values[1] >= 60) && (values[4] < 60))
    frases += ({"el viento cesa"});  
  
  if ((values[1] < 60) && (values[4] >= 60))
    frases += ({"el viento comienza a soplar"});  

  if ((values[1] >= 60) && (values[4] >= 60) &&
    (difs[1] > 15))
    frases += ({"el viento cesa levemente"});  

  if ((values[1] >= 60) && (values[4] >= 60) &&
    (difs[1] < -15))
    frases += ({"el viento sopla con más fuerza"});  

  // TEMPERATURA:
  if (difs[2] < -10){
    // Si ya hacía frío
    if (values[0] > 50)
      frases += ({"el frío aumenta"});  
    else
      frases += ({"baja un poco la temperatura"});  
  }
  else if (difs[2] > 10){
    // Si ya hacía calor
    if (values[0] < 50)
      frases += ({"el calor aumenta"});  
    else
      frases += ({"sube un poco la temperatura"});
  }
  
  ret = query_multiple_short(frases);
  if (sizeof(frases))
    tell_player(this_object(), "Notas cómo " + ret + ".\n");
  return;

  /* try to leave it out, I don't like it tho...
   * Baldrick, june '96.
  if (wetness>0 && random(100) < 10)
    tell_object(this_object(),"Squelch.\n");   
  */
}
 
int make(string str) 
{
    object ob; 

  if ( environment(this_object()) && 
    (environment(this_object())->query_property("location") == "inside") )
  {
    notify_fail("Debes estar al aire libre para hacer eso.\n");
    return 0;
  }

  if (environment(this_object()) && 
    environment(this_object())->query_water_environment() )
  {
    notify_fail("No puedes hacer eso en el agua.\n");
    return 0;
  }

  if (str == "bola de nieve") 
  {
    /* well lets do it then ;) */
    if (!WEATHER->query_snowing()) {
      notify_fail("Necesitas nieve para hacer una bola de nieve.\n");
      return 0;
    }
    ob = clone_object(SNOWBALL_OB);
    ob->move(this_object());
    write("Haces una bonita bola de nieve. Disfrútala.\n");
    return 1;
  }
  if (str == "muñeco de nieve") {
    if (!WEATHER->query_snowing()) {
      notify_fail("Necesitas nieve para hacer un muñeco de nieve.\n");
      return 0;
    }

    if (environment()->query_static_property(NO_SNOWMAN)){
       notify_fail("¿Cuántos muñecos de nieve quieres hacer en el mismo lugar?\n");
       return 0;
    }
    ob = clone_object(SNOWMAN_OB);
    ob->move(environment());
    // 300 hb's == 10 minutos
    environment()->add_static_property(NO_SNOWMAN, 1);
    write("Haces un decorativo y bonito muñeco de nieve.\n");
    return 1;
  }
  notify_fail("¡¡No puedes hacer eso!!\n");
  return 0;
}
 
int splash(string str) 
{
  object *obs;
  int i;

  if ( environment(this_object()) && 
    (environment(this_object())->query_property("location") == "inside") )
  {
    notify_fail("Debes estar al aire libre para hacer eso.\n");
    return 0;
  }

  if (environment(this_object()) && 
    environment(this_object())->query_water_environment() )
  {
    notify_fail("No puedes hacer eso en el agua.\n");
    return 0;
  }

  if (!str) {
    notify_fail("¿A quién quieres mojar?\n");
    return 0;
  }
  // env = environment();
  if (!WEATHER->query_raining(environment())) {
    notify_fail("Debe estar lloviendo o haber charcos para mojar a alguien.\n");
    return 0;
  }
  obs = find_match(str, environment());
  if (!sizeof(obs)) {
    notify_fail(capitalize(str)+" no esta aquí... ¿A qué juegas?\n");
    return 0;
  }

  if (sizeof(obs) > 1)
  {
    for (i = 0; i < sizeof(obs); i++) { 
      if(!obs[i]->query_hide_shadow() && !obs[i]->query_hidden())
      { //evitamos que encuentre a los escondidos
         obs[i]->adjust_wetness(2+random(4));
         tell_object(obs[i],(string)this_object()->query_cap_name()+
          " te moja"+(sizeof(obs)==2?" a ti y a ":", ")+
          query_multiple_short(obs - ({ obs[i] }))+".\n");
      }
    }
    str = query_multiple_short(obs);
    write("Pisas enérgicamente un charco y mojas a "+str+".\n");
    say(this_object()->query_cap_name()+" pisa un charco y moja a "+str+".\n", obs);
  }
  else
  {
    if(!obs[0]->query_hide_shadow() && !obs[0]->query_hidden()){
      obs[0]->adjust_wetness(2+random(4));
      if (this_object() != obs[0])
      {
      tell_object(obs[0], this_object()->query_cap_name()+" te moja.\n");
      write("Pisas enérgicamente un charco y mojas a "+obs[0]->query_cap_name()+".\n");
      say(this_object()->query_cap_name()+" pisa un charco y moja a "+obs[0]->query_cap_name()+".\n", obs);
      }
      else
      {
      write("Pisas enérgicamente un charco y te mojas. ¿Divertido?\n");
      say(this_object()->query_cap_name()+" pisa un charco y se moja.\n", obs);
      }
    }
  }
  return 1;
}

// neverbot 21/02/04
// Esto no se llama cada heart_beat!!!
// Buscar el void heart_beat() de player.c para ver cuando se llama!!
void weather_heart_beat()
{
  int * datos;
  int cuanto, do_damage, my_time;

  cuanto = 0, do_damage = 1, my_time = 0;

  // Añadida comprobacion, si estas muerto ignoras el clima, neverbot 24/02/04  
  if (!this_object()->query_dead() && environment(this_object()))
  {    
    // Los event_weather que 'secan' al personaje solo se producen
    // cuando esta al aire libre... si esta bajo techo lo secamos tambien
    if (!environment(this_object())->query_outside() && 
       (this_object()->query_wetness() > 0) && (random(3) == 1))
    {
       tell_object(this_object(), "Te secas poco a poco.\n");   
         this_object()->adjust_wetness(-10);
    }
    
    // Hack, neverbot 12/08/05
    // Los newbies recien creados podrian morir si tienen mala suerte
    // El clima no hace daño!!!
    // Protegemos durante 12 horas de personaje, a partir de ahi el clima actua
    // normalmente :)
    my_time = this_object()->query_time_on();
    my_time = -my_time;
    
    if (environment(this_object())->query_outside())
    {
      // Con mas de 50 de proteccion contra el frio, el clima no hace nada      
      if (this_object()->query_resistance("cold") >= 50)
        do_damage = 0;
      // Con entre 20 y 50 de proteccion, el clima solo afecta a veces
      else if (this_object()->query_resistance("cold") >= 40)
      {
        if (random(4) != 1)
          do_damage = 0;
      }
      else if (this_object()->query_resistance("cold") >= 30)
      {
        if (random(3) != 1) 
          do_damage = 0;
      }
      else if (this_object()->query_resistance("cold") >= 20)
      {
        if (random(2) != 1)
          do_damage = 0;
      }

      if (do_damage == 0)
      {
        // if (this_object()->query_coder())
        //  tell_object(this_object(), "DEBUG (player_weather): Proteccion 'cold' evita daño del clima.\n");
        return;      
      }
     
      // datos == ({ lluvia, viento, temperatura })
      datos = WEATHER->query_actual_data(environment(this_object()));

      // Daño por frío
      if (datos[2] > 80)
      {
        cuanto = (datos[2] - 80)/6; 
        if (cuanto <= 0) 
           cuanto = 1; // cuanto = [1..3]
    
        tell_object(this_object(), DFF+"Notas cómo el frío a tu alrededor te congela.\n");
       // Con el 20% de vida o menos no hacemos daño, ponemos shadow
        if (this_object()->query_max_hp() / this_object()->query_hp() >= 2)
        {
          if (!this_object()->query_weather_shadow())
          {
            object shadow;
            shadow = clone_object(WEATHER_SH);
            if (shadow)
              shadow->setup_shadow(this_object());
          }
        }
        // Proteccion de newbies
        else if (my_time > 43200)
          this_object()->spell_damage(cuanto, "cold");
      }

      // Daño por granizo
      if ((datos[2] > 80) && (datos[2] <= 85) && (datos[0] > 50))
      {
        cuanto = (datos[2] - 80)/6;
        if (cuanto <= 0) 
          cuanto = 1; // cuanto = [1..3]
        
        tell_object(this_object(), DFF+"El granizo te golpea con fuerza.\n");
          
        // Con el 20% de vida o menos no hacemos daño, ponemos shadow
        if (this_object()->query_max_hp() / this_object()->query_hp() >= 2)
        {
          if (!this_object()->query_weather_shadow())
          {
            object shadow;
            shadow = clone_object(WEATHER_SH);
            if (shadow)
              shadow->setup_shadow(this_object());
          }
        }            
        // Proteccion de newbies
        else if (my_time > 43200)
          this_object()->spell_damage(cuanto, "cold");
      }
    }
  }
}

mixed * stats() 
{
  return ({ 
    ({ "Wetness", wetness, }),
          });
}