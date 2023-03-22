
#include <basic/communicate.h>
#include <living/food.h>

#include <translations/language.h>
#include <translations/common.h>
#include <language.h>

// #include <player.h>
// #include <chat.h>
// #include <inet.h>
// #include <network.h>

int social_points, max_social_points;
    string *languages,
    cur_lang;

static int social_hb_num; // heart_beat counter

string drunk_speech(string str);

void communicate_commands() 
{
  add_private_action("do_say", _LANG_SAY_VERBS);
  add_private_action("do_tell", _LANG_TELL_VERBS);
  add_private_action("do_whisper", _LANG_WHISPER_VERBS);
  add_private_action("set_language", _LANG_SPEAK_VERBS);
  add_private_action("do_shout", _LANG_SHOUT_VERBS);

  // deactivated for a while
  // add_private_action("do_emote", ({ "emote", "emocion"}));

  add_private_action("do_channels", "emergencia");
  add_private_action("do_channels", "gremio");
  add_private_action("do_channels", "clan");
  add_private_action("do_channels", "aventurero");
  add_private_action("do_channels", "raza");
} 

// neverbot 6/03
void create()
{
  social_points = 0;
  max_social_points = 0;
  languages = ({ });
  cur_lang = "";
  social_hb_num = 0;
}

void heart_beat()
{
  if (++social_hb_num%8)  // How about half as fast?
  {
    social_points++;
    if(social_points > max_social_points)
        social_points = max_social_points;

    if (social_hb_num > 500) 
    {
      if(max_social_points < 1000)
          max_social_points++;
      social_hb_num = 0;
    }
  }  
}

void set_max_social_points(int num) 
{
  max_social_points = num;
} 

int query_max_social_points() 
{
  return max_social_points;
} 

void set_social_points(int num) 
{
  social_points = num;
}

int query_social_points() 
{
  return social_points;
} 

int adjust_social_points(int num)
{
  int temp;

  temp = social_points + num;
  if (temp >= 0) 
  {
    social_points = temp;
    if (social_points > max_social_points)
      social_points = max_social_points;
    return 1;
  }
  return -1;
}

/*
int hard_adjust_social_points(int num) {
  social_points += num;
} 
*/

string * query_word_type(string str)
{
  int i;

  for (i = strlen(str) - 1; str[i] == ' '; i--);

  switch (str[i]) 
  {
    case '!' : 
      return ({ _LANG_COMM_I_EXCLAIM, _LANG_COMM_THEY_EXCLAIM, _LANG_COMM_EXCLAIMING });
    case '?' : 
      return ({ _LANG_COMM_I_ASK, _LANG_COMM_THEY_ASK, _LANG_COMM_ASKING });
    default:   
      return ({ _LANG_COMM_I_SAY, _LANG_COMM_THEY_SAY, _LANG_COMM_SAYING });
  }
} 

string query_shout_word_type(string str) 
{
  int i;

  for (i = strlen(str)-1; str[i] == ' '; i--);

  switch (str[i]) 
  {
    case '!' : 
      return " exclamando";
    case '?' : 
      return " preguntando";
    default:   
      return "";
  }
} 

string query_whisper_word_type(string str) 
{
  int i;

  for (i = strlen(str)-1; str[i] == ' '; i--);
  switch (str[i]) 
  {
    case '!' : 
      return " apresuradamente";
    case '?' : 
      return " dubitativo";
    default:   
      return "";
  }
}

/* to properly columnate word_typed things */
void my_mess(string fish, string erk)
{
  string tmp;

  if (!interactive(this_object())) 
    return;

  tell_object(this_object(), fish + erk + "\n");
}

int do_say(string arg, varargs int no_echo) 
{
  string * words, word;

  // Taniwha, sanity/ no debug errors
  if (!environment(this_object()))
  {
    notify_fail(_LANG_COMMS_NO_ENV);
    return 0;
  }

  if (!strlen(arg) || arg == " ")
  {
    notify_fail(_LANG_COMMS_SYNTAX);
    return 0;
  }
   
  // neverbot
  if (!strlen(cur_lang))
  {
    notify_fail(_LANG_COMMS_NO_CURRENT_LANG);
    return 0;
  }
   
  if (!handler("languages")->query_language_spoken(cur_lang)) 
  {
    notify_fail(capitalize(cur_lang) + " " + _LANG_COMMS_NO_SPOKEN_LANG);
    return 0;
  }

  words = query_word_type(arg);
  
  if (this_object()->query_volume(D_ALCOHOL))
    arg = drunk_speech(arg);

  event(environment(this_object()), "person_say", this_object()->query_cap_name()+
      " " + words[1], arg, cur_lang);

  if (!no_echo) 
  {
    if (cur_lang != STD_LANG)
      word = words[0] + " " + _LANG_PREPOSITION + " " + cur_lang + ": ";
    else
      word = words[0] + ": ";

    my_mess(capitalize(word), arg);
  }

  return 1;
} 

