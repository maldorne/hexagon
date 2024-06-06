// NPC statistic tracker for FR-Mud
// Radix - April 6, 1996
// Version 1.7
// Hacked up for money... Anirudh

#include <mud/time.h>
#include <basic/money.h>

/* The data mappings...
  ([ "directory of transaction room" :
            ({
                       taken in, 
                       taken in per level,
                       taken per level under 15,
                       last taken,
                       time averaged,
                       given out,
                       given out/level,
                       given per level under 15,
                       last given,
                       time averaged,
             })
  ])
*/

inherit "/lib/core/object.c";

#define SAVE "/save/"
#define INFLATION_LOG_FILE "inflation"
#define MONEY_TRACKER_LOG "money_tracker"
#define MONEY_TRACKER_SAVE_FILE "money_tracker.o"

float *mult;
float last_adj;
mapping data;
float totaldif,diftimes;

void reset_la() { last_adj = 0.0; }
void reset_screw_rate() 
{
  totaldif = 0.0;
  diftimes = 0.0;
}

float query_screw_rate() 
{
  float num, dif;
  num = diftimes;
  dif = totaldif;
  
  write(num+"\n");
  
  if (num > 20000.0) 
    reset_screw_rate();

  if (num <= 0.0) 
    return 1.0; // Taniwha

  return dif/num;
}

float query_number_screwed() { return diftimes; }
float *query_adj_data() { return ({last_adj }) +mult; }

mapping query_data() { return data; }

void load_this_ob() 
{
  if (!m_sizeof(data))
    restore_object(SAVE + MONEY_TRACKER_SAVE_FILE);
}

void save_this_ob() 
{
  if (m_sizeof(data)) 
    save_object(SAVE + MONEY_TRACKER_SAVE_FILE,1);
}

void clear_data() 
{
  data = ([ ]);
  mult = ({ 0.0,0.0,0.0,0.0 });
  save_object(SAVE + MONEY_TRACKER_SAVE_FILE,1);
}

void create()
{
  ::create();
  set_name("money tracker");
  data = ([ ]);
  mult = ({ 0.0,0.0,0.0,0.0 });
  load_this_ob();
}

void dest_me()
{
   save_this_ob();
   destruct(this_object());
}

string crop_string(string longpa, int howlong) 
{
  string shorter;
  shorter = longpa[strlen(longpa)-howlong..strlen(longpa)];
  return shorter;
}

string environment_path(object obj)
{
  string fname;
  string *dom;
  
  fname = file_name(environment(obj));
  dom = explode(fname,"/");
  dom -= ({ dom[sizeof(dom)-1] });
  return implode(dom,"/");
}

mapping query_moneystats() 
{
  load_this_ob();
  return data;
}

mapping query_domain_moneystats( string dom ) 
{ 
  load_this_ob();
  if (data[dom]) 
    return data[dom];
  return ([ ]);
}

mixed *query_money_stat( string dom, string pathname ) 
{
  load_this_ob();
  if (data[dom]) 
  {
    if (data[dom][pathname]) 
      return data[dom][pathname];
  }
  return ({ });
}

