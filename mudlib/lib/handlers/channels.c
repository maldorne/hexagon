/*
 * Changed to maintain an mapping of arrays of everyone on a channel
 * Also performed so major cleanups...more than doubled the efficiency
 *  -- Wahooka April '97
 * Retoques para CcMud, neverbot 01/2006
 *  - Eliminado el canal @<texto>, ahora todos los mensajes
 *  se ponen por defecto con ese formato.
 *  - Añadido log de horas para el historial del canal.
 * Añadido canal de grupo ('aventurero'), neverbot 05/09
 * Añadidas funciones open_channel y close_channel
 */

#include <basic/communicate.h>
#include <language.h>

#define GUILD this_player()->query_guild_ob()
#define GROUP this_player()->query_group_ob()
#define RACEG this_player()->query_race_group_ob()


mapping channels, history;

int query_channel_on(object ob, string chan);
int query_channel_permission(object ob, string chan);
void do_channel(string verb, string str, varargs string who, string mud, int flg);
string query_channel_display(string chan);

// The channel a typed word means, or nil if no channel answers to it. The
// dynamic channels (a guild's, a clan's, a party's) are named after the group
// itself and never come through here.
string query_channel_id(string verb)
{
  mapping verbs;

  verbs = _LANG_CHANNEL_VERBS;
  return verbs[verb];
}

// The words this mud's players type, for whoever registers the actions.
string * query_channel_verbs()
{
  return map_indices(_LANG_CHANNEL_VERBS);
}

// What a channel is called on screen. A channel named after a group, or a
// temporary one, is shown by its own name.
string query_channel_display(string chan)
{
  mapping names;

  if (!stringp(chan) || !strlen(chan))
    return "";

  names = _LANG_CHANNEL_NAMES;
  return names[chan] ? names[chan] : chan;
}

void create() 
{
  channels = ([ ]);
  history = ([ ]);
  // call_out("do_sockets",1);
}

int close_channel(string channel, object ob)
{
  string name;

  if (!query_channel_on(ob, channel)) 
    return 0;
  
  channels[channel] -= ({ ob });
  name = query_channel_display(channel);
  
  if (channel[0..0] != "#")
    message(_LANG_CHANNEL_CLOSED,"",ob);
  
  ob->adjust_property(CHANNELS_PROPERTY, ({channel}), -1);
  return 1;
}

int open_channel(string channel, object ob)
{
  string name;
  mixed open;

  if (query_channel_on(ob, channel)) 
    return 0;

  if (!channels[channel]) 
    channels[channel] = ({ });

  name = query_channel_display(channel);

  if (channel[0..0] != "#")
    message(_LANG_CHANNEL_OPENED,"",ob);

  // the property is empty until something is written on it
  open = ob->query_property(CHANNELS_PROPERTY);

  if (!pointerp(open) || member_array(channel, open) == -1)
    ob->adjust_property(CHANNELS_PROPERTY, ({channel}));

  channels[channel] += ({ ob });
  return 1;
}

void init_player_channels(mixed chans, object player) 
{
  int i;

  if (undefinedp(chans) || !arrayp(chans))
    chans = ({ });

  for (i = 0; i < sizeof(chans); i++) 
  {
    // a temporary channel is dropped on reconnecting
    if (chans[i][0..0] == "#")
    {
      player->adjust_property(CHANNELS_PROPERTY, ({ chans[i] }), -1);
      continue;
    } 

    if (!query_channel_permission(player, chans[i])) 
      continue;

    if (!channels[chans[i]]) 
      channels[chans[i]] = ({ });

    if (query_channel_on(player, chans[i])) 
      continue;

    channels[chans[i]] += ({ player });
  }

  if (player->query_coder() &&
      !query_channel_on(player, EMERGENCY_CHANNEL)) 
  {
    if (!channels[EMERGENCY_CHANNEL]) 
      channels[EMERGENCY_CHANNEL] = ({ });
    channels[EMERGENCY_CHANNEL] += ({ player });
  }
}