int do_tell(string arg, varargs object ob, int silent) 
{
  string str, rest;
  string * words;
  // string person, mud;

  if (!strlen(arg) && !ob) 
  {
    notify_fail(_LANG_TELL_SYNTAX);
    return 0;
  }

  if (!ob) 
  {
    if (sscanf(arg, "%s %s", str, rest) != 2) 
    {
      notify_fail(_LANG_TELL_SYNTAX);
      return 0;
    }
  } 
  else
    rest = arg;

  // neverbot
  if (!strlen(cur_lang))
  {
    notify_fail(_LANG_TELL_NO_LANGUAGE);
    return 0;
  }

  if (!handler("languages")->query_language_spoken(cur_lang)) 
  {
    notify_fail(capitalize(cur_lang) + _LANG_TELL_LANG_NOT_SPOKEN);
    return 0;
  }

  if (!handler("languages")->query_language_distance(cur_lang)) 
  {
    notify_fail(capitalize(cur_lang) + _LANG_TELL_LANG_NOT_DISTANCE);
    return 0;
  }

  if (!ob) 
  {
    str = lower_case(str);
    str = (string)this_object()->expand_nickname(str);
    ob = find_player(str);
    
    if (!ob)
      ob = find_living(str);
    
    if (!ob) 
    {
      // Might be intermud...
      // if (this_object()->query_coder() && sscanf(str, "%s@%s", person, mud) == 2) 
      // {
      //        SERVICES_D->eventSendTell(person, mud, rest);
      //        return 1;
      // }
      notify_fail(_LANG_TELL_CANNOT_FIND);
      return 0;
    }
  }

  if (ob == this_player()) 
  {
    notify_fail(_LANG_TELL_YOURSELF);
    return 0;
  }

  if (ob->query_player() && !interactive(ob))
  {
    notify_fail(_LANG_TELL_NO_CONNECTION);
    return 0;
  }
    
  if (adjust_social_points(-TELL_COST) < 0)
  {
    notify_fail(NO_POWER);
    return 0;
  }
  
  words = query_word_type(rest);
  
  if (this_object()->query_volume(D_ALCOHOL))
    arg = drunk_speech(arg);
  
  if (words[2] != _LANG_COMM_ASKING) 
  {
    ob->event_person_tell(this_object(), this_object()->query_cap_name() + " " +
              _LANG_TELL_MSG_THEM + 
              (words[2] == _LANG_COMM_EXCLAIMING ? " " + words[2] : "") +
               ": ", rest, cur_lang);

    if (!silent)
      my_mess(_LANG_TELL_MSG_ME + " " +  ob->query_cap_name() + 
              (words[2] == _LANG_COMM_EXCLAIMING ? " " + words[2] : "") +
              (cur_lang != STD_LANG ? " " + _LANG_IN + " " + cur_lang : "") +
               ": ", rest);
  } 
  else 
  {
    ob->event_person_tell(this_object(), this_object()->query_cap_name() + " " +
              _LANG_TELL_MSG_ASK_THEM + ": ", rest, cur_lang);

    if (!silent)
      my_mess(_LANG_TELL_MSG_ASK_ME + " " + ob->query_cap_name() + 
              (cur_lang != STD_LANG ? " " + _LANG_IN + " " + cur_lang : "") +
              ": ", rest);
  }
  
  return 1;
} 

