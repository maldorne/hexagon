/* Author: unknown.  Modifications by Hamlet                      */
/* This handles mirroring between the normal mud and the testmud. */
/* July 1996 - Hamlet made me truly external from other objects. */

#include <net/socket.h>
#include <net/network.h>
#include <basic/communicate.h>
#include <mud/config.h>


#define LOGFILE "mirror"
#define SERVICES ({ "query", "reply", "action" });

/* This is for potential future expansion. */
// Ahora en config.h
// #define ALLOWED_MUDS ({"CC","CC_DEV"})
// #define THIS_MUD "CC_DEV"

int cs, rs, reset, local_port, remote_port;
int service_response;
 
void do_cre_socket();
void do_remote_socket();
void send_service_reply(string service, string msg);
int transmit(mixed user, string chan, string msg);

void create() {
  if(THIS_MUD == "CC") {
    local_port = 3010;
    remote_port = 3011;
  } 
  else {
    local_port = 3011;
    remote_port = 3010;
  }

  log_file(LOGFILE,"--- "+ctime(time())+" ---\n");
  do_cre_socket();
  do_remote_socket();
} /* create() */
 
void do_cre_socket() {
  int error;
 
  cs = socket_create(DATAGRAM, "read_callback");
  if (cs < 0) {
    log_file(LOGFILE,"**socket_create: " + socket_error(cs) + "\n");
    return;
  }
  log_file(LOGFILE,"Created Socket " + cs + "\n");
  error = socket_bind(cs, local_port);
  if (error != EESUCCESS) {
    log_file(LOGFILE,"**socket_bind: " + socket_error(error) + "\n");
    socket_close(cs);
    return;
  }
} /* do_cre_socket() */
 
void do_remote_socket() {
  int error;
 
  rs = socket_create(DATAGRAM, "read_callback");
  if (rs < 0) {
    log_file(LOGFILE,"**socket_create: " + socket_error(rs) + "\n");
    return;
  }
  log_file(LOGFILE,"Created Socket " + rs + "\n");
  error = socket_bind(rs, 0);
  if (error != EESUCCESS) {
    log_file(LOGFILE,"**socket_bind: " + socket_error(error) + "\n");
    socket_close(rs);
    return;
  }
} /* do_remote_socket() */

/* format for data: space delimited fields.
   <mud_name> <user> <channel> <message ... >
              -or-
   <mud_name> query <service_request> <additional data...>
              -or-
   <mud_name> action <action type>
   
   Why is <mud_name> there?  Well, paranoia, of course.  Schemes of
     future expansion.
*/

void read_callback(int fd, string data) {
  string mudname, user, channel, msg;
  string *tmp;
  string response;
  
  tmp = explode(data," ");
  
  if(sizeof(tmp) < 4) {  /* incomplete transmission.  Log and give up. */
    log_file(LOGFILE,"**read_callback (bad packet): " + data+"\n");
    return;
  }
  
  mudname = tmp[0];
  user = tmp[1];
  channel = tmp[2];
  msg = implode(tmp[3..(sizeof(tmp)-1)]," ");
  
  if(member_array(mudname, ALLOWED_MUDS) == -1) {
    log_file(LOGFILE,"SECURITY VIOLATION: the mud \""+mudname+
                     "\" attempted to communicate.\n");
    return;
  }
  
  if(user == "query") { /* We have a service request! */
    /* What services do we have?
         "who <arg>"           :  users on the other mud.  Valid args will 
                                  eventually be "players", "immortals", 
                                  "who" (for a full list)
         "listening <channel>" :  get a list of who's listening on a
                                  channel.
    */
    if(channel == "who") {
      service_response = 1;

      /* This bit obviously needs made smarter eventually */
      response = msg+"<$"+("/cmds/mortal/who.c"->who_string(79,1,0));
      transmit("reply","who", response);
    }
    /*
    if(channel == "listening") {
      service_response = 1;

      response = implode(CHANNEL_HAND->get_chan_users()," ");
      transmit("reply","listening", response);
    }
    */
  }
  
  else if(user == "reply") /* The other mud is answering the service request! */
    send_service_reply(channel,msg);

  else if(user == "action") {
    if(channel == "reboot")
    {
      //log_file(REPLY_LOG,"Shutdown llamado por "+capitalize(msg)+" en "+ctime(time())+"\n");
      load_object("/obj/shut.c");
      "/obj/shut.c"->shut(10);
      transmit("reply","reboot",msg+"<$Reboot iniciado con éxito");
    }
  }
  else
  CHAT_HANDLER->do_channel(channel, msg, user, tmp[0]);
// CHAT_HANDLER->do_channel(channel, msg, user+"%^RED%^%^BOLD%^@"+tmp[0]+"%^RESET%^%^RESET%^");
} /* read_callback() */
 
int transmit(mixed user, string chan, string msg) {
  int error;
  string to_transmit;
 
  /*
  if( objectp(user) && 
      (file_name(previous_object())[0..strlen(CHANNEL_HAND)-1] !=
       CHANNEL_HAND) )
    return 0; // nope.  That isn't allowed.
    
  else if(!objectp(user)) {
    if( (user == "query") && (!this_player(1) || 
        (CHANNEL_HAND->query_imm_lvl(this_player(1)) < 70)) )
      return 0;
      
    if(user == "reply") {
      if(!service_response)
        return 0;
      else
        service_response = 0;
    }
  }
  */
  
  to_transmit = THIS_MUD + " ";
  if(objectp(user))
    to_transmit += capitalize(user->query_name());
  else
    to_transmit += user;
    
  to_transmit += " " + chan + " " + msg;
  
  error = socket_write(rs, to_transmit, "127.0.0.1 "+remote_port);
  if(error == EEBADF) {  /* its disappeared! lets get it back.. */
    socket_close(rs);
    do_remote_socket();
  }
  if (error < 0) {
    log_file(LOGFILE,"**socket_write: " + socket_error(error) + "\n");
    return 0;
  }
  
  return 1;
} /* transmit() */

void send_service_reply(string service, string msg)
{
  string *aux;
  /* This needs written, but it's awfully low priority right now. */
  if(service=="who" || service=="reboot")
  {
    if(sizeof(aux= explode(msg,"<$"))>1)
    {
      tell_object(find_player(aux[0]),aux[1]+"\n");
      //log_file(REPLY_LOG,"Reply de la accion "+service+" en "+ctime(time())+" por "+capitalize(aux[0])+"\n");
    }
  }
}
