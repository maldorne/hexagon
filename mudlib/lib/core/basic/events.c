
#include <common/properties.h>
#include <basic/communicate.h>
#include <user/user.h>
#include <translations/language.h>
#include <language.h>

void create()
{
}

// all the posible events will be called from
// the event efun:
//   call_other( event_obs, "event_"+ event_name, who, arg... ) ;


// show general info to the object, this message is also sent to 
// every object in the same environment
// used from say() and also from tell_room()

void event_say(object caller, string msg, varargs mixed avoid)
{
  if (!interactive(this_object()))
    return;

  if (!strlen(msg))
    return;

  if (avoid)
  {
    if (pointerp(avoid) && (member_array(this_object(), avoid) != -1))
      return;
    else if (objectp(avoid) && (avoid == this_object()))
      return;
  }

  msg = fix_string(msg);

  this_object()->catch_tell(msg);
}

void event_inform(object caller, string msg, string type, varargs mixed avoid)
{
  if (!interactive(this_object()))
    return;

  if (!strlen(msg))
    return;

  if (avoid)
  {
    if (pointerp(avoid) && (member_array(this_object(), avoid) != -1))
      return;
    else if (objectp(avoid) && (avoid == this_object()))
      return;
  }

  if (this_object()->query_property(NO_INFORM) ||
     (caller->query_invis() && !this_object()->query_coder()) ||
     (caller->query_user() && caller->user()->query_invis() == 2 && !this_object()->query_admin()))
  {
    return;
  }

  if (!this_object()->user()->query_inform(type))
    return;

  msg = fix_string("\n[" + msg + "]\n\n");

  this_object()->catch_tell(msg);
}

void event_enter(object ob, varargs string msg, object from, mixed avoid)
{
  if (msg && !strlen(msg))
    return;

  if (avoid)
  {
    if (pointerp(avoid) && (member_array(this_object(), avoid) != -1))
      return;
    else if (objectp(avoid) && (avoid == this_object()))
      return;
  }

  stderr(" * event_enter " + object_name(this_object()) + "\n");
}

void event_exit(object ob, varargs string msg, object dest, mixed avoid)
{
  if (msg && !strlen(msg))
    return;

  if (avoid)
  {
    if (pointerp(avoid) && (member_array(this_object(), avoid) != -1))
      return;
    else if (objectp(avoid) && (avoid == this_object()))
      return;
  }

  stderr(" * event_exit " + object_name(this_object()) + "\n");
}

void event_login(object ob, varargs mixed avoid)
{
  if (!interactive(this_object()))
    return;

  if (avoid)
  {
    if (pointerp(avoid) && (member_array(this_object(), avoid) != -1))
      return;
    else if (objectp(avoid) && (avoid == this_object()))
      return;
  }

  stderr(" * event_login " + object_name(this_object()) + "\n");

  // this information is given in the event_inform, not needed here
  // this_object()->catch_tell("[" + ob->query_cap_name() + _LANG_EVENTS_ENTERS + 
  //                           game_pretty_name(this_object()) + "]\n");
}

void event_soul(object ob, string msg, varargs mixed avoid)
{
  if (ob != this_object())
    event_say(ob, msg, avoid);
  else
    this_object()->catch_tell(msg + "\n");
}

void event_person_say(object ob, string start, string msg, string lang)
{
  // nothing here, but the function exists for every object
}

void event_person_whisper(object ob, string start, string msg,
                          object * obs, string lang)
{
  string oblue;
  string tmp;

  tmp = "";
  msg = fix_string(msg);
  oblue = "";

  if (this_object() == ob)
    return;

  if (member_array(lang, this_object()->query_languages()) == -1)
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
  // else
  // if (lang != STD_LANG)
  //   start = start[0..strlen(start)-3]+" en "+lang+": ";
  // else {
  if (lang != STD_LANG)
    oblue = " en "+lang+": ";
  else
    // if (!stringp(oblue))
    oblue = ": ";
  // }

  if (member_array(this_object(), obs) == -1)
  {
    tmp = fix_string(start + " a " + query_multiple_short(obs) +
          oblue[0..strlen(oblue)-3] + ".");
  }
  else if (sizeof(obs) == 1)
  {
    start = replace_string(start, "susurra", "te susurra");
    tmp = fix_string(start + oblue + msg);
    this_object()->add_past_g( tmp );
  }
  else
  {
    start = replace_string(start, "susurra", "te susurra");
    tmp = fix_string(start + " a ti y a " +
          query_multiple_short(obs - ({this_object()})) +
          oblue + msg);
    this_object()->add_past_g( tmp );
  }

  this_object()->catch_tell("\n" + tmp + "\n");
}

void event_person_shout(object ob, string start, string msg, string lang)
{
  string tmp;

  if (ob == this_object())
    return;

  if (this_object()->query_earmuffs() == 1 &&
    ((string)ob->query_verb() != "coders!") &&
    ((string)ob->query_verb() != "shout!"))
    return;

  msg = fix_string(msg);

  if (member_array(lang, this_object()->query_languages()) == -1)
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
  // else
  if (lang != STD_LANG)
    start = start[0..strlen(start)-3]+" en "+lang+": ";

  tmp = start + msg;

  // if (ob && interactive(ob))
  this_object()->add_past_g( tmp );

  this_object()->catch_tell("\n" + tmp + "\n");
}

void event_creator_tell(object ob, string start, string msg)
{
}

nomask void event_god_inform(object ob, string start, string msg)
{
  if (!interactive(this_object()))
    return;

  if (!strlen(msg))
    return;

  msg = fix_string("\n" + start + " " + msg);

  this_object()->catch_tell(msg);
}

void event_inter_creator_tell(object ob, string mname, string pname,
                              string msg, object ig, int emote)
{
}

void event_player_echo(object ob, string msg)
{
  if (!interactive(this_object()))
    return;

  if (ob == this_object())
    return;

  if (this_object()->query_admin())
    msg = ob->query_cap_name()+" echo's:\n" + msg;

  msg = fix_string("\n" + msg);

  // only will do if interactive(this_object())
  this_object()->catch_tell(msg);
}

void event_player_echo_to(object ob, string msg)
{
  if (!interactive(this_object()))
    return;

  if (this_object()->query_admin())
    msg = ob->query_cap_name()+" echo to's:\n" + msg;

  msg = fix_string("\n" + msg);

  // only will do if interactive(this_object())
  this_object()->catch_tell(msg);
}

void event_player_emote_all(object ob, string msg)
{
  if (!interactive(this_object()))
    return;

  if (ob == this_object())
    return;

  if (this_object()->query_admin())
    msg = ob->query_cap_name()+" emote all's:\n" + msg;

  msg = fix_string("\n" + msg);

  // only will do if interactive(this_object())
  this_object()->catch_tell(msg);
}

void event_death(object caller, varargs object killer, object * attackers, mixed avoid)
{
  if (avoid)
  {
    if (pointerp(avoid) && (member_array(this_object(), avoid) != -1))
      return;
    else if (objectp(avoid) && (avoid == this_object()))
      return;
  }

  stderr(" * event_death in " + object_name(this_object()) + " because of " + object_name(caller) + "\n");
}

mixed * stats()
{
  return ({ });
}
