#include "access.h"
#include "path.h"
inherit "/std/room";
#define MASTER "/secure/master"

void setup() {
//  string *doms, com;
//  int i;
  
  set_light(100);
  set_short("Site access control room");
  set_long("This room allows Demi-Gods and higher to easily remove "
   "troublesome players and even creators.  You can also banish a site "
   "from here entirely, but be absolutely certain that is approved by "
   "a God before you do so (or face demotion).\n\n"
"Available commands:\n"
"For specific character names\n"
"----------------------------\n"
"banish <player> <reason>       :  Banish a player name.\n"
"unbanish <player>              :  Unbanish a player name.\n"
"suspend <player> <time>        :  Suspend someones access for <time> hours.\n"
"unsuspend <player>             :  Unsuspend someones access.\n\n"
"For entire sites\n"
"----------------\n"
"access                         :  Show the current access of various sites.\n"
"unsite*banish [<ident>@]<ip>           :  Completely unbanish a site.\n"
"grant [<ident>@]<ip>                   :  Grant access to a specific\n"
"                                          user/machine\n"
"nonew [<ident>@]<ip> <reason>          :  Disallow new players from a site.\n"
"noaccess [<ident>@]<ip> <reason>       :  Disallow all access from a site.\n"
"noguest [<ident>@]<ip> <reason>        :  Disallow guests from a site.\n");

add_exit("norte", HEAVEN+"admin3","standard");
  seteuid("Admin");
} /* setup() */

void init() {
  ::init();
// YES, should be lord, certainly NOT high programmer.
if ("/secure/master"->query_lord(geteuid(previous_object()))) {
    add_action("do_site_banish", "site*banish"); /* Site banishing... */
  add_action("do_access", "access"); /* Show the current access list. */
  add_action("do_banish","banish");
  add_action("do_unbanish","unbanish");
  add_action("do_suspend", "suspend");
  add_action("do_unsuspend", "unsuspend");
       add_action("do_unsite_banish","unsite*banish");
       add_action("do_grant","grant");
       add_action("do_nonew","nonew");
       add_action("do_noaccess","noaccess");
       add_action("do_noguest","noguest");
 }
} /* init() */

int print_access(string bit, mapping bing, int depth, int cols) {
  mixed *bits;
  int i;

  if (this_player() != this_player(1))
    return 0;
  bits = m_indices(bing);
  if (depth == 4) {
/* Do the ident printing... */
    for (i=0;i<sizeof(bits);i++)
      switch (bing[bits[i]]) {
        case NO_NEW :
          printf("%s@%-=*s", bits[i], cols - strlen(bits[i]), bit +
                             " set to no new characters.\n");
          break;
        case NO_ACCESS :
          printf("%s@%-=*s", bits[i], cols - strlen(bits[i]), bit +
                             " set to no characters.\n");
          break;
        case ACCESS :
          printf("%s@%-=*s", bits[i], cols - strlen(bits[i]), bit +
                             " set to normal access.\n");
          break;
          case NO_GUEST :
            printf("%s@%-=*s", bits[i], cols - strlen(bits[i]), bit +
                               " set to no guests.\n");
          break;
          case NO_IMMORTS :
            printf("%s@%-=*s", bits[i], cols - strlen(bits[i]), bit +
                               " set to no immortals.\n");
          break;
          case NO_PLAYERS :
            printf("%s@%-=*s", bits[i], cols - strlen(bits[i]), bit +
                               " set to no players.\n");
          break;
    }
    return 1;
  }
  for (i=0;i<sizeof(bits);i++)
    print_access(bit+"."+bits[i], bing[bits[i]], depth+1, cols);
  return 1;
} /* print_access() */

int do_access() {
  mixed bing;

  if (this_player() != this_player(1))
    return 0;
  bing = (mixed)"/secure/bastards"->query_all_access();
  if (!m_sizeof(bing)) {
    notify_fail("No access control defined.\n");
    return 0;
  }

  print_access("", bing, 0, (int)this_player()->query_cols());
  return 1;
} /* do_access() */

int do_site_banish(string str) {
  string ip, ident, reason;
  int level;

  if (this_player() != this_player(1))
    return 0;
/*  if (!"/secure/master"->high_programmer(geteuid(previous_object())))
  {
    notify_fail("You need to be Demi-God or higher to sitebanish.\n");
    return 0;
  }*/
   notify_fail("Syntax: sitebanish <ip number> <ident> <severity> <reason>\n"
              "        <severity> :  0 delete ident,   1 normal access,\n"
               "                      2 no new players, 3 No access.\n"
               "                      4 no guests,       5 no immorts\n"
                "                      6 no players.\n");
  if (!str)
    return 0;
  if (sscanf(str, "%s %s %d %s", ip, ident, level, reason) != 4)
    return 0;

  if (!"/secure/bastards"->change_access(explode(ip, "."), ident, level,
                                         reason))
    return 0;
  write("Access permisions changed.\n");
  return 1;
} /* do_site_banish() */

int do_unsite_banish(string str) {
  string *junk;
  string ident;
  string addr;

  notify_fail("Syntax: unsitebanish [<ident>@]<ip>\n");

  if(!str)  return 0;

  if(sscanf(str,"%s@%s",ident,addr) != 2) {
    ident = "*";
    addr = str;
  }

  junk = explode(addr,".");
  if(sizeof(junk) != 4)  return 0;

  if((junk[0] == "*") || (junk[1] == "*")) {
    tell_object(this_player(),"Naughty.  Only the last number should "
                              "be a *\n");
          return 1;
  }

  return do_site_banish(addr+" "+ident+" 0 ""No reason");
}

