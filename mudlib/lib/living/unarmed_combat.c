
#include <living/combat.h>

/*
    Unarmed Combat for FR by Sojan
    Currently unlobotomised/castrated by Baldrick :)
    (sorry B)

    * On my way... Baldrick. June '94

    Ummm this is designed to be as similar to the weapon_logic
    as I could get it ... there are a few notable differences
    but not enough to be significant ..
    Okay due to current problems the unarmed/armed combat
    is gonna yield the same actual combat result ....
    damage will still be better for martial arts than for
    standard brawling though ......
    Oh - also most of the stuff in this file will come from
    an include file cause this doesn't rely on different weapons
    classes and stuff ... bingles ...
    
    Retocado por neverbot para Cc, 04/2003, para añadir el sistema
    de localizaciones de Iolo y utilizar (por fin) el sistema
    de estilos de combate.

    Añadido sistema de aprendizaje automatico de los estilos de 
    combate, neverbot 7/03
    
*/

static int ovr_num, ovr_type;
static object defender,
              attacker;
// Mensajes de daño desarmado segun el estilo de combate. Se almacenan solo
// una vez cada cambio de estilo, para no tener que estar comprobando mappings
// para cada golpe
static mixed * messages;

// Estilos de combate pasa a ser un mapping con ([ "estilo":dominio ])
// siendo dominio el porcentaje de control sobre ese estilo de lucha.
mapping known_unarmed_styles;
string current_unarmed_style;
// Dominio sobre la tecnica actual de combate (lo hacemos variable propia)
// para agilizar los accesos (y minimizar la busqueda en el mapping)
int unarmed_ability;

// Numero de golpes seguidos sin armas (para mejorar los estilos de lucha)
private int num_unarmed_hits;

int query_num_unarmed_hits(){ return num_unarmed_hits; }
void reset_num_unarmed_hits(){ num_unarmed_hits = 0; }

int unarmed_attack(object def, object att);
mixed *workout_attack(string unarmed_type);
mixed *recalc_damage(int i);

void write_message(int damage, string local, object att, object defdr);
void unarmed_combat_commands();

// Estilos desarmados
int set_unarmed_combat_style(string style);
int add_known_unarmed_style(string style, varargs int dominio);
int remove_known_unarmed_style(string style);
mapping query_known_unarmed_combat_styles();
int adjust_unarmed_ability(string style, int i);

/* mapping valid_attack(); */

void unarmed_combat_commands()
{
  add_private_action("do_combat_styles", "estilo");
  add_private_action("do_combat_styles", "estilos");
}

void create()
{
  ovr_num = 0;
  ovr_type = 0;
  num_unarmed_hits = 0;
  // Damos a todos los jugadores un 50% de habilidad en el tipo de lucha predeterminado
  known_unarmed_styles= ([ DEF_UNARMED:50, ]);
  current_unarmed_style = DEF_UNARMED;
  unarmed_ability = 50;
  messages = ({ });
}