int do_whisper(string str) 
{
  object *obs;
  string s, s2, *bits;
  int i;

  if (!strlen(str))
  {
    notify_fail("Sintaxis: susurrar [a] <persona> <texto>\n");
    return 0;
  }

  // neverbot
  if (!cur_lang || (cur_lang == ""))
  {
    notify_fail("Debes seleccionar un idioma para hablar.\n");
    return 0;
  }

  if (!handler("languages")->query_language_spoken(cur_lang)) 
  {
    notify_fail(capitalize(cur_lang)+" no es un lenguaje hablado.\n");
    return 0;
  }
  
  sscanf(str, "a %s", str);
  bits = explode(str, " ");
  obs = ({ });
  
  for (i = 0; i < sizeof(bits); i++)
    if (sizeof(obs = find_match(implode(bits[0..i], " "), environment())))
      break;
  
  if (i < sizeof(bits) - 1)
    s2 = implode(bits[i + 1..], " ");
  else
  {
    notify_fail("Sintaxis: susurrar [a] <persona> <texto>\n");
    return 0;
  }

  if (sizeof(obs) == 1 && obs[0] == this_player()) 
  {
    say(this_player()->query_cap_name()+" se susurra a si mismo.\n");
    write("¿¡Susurrándote a ti mismo!?\n");
    return 1;
  }
  
  obs = obs - ({ this_player() });
  
  for (i = 0; i < sizeof(obs); i++)
    if (!living(obs[i]))
      obs = delete(obs, i--, 1);
  
  if (!sizeof(obs)) 
  {
    notify_fail("No hay nadie con ese nombre a quien susurrar.\n");
    return 0;
  }
  
  if (this_object()->query_volume(D_ALCOHOL))
    s2 = drunk_speech(s2);

  s = query_whisper_word_type(s2);
  s2 += "%^RESET%^";

  event(environment(), "person_whisper", this_object()->query_cap_name() +
      " susurra" + s, s2, obs, cur_lang);

  my_mess("Susurras" + s + " a " + query_multiple_short(obs) + ": ", s2);

  // this_player()->adjust_time_left(-5);
  return 1;
}

int do_emote(string arg) 
{
  string str;

  if (!this_object()->query_coder() && !this_player()->query_property("emote"))
  {
    notify_fail(NOT_ALLOWED);
    return 0;
  }
 
  if (!strlen(arg))
    arg = "";

  str = query_verb();

  if (arg == "" || arg == " ") 
  {
    notify_fail("Sintaxis: emote <emocion a expresar>\n");
    return 0;
  }

  if (adjust_social_points(-EMOTE_COST) < 0) 
  {
    notify_fail(NO_POWER);
    return 0;
  }
  
  // this_player()->adjust_time_left(-5);
  str = (string)this_object()->query_cap_name()+" "+arg+"%^RESET%^\n";

  say(str);
  write(str);
  return 1;
} 

/* ok... the shout cost is charged for every 10 letters, plus a bonus
 * one for the start charge..  SO a shout of yes will cost 1 social
 * point... where as a shout of lots a letters will cost lots
 */
// Flode added 2-round lockout  -  211197
int do_shout(string str) 
{
  string s1, s;
  object* usrs;
  usrs = users();

  if (!strlen(str)) 
  {
    notify_fail("Sintaxis: gritar <texto>\n");
    return 0;
  }

  if (this_object()->query_property(NOSHOUT_LOCK)) 
  {
    notify_fail("Espera a coger un poco de aire antes de volver a gritar.\n");
    return 0;
  }

  /*
  if (this_object()->query_level() < 6) {
  notify_fail("Cuando crezcas un poco tal vez puedas.\n");
  return 0;
  }
  */

  if (this_object()->query_earmuffs()) 
  {
    notify_fail("¿Para qué gritar si no vas a oir cómo te contestan?\n");
    return 0;
  }

  // neverbot
  if (!cur_lang || (cur_lang == ""))
  {
    notify_fail("Debes seleccionar un idioma para gritar.\n");
    return 0;
  }

  if (!handler("languages")->query_language_spoken(cur_lang)) 
  {
    notify_fail(capitalize(cur_lang)+" no es un idioma hablado.\n");
    return 0;
  }
  
  if (!handler("languages")->query_language_distance(cur_lang)) 
  {
    notify_fail(capitalize(cur_lang)+" no se puede hablar a distancia.\n");
    return 0;
  }
  
  if (adjust_social_points(-SHOUT_COST*((strlen(str)/10)+1)) < 0) 
  {    
    notify_fail(NO_POWER);
    return 0;
  }

  s1 = query_shout_word_type(str);
  //  if (s1 != " exclamando")
  s = "grita" + s1;
  // else
  //  s = s1+"s";

  str = PROFANITY_HANDLER->clean_language(str);

  if (this_object()->query_volume(D_ALCOHOL))
    str = drunk_speech(str);
    
  log_file(SHOUT_LOGFILE, "[" + ctime(time(), 4) + " - " + 
          this_object()->query_cap_name() + "] (" + str + ")\n");

  event(usrs, "person_shout", this_object()->query_cap_name()+
     " "+s+": ", str, cur_lang);

  // if (s1 != " exclamando") {
  if (cur_lang != STD_LANG) 
    s1 += " en "+cur_lang;

  my_mess("Gritas" + s1 + ": ", str);
  /*
  } else {
  if (cur_lang != STD_LANG) 
  s1 += " en "+cur_lang;
  my_mess(s1 + ": ", str);
  }
  */

  /* ahora en el event_person_shout (events.c), neverbot
  for(int i = 0; i < sizeof(usrs);i++){
  if (usrs[i] != this_object()) // neverbot
  usrs[i]->add_past_g((string)this_object()->query_cap_name()+" gritó: "+str);
  }
  */
  // str = " "+lower_case(str);

  this_object()->add_timed_property(NOSHOUT_LOCK, 1, 2);
  return 1;
} 

