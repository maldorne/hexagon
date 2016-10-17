
#include <common/properties.h>
#include <living/drinks.h>
#include <living/combat.h>
#include <common/quests.h>
#include <mud/secure.h>

// Logging Taniwha 1995
#define XP_LIMIT 15000
#define XP_LOGFILE "xp_log"

int max_hp, max_gp, total_xp, wimpy;
int hp, xp, gp;

private int * drink_info;
private int display_monitor_handle;

static mapping damage_done;
static mapping aggro_done;

void create() 
{
  damage_done = ([ ]);
  aggro_done = ([ ]);
  drink_info = allocate(D_SIZEOF);
  max_gp = 1;
  max_hp = 1;
  display_monitor_handle = 0;
}

int query_hp() { return hp; }
int query_max_hp() { return max_hp; }
int query_gp() { return gp; }
int query_max_gp() { return max_gp; }
int query_xp() { return xp; }
int query_total_xp() { return total_xp; }
mapping query_damage_done() { return damage_done; }
mapping query_aggro_done() { return aggro_done; }

int set_wimpy(int i)
{
  int j;
  j = this_object()->query_property(FEAR_PROP);
  if (i < j) i = j;
  if (i <0 || i > 100)
    return -1;
  return (wimpy = i);
}

int query_wimpy() { return wimpy; }

int set_max_hp(int i) 
{
  if (max_hp)
    hp = hp*i/max_hp;
  else
    hp = max_hp;
  max_hp = i;
  if (max_hp==1)
    max_hp = 2;
  if (hp>max_hp)
    hp = max_hp;
  return max_hp;
}

// Made varargs : Radix - March 3, 1997
int set_hp(int i, varargs object hp_remover)
{
  if (i>max_hp)
    i = max_hp;
  hp = i;
  if (hp<0)
    call_out("do_death", 0,  hp_remover);
  return hp;
}

void end_practise(object loser, object winner)
{
  tell_player(loser, "El golpe de "+winner->query_cap_name()+" te deja incapacitado. Caes al suelo inconsciente.\n");
  tell_player(winner, "Tu contrincante cae inconsciente, has ganado la pelea.\n");
  loser->stop_fight(winner);
  winner->stop_fight(loser);    
  // 5 hbs == 10 segundos
  loser->add_timed_property(PASSED_OUT_PROP, "Estás inconsciente.\n", 5);
}

int adjust_aggro(int i, object aggro_doer)
{
  if (aggro_doer && !(int)aggro_doer->query_dead() )
  {
    aggro_done[aggro_doer] += i;
  } 

  return aggro_done[aggro_doer];
}

string query_main_aggro_doer()
{
  string doer;
  string * atts;
  int i, max;
  
  doer = nil;
  atts = this_object()->query_attacker_list();
  max = 0; // Los valores de aggro son negativos!!

  for (i = 0; i < sizeof(atts); i++)
  {
    if (aggro_done[atts[i]] < max)
    {
      doer = atts[i];
      max = aggro_done[atts[i]];
    }
  }

  // En el caso de que este comenzando combates
  if (sizeof(atts) && !doer)
    doer = atts[0];

  return doer;
}

int adjust_hp(int i, object hp_remover)
{
  int color;
  hp += i;

  color = 1;    // for color on hp monitor - Radix

  if (hp_remover && !(int)hp_remover->query_dead() )
  {
    damage_done[hp_remover] += i;
    aggro_done[hp_remover] += i;
  }

  if ( hp > max_hp )
    hp = max_hp;

  if ( hp < 0 ) /* eeek we died! */
  {
    // Combate seguro, damos por terminado el entrenamiento
    if ( // (sizeof(this_object()->query_attacker_list()) == 1) &&  
      hp_remover && (hp_remover->query_combat_mode() == SAFE_COMBAT))
    {
      hp = 1;
      this_object()->stop_fight(hp_remover);
      hp_remover->stop_fight(this_object());
      call_out("end_practise", 0, this_object(), hp_remover);
      
      // Quests de vencer en pelea (combate no letal)
      if (hp_remover->is_doing_quest(base_name(this_object()), TYPE_BEAT) )
        hp_remover->update_quest(base_name(this_object()));
      else if (hp_remover->is_doing_quest_from_name(this_object()->query_short(), TYPE_BEAT) )
        hp_remover->update_quest_from_name(this_object()->query_short());
    }
    else // combate no seguro, ha muerto
    {
      if (i > 0 && hp_remover == this_object() ) 
        call_out("do_death", 0, 0); /* NOT this object, use attacker list */
      else
        call_out("do_death", 0, hp_remover);
    }   
  }

  // Radix Jan 1996
  if (hp < max_hp/5)
    color = 0;

  if (interactive(this_object()))
  {
    remove_call_out(display_monitor_handle);
    display_monitor_handle = call_out("display_monitor", 0, color);
  }
  return hp;
}

int set_max_gp(int i) 
{
  if (max_gp)
    gp = gp*i/max_gp;
  else
    gp = max_gp;
  max_gp = i;
  if (gp>max_gp)
    gp = max_gp;
  return max_gp;
}

int set_gp(int i) 
{
  gp = i;
  if (gp > max_gp)
    gp = max_gp;
  return gp;
}

int adjust_gp(int i) 
{
  if (gp + i < 0)
    return -1;
  gp += i;
  if (gp>max_gp)
    gp = max_gp;
  return gp;
}