int unarmed_attack(object def, object att)
{
  mixed *att_val;
  mixed damage;
  int absorbed_damage, j, percent;
  object * obs;
  
  attacker = att;
  defender = def;
  att_val = ({ });
  att_val = workout_attack(current_unarmed_style);

  if (att_val[0] == HIT) 
  {
    damage = recalc_damage(att_val[1]);
    absorbed_damage = 0;

    // Buscamos el objeto que protege para ver cuanto daño absorbe
    if (damage[1] != "")
    {
      obs = defender->query_worn_ob();
      for (j = 0; j < sizeof(obs); j++)
      {
        if (obs[j]->query_localization() == damage[1])
        {
          // Calculamos cuanto % de daño absorbe la armadura
          if (obs[j]->query_ac() + obs[j]->query_blunt_bon() > 0) 
          {
              if ((obs[j]->query_ac() + obs[j]->query_blunt_bon()) > 10)
                percent = 100;
              else
                percent = (obs[j]->query_ac() + obs[j]->query_blunt_bon())*10;

              absorbed_damage = damage[0]*percent/100;
          }
          break;
        }
      }
    }
 
    // Aplicamos el daño definitivo
    defender->adjust_hp(-(damage[0] - absorbed_damage), attacker);

    write_message(damage[0],damage[1], attacker, defender);

    // Sistema de aprendizaje de estilos desarmados, neverbot 7/03
    num_unarmed_hits++;
    if ((num_unarmed_hits > 0) && (num_unarmed_hits%100 == 0))
    {
      // Cuanto mas dominemos el estilo, mas dificil es mejorarlo
        if ((unarmed_ability < MAX_UNARMED_AUTO_LEARN) && (random(100) > unarmed_ability))
         adjust_unarmed_ability(current_unarmed_style, 1);
      num_unarmed_hits = 0;
    }   
  }
  else 
  {
    if (att_val[0] == DODGE)
    {
      damage = ({ 0 });
      tell_object(attacker,defender->query_cap_name()+" logra esquivar tu ataque.\n");
      tell_object(defender,attacker->query_cap_name()+" intenta golpearte pero logras "+
         "esquivar su ataque.\n");
      tell_room(environment(attacker), attacker->query_cap_name() +
                " trata de golpear a " + defender->query_cap_name() +
                " pero "+defender->query_demonstrative()+" logra esquivar su ataque.\n", ({attacker,defender}));
    }
    else
    {
      // att_val[0] == MISS
      damage = ({ 0 });
      tell_object(att,def->query_cap_name()+" logra bloquear tu ataque.\n");
      tell_object(def,att->query_cap_name()+" intenta golpearte pero logras "+
         "bloquear su ataque.\n");
      tell_room(environment(att),att->query_cap_name()+" trata de golpear a "+
         def->query_cap_name()+" pero "+defender->query_demonstrative()+" para el ataque.\n",({att,def}));
    }
  }
  return damage[0];
} /* unarmed_attack */

mixed * workout_attack(string unarmed_type)
{
  string happen;
  int result, attackerwc, defenderac, damage_done;
  int tmp;

  // Comprobacion por si acaso (aunque nunca deberia darse), neverbot 4/03
  if (!defender || !attacker)
    return ({ MISS, 0 });
     
  // Comprobacion: ovr_num y ovr_type son nosave, por lo que originalmente no los
  //  tendremos. Para evitar obtenerlos cada vez que ataquemos, solo los buscaremos
  //  en la tabla la primera vez (para no realizar mas busquedas en el mapping.
  if ((ovr_num == 0) || (ovr_type == 0))
  {
    tmp = UNARMED_BASE->set_damage_dice(current_unarmed_style, this_object());
    if (tmp == 0)
    {
      ovr_num = BDEFNUM;
      ovr_type = BDEFTYPE;
    }
  }

  damage_done = 0;

  // Luck factor  
  if (roll(1, 200) < 10 )
  {
    // Mensaje de pifia añadido, habria que hacerlo personalizado mas adelante
    tell_object(this_object(), "¡Oh, qué torpeza!\n");
    return ({ FUMBLE, 0 });
  }

  attackerwc = (int)attacker->query_total_wc();

  // Penalizador por el uso de equipo (se aplica al AC pero solo para el caso
  //  de calcular si se impacta o no)
  defenderac = defender->query_total_ac() + defender->query_skill_malus();
  
  // Idea sacada del sistema de bonificaciones de Iolo para Rl, cuantos
  // mas atacantes tengas, mas leches te caen.
  //   1 o 2 atacantes, misma AC
  //   3 o 4 atacantes, se reduce a la mitad
  //   5 o 6 atacantes, se reduce a un tercio, ...  

  // if ( sizeof(defender->query_attacker_list()) > 2)
  //   defenderac = (defenderac / (sizeof(defender->query_attacker_list()) / 2));

  // Cambiado por neverbot, 10/2009. No depende de tu numero de atacantes, sino de la diferencia
  // entre tus atacantes y los de tu contrario. Asi evitamos que la AC se modifique en batallas campales
  // 6 vs 6, por ejemplo.
  {
    int ac_diff;
    ac_diff = sizeof(defender->query_attacker_list()) - sizeof(attacker->query_attacker_list());
    if ( ac_diff >= 2 )
      defenderac = (defenderac / (ac_diff / 2));  
  }

  // A la habilidad del atacante (desarmado) le sumamos el factor suerte para
  // ambos jugadores (1d10), y le restamos el AC del defensor.
  result = ((attackerwc + roll(1, 10)) - defenderac);

  if (result > 1)
  {
    happen = HIT;
    /* Con las nuevas comprobaciones esto ya no es necesario, siempre
       tendremos actualizados los dados
    if (member_array(unarmed_type, keys(known_unarmed_styles))>-1)
      damage_done = (int)UNARMED_BASE->
                 unarmed_damage(unarmed_type, ovr_num, ovr_type) +
                 (int)attacker->query_damage_bonus();
    else
      damage_done = roll(BDEFNUM, BDEFTYPE);
    */
    damage_done = (int)roll(ovr_num, ovr_type) + 
                  (int)attacker->query_damage_bonus() +
                  (int)attacker->query_stat_bonus_to_str();

    if (damage_done <= 0)
      damage_done = 1;

    // Luck factor  
    if ( roll(1, 200) < 10 ) 
    {
      // Mensaje de critico añadido
      tell_object(this_object(), "¡Oh, qué habilidad!\n");
      damage_done *= 2;
    }
  }
  else
  {
    // Calculamos si el fallo ha sido porque el defensor esquiva o 
    // porque el atacante falla (el resultado es irrelevante excepto a terminos
    // "narrativos", solo influira en el tipo de mensajes que se le 
    // darán a cada uno)
    result = ((attacker->query_level() + 
               attacker->query_dex() + 
               attacker->query_skill_malus()) + 
               roll(1,10)) -
             ((defender->query_level() + 
               defender->query_dex() + 
               defender->query_skill_malus()) + 
               roll(1,10));
  
    if (result < 0)
    {
      happen = DODGE;
      damage_done = 0;
    }
    else
    {
      happen = MISS;
      damage_done = 0;
    }
  }

  return ({ happen, damage_done });
} /* workout_attack */

