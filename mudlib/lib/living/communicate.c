
#include <basic/communicate.h>
#include <living/food.h>
#include <user/user.h>
#include <common/properties.h>
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
  string * verbs;
  int i;

  add_private_action("do_say",       _LANG_SAY_VERBS);
  add_private_action("do_tell",      _LANG_TELL_VERBS);
  add_private_action("do_whisper",   _LANG_WHISPER_VERBS);
  // the speak/hablar verb moved to the command /lib/cmds/player/speak.c,
  // which delegates to set_language (still defined below and used internally
  // by monster setup and social race assignment)
  add_private_action("do_shout",     _LANG_SHOUT_VERBS);

  add_private_action("do_emote",     _LANG_EMOTE_VERBS);

  // the words the channels answer to belong to the channel handler, which
  // is where they are translated
  verbs = (string *)CHAT_HANDLER->query_channel_verbs();

  for (i = 0; i < sizeof(verbs); i++)
    add_private_action("do_channels", verbs[i]);
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
      return _LANG_COMM_EXCLAIMING;
    case '?' : 
      return _LANG_COMM_ASKING;
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
      return "";
    case '?' : 
      return _LANG_COMM_ASKING;
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

void their_mess(object ob, string start, string msg, string lang)
{
  int id;
  mixed str;
  string tmp;

  // TODO blocks
  // if (ob && !ob->query_coder() && ob->query_name() == block)
  // {
  //   tell_object(this_object(), "Esta persona está bloqueando tus mensajes.\n");
  //   return;
  // }

  // do the fix_string() here so that we can garble the 
  // string after processing colors, etc
  msg = fix_string(msg);

  if (member_array(lang, ob->query_languages()) == -1)
  {
    mixed str;

    if ((str = (mixed)handler("languages")->query_garble_object(lang)))
      if ((str = (mixed)str->garble(start, msg)))
      {
        start = str[0];
        msg = str[1];
      }
      else
        return;
    else
      return;
  }

  // The following block of code was added to tell players that the target
  //  is idle and may not reply for a period of time.
  //  Firestorm 9/3/93
  if (interactive(ob) && ob->query_idle() &&
     (id = ob->query_idle()) > TELL_WARN_TIME)
  {
    tell_object(this_object(), ob->query_cap_name() + " ha estado inactiv"+
      ob->query_vowel()+" durante "+
      seconds_to_time_string(id) + ".\n");
  }

  if (ob->query_in_editor())
  {
    tell_object(this_object(), ob->query_cap_name() + " está editando un fichero "+
      "y puede tardar un poco en contestar.\n");
  }

  if (ob->query_static_property(AWAY_PROP))
    tell_object(this_object(), ob->query_cap_name() + _LANG_COMM_AWAY_MSG);

  tmp = start + msg;

  // if (ob && interactive(ob))
  ob->add_past_g(tmp);

  tell_object(ob, "\n" + tmp + "\n");
}

int do_say(string arg, varargs int no_echo) 
{
  string * words, word, name;

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
  
  if (this_object()->query_intoxication())
    arg = drunk_speech(arg);

  event(environment(this_object()), "person_say", this_object()->query_cap_name()+
      " " + words[1], arg, cur_lang);

  if (!no_echo) 
  {
    if (cur_lang != STD_LANG)
    {
      name = (string)handler("languages")->query_language_display(cur_lang);
      word = words[0] + " " + _LANG_PREPOSITION + " " + name + ": ";
    }
    else
      word = words[0] + ": ";

    my_mess(capitalize(word), arg);
  }

  return 1;
} 