string select_domain_stats(varargs string realdom) 
{
  int i;
  float tim, timt, timtt, time_now;
  float ttl15t,gtl15t;
  float gtt,ttt,gtlt,ttlt;
  float tt, ttl,ttl15,gt,gtl,gtl15;
  string dom, outgoing;
  string *ind, *tmp;

  load_this_ob();
  time_now = (float)TIMEKEEPER->query_running_time()/60.0;

  // outgoing ="\nDir              ggive     ngive     ggivel    ngivel    ggl15     ngl15\n";

  outgoing = sprintf("%-20s %9s %9s %9s %9s %9s %9s\n", "Dir", "ggive", "ngive", "ggivel", "ngivel", "ggl15", "ngl15");
  outgoing += "                         (money in copper, time in player hours)\n\n";

  if (!mappingp(data)) 
    data = ([ ]); // Taniwha
  
  ind = m_indices(data);
  
  for (i = 0; i < sizeof(ind); i++) 
  {
    tmp = explode(ind[i],"/");

    if (sizeof(tmp) < 2) 
      continue;

    if (!realdom || tmp[1] == realdom) 
    {
      dom = ind[i];

      if (!sizeof(data[dom])) 
      {
        data = m_delete(data,data[dom]);
        continue;
      }

      tt = data[dom][0];
      ttl = data[dom][1];
      ttl15 = data[dom][2];
      gt = data[dom][5];
      gtl = data[dom][6];
      gtl15 = data[dom][7];

      if (((time_now-data[dom][3])>(float)BASE_WEEK/30.0
        &&(time_now-data[dom][8])>(float)BASE_WEEK/30.0)
        ||(time_now-data[dom][3])<0.0
        ||(time_now-data[dom][8])<0.0)
      {
        data = m_delete(data,dom);
        continue;
      }

      tim = data[dom][9];
      if (data[dom][4]>tim) 
        tim = data[dom][4];
      if (tim > timt) 
        timt = tim;
      // if (dom)
      //   dom = dom[strlen(realdom)+3..]+"/";
      if (timt) 
      {
        outgoing+=sprintf("%-20s %8.2f ", dom, (gt*60.0)/timt);
        outgoing+=sprintf(" %8.2f ", (gt+tt)*60.0/timt);
        outgoing+=sprintf(" %8.2f ", (gtl)*60.0/timt);
        outgoing+=sprintf(" %8.2f ", (gtl+ttl)*60.0/timt);
        outgoing+=sprintf(" %8.2f ", (gtl15)*60.0/timt);
        outgoing+=sprintf(" %8.2f\n", (gtl15+ttl15)*60.0/timt);
      }

      gtt+=gt;
      ttt+=tt;
      gtlt+=gtl;
      ttlt+=ttl;
      gtl15t+=gtl15;
      ttl15t+=ttl15;

      if (timt > timtt) 
        timtt = timt;
      gt=0.0;
      tt=0.0;
      gtl=0.0;
      ttl=0.0;
      gtl15=0.0;
      ttl15=0.0;
      timt=0.0;
    }
  }

  outgoing+="\n";

  if (timtt) 
  {
    outgoing+=sprintf("%-20s %8.2f ", "Totals", (gtt*60.0)/timtt);
    outgoing+=sprintf(" %8.2f ", (gtt+ttt)*60.0/timtt);
    outgoing+=sprintf(" %8.2f ", (gtlt)*60.0/timtt);
    outgoing+=sprintf(" %8.2f ", (gtlt+ttlt)*60.0/timtt);
    outgoing+=sprintf(" %8.2f ", (gtl15t)*60.0/timtt);
    outgoing+=sprintf(" %8.2f\n", (gtl15t+ttl15t)*60.0/timtt);
  }

  return outgoing;
}

