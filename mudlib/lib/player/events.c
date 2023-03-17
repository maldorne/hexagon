/*
 *  old /global/events.c, neverbot 05/19
 *
 *  some changes and improvements for ccmud, neverbot 10/04
 */

#include <common/properties.h>
#include <basic/communicate.h>
#include <user/hud.h>
#include <user/user.h>

#include <language.h>
#include <translations/language.h>

inherit events "/lib/core/basic/events.c";

void create()
{
  events::create();
}

void event_enter(object ob, varargs string msg, object from, mixed avoid)
{
  ::event_enter(ob, msg, from, avoid);

  if (undefinedp(msg) || !strlen(msg))
    return;

  // items added to our inventory also trigger event_enter
  if (environment(ob) == this_object())
    return;

  if (avoid)
  {
    if (pointerp(avoid) && (member_array(this_object(), avoid) != -1))
      return;
    else if (objectp(avoid) && (avoid == this_object()))
      return;
  }

  if (living(ob) && environment(this_object()))
  {
    if (this_object()->check_dark(environment(this_object())->query_light()) == 0)
    {
      string color, new_name;
      
      // change player appereance from /table/hud_table.c, neverbot 10/04
      color = (string)HUD->query_color(this_object(), ob);
      new_name = (string)HUD->query_name_changed(this_object(), ob);

      if (strlen(color))
        msg = replace_string(msg, ob->query_cap_name(), 
          color + new_name + "%^RESET%^");
      else
        msg = replace_string(msg, ob->query_cap_name(), new_name);

      tell_object(this_object(), msg);
    }
    else
      tell_object(this_object(), _LANG_EVENTS_SOMEBODY_ARRIVES);
  }
}

void event_exit(object ob, varargs string msg, object dest, mixed avoid)
{
  ::event_exit(ob, msg, dest, avoid);

  if (undefinedp(msg) || !strlen(msg))
    return;

  // items added to our inventory also trigger event_enter
  if (environment(ob) == this_object())
    return;

  if (avoid)
  {
    if (pointerp(avoid) && (member_array(this_object(), avoid) != -1))
      return;
    else if (objectp(avoid) && (avoid == this_object()))
      return;
  }

  if (living(ob) && environment(this_object()))
  {
    if (this_object()->check_dark(environment(this_object())->query_light()) == 0)
    {
      string color, new_name;
      
      // change player appereance from /table/hud_table.c, neverbot 10/04
      color = (string)HUD->query_color(this_object(), ob);
      new_name = (string)HUD->query_name_changed(this_object(), ob);

      if (strlen(color))
        msg = replace_string(msg, ob->query_cap_name(), 
          color + new_name + "%^RESET%^");
      else
        msg = replace_string(msg, ob->query_cap_name(), new_name);

      tell_object(this_object(), msg);
    }
    else
      tell_object(this_object(), _LANG_EVENTS_SOMEBODY_LEAVES);
  }
}

// void event_say(object caller, string str, mixed avoid)
// {
//   if (pointerp(avoid)) {
//     if (member_array(this_object(), avoid) != -1)
//     return ;
//   } else if (avoid == this_object())
//   return ;

//   // sprintf añadido, neverbot 7/03
//   // A ver si asi los mensajes no se salen del ancho de pantalla!!
//   str = fix_string(str);
//   // str = sprintf("%*-=s", (this_object()->query_cols()?this_object()->query_cols():79), str);

//   // write(str);
//   write(str);

//   // write(fix_string(str));
// } /* event_say() */

// void event_write(object caller, string str)
// {
//   if (!str || str == "")
//     return;

//   // sprintf añadido, neverbot 7/03
//   // A ver si asi los mensajes no se salen del ancho de pantalla!!
//   str = fix_string(str);
//   // str = sprintf("%*-=s", (this_object()->query_cols()?this_object()->query_cols():79), str);
//   write(str);

//   // write(fix_string(str));
// } /* event_write() */

// void do_efun_write(string str)
// {
//   write(fix_string(str));
// } /* do_efun_write() */

// void event_soul(object ob, string str, mixed avoid)
// {
//   if (ob != this_object())
//   event_say(ob, str, avoid);
//   else
//   do_efun_write(str);
// } /* event_soul() */