int query_channel_on(object ob, string channel) 
{
  return member_array(ob, channels[channel]) != -1;
}

int query_channel_permission(object ob, string chan) 
{
  switch(chan) 
  {
    // coders channel only coders
    case "coders":
      if (ob->query_coder()) 
            return 1;
      return 0;

      /*
        case "coders"  :
             if (ob->query_coder()) return 1;
        case "thane":
            if (ob->query_thane()) return 1;
        case "patron":
            if (ob->query_patron()) return 1;
        case "semi":
            if (ob->query_admin()) return 1;
            return 0;
        case "god":
            if (ob->query_god()) return 1;
      */

    // the emergency channel is for players and admins
    case EMERGENCY_CHANNEL:
      if (ob->query_coder()) 
            if (ob->query_admin())
              return 1;
      else
        return 0;
            else
      return 1;

    default: 
      return 1;
  }
}

object * query_who_on(string chan) 
{
  object *ret;
  ret = channels[chan];
  if (!this_player()->query_admin()) 
  {
  int i;
  for(;i<sizeof(ret);i++)
    if (ret[i]->query_invis() == 2) 
    {
      ret -= ret[i..i];
      i--;
    }
  }
  return ret;
}

static void add_history(string chan, string mess) 
{
  mess = "[%^CYAN%^%^BOLD%^" + ctime(time(), 7) + "%^RESET%^] " + mess;

  if (!history[chan]) 
  {
    history[chan] = ({ mess });
    return;
  }

  if (sizeof(history[chan]) >= MAX_CHANNEL_HISTORY) 
  {
    history[chan] = history[chan][1..] + ({ mess });
    return;
  }

  history[chan] += ({ mess });
}

string get_history(string chan) 
{
  if (this_player() != this_object()) 
    if (this_player() && !this_player()->query_admin() && 
      !query_channel_permission(this_player(), chan))
    {
      tell_object(this_player(), _LANG_CHANNEL_HISTORY_NOT_ALLOWED);
      event(users(), "inform", this_player()->query_short(1)+" tried get_history() on the channel "+chan, "person_cheat");
      return "";
    }

  if (!history[chan]) 
    return _LANG_CHANNEL_HISTORY_EMPTY;
  return implode(history[chan], "\n");
}