string full_domain_stats(varargs string realdom) 
{
  int i;
  float tim, timt, timtt, time_now;
  float ttl15t,gtl15t;
  float gtt,ttt,gtlt,ttlt;
  float tt, ttl,ttl15,gt,gtl,gtl15;
  string dom, outgoing;
  string *ind, *tmp;

  load_this_ob();
  time_now = (float)TIMEKEEPER->query_running_time()/60.0;
  
  // outgoing="\nDir           grs give  grs take  ggive/lev  gtak/lev  gg/lev<15  gt/lev<15\n";
  outgoing = sprintf("%-20s %9s %9s %9s %9s %9s %9s\n", "Dir", "grs give", "grs take", "ggive/lev", "gtak/lev", "gg/lev<15", "gt/lev<15");
  outgoing += "                      (money in copper, per time in player hours)\n\n";

  if (!mappingp(data)) 
    data = ([ ]); // Taniwha
  
  ind = m_indices(data);

  for (i = 0; i < sizeof(ind); i++) 
  {
    tmp = explode(ind[i],"/");

    if (sizeof(tmp) < 2) 
      continue;

    if (!realdom || tmp[1] == realdom) 
    {
      dom = ind[i];

      if (!sizeof(data[dom])) 
      {
        data = m_delete(data, data[dom]);
        continue;
      }

      tt = data[dom][0];
      ttl = data[dom][1];
      ttl15 = data[dom][2];
      gt = data[dom][5];
      gtl = data[dom][6];
      gtl15 = data[dom][7];

      if (((time_now-data[dom][3])>(float)BASE_WEEK/30.0
        &&(time_now-data[dom][8])>(float)BASE_WEEK/30.0)
        ||(time_now-data[dom][3])<0.0
        ||(time_now-data[dom][8])<0.0)
      {
        data = m_delete(data, dom);
        continue;
      }

      tim = data[dom][9];

      if (data[dom][4]>tim) 
        tim = data[dom][4];
      if (tim > timt) 
        timt = tim;
      // if (dom)
      //   dom = dom[strlen(realdom)+3..]+"/";
      if (timt) 
      {
        outgoing+=sprintf("%-20s %8.2f ", dom, (gt*60.0)/timt);
        outgoing+=sprintf(" %8.2f ", (tt)*60.0/timt);
        outgoing+=sprintf(" %8.2f ", (gtl)*60.0/timt);
        outgoing+=sprintf(" %8.2f ", (ttl)*60.0/timt);
        outgoing+=sprintf(" %8.2f ", (gtl15)*60.0/timt);
        outgoing+=sprintf(" %8.2f\n", (ttl15)*60.0/timt);
      }

      gtt+=gt;
      ttt+=tt;
      gtlt+=gtl;
      ttlt+=ttl;
      gtl15t+=gtl15;
      ttl15t+=ttl15;

      if (timt > timtt) 
        timtt = timt;

      gt=0.0;
      tt=0.0;
      gtl=0.0;
      ttl=0.0;
      gtl15=0.0;
      ttl15=0.0;
      timt=0.0;
    }
  }
  
  outgoing+="\n";

  if (timtt) 
  {
    outgoing+=sprintf("%-20s %8.2f ", "Totals",(gtt*60.0)/timtt);
    outgoing+=sprintf(" %8.2f ", (ttt)*60.0/timtt);
    outgoing+=sprintf(" %8.2f ", (gtlt)*60.0/timtt);
    outgoing+=sprintf(" %8.2f ", (ttlt)*60.0/timtt);
    outgoing+=sprintf(" %8.2f ", (gtl15t)*60.0/timtt);
    outgoing+=sprintf(" %8.2f\n", (ttl15t)*60.0/timtt);
  }

  return outgoing;
}

float *mudwide_sums() {
  int i;
  float *vals;
  string *ind, *tmp;
  string dom;
  float time_now;
  time_now = TIMEKEEPER->query_running_time()/60.0;
  load_this_ob();
  if (!mappingp(data)) data = ([ ]); // Taniwha
  vals = ({ 0.0,0.0,0.0,0.0,0.0,0.0,0.0 });
  ind = m_indices(data);
  for (i=0;i<sizeof(ind);i++) {
    tmp = explode(ind[i],"/");
    if (sizeof(tmp) < 2) continue;
      dom = ind[i];
        if (((time_now-data[dom][3])>(float)BASE_WEEK/30.0
          &&(time_now-data[dom][8])>(float)BASE_WEEK/30.0)
          ||(time_now-data[dom][3])<0.0
          ||(time_now-data[dom][8])<0.0)
         {
          data = m_delete(data,dom);
          continue;
        }
         vals[0] += 0.002*data[dom][0];
         vals[1] += 0.002*data[dom][1];
         vals[2] += 0.002*data[dom][2];
         vals[3] += 0.002*data[dom][5];
         vals[4] += 0.002*data[dom][6];
         vals[5] += 0.002*data[dom][7];
         if (data[dom][4]>vals[6]) vals[6]=data[dom][4];
         if (data[dom][9]>vals[6]) vals[6]=data[dom][9];
  }
  return vals;
}