int set_unarmed_combat_style(string style)
{
  // int i;
  int tmp;
  // string * estilos = keys(known_unarmed_styles);

  if (this_object()->query_dead())
  {
     notify_fail("Estás en forma espiritual, no necesitas conocer eso.\n");
     return 0;
  }

  // Ahora hay un comando centralizado en combat.c, esto no se vera nunca
  /*
  if (!style || (style == "")){
    tell_object(this_object(), "------------------------------------------\n");
    tell_object(this_object(), " Conoces los siguientes estilos de lucha:\n");
    tell_object(this_object(), "------------------------------------------\n");
    for(i = 0; i < sizeof(estilos); i++){
      tell_object(this_object(), "\t%^BOLD%^"+capitalize(estilos[i])+"%^RESET%^ al "+
                known_unarmed_styles[estilos[i]] + "%\n");
    }
    tell_object(this_object(), "------------------------------------------\n");
    tell_object(this_object(), "Tu estilo de lucha actual es '"+current_unarmed_style+"'.\n");
    tell_object(this_object(), "Utiliza: estilo <nombre> para cambiarlo.\n");
    tell_object(this_object(), "------------------------------------------\n");
    return 1;
  }
  */
    
  if (member_array(style, keys(known_unarmed_styles))>-1)
  {
    current_unarmed_style = style;
    // Minimo de habilidad
    unarmed_ability = (known_unarmed_styles[style]>=MIN_UNARMED_ABILITY?known_unarmed_styles[style]:MIN_UNARMED_ABILITY);

    // Actualizamos tambien el tipo de dados de daño que vamos a utilizar.
    tmp = UNARMED_BASE->set_damage_dice(current_unarmed_style, this_object());
    if (tmp == 0)
    {
      ovr_num = BDEFNUM;
      ovr_type = BDEFTYPE;
    }

    write("Estilo de combate desarmado: '"+style+"'. Nivel de habilidad: "+unarmed_ability+"%.\n");

    // Reseteamos su contador de 'golpes' (por haber cambiado de estilo)
    reset_num_unarmed_hits();
    
    // Actualizamos los mensajes a utilizar
    messages = UNARMED_BASE->query_messages(current_unarmed_style, unarmed_ability, this_object());      
    
    return 1;
  }
  notify_fail("No conoces el estilo de combate '"+style+"'.\n");
  return 0;
}