void event_person_say(object ob, string start, string msg, string lang)
{
  string tmp;

  if (!interactive(this_object()))
    return;

  if (ob == this_object())
    return;

  msg = fix_string(msg);

  // if the listener does not speak the language
  if (member_array(lang, this_object()->query_languages()) == -1)
  {
    mixed str;

    if ((str = (mixed)handler("languages")->query_garble_object(lang)))
      if ((str = (mixed)str->garble(start, msg)))
      {
        start = str[0] + ": ";
        msg = str[1];
      }
      else
        return;
    else
      return;
  } 
  else
  {
    if (lang != STD_LANG)
      start += " " + _LANG_PREPOSITION + " " + lang + ": ";
    else
      start += ": ";
  }

  tmp = start + msg;

  if (ob && interactive(ob))
    this_object()->add_past_g(tmp);

  if (interactive(this_object()))
    this_object()->catch_tell("\n" + tmp + "\n");
}


// void event_person_tell(object ob, string start, string mess, string lang)
// {
//   int id;
//   mixed str;
//   string tmp;

//   if (ob && !ob->query_coder() && this_object()->query_blocking(ob->query_name()))
//   {
//     tell_object(this_player(), "Esta persona está bloqueando tus mensajes.\n");
//     return;
//   }

//   mess = fix_string(mess);

//   if (member_array(lang, this_object()->query_languages()) == -1) {

//     if (str = (mixed)LANGUAGE_HAND->query_garble_object(lang))
//     if (str = (mixed)str->garble_say(start, mess)) {
//       start = str[0];
//       mess = str[1];
//     } else
//     return ;
//     else
//     return ;
//   }
//   // else
//   if (lang != "comun")
//   start = start[0..strlen(start)-3]+" en "+lang+": ";

//   /* The following block of code was added to tell players that the target
//   is idle and may not reply for a period of time.
//   Firestorm 9/3/93
//   */

//   if (interactive(this_object()) && (id = this_user()->query_idle(this_object()))
//   > TELL_WARN_TIME)
//   /* tell warn time is defined in player.h in case you wondered -- FS */
//   {
//     str = ({  });
//     /* Need to use some neato formulas to convert to standard time -- FS */
//     // This needs cleaned up, a simul_efun does this... have to come back
//     // Radix

//     if (id/(60*60))
//       str += ({   (id/(60*60))+ " horas"  });
//     if ((id/60)%60)
//       str += ({   ((id/60)%60) + " minutos"   });
//     if (id%60)
//       str += ({   (id%60) + " segundos"   });
//     tell_object(this_player(), this_object()->query_cap_name() + " ha estado inactiv"+
//       this_object()->query_vowel()+" durante "+
//       query_multiple_short(str) + ".\n");
//   }

//   if (this_object()->query_in_editor())
//   {
//     tell_object(this_player(), this_object()->query_cap_name() + " está editando un fichero "+
//     "y puede tardar un poco en contestar.\n");
//   }

//   if (this_object()->query_static_property(AWAY_PROP))
//     {
//     tell_object(this_player(), this_object()->query_cap_name()+" está ocupad"+this_object()->query_vowel()+
//       ", razón: \"%^RED%^"+
//       (string)this_object()->query_static_property(AWAY_PROP)+"%^RESET%^\"\n");
//   }

//   tmp = start + mess;
//   this_object()->add_past_g( tmp );

//   tmp += "\n";

//   // sprintf añadido, neverbot 7/03
//   // A ver si asi los mensajes no se salen del ancho de pantalla!!
//   // tmp = sprintf("%*-=s", (this_object()->query_cols()?this_object()->query_cols():79), tmp);

//   write(tmp);

//   // write(fix_string(start + mess +"\n"));

// } /* event_person_tell() */

// void event_whisper(object ob, string start, string mess, object *obs,
// string lang)
// {
//   string oblue;
//   mixed str;
//   string tmp;
//   tmp = "";

//   mess = fix_string(mess);

//   oblue = "";
//   if (member_array(lang, this_object()->query_languages()) == -1)
//   {
//     if (str = (mixed)LANGUAGE_HAND->query_garble_object(lang))
//     {
//       // if (str = (mixed)str->garble_whisper(start, mess))
//       if (str = (mixed)str->garble_say(start, mess))
//       {
//         start = str[0];
//         mess = str[1];
//         // oblue = "en " + lang + ": ";
//       }
//       else
//       return ;
//     }
//     else
//     return ;
//   }
//   // else {
//   if (lang != "comun")
//     oblue = " en "+lang+": ";
//   else
//     // if (!stringp(oblue))
//     oblue = ": ";
//   // }