int do_grant(string str) {
  string *junk;
  string ident;
  string addr;

  notify_fail("Syntax: grant [<ident>@]<ip>\n");

  if(!str)  return 0;

  if(sscanf(str,"%s@%s",ident,addr) != 2) {
    ident = "*";
    addr = str;
  }

  junk = explode(addr,".");
  if(sizeof(junk) != 4)  return 0;

  if((junk[0] == "*") || (junk[1] == "*")) {
    tell_object(this_player(),"Naughty.  Only the last number should "
                              "be a *\n");
        return 1;
  }

  return do_site_banish(addr+" "+ident+" 1 ""No reason");
}

int do_nonew(string str) {
  string *junk;
  string ident;
  string addr;
  string reason;

  notify_fail("Syntax: nonew [<ident>@]<ip> <reason>\n");

  if(!str)  return 0;

  if(sscanf(str,"%s@%s %s",ident,addr,reason) != 3) {
    if(sscanf(str,"%s %s",addr,reason) != 2)  return 0;
    ident = "*";
  }

  junk = explode(addr,".");
  if(sizeof(junk) != 4)  return 0;

  if((junk[0] == "*") || (junk[1] == "*")) {
    tell_object(this_player(),"Naughty.  Only the last number should "
                              "be a *\n");
         return 1;
  }

  return do_site_banish(addr+" "+ident+" 2 "+reason);
}

int do_noaccess(string str) {
  string *junk;
  string ident;
  string addr;
  string reason;

  notify_fail("Syntax: noaccess [<ident>@]<ip> <reason>\n");

  if(!str)  return 0;

  if(sscanf(str,"%s@%s %s",ident,addr,reason) != 3) {
    if(sscanf(str,"%s %s",addr,reason) != 2)  return 0;
    ident = "*";
  }

  junk = explode(addr,".");
  if(sizeof(junk) != 4)  return 0;

  if((junk[0] == "*") || (junk[1] == "*")) {
    tell_object(this_player(),"Naughty.  Only the last number should "
                              "be a *\n");
           return 1;
  }

  return do_site_banish(addr+" "+ident+" 3 "+reason);
}

int do_noguest(string str) {
  string *junk;
  string ident;
  string addr;
  string reason;

  notify_fail("Syntax: noguest [<ident>@]<ip> <reason>\n");

  if(!str)  return 0;

  if(sscanf(str,"%s@%s %s",ident,addr,reason) != 3) {
    if(sscanf(str,"%s %s",addr,reason) != 2)  return 0;
    ident = "*";
  }

  junk = explode(addr,".");
  if(sizeof(junk) != 4)  return 0;

  if((junk[0] == "*") || (junk[1] == "*")) {
    tell_object(this_player(),"Naughty.  Only the last number should "
                              "be a *\n");
          return 1;
  }

  return do_site_banish(addr+" "+ident+" 4 "+reason);
}

int do_banish(string str) {
  string name;
  string reason;

  if (this_player() != this_player(1))
    return 0;
  if (!"/secure/master"->query_lord(geteuid(previous_object())))
  {
    notify_fail("You need to be a Demi-God or higher to banish someone.\n");
    return 0;
  }
  if (sscanf(str, "%s %s", name, reason) != 2) {
    notify_fail("Syntax: banish <name> <reason>\n");
    return 0;
  }
  notify_fail("We failed!\n");
  if (!"/secure/bastards"->banish_playername(name, reason))
    return 0;
  write(name+" banished because: "+reason+"\n");
  return 1;
} /* do_banish() */

int do_unbanish(string str) {
  if (this_player() != this_player(1))
    return 0;
  if (!"/secure/master"->query_lord(geteuid(previous_object())))
    return 0;
  notify_fail("We failed!\n");
  if (!"/secure/bastards"->unbanish_playername(str))
    return 0;
  write(str+" unbanished.\n");
  return 1;
} /* do_unbanish() */

int do_suspend(string str) {
  string name;
  int tim;

  if (this_player() != this_player(1))
    return 0;
  if (!"/secure/master"->query_lord(geteuid(previous_object())))
  {
    notify_fail("You need to be a Demi-God or higher to suspend someone.\n");
    return 0;
  }
  if (sscanf(str, "%s %d", name, tim) != 2) {
    notify_fail("Syntax: suspend <name> <time>\nWhere time is in hours.\n");
    return 0;
  }
  notify_fail("We failed!\n");
  if (!"/secure/bastards"->suspend_person(name, tim*60*60))
    return 0;
  write(name+" suspended until "+ctime(time()+tim*60*60)+"\n");
  return 1;
} /* do_suspend() */

int do_unsuspend(string str) {
  if (this_player() != this_player(1))
    return 0;
  if (!"/secure/master"->query_lord(geteuid(previous_object())))
    return 0;
  notify_fail("We failed!\n");
  if (!"/secure/bastards"->unsuspend_person(str))
    return 0;
  write(str+" unsuspended.\n");
  return 1;
} /* do_unsuspend() */