int do_tell(string arg, varargs object ob, int silent) 
{
  string str, rest, name;
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

  // the one speaking, not whoever typed: an NPC can tell somebody too
  if (ob == this_object()) 
  {
    notify_fail(_LANG_TELL_YOURSELF);
    return 0;
  }

  if (ob->query_player() && !interactive(ob))
  {
    notify_fail(_LANG_TELL_NO_CONNECTION);
    return 0;
  }

  // the list of blocked players is kept by the user of whoever is told
  if (ob->user() && ob->user()->query_blocking(this_object()->query_name()))
  {
    notify_fail(_LANG_COMMS_BLOCKED);
    return 0;
  }
    
  if (adjust_social_points(-TELL_COST) < 0)
  {
    notify_fail(_LANG_COMMS_NO_POWER);
    return 0;
  }
  
  words = query_word_type(rest);
  name = (string)handler("languages")->query_language_display(cur_lang);
  
  if (this_object()->query_intoxication())
    arg = drunk_speech(arg);
  
  if (words[2] != _LANG_COMM_ASKING) 
  {
    their_mess(ob, this_object()->query_cap_name() + " " +
              _LANG_TELL_MSG_THEM + 
              (words[2] == _LANG_COMM_EXCLAIMING ? " " + words[2] : "") +
               ": ", rest, cur_lang);

    if (!silent)
      my_mess(_LANG_TELL_MSG_ME + " " +  ob->query_cap_name() + 
              (words[2] == _LANG_COMM_EXCLAIMING ? " " + words[2] : "") +
              (cur_lang != STD_LANG ? " " + _LANG_IN + " " + name : "") +
               ": ", rest);
  } 
  else 
  {
    their_mess(ob, this_object()->query_cap_name() + " " +
              _LANG_TELL_MSG_ASK_THEM + ": ", rest, cur_lang);

    if (!silent)
      my_mess(_LANG_TELL_MSG_ASK_ME + " " + ob->query_cap_name() + 
              (cur_lang != STD_LANG ? " " + _LANG_IN + " " + name : "") +
              ": ", rest);
  }
  
  return 1;
} 

int do_whisper(string str) 
{
  object *obs, ob;
  string s, s2, *bits;
  int i;

  if (!strlen(str))
  {
    notify_fail(_LANG_WHISPER_SYNTAX);
    return 0;
  }

  // neverbot
  if (!cur_lang || (cur_lang == ""))
  {
    notify_fail(_LANG_WHISPER_NO_LANGUAGE);
    return 0;
  }

  if (!handler("languages")->query_language_spoken(cur_lang)) 
  {
    notify_fail(_LANG_WHISPER_LANG_NOT_SPOKEN);
    return 0;
  }
  
  sscanf(str, _LANG_WHISPER_PREPOSITION, str);
  bits = explode(str, " ");
  obs = ({ });
  
  for (i = 0; i < sizeof(bits); i++)
    if (sizeof(obs = find_match(implode(bits[0..i], " "), environment())))
      break;
  
  if (i < sizeof(bits) - 1)
    s2 = implode(bits[i + 1..], " ");
  else
  {
    notify_fail(_LANG_WHISPER_SYNTAX);
    return 0;
  }

  if (sizeof(obs) == 1 && obs[0] == this_object()) 
  {
    say(_LANG_WHISPER_TO_YOURSELF_ROOM);
    write(_LANG_WHISPER_TO_YOURSELF_ME);
    return 1;
  }
  
  obs = obs - ({ this_object() });
  
  for (i = 0; i < sizeof(obs); i++)
    if (!living(obs[i]))
      obs = delete(obs, i--, 1);
  
  if (!sizeof(obs)) 
  {
    notify_fail(_LANG_WHISPER_NOBODY);
    return 0;
  }

  // nobody who blocks the whisperer is whispered to
  for (i = 0; i < sizeof(obs); i++)
    if (obs[i]->user() &&
        obs[i]->user()->query_blocking(this_object()->query_name()))
    {
      ob = obs[i];
      write(_LANG_COMMS_BLOCKED);
      obs = delete(obs, i--, 1);
    }

  if (!sizeof(obs))
    return 1;
  
  if (this_object()->query_intoxication())
    s2 = drunk_speech(s2);

  s = query_whisper_word_type(s2);
  s2 += "%^RESET%^";

  event(environment(), "person_whisper", _LANG_WHISPER_THEM, s2, obs, cur_lang);

  my_mess(_LANG_WHISPER_ME, s2);

  // this_player()->adjust_time_left(-5);
  return 1;
}