float query_adj_fact(int arg) {
  float *vals;
  float adj;
  float base;
  float time_now;
  time_now=TIMEKEEPER->query_running_time();
  load_this_ob();
  if (last_adj == 0.0) {
       last_adj=time_now;
       save_this_ob();
       return 1000.0;
     }
  if (!mult) mult = ({ 0.0,0.0,0.0,0.0 });
  if (time_now>2000000.0+last_adj) {
  vals = mudwide_sums();
  if (vals[0]==0.0) return 1000.0;
  base=(float)MONEYSCALE*(vals[0]+vals[3])/(-vals[0]);
    adj=base*SSFRAC;
    if (adj>SSMAXC) adj = SSMAXC;
    if (adj<0.0-SSMAXC) adj = 0.0-SSMAXC;
    mult[SSFLAG] += adj;
    if (mult[SSFLAG]>SSMAXM) mult[SSFLAG]=SSMAXM;
    if (mult[SSFLAG]<SSMINM) mult[SSFLAG]=SSMINM;

    adj=base*MONFRAC;
    if (adj>MONMAXC) adj = MONMAXC;
    if (adj<0.0-MONMAXC) adj = 0.0-MONMAXC;
    mult[MONFLAG] -= adj;
    if (mult[MONFLAG]>MONMAXM) mult[MONFLAG]=MONMAXM;
    if (mult[MONFLAG]<MONMINM) mult[MONFLAG]=MONMINM;

    adj=base*SBFRAC;
    if (adj>SBMAXC) adj = SBMAXC;
    if (adj<0.0-SBMAXC) adj = 0.0-SBMAXC;
    mult[SBFLAG] -= adj;
    if (mult[SBFLAG]>SBMAXM) mult[SBFLAG]=SBMAXM;
    if (mult[SBFLAG]<SBMINM) mult[SBFLAG]=SBMINM;

    adj=base*TAVFRAC;
    if (adj>TAVMAXC) adj = TAVMAXC;
    if (adj<0.0-TAVMAXC) adj = 0.0-TAVMAXC;
    mult[TAVFLAG] += adj;
    if (mult[TAVFLAG]>TAVMAXM) mult[TAVFLAG]=TAVMAXM;
    if (mult[TAVFLAG]<TAVMINM) mult[TAVFLAG]=TAVMINM;

  log_file(INFLATION_LOG_FILE, ctime(time())+"\n"+mult[0]+
      ", "+mult[1]+", "+mult[2]+", "+mult[3]+", "+base+"\n");
  last_adj = time_now;
  save_this_ob();
  }
  return 1000.0+mult[arg];
}

