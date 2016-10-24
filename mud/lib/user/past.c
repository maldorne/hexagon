
#include <mud/secure.h>
#include <basic/communicate.h>

static string *past_g;
static int poffset, pnumoff;

void create()
{

}

void add_past_g(string arg)
{
  arg = "[%^CYAN%^%^BOLD%^" + ctime(time(), 7) + "%^RESET%^] " + arg;

  if (!past_g) 
  {
    past_g = allocate(PAST_LENGHT);
    pnumoff = -PAST_LENGHT;
  }
  
  past_g[poffset++] = arg;
  
  if (poffset>=sizeof(past_g)) 
  {
    pnumoff += sizeof(past_g);
    poffset = 0;
  }
}
 
int print_past_g(string arg) 
{
  int i, from, to, num;
 
  if (this_player()!=this_object()) 
  {
      if (!interactive(this_player())) 
          return 0;
      if (!(SECURE->query_admin(geteuid(this_player())))) 
          return 0;
  }
  
  if (!sizeof(past_g))
  {
    write("No te han dicho nada de momento.\n");
    return 1;
  }

  write("Historial de conversaciones:\n");

  from = poffset;
  num = sizeof(past_g);

  if (!arg)
    arg = "";

  if (sscanf(arg,"%d,%d",from,to)==2) 
  {
    num = to-from;
    if (num<0) 
    {
      from = from+num;
      num = -num;
    }
  } 
  else if (sscanf(arg,",%d",to)==1) 
  {
    from = poffset+1+sizeof(past_g);
    num = from - (to%sizeof(past_g))+sizeof(past_g);
  } 
  else if (sscanf(arg,"%d,",from)==1) 
  {
    from = from % sizeof(past_g);
    num = from - poffset;
    if (num<0)
      num += sizeof(past_g);
  } 
  else if (sscanf(arg,"%d",num)!=1)
    num = sizeof(past_g);

  from = from % sizeof(past_g);

  if (num>sizeof(past_g))
    num = sizeof(past_g);

  for (i=from;i<sizeof(past_g);i++,num--) 
  {
    if (past_g[i])
      write((i+pnumoff)+": "+past_g[i]+"\n");
    if (!num)
      return 1;
  }

  for (i=0;i<from;i++,num--) 
  {
    if (past_g[i])
      write((i+pnumoff+sizeof(past_g))+": "+past_g[i]+"\n");
    if (!num)
      return 1;
  }
  return 1;
}

mixed *stats() 
{
  return ({
          });
}