int do_chat( string str ) 
{
  string verb, kind, name;

  // the word typed says which channel this is; a channel of a group is then
  // named after the group itself
  kind = query_channel_id(query_verb());
  verb = kind ? kind : query_verb();

  if (kind == "guild") 
  {
    if (!GUILD) 
    {
      message(_LANG_CHANNEL_NO_GUILD,"",this_player());
      return 1;
    }
    if (!GUILD->query_channel()) 
    {
      message(_LANG_CHANNEL_GUILD_NO_CHANNEL,"",this_player());
      return 1;
    }
    verb = (string)GUILD->query_name();
  }

  if (kind == "clan") 
    {
    if (!GROUP) 
    {
      message(_LANG_CHANNEL_NO_CLAN,"",this_player());
      return 1;
    }
    if (!GROUP->query_channel()) 
    {
      message(_LANG_CHANNEL_CLAN_NO_CHANNEL,"",this_player());
      return 1;
    }
    verb = (string)GROUP->query_name();
  }

  if (kind == "race") 
  {
    if (!RACEG) 
    {
      message(_LANG_CHANNEL_NO_RACE_GROUP,"",this_player());
      return 1;
    }
    if (!RACEG->query_channel()) 
    {
      message(_LANG_CHANNEL_RACE_NO_CHANNEL,"",this_player());
      return 1;
    }
    verb = (string)RACEG->query_name();
  }
  
  if (kind == "adventurer") 
  {
    // not in a party
    if (!this_player()->query_adventurer()) 
    {
      message(_LANG_CHANNEL_NO_PARTY,"",this_player());
      return 1;
    }
    
    if (str == "off" || str == "on") 
    {
      message(_LANG_CHANNEL_PARTY_AUTOMATIC,"",this_player());
      return 1;
    }
  }

  name = query_channel_display(verb);

  if (this_player()->query_property("chan_"+verb) == "nope") 
  {
    message(_LANG_CHANNEL_DISABLED, "", this_player());
    return 1;
  }

  if (!query_channel_permission(this_player(), verb)) 
  {
    message(_LANG_CHANNEL_NO_PERMISSION,"",this_player());
    return 0;
  }

  if (!strlen(str)) 
  {
    message(_LANG_CHANNEL_NEEDS_MESSAGE,"",this_player());
    return 1;
  }

  switch( str ) 
  {
    case "off" :
      if (!query_channel_on(this_player(), verb)) 
      {
        message(_LANG_CHANNEL_ALREADY_OFF, "", this_player());
        return 0;
      }
      
        close_channel(verb, this_player());
      return 1;

    case "on" :
      if (query_channel_on(this_player(), verb)) 
      {
        message(_LANG_CHANNEL_ALREADY_ON, "", this_player());
        return 0;
      }
    
      open_channel(verb, this_player());
      return 1;
  }


  if (kind != "adventurer") 
    if (!query_channel_on(this_player(), verb)) 
    {
      message(_LANG_CHANNEL_NOT_LISTENING,"",this_player());
      return 1;
    }

  if (kind == "adventurer")
    verb = this_player()->query_adventurer_channel();

  do_channel(verb, str, this_player()->query_cap_name());

  /*
  
  TODO network

  // the emergency channel goes out to the other muds too, neverbot 02/06
  if ((verb == EMERGENCY_CHANNEL) &&
      (str[0] != '!') &&
      (str[0] != '@') &&
      (str[0] != '.') &&
      (str[0] != '?') )
  {
   REMOTE_CHANNELS->transmit(this_player(),verb,str);
       log_file(EMERGENCY_CHANNEL, "[" + ctime(time(),4) + "] " + 
                  this_player()->query_cap_name() + ": " +
                  str + "\n");
  }

  if (( verb == "coders" ) && 
      (str[0] != '!') &&
      (str[0] != '@') &&
      (str[0] != '.') &&
      (str[0] != '?') )
  {
    REMOTE_CHANNELS->transmit(this_player(),verb,str);
  }
  */

  /* not on this mud. Baldrick
  if ( verb == "coders" || verb == "thane" || verb == "demi" ) {
  add_send_mess(verb+" "+this_player()->query_cap_name()+" "+
  implode(explode(mud_name()," "),"_")+" "+str);
  transmit();
  }
  */
  /*
  if (verb == "intercre" ) {
  SERVICES_D->eventSendChannel(this_player()->query_short(), verb,
  str, 0, 0, 0);
  }
  */
  return 1;
}

string get_channel_help(string verb) 
{
  string ret, name;

  if (!verb || (verb[0..0] == "#") ) 
    name = _LANG_CHANNEL_ANY;
  else
    name = query_channel_display(verb);

  ret =
    _LANG_CHANNEL_HELP_MESSAGE +
    _LANG_CHANNEL_HELP_HELP +
    // "\t"+verb+" @' Emote a traves del canal.\n"
    _LANG_CHANNEL_HELP_HISTORY;
  if (this_player()->query_coder())
    ret += _LANG_CHANNEL_HELP_WHO;
  return ret;
}

// #define chan_msg() message(str, name+" "+(flg?"(":tmp1)+capitalize(verb)+(flg?")":tmp2)\
// +": ", channels[verb]);\
// add_history(verb, name+" "+(flg?"(":tmp1)+capitalize(verb)+(flg?")":tmp2)\
// +": "+str);
// #define check_cmd() if (strlen(str) > 1) { chan_msg(); return; }

