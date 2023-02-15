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

#define GUILD this_player()->query_guild_ob()
#define GROUP this_player()->query_group_ob()
#define RACEG this_player()->query_race_group_ob()


mapping channels, history;

int query_channel_on(object ob, string chan);
int query_channel_permission(object ob, string chan);
void do_channel(string verb, string str, varargs string name, string mud, int flg);

void create() 
{
  channels = ([ ]);
  history = ([ ]);
  // call_out("do_sockets",1);
}

int close_channel(string channel, object ob)
{
  if (!query_channel_on(ob, channel)) 
    return 0;
  
  channels[channel] -= ({ ob });
  
  if (channel[0..0] != "#")
    message("Ok, apagas el canal %^RED%^" + channel + "%^RESET%^.\n","",ob);
  
  ob->adjust_property(CHANNELS_PROPERTY, ({channel}), -1);
  return 1;
}

int open_channel(string channel, object ob)
{
  if (query_channel_on(ob, channel)) 
    return 0;

  if (!channels[channel]) 
    channels[channel] = ({ });

  if (channel[0..0] != "#")
    message("Ok, abres el canal %^BOLD%^WHITE%^"+channel+"%^RESET%^.\n","",ob);

  if (member_array(channel, ob->query_property(CHANNELS_PROPERTY)) == -1)
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
    // Los canales temporales los eliminamos al reconectar
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

    // Canal emergencia solo jugadores y admins
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
      tell_object(this_player(),"Intento de ver el histórico de canales a los que no tienes permiso.\n");
      event(users(), "inform", this_player()->query_short(1)+" intento de get_history() sobre el canal "+chan, "person_cheat");
      return "";
    }

  if (!history[chan]) 
    return "El canal está vacío.\n";
  return implode(history[chan], "\n");
}

int do_chat( string str ) 
{
  string verb;
  verb = query_verb();

  if (verb == "gremio") 
  {
    if (!GUILD) 
    {
      message("No eres miembro de ningún gremio.\n","",this_player());
      return 1;
    }
    if (!GUILD->query_channel()) 
    {
      message("Tu gremio no tiene esa habilidad.\n","",this_player());
      return 1;
    }
    verb = (string)GUILD->query_name();
  }

  if (verb == "clan") 
    {
    if (!GROUP) 
    {
      message("No eres miembro de ningún clan.\n","",this_player());
      return 1;
    }
    if (!GROUP->query_channel()) 
    {
      message("Tu clan no tiene esa habilidad.\n","",this_player());
      return 1;
    }
    verb = (string)GROUP->query_name();
  }

  if (verb == "raza") 
  {
    if (!RACEG) 
    {
      message("No eres miembro de ningún grupo racial.\n","",this_player());
      return 1;
    }
    if (!RACEG->query_channel()) 
    {
      message("Tu grupo racial no tiene esa habilidad.\n","",this_player());
      return 1;
    }
    verb = (string)RACEG->query_name();
  }
  
  if (verb == "aventurero") 
  {
    // Si no perteneces a un grupo
    if (!this_player()->query_adventurer()) 
    {
      message("Debes pertenecer a un grupo para utilizar este canal.\n","",this_player());
      return 1;
    }
    
    if (str == "off" || str == "on") 
    {
      message("No puedes abrir ni cerrar el canal de tu grupo, funciona automáticamente.\n","",this_player());
      return 1;
    }
  }

  if (this_player()->query_property("chan_"+verb) == "nope") 
  {
    message("Tu canal ha sido deshabilitado.\n", "", this_player());
    return 1;
  }

  if (!query_channel_permission(this_player(), verb)) 
  {
    message("No tienes permiso para usar el canal '"+verb+"'.\n","",this_player());
    return 0;
  }

  if (!strlen(str)) 
  {
    message("Funciona mejor si introduces un mensaje.\n","",this_player());
    return 1;
  }

  switch( str ) 
  {
    case "off" :
      if (!query_channel_on(this_player(), verb)) 
      {
        message("¡El canal-"+verb+" ya está desactivado!\n", "", this_player());
        return 0;
      }
      
        close_channel(verb, this_player());
      return 1;

    case "on" :
      if (query_channel_on(this_player(), verb)) 
      {
        message("¡El canal '"+verb+"' ya está activado!\n", "", this_player());
        return 0;
      }
    
      open_channel(verb, this_player());
      return 1;
  }


  if (verb != "aventurero") 
    if (!query_channel_on(this_player(), verb)) 
    {
      message("No tienes ese canal activado.\n","",this_player());
      return 1;
    }

  if (verb == "aventurero")
    verb = this_player()->query_adventurer_channel();

  do_channel(verb, str, this_player()->query_cap_name());

  /*
  
  TODO network

  // Hacemos remote el canal emergencia, neverbot 02/06
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
  string ret;
  if (!verb || (verb[0..0] == "#") ) 
    verb = "<canal>";

  ret =
    "\t"+verb+" <mensaje> : Envía un mensaje a la gente que escucha el canal.\n"+
    "\t"+verb+" ?     : Muestra este mensaje de ayuda.\n"+
    // "\t"+verb+" @' Emote a traves del canal.\n"
    "\t"+verb+" !     : Ver los últimos "+number_as_string(MAX_CHANNEL_HISTORY)+" mensajes enviados al canal.\n";
  if (this_player()->query_coder())
    ret += "\t"+verb+" .     : Lista quienes están escuchando el canal.\n";
  return ret;
}

// #define chan_msg() message(str, name+" "+(flg?"(":tmp1)+capitalize(verb)+(flg?")":tmp2)\
// +": ", channels[verb]);\
// add_history(verb, name+" "+(flg?"(":tmp1)+capitalize(verb)+(flg?")":tmp2)\
// +": "+str);
// #define check_cmd() if (strlen(str) > 1) { chan_msg(); return; }

// Arreglo para los canales temporales (como el canal aventurero), neverbot 05/09

#define channel_name(verb) (verb[0..0]=="#"?explode(verb,"#")[sizeof(explode(verb,"#"))-1]:verb)

// neverbot, 01/06
#define new_chan_msg() message(name + ": " + str + "\n", (flg?"(":tmp1)+capitalize(channel_name(verb))+(flg?")":tmp2)\
, channels[verb]);\
add_history(verb, (flg?"(":tmp1)+capitalize(channel_name(verb))+(flg?")":tmp2)\
+" "+name+": "+str);

#define admin_chan_msg() message(name + ": " + str + "\n", (flg?"(":tmp1)+capitalize(channel_name(verb))+(flg?")":tmp2)\
, filter_array(channels[verb], "only_admins") );\
add_history(verb, (flg?"(":tmp1)+capitalize(channel_name(verb))+(flg?")":tmp2)\
+" "+name+": "+str);

#define check_cmd() if (strlen(str) > 1) { new_chan_msg(); return; }

int only_admins(object who, varargs mixed args...)
{
  return (who && (who->query_admin() || (who == this_player())));
}

void do_channel(string verb, string str, varargs string name, string mud, int flg) 
{
  string tmp1, tmp2;
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
      message("Historia del canal '"+channel_name(verb)+"':\n"+get_history(verb) + "\n", "", this_player());
      return;
    case '?':
      check_cmd();
      message("Ayuda del canal '"+channel_name(verb)+"':\n"+get_channel_help(verb) + "\n", "", this_player());
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
      message(query_multiple_short(query_who_on(verb)) + "\n",
        "Gente escuchando en el canal '"+channel_name(verb)+"':", this_player());
      break;
    default:
      // chan_msg();
      // No permitimos que los jugadores vean el canal emergencia
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
