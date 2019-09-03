/* Sorted netstat for catching those multiplayers.  Lists player names,
   IP name, and room they're in, sorted by IP number.
*/
/* Hamlet wrote June 1995 */
/* Grimbrand wrote some mean nasty additions Jan 1996. */
/* yes... the code needs optimized.  I'm lazy, what can I say? */
/* Hamlet added ident stuff, Jan 1996 */
/* Radix changed virtual_file_name to simul real_file_name, Jan 22, 1996 */

#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;

void setup()
{
  position = 1;
}

protected int cmd(string str, object me, string verb) {
  // Use peopler code to get players - Radix, July 1996
   object *players = users();
  int sz = sizeof(players);
  string *info = allocate(4 * sz);
  int    *sortstuff = allocate(sz);
  int i,j,k;
  int least, tmp;
  int relevant, hits;
  string ipn;
  object ob;
  int LORD_PER = 0;

  if("/secure/master"->query_administrator(this_player(1)->query_name()))
    LORD_PER = 1;
  
  /* initialization */
  for(i=0;i<sz;i++) {
    info[i*4]       = query_ip_number(players[i]);
    info[(i*4)+1]   = players[i]->query_name();
    if(LORD_PER && (tmp = players[i]->query_ident()))
      info[(i*4)+2] = tmp+"@"+query_ip_name(players[i]);
    else
      info[(i*4)+2]   = query_ip_name(players[i]);
    if(environment(players[i]))
      info[(i*4)+3] = real_file_name(environment(players[i]));
    else
      info[(i*4)+3] = "No environment";
  }
  
  for(i=0;i<sz;i++)
    sortstuff[i] = i*4;
    
  /* sorting */
  for(j=0;j<(sz-1);j++) {
    least = j;
    for(k=j+1;k<sz;k++)
      if( info[sortstuff[k]] < info[sortstuff[least]] )
        least = k;
    
    tmp = sortstuff[least];
    sortstuff[least] = sortstuff[j];    
    sortstuff[j] = tmp;
  }
                    
  /* output */
  if(!str) {
    tell_object(this_player(),
                "Name         IP Name                       Room\n"); 

    for(i=0;i<sz;i++) {
      k = strlen(info[sortstuff[i]+3]); 
      j = k - 36;
      if( (tmp = strlen(info[sortstuff[i]+2]) - 29) > 0)
        j += tmp;
      if(j<0) j = 0;
    
      tell_object(this_player(),sprintf("%-13s%-29s %-s\n",
                  info[sortstuff[i]+1], info[sortstuff[i]+2], 
                  info[sortstuff[i]+3][j..k]));
    }
  }
  else {
    ob = find_player(str);

    if(!ob) {
      tell_object(this_player(),
                  "  Unable to locate "+capitalize(str)+" in the "
                  "realms.\n\n");
      return 1;
    }
 
    ipn = query_ip_number(ob);
    for(relevant=0;relevant<strlen(ipn);relevant++)
      if(ipn[relevant..relevant] == ".") {
        hits++;
        if(hits == 3)
          break;
      }

    tell_object(this_player(),
                "Name         IP Name                       Room\n");

    for(i=0;i<sz;i++) {
      k = strlen(info[sortstuff[i]+3]);
      j = k - 36;
      if( (tmp = strlen(info[sortstuff[i]+2]) - 29) > 0)
        j += tmp;
      if(j<0) j = 0;

      if(ipn[0..relevant] == info[sortstuff[i]][0..relevant]) {
        tell_object(this_player(),sprintf("%-13s%-29s %-s\n",
                    info[sortstuff[i]+1], info[sortstuff[i]+2], 
                    info[sortstuff[i]+3][j..k]));
      }
    }
  }
  return 1;
}