int add_known_unarmed_style(string style, varargs int dominio)
{
  if (!style) 
    return 0;

  if (!dominio)
    known_unarmed_styles[style] = MIN_UNARMED_ABILITY;
  else
    known_unarmed_styles[style] = dominio;
   
  return 1;
}

int remove_known_unarmed_style(string style)
{
  if (!style) 
    return 0;

  map_delete(known_unarmed_styles, style);
  
  if (style == current_unarmed_style)
  {
    current_unarmed_style = DEF_UNARMED;
    unarmed_ability = MIN_UNARMED_ABILITY;
  }

  return 1;
}

string query_current_unarmed_style()
{
  return current_unarmed_style;
}

int query_current_unarmed_ability()
{
  return unarmed_ability;
} 

// Ajustamos el dominio sobre el estilo, devolvemos la
//  habilidad resultante
int adjust_unarmed_ability(string style, int i)
{
  if (!known_unarmed_styles[style])
    return 0;
  
  // El estilo no existe
  if (member_array(style, UNARMED_BASE->query_unarmed_styles()) == -1)
    return 0;

  known_unarmed_styles[style] += i;

  if (style == current_unarmed_style)
    unarmed_ability += i;

  if (i > 0)
    tell_player(this_object(), "¡Has mejorado tus habilidades en "+style+"!\n");

  return known_unarmed_styles[style];   
}

mapping query_known_unarmed_combat_styles()
{
  return known_unarmed_styles;
}

int do_combat_styles(string style)
{
  int i;
  string ret;
  string line;
  string * _unarmed_styles;

  _unarmed_styles = keys(query_known_unarmed_combat_styles());
  // string * estilos_armed = keys(query_known_armed_combat_styles());
  // string a1, a2, a3;

  if (this_object()->query_dead())
  {
    notify_fail("Estás en forma espiritual, no necesitas conocer eso.\n");
    return 0;
  }

  if (!style || (style == ""))
  {
    line = "";
    // for( i = 0; i < this_object()->query_cols(); i++)
    //    line += "-"; // Tengo que averiguar como se hace esto con un sprinf :S
    // line += "\n";
    
    // line = sprintf("%*'-'s\n", this_object()->query_cols(), "");
    line = sprintf("%*s\n", this_object()->query_cols(), "");
    
    // ret = sprintf("%*'-'|s\n\n", this_object()->query_cols()+18, 
    ret = sprintf("%*s\n\n", this_object()->query_cols()+18, 
      "> %^GREEN%^Conoces los siguientes estilos de lucha: %^RESET%^<");
    // ret += "   Sin armas:\n";

    for (i = 0; i < sizeof(_unarmed_styles); i++){
      ret += sprintf("\t%35-s %25|s (%s)\n", "%^BOLD%^"+capitalize(_unarmed_styles[i]) +"%^RESET%^", 
                "["+percentage_bar(query_known_unarmed_combat_styles()[_unarmed_styles[i]])+"]",
                ""+query_known_unarmed_combat_styles()[_unarmed_styles[i]]+"%");
    }
    /*
    ret += "   Con armas:\n";
    for(i = 0; i < sizeof(estilos_armed); i++){
            ret += sprintf("\t%35-s %25|s (%s)\n", "%^BOLD%^"+capitalize(estilos_armed[i]) +"%^RESET%^", 
                      "["+percentage_bar(query_known_armed_combat_styles()[estilos_armed[i]])+"]",
                      ""+query_known_armed_combat_styles()[estilos_armed[i]]+"%");
            ret += "\t("+capitalize(query_multiple_short(ARMED_BASE->query_style_weapons(estilos_armed[i])))+")\n";
    }
    */

    ret += "\n" + line;
    ret += "  Tu estilo de lucha actual es '"+query_current_unarmed_style()+"'.\n";
    //  "' y '" + query_current_armed_style()+"'.\n";
    // ret += "  Utiliza: estilo {con armas|sin armas} <nombre> para cambiarlos.\n";
    ret += "  Utiliza: estilo <nombre> para cambiarlo.\n";
    ret += line;
    tell_object(this_object(), ret);
    return 1;
  }

  /*
  sscanf(style, "%s %s %s", a1, a2, a3);
  if (   ((a1 != "con") && (a1 != "sin")) ||
         (a2 != "armas") ||
         (!a3 || (a3 == ""))  )
  {
     notify_fail("Sintaxis: 'estilo [con armas|sin armas] <nombre>'\n"+
                 "        o 'estilo' para ver los estilos disponibles.\n");
     return 0;
  }

  if (a1 == "con")
  {
    if (member_array(a3, estilos_armed) >- 1)
    {
      set_armed_combat_style(a3);
    }
    else
    {
      notify_fail("No conoces el estilo de combate con armas '"+a3+"'.\n");
      return 0;
    }
    // Este mensaje ya esta en la funcion set_armed_combat_style()
    // tell_object(this_object(), "Estilo de combate con armas: '"+a3+"'. "
    //             "Nivel de habilidad: "+query_current_armed_ability()+"%.\n");
  }
  else if (a1 == "sin")
  {
    if (member_array(a3, _unarmed_styles) >- 1)
    {
      set_unarmed_combat_style(a3);
    }
    else
    {
      notify_fail("No conoces el estilo de combate sin armas '"+a3+"'.\n");
      return 0;
    }
    // Este mensaje ya esta en la funcion set_unarmed_combat_style()
    // tell_object(this_object(), "Estilo de combate sin armas: '"+a3+"'. "
    //             "Nivel de habilidad: "+query_current_unarmed_ability()+"%.\n");
  }
  */

  if (member_array(style, _unarmed_styles) >- 1)
  {
    set_unarmed_combat_style(style);
  }
  else
  {
    notify_fail("No conoces el estilo de combate '"+style+"'.\n");
    return 0;
  }

  return 1;
}