int do_channels( string str )
{
  CHAT_HANDLER->do_chat( str );
  return 1;
}  

string drunk_speech(string str) 
{
  return replace(str, ({ "s", "sh", "r", "rr", "ing", "in'", "x", "xsh",
    "S", "SH", "R", "RR" }));
} 

// Flode, 080997. Hopefully my fix will make sure that people won't learn
// fifteen dwarfs or similar anymore
void add_language(string lang) 
{
  int i;

  if (!handler("languages")->language_exists(lang))
    return ;

  if (!languages)
    languages = ({ });

  i = member_array(lang, languages);

  if (i == -1)
  {
    if (sizeof(languages))
      languages += ({ lang });
    else
      languages = ({ lang });
  }
  else
  {
    // Si el idioma ya esta en la lista, no lo añadimos
    return;

    // while(i < sizeof(languages))
    //  if (languages[i] == lang)
    //    languages = delete(languages, i, 1);
    //  else
    //    ++i;
  }
} 

// neverbot 10/03
void add_languages(string *list)
{
  int i;
  for (i = 0; i < sizeof(list); i++)
    add_language(list[i]);
}

// Eliminado el grunt, neverbot 12/10/03
void remove_language(string lang) 
{
  int i;

  if ((i = member_array(lang, languages)) == -1)
    return ;
  languages = delete(languages, i, 1);
  if (lang == cur_lang) 
  {
    if (!sizeof(languages))
    {
      cur_lang = "";
      // Si estamos creando el personaje no damos mensajes
      if (this_object()->query_level() >= 1)
        tell_object(this_object(), "Has olvidado el idioma que estabas " +
           "hablando.\n");
    }
    else
    {
      cur_lang = languages[0];
      tell_object(this_object(), "Has olvidado el lenguaje que estabas " +
          "hablando. Ahora hablas en "+cur_lang+".\n");
    }
  }
} /* remove_language() */

// neverbot 10/03
void remove_languages(string *list)
{
  int i;
  for (i = 0; i < sizeof(list); i++)
    remove_language(list[i]);
}

// Eliminado el grunt, neverbot 12/10/03
int set_language(string str) 
{
  string res;
  int i;

  if (!strlen(str))
  {
    if (!strlen(cur_lang))
      res = "Debes seleccionar un idioma para hablar.\n";
    else
      res = "Ahora hablas en '" + cur_lang + "'.\n";

    if (sizeof(languages) > 0)
    {
      res += "Puedes hablar en los siguiente idiomas:\n";

      for(i = 0; i < sizeof(languages); i++)
      {
        res += "\t"+capitalize(languages[i])+"\n";
      }
    }
    else
    {
      res += "No conoces ningún idioma.\n";
    }

    notify_fail(res);
    return 0;
  }
  /*           
  if (!str) {
    notify_fail("Ahora hablas en "+cur_lang+" y puedes hablar en "
          +query_multiple_short(languages)+".\n");
    return 0;
  }
  */
  
  if (member_array(str, languages) == -1) 
  {
    notify_fail("No conoces el idioma '"+str+"'.\n");
    return 0;
  }

  cur_lang = str;

  // Si estamos creando la ficha no damos mensajes
  if (this_object()->query_level() >= 1)
    tell_object(this_object(),"Usando el idioma '"+str+"' para hablar y "+
      "escribir.\n");
  
  return 1;
} 

string query_current_language() { return cur_lang; }
string * query_languages() 
{
  if (!languages) 
    languages = ({ });
  return languages; 
}

// Needed to be here - Radix Jan 1996
int query_known_language(string lang)
{
  if (!lang) 
    return 0;
  if (member_array(lang,languages) != -1) 
    return 1;
  return 0;
}

mixed stats() 
{
  return ({ 
          ({"Social Points", social_points, }),
          ({"Max Social Points", max_social_points, }),
          ({"Languages", languages, }),
          ({"Cur Lang", cur_lang, }),
          });
}