int update_money_given(float amount, object player)
{
  mixed vals;
  string domname;
  float temp,plev;
  float time_now, time_since, total_time, time_extra;

  if (!(domname = environment_path(player)))
     return 1;

  load_this_ob();
  vals = data[domname];

  if (!vals) 
    vals = ({0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0});
  
  if (player->query_coder() || 
      strsrch(player->query_name(),"test") != -1) 
    return 1;

  time_now = (float)TIMEKEEPER->query_running_time()/60.0;

  if (!vals[8]) 
    vals[8] = time_now;

  time_since = time_now - vals[8];

  if (time_since < 0.0) 
    time_since = 0.0;

  if (time_since > (float)BASE_WEEK/30.0) 
  {
    vals[5]=0.0;
    vals[6]=0.0;
    vals[7]=0.0;
    vals[8]=time_now;
    vals[9]=(float)BASE_WEEK/30.0;
  }
  else 
  {
    total_time = vals[9]+time_since;
    if (total_time > (float)BASE_WEEK/30.0) 
    {
      time_extra = total_time - (float)BASE_WEEK/30.0;
      temp = vals[5]*((vals[9]-time_extra)*10.0/vals[9])/10;
      vals[5] = temp;
      temp = vals[6]*((vals[9]-time_extra)*10.0/vals[9])/10;
      vals[6] = temp;
      temp = vals[7]*((vals[9]-time_extra)*10.0/vals[9])/10;
      vals[7] = temp;
      temp = (float)BASE_WEEK/30.0;
      vals[9] = temp;
    }
    else 
      vals[9] = total_time;
    vals[8] = time_now;
  }
  
  if (vals[9] < 0.0) 
    vals[9] = (float)BASE_WEEK/30.0;
  vals[5] += amount;
  plev = (float)player->query_level();
  vals[6] = (vals[6]*plev+amount)/plev;
  if (plev<16.0) 
    vals[7] =(vals[7]*plev+amount)/plev;
  
  data[domname] = vals;
  save_this_ob();

  return 1;
}

void update_money_taken(float amount, object player)
{
  mixed vals;
  string domname;
  float temp, plev;
  float time_now, time_since, total_time, time_extra;

  if (player->query_coder() || 
      strsrch(player->query_name(),"test") != -1) 
    return;

  if (!(domname = environment_path(player)))
    return;

  plev = (float)player->query_level();
  load_this_ob();
  vals = data[domname];

  if (!vals) 
    vals = ({0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0});
  
  time_now = (float)TIMEKEEPER->query_running_time()/60.0;

  if (!vals[3]) 
    vals[3] = time_now;

  time_since = time_now - vals[3];

  if (time_since < 0.0) 
    time_since = 0.0;

  if (time_since > (float)BASE_WEEK/30.0) 
  {
    vals[0]=0.0;
    vals[1]=0.0;
    vals[2]=0.0;
    vals[3]=time_now;
    vals[4]=(float)BASE_WEEK/30.0;
  }
  else 
  {
    total_time = vals[4]+time_since;

    if (total_time > (float)BASE_WEEK/30.0) 
    {
      time_extra = total_time - (float)BASE_WEEK/30.0;
      temp = vals[0]*((vals[4]-time_extra)*10.0/vals[4])/10;
      vals[0] = temp;
      temp = vals[1]*((vals[4]-time_extra)*10.0/vals[4])/10;
      vals[1] = temp;
      temp = vals[2]*((vals[4]-time_extra)*10.0/vals[4])/10;
      vals[2] = temp;
      temp = (float)BASE_WEEK/30.0;
      vals[4] = temp;
    }
    else 
      vals[4] = total_time;
    vals[3] = time_now;
  }
  
  if (vals[4] < 0.0) 
   vals[4] = (float)BASE_WEEK/30.0;

  vals[0] += amount;
  vals[1] = (vals[1]*plev+amount)/plev;

  if (plev<16.0) 
   vals[2] = (vals[2]*plev+amount)/plev;
  
  data[domname] = vals;
  save_this_ob();

  return;
}

int update_statistics(int amount, object player)
{
  // if (environment(player)&&(amount>5000 || amount<0-5000))
  //   tell_object(find_player("anirudh"),player->query_name()+" "+amount);

  if (player && 
    environment(player) && 
    (amount>250000||amount<0-250000))
  {
    log_file(MONEY_TRACKER_LOG, player->query_name() + ", " + 
            file_name(environment(player))+
            "\n" + amount + ", " + ctime(time()) + "\n");
  }

  if (player && environment(player) &&
    !(environment(player)->query_treasury()) &&
    !(environment(player)->query_bank())) 
  {
    if (interactive(player) && amount < 0) 
    {
      update_money_taken((float)amount, player);
      return 1;
    }

    if (interactive(player) && amount > 0) 
      return update_money_given((float)amount, player);
    
    return 1;
  }

  return 0;
}