/* This needs the attacker's object too.. */
/* Aqui he aplicado tambien el sistema de localizaciones de Iolo en Rl,
 * neverbot 4/03
 */
mixed *recalc_damage(int i)
{
  int hps;
  mixed local;

  hps = 0;

  local = defender->obtain_location();
  
  if (!local) 
    local = ({ 1, "" });
  
  // Aplicamos aqui el modificador por localizacion
  // (segun donde demos haremos mas o menos daño con un golpe
  //  de la misma fuerza).
  hps = (int)((float)i * (float)local[0]);
    
  return ({hps, local[1]});
}

// Sistema de Iolo@RL de localizaciones!!
// Sistema de mensajes generalizado con tecnicas, neverbot 4/03
void write_message(int damage, string local, object att, object defdr)
{
  string place;

  if (!local || local=="") 
    place = "";
  else 
    place = " en " + local;
  
  if (sizeof(messages) != 3)  
    messages = UNARMED_BASE->query_messages(current_unarmed_style, unarmed_ability, att);
  
  if (sizeof(messages) != 3)
  {
    tell_object(att, "Parece que ha habido algún problema con tus técnicas de combate, "+
                     "habla con algún programador.\n");
    return;
  }
  
  // Si son strings, damos mensajes simples
  if (stringp(messages[0]))
  {
    tell_object(att, ATT+capitalize(messages[0])+" a "+defdr->query_cap_name()+place+".\n");  
    tell_object(defdr, DFF+att->query_cap_name()+" "+messages[1]+place+".\n");
    tell_room(environment(att),att->query_cap_name()+" "+messages[2]+" a "+defdr->query_cap_name()+
        place+".\n",({att,defdr}));
  }
  else // cada elemento de messages es a su vez una lista de mensajes
  {
    int i;
    i = random(sizeof(messages[0]));

    tell_object(att, ATT+capitalize(messages[0][i])+" a "+defdr->query_cap_name()+place+".\n");  
    tell_object(defdr, DFF+att->query_cap_name()+" "+messages[1][i]+place+".\n");
    tell_room(environment(att),att->query_cap_name()+" "+messages[2][i]+" a "+defdr->query_cap_name()+
        place+".\n", ({att, defdr}));      
  }
  
  return;
}

mixed * query_messages() { return messages; }
int set_messages(mixed * list)
{
  // Solo se puede personalizar de este modo con npcs
  if (interactive(this_object()))
    return 0;
  
  // Tiene que ser una lista de tres
  if (sizeof(list) != 3)
    return 0;
  
  // Si es una lista de listas pero tienen distinto tamaño
  if ( arrayp(list[0]) && 
    ( (sizeof(list[0]) != sizeof(list[1])) || (sizeof(list[1]) != sizeof(list[2])) ))
    return 0;

  messages = list;
  return 1;
}