void logit(string what,int amount)
{
  if (interactive(this_object()))
  {
    log_file(XP_LOGFILE, "["+ctime(time(),4)+"] " + (string)this_object()->query_cap_name()+" ganó "+amount+" "+what+
      " de: "+previous_object()->query_name()+", archivo: "+file_name(previous_object())+
      "\n");
  }
}

// Nuevo sistema de ajuste automatico de niveles, Folken 7/03
int adjust_xp(int i)
{
  object ob;
  int next_level;

  if (!SECURE->valid_adjust_xp(previous_object(), this_object(), i))
    return xp;

  if (i > XP_LIMIT)
  { 
    logit("xp", i);
    i = XP_LIMIT;
  }

  if (i > 0) 
  {
    i = (100-wimpy)*i/100;
    total_xp += i;
  }

  xp += i;

  /* 
  // Cambiado, ahora la subida de nivel de gremios es por quest,
  // Folken 06/2010
  
  ob = this_object()->query_guild_ob();
  if (ob && (i > 0)) 
  {
    next_level = ob->query_next_level_xp(this_object());
    if ((xp > next_level) && 
      (this_object()->query_guild_level() < ob->query_max_level()) )
    {
      this_object()->adjust_guild_level(1);
      xp -= next_level;
      this_object()->save();
    }
  }
  */

  ob = this_object()->query_class_ob();
  if (ob && (i > 0)) 
  {
    next_level = ob->query_next_level_xp(this_object());
    if ((xp > next_level) && 
      (this_object()->query_class_level() < ob->query_max_level()) )
    {
      this_object()->adjust_level(1);
      xp -= next_level;
      this_object()->save();
    }
  }

  return xp;
}

/* int set_xp(int i) { xp = i; return xp; } */
int set_xp(int i) 
{
  // Radix : Aug 1997
  if (!SECURE->valid_adjust_xp(previous_object(), this_object(), i))
    return xp;

  if ( (i-xp) > XP_LIMIT) 
    logit("xp",(i-xp));

  if ( (xp-i) > XP_LIMIT) 
    logit("xp", -(xp-i));

  if (i == -1 || !i) 
    logit("xp", i);

  xp = i;
  return xp;
}

// Añadido self, Folken 02/2006
string health_string(int self) 
{
  if (!self)
  {
    if (hp < max_hp/20)
      return "está al borde la muerte";
    if (hp < max_hp/10)
      return "está en muy mal estado";
    if (hp < max_hp/5)
      return "está en mal estado";
    if (hp < max_hp/2)
      return "no está en muy buen estado";
    if ((float)hp < (float)max_hp/1.2)
    {
      if (this_object()->query_gender() == 2) 
        return "está ligeramente herida";
      else
        return "está ligeramente herido";
    }
    if (hp == max_hp)
      return "está en perfecto estado";
    return "está en buen estado";
  }
  else
  {
    if (hp < max_hp/20)
      return "Estás al borde la muerte";
    if (hp < max_hp/10)
      return "Estás en muy mal estado";
    if (hp < max_hp/5)
      return "Estás en mal estado";
    if (hp < max_hp/2)
      return "No estás en muy buen estado";
    if ((float)hp < (float)max_hp/1.2)
    {
      if (this_object()->query_gender() == 2) 
        return "Estás ligeramente herida";
      else
        return "Estás ligeramente herido";
    }
    if (hp == max_hp)
      return "Estás en perfecto estado";
    return "Estás en buen estado";
  }
}

// Funciones relativas a bebidas
int adjust_volume(int type, int amt) 
{
  if (!pointerp(drink_info))
    drink_info = allocate(D_SIZEOF);
  if (type >= sizeof(drink_info))
    return 0;
  return drink_info[type] += amt;
}

int query_volume(int type) 
{
  if (!pointerp(drink_info))
    drink_info = allocate(D_SIZEOF);
  if (type >= sizeof(drink_info))
    return 0;
  return drink_info[type];
}

void update_volumes() 
{
  int i;

  if (!drink_info)
    return ;
  for (i=0;i<sizeof(drink_info);i++)
    if (drink_info[i] > 0)
    drink_info[i]--;
  else
    drink_info[i] = 0;
}

string volume_string()
{
  int i;
  i = 0;

  if (drink_info[i]<= 0)
    return "sobrio";
  if (drink_info[i] <= 50)
    return "alegre";
  if (drink_info[i] <= 100)
    return "ciego";
  if (drink_info[i] <= 500)
    return "muy ciego";
  if (drink_info[i] <= 2000)
    return "muerto para el mundo";
  if (drink_info[i] <= 6000)
    return "cerca de la muerte";
  return "reza por una muerte sin dolor";
}

// stats añadido
mixed stats() {

  mixed * ret;
  ret = ({ ({"Max Hp", max_hp, }),
    ({"Hp", hp, }),
    ({"Max Gp", max_gp, }),
    ({"Gp", gp, }),
    ({"Xp", xp, }),
    ({"Total Xp", total_xp, }),
    ({"Wimpy", wimpy, }),
    ({"Damage Done (nosave)", damage_done, }),
    ({"Aggro Done (nosave)", aggro_done, }),
    ({"Drink Info", drink_info, }),
    });
  return ret;
}