// A free-form action: whatever is typed is shown after your name. Anybody
// can write anything with it, so it stays in the hands of the staff.
int do_emote(string arg) 
{
  string str;

  if (!this_object()->query_coder())
  {
    notify_fail(_LANG_EMOTE_NOT_ALLOWED);
    return 0;
  }
 
  if (!strlen(arg))
    arg = "";

  if (arg == "" || arg == " ") 
  {
    notify_fail(_LANG_EMOTE_SYNTAX);
    return 0;
  }

  if (adjust_social_points(-EMOTE_COST) < 0) 
  {
    notify_fail(_LANG_COMMS_NO_POWER);
    return 0;
  }
  
  log_file("echoes", this_object()->query_cap_name() + " emotes: " +
           arg + " [" + ctime(time(), 4) + "]\n");

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
  string s1, s, name;

  if (!strlen(str)) 
  {
    notify_fail(_LANG_SHOUT_SYNTAX);
    return 0;
  }

  if (this_object()->query_property(NOSHOUT_LOCK)) 
  {
    notify_fail(_LANG_SHOUT_LOCKED);
    return 0;
  }

  /*
  if (this_object()->query_level() < 6) {
  notify_fail("Cuando crezcas un poco tal vez puedas.\n");
  return 0;
  }
  */

  // the earmuffs are kept by the user
  if (this_object()->user() && this_object()->user()->query_earmuffs()) 
  {
    notify_fail(_LANG_SHOUT_EARMUFFS);
    return 0;
  }

  // neverbot
  if (!cur_lang || (cur_lang == ""))
  {
    notify_fail(_LANG_SHOUT_NO_LANGUAGE);
    return 0;
  }

  if (!handler("languages")->query_language_spoken(cur_lang)) 
  {
    notify_fail(_LANG_SHOUT_LANG_NOT_SPOKEN);
    return 0;
  }
  
  if (!handler("languages")->query_language_distance(cur_lang)) 
  {
    notify_fail(_LANG_SHOUT_LANG_NOT_DISTANCE);
    return 0;
  }
  
  if (adjust_social_points(-SHOUT_COST*((strlen(str)/10)+1)) < 0) 
  {    
    notify_fail(_LANG_COMMS_NO_POWER);
    return 0;
  }

  s1 = query_shout_word_type(str);
  //  if (s1 != " exclamando")
  s = _LANG_SHOUT_WORD;
  // else
  //  s = s1+"s";

  str = PROFANITY_HANDLER->clean_language(str);

  if (this_object()->query_intoxication())
    str = drunk_speech(str);
    
  log_file(SHOUT_LOGFILE, "[" + ctime(time(), 4) + " - " + 
          this_object()->query_cap_name() + "] (" + str + ")\n");

  // every living hears it: the event asks the listener which languages it
  // knows, and that is a living's answer to give
  event(livings(), "person_shout", this_object()->query_cap_name()+
     " "+s+": ", str, cur_lang);

  // if (s1 != " exclamando") {
  if (cur_lang != STD_LANG) 
  {
    name = (string)handler("languages")->query_language_display(cur_lang);
    s1 += _LANG_SHOUT_IN_LANGUAGE;
  }

  my_mess(_LANG_SHOUT_ME, str);
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
  string id;

  // Resolve to the canonical lowercase-English id, so a translated name or
  // an alias, stored on a character or passed in, is normalised to the id.
  // Unknown => not a language.
  id = handler("languages")->resolve_language(lang);
  if (!id)
    return;
  lang = id;

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
// `quiet` drops the messages: a race change hands one set of tongues over for
// another, and the body is not forgetting anything.
void remove_language(string lang, varargs int quiet) 
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
      if (!quiet && this_object()->query_level() >= 1)
        tell_object(this_object(), _LANG_SPEAK_FORGOT_ALL);
    }
    else
    {
      string name;
      cur_lang = languages[0];
      name = handler("languages")->query_language_display(cur_lang);
      if (!quiet)
        tell_object(this_object(), _LANG_SPEAK_FORGOT_SWITCH);
    }
  }
} /* remove_language() */

// neverbot 10/03
void remove_languages(string *list, varargs int quiet)
{
  int i;
  for (i = 0; i < sizeof(list); i++)
    remove_language(list[i], quiet);
}

// Eliminado el grunt, neverbot 12/10/03
// Pure setter: resolve the input (id, alias or display name) to a language id
// and, if this living knows it, make it the current language. Returns 1 on
// success, 0 when the input is empty, not a language, or one this living does
// not know. All user-facing messages (the no-argument listing, the "you do
// not know that language" error, the confirmation) live in the speak command
// /lib/cmds/player/speak.c; internal callers (monster setup, social race
// assignment) only want the assignment, with no output.
int set_language(string str)
{
  string id;

  if (!str || !strlen(str))
    return 0;

  id = handler("languages")->resolve_language(str);
  if (!id || member_array(id, languages) == -1)
    return 0;

  cur_lang = id;
  return 1;
}

string query_current_language() { return cur_lang; }

// The language spoken, by the name the player reads it under.
string query_current_language_display()
{
  if (!strlen(cur_lang))
    return "";

  return handler("languages")->query_language_display(cur_lang);
}
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

mixed * stats() 
{
  return ({ 
          ({"Social Points", social_points, }),
          ({"Max Social Points", max_social_points, }),
          ({"Languages", languages, }),
          ({"Cur Lang", cur_lang, }),
          });
}