//   if (member_array(this_object(), obs) == -1)
//   {
//     tmp = fix_string(start + query_multiple_short(obs) +
//     oblue[0..strlen(oblue)-3] + ".");
//   }
//   else if (sizeof(obs) == 1)
//   {
//     tmp = fix_string(start +  "ti" + oblue + mess);
//     this_object()->add_past_g( tmp );
//   }
//   else
//   {
//     tmp = fix_string(start + "ti y a " +
//     query_multiple_short(obs - ({this_object()})) +
//     oblue + mess);
//     this_object()->add_past_g( tmp );
//   }

//   tmp += "\n";

//   // neverbot
//   // tmp = sprintf("%*-=s", (this_object()->query_cols()?this_object()->query_cols():79), tmp);

//   write(tmp);

// } /* event_whisper() */

// void event_person_shout(object ob, string start, string mess, string lang)
// {
//   string tmp;

//   if (ob == this_object()) return ;
//   if (this_user()->query_earmuffs() && (string)ob->query_verb() != "cre!" &&
//   (string)ob->query_verb() != "shout!"){ return; }

//   mess = fix_string(mess);

//   if (member_array(lang, this_object()->query_languages()) == -1) {
//     mixed str;

//     if (str = (mixed)LANGUAGE_HAND->query_garble_object(lang))
//     if (str = (mixed)str->garble_say(start, mess)) {
//       start = str[0];
//       mess = str[1];
//     } else
//     return ;
//     else
//     return ;
//   }
//   // else

//   if (lang != "comun")
//   start = start[0..strlen(start)-3]+" en "+lang+": ";

//   // sprintf añadido, neverbot 7/03
//   // A ver si asi los mensajes no se salen del ancho de pantalla!!
//   tmp = start + mess;

//   // Solo parseamos si el grito no tenia colores!
//   // tmp = sprintf("%*-=s", (this_object()->query_cols()?this_object()->query_cols():79), tmp);

//   this_object()->add_past_g( tmp );
//   tmp += "\n";

//   write(tmp);

//   // write(fix_string(
//   //    start+mess));
// } /* event_person_shout() */

// void event_creator_tell(object ob, string start, string mess)
// {
//   string tmp;
//   if (ob == this_object() || (this_user()->query_earmuffs() && query_verb() != "cre!") ||
//   !this_object()->query_coder())
//   return;

//   // sprintf añadido, neverbot 7/03
//   // A ver si asi los mensajes no se salen del ancho de pantalla!!
//   tmp = fix_string(start + mess + "\n");
//   tmp = sprintf("%*-=s", (this_object()->query_cols()?this_object()->query_cols():79), tmp);

//   write(tmp);

//   // write(fix_string(
//   //   start+mess));
// } /* event_creator_tell() */

// nomask void event_god_inform(object ob, string start, string mess)
// {
//   write(fix_string(sprintf("%s%-=*s\n",
//   start, cols - strlen(start), mess)));
// }

// void event_inter_creator_tell(object ob, string mname, string pname,
// string mess, object ig, int emote) {
//   if (!this_object()->query_coder()
//   || this_user()->check_earmuffs("inter-creator-tell")
//   || this_user()->query_earmuffs()
//   || this_object() == ig)
//   return ;
//   write(fix_string(sprintf("%s@%s%s%-=*s\n", pname, mname,(emote?" ":": ")+mess,
//   this_user()->query_cols()-strlen(mname)-strlen(pname)-3)));
// } /* event_inter_creator_tell() */

// void event_player_echo_to(object ob, string mess) {
//   if (this_object()->query_admin())
//   write(ob->query_cap_name()+" echo to's:\n");
//   write(fix_string(mess));
// } /* event_player_echo_to() */

// void event_player_emote_all(object ob, string mess)
// {
//   if (ob == this_object())
//   return;
//   if (this_object()->query_admin())
//   write(ob->query_cap_name()+" emote all's:\n");
//   write(fix_string(mess));
// } /* event_player_echoall() */

// void event_player_echo(object ob, string mess)
// {
//   if (ob == this_object())
//   return;
//   if (this_object()->query_admin())
//   write(ob->query_cap_name()+" echo's:\n");
//   write(fix_string(mess));
// } /* event_player_echo() */

/*
int query_see_magic(){
string guild;

if (!(guild = (string)this_object()->query_guild_ob())) return 0;
return (int)guild->query_see_magic();
}*/


mixed stats()
{
  return ({
          }) + events::stats();
}