// temporary channels (a party's, say) are named with a leading #, neverbot 05/09

#define channel_name(verb) (verb[0..0]=="#"?explode(verb,"#")[sizeof(explode(verb,"#"))-1]:verb)

// neverbot, 01/06
#define new_chan_msg() message(who + ": " + str + "\n", (flg?"(":tmp1)+capitalize(query_channel_display(channel_name(verb)))+(flg?")":tmp2)\
, channels[verb]);\
add_history(verb, (flg?"(":tmp1)+capitalize(query_channel_display(channel_name(verb)))+(flg?")":tmp2)\
+" "+who+": "+str);

#define admin_chan_msg() message(who + ": " + str + "\n", (flg?"(":tmp1)+capitalize(query_channel_display(channel_name(verb)))+(flg?")":tmp2)\
, filter_array(channels[verb], "only_admins") );\
add_history(verb, (flg?"(":tmp1)+capitalize(query_channel_display(channel_name(verb)))+(flg?")":tmp2)\
+" "+who+": "+str);

#define check_cmd() if (strlen(str) > 1) { new_chan_msg(); return; }

int only_admins(object who, varargs mixed args...)
{
  return (who && (who->query_admin() || (who == this_player())));
}

// `who` is the one speaking; `name` is the channel's own name on screen.
void do_channel(string verb, string str, varargs string who, string mud, int flg) 
{
  string tmp1, tmp2, name;
  tmp1 = "[";
  tmp2 = "]";
  
  if (mud) 
  { 
    tmp1="%^BOLD%^%^RED%^(*"; 
    tmp2="*)%^RESET%^"; 
  }

  if (channels[verb] && sizeof(channels[verb])) // Taniwha 01/05/97, make sure it exists
    channels[verb] -= ({ nil });
  else 
    channels[verb] = ({ });

  switch(str[0]) 
  {
    case '!':
      check_cmd();
      name = query_channel_display(channel_name(verb));
      message(_LANG_CHANNEL_HISTORY_HEADER+get_history(verb) + "\n", "", this_player());
      return;
    case '?':
      check_cmd();
      name = query_channel_display(channel_name(verb));
      message(_LANG_CHANNEL_HELP_HEADER+get_channel_help(verb) + "\n", "", this_player());
      return;
     /* Desactivado, neverbot 01/06
    case '@' :
      message(name+" "+str[1..], (flg?"(":tmp1)+capitalize(verb)+(flg?") ":tmp2+" "),
      channels[verb]);
      add_history(verb,
      (flg?"(":tmp1)+capitalize(verb)+(flg?") ":tmp2+" ")+name+" "+str[1..]);
      break;
     */
    case '.':
      check_cmd();
      if (!this_player() || !this_player()->query_coder()) 
      {
        new_chan_msg();
        return;
      }
      name = query_channel_display(channel_name(verb));
      message(query_multiple_short(query_who_on(verb)) + "\n",
        _LANG_CHANNEL_WHO_HEADER, this_player());
      break;
    default:
      // chan_msg();
      // players do not read the emergency channel
      if (verb == EMERGENCY_CHANNEL)
      {
        admin_chan_msg();
      }
      else
      {
        new_chan_msg();
      }
    break;
  }
}

/*
varargs void received_cre(string s) {
string verb, str, name, mud;
object *subjs;
int i;

if (s)  received += ({ s });

if (sizeof(received))
for(i=0;i<sizeof(received);i++) {
sscanf(received[i],"%s %s %s %s", verb, name, mud, str);
switch(verb) {
case "dwcre":
case "dwadmin":
case "intercre":
do_channel(verb, str, name, mud);
break;
default:
do_channel(verb, str, name, 0, 1);
}
}
received = ({ });
}*/ /* received_cre() */

// Radix
int clean_up() { return 0; }