/*
mapping valid_attack(){
  int bing, bong;
  bing = random(3);
  bong = random(2);
  return ([
        "punch" : ({ AN+" da un punyetazo a "+DN+" en "+({ "el estomago", "la cara", "el plexo solar"})[bing]+".\n",
                     "Das un punyetazo a "+DN+" en "+({ "el estomago", "la cara", "plexo solar"})[bing]+".\n",
                     AN+" te da un punyetazo en "+({ "el estomago", "cara", "plexo solar"})[bing]+".\n" }),
        "rapid" : ({ AN+" da un rapido punyetazo a "+DN+" en "+({ "el estomago", "cara", "plexo solar"})[bing]+".\n",
                     "Das un rapido punyetazo a "+DN+" en "+({ "el estomago", "cara", "plexo solar"})[bing]+".\n",
                     AN+" te da un rapido punyetazo en "+({ "el estomago", "cara", "plexo solar"})[bing]+".\n" }),
        "kick" : ({ AN+" patea a "+DN+" en "+({ "el estomago", "la ingle", "la espinilla"})[bing]+".\n",
                     "Pateas a "+DN+" en "+({ "el estomago", "la ingle", "la espinilla"})[bing]+".\n",
                     AN+" te patea en "+({ "el estomago", "la ingle", "la espinilla"})[bing]+".\n" }),
        "spinkick" : ({ AN+" da una patada giratoria a "+DN+" en "+({ "un lado de la cabeza", "el cuello", "el pecho"})[bing]+".\n",
                     "Das una patada giratoria a "+DN+" en "+({ "un lado de la cabeza", "el cuello", "el pecho"})[bing]+".\n",
                     AN+" te da una patada giratoria en "+({ "un lado de la cabeza", "el cuello", "el pecho"})[bing]+".\n" }),
        "chop" : ({ AN+" da un golpe de karate a "+DN+" en "+({ "un lado de la cabeza", "la garganta", "el pecho"})[bing]+".\n",
                     "Das un golpe de karate a "+DN+" en "+({ "un lado de la cabeza", "la garganta", "el pecho"})[bing]+".\n",
                     AN+" te da un golpe de karate en "+({ "un lado de la cabeza", "la garganta", "el pecho"})[bing]+".\n" }),
        "roundhouse" : ({ AN+" da una patada voladora a "+DN+" en "+({ "un lado de la cabeza", "la cara", "el pecho"})[bing]+".\n",
                     "Das una patada voladora a "+DN+" en "+({ "un lado de la cabeza", "la cara", "el pecho"})[bing]+".\n",
                     AN+" te da una patada voladora en "+({ "un lado de la cabeza", "la cara", "el pecho"})[bing]+".\n" }),
        "knee" : ({ AN+" da un rodillazo a "+DN+" en "+({ "el costado", "el estomago"})[bong]+".\n",
                     "Das un rodillazo a "+DN+" en "+({ "el costado", "el estomago"})[bong]+".\n",
                     AN+" te da un rodillazo en "+({ "el costado", "el estomago"})[bong]+".\n" }),
        "headbutt" : ({ AN+" da un cabezazo a "+DN+".\n",
                        "Das un cabezazo a "+DN+".\n",
                        AN+" te da un cabezazo.\n" }),
        "footsweep" : ({ AN+" le hace un barrido de piernas a "+DN+".\n",
                        "Le haces un barrido de piernas a "+DN+".\n",
                        AN+" te hace un barrido de piernas.\n" }),
         ]);
}

*/

mixed query_damage_dice(){ return ({ ovr_num, ovr_type }); }

void set_damage_dice(int numdie, int dietype)
{
  if ((numdie > 0) && (dietype > 0))  
  {
    ovr_num = numdie;
    ovr_type = dietype;
  }
  return;
}

// stats añadido
mixed stats() 
{
  mixed * ret;
  ret = ({ 
           ({"Known Unarmed Styles", known_unarmed_styles, }),
           ({"Current Unarmed Style", current_unarmed_style, }),
           ({"Num Unarmed Hits", num_unarmed_hits, }),
           ({"Messages", messages, }),
       });
  return ret;
}
