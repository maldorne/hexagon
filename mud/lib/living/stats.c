/* The stat data for the living objects... 
 *
 * Added extra_defend bonus calculations.
 * this will add to the body_ac, right now it's the only 
 * body modifier. It is based on DEX 
 * Baldrick, april '94 
 * Also addin attack_bonus.
 * Adding thac0 here. Skills.c is too heavy already. 
 * Adding kill_xp here for now..
 *
 * - Añadido sistema de bonos por caracteristica.
 * - Eliminado thac0, ahora hay un nuevo sistema de combate.
 * - Funciones del antiguo stats_table.c ahora eliminadas (la subida de hps y gps se
 *   calcula de otra forma).
 * - Eliminadas dos variables mas, hp_bonus y gp_bonus. 
 * neverbot Julio'03
 *
 * neverbot metio mano aqui para reducir el numero de variables drasticamente
 * Y si, la solucion es mala, pero por algun sitio hay que reducir variables!
 * - Eliminado nivel de agresividad.
 * - Antiguo /table/stats_table.c ahora añadido aqui (no se utilizaba en ningun
 *   otro lugar).
 * - Añadido sistema de resistencias como parte de los stats, mediante mapping
 *   originalmente sin definir (para no limitar el tipo de daños que podremos hacer en
 *   el futuro). Tendremos resistances[tipo] para las basicas y tmp_resistances[tipo]
 *   para las temporales (por armas, hechizos, etc).
 * neverbot@Cc, Abril'04
 *
 * Añadidas dos nuevas caracteristicas para definir a cada personaje:
 *  Percepcion y Suerte
 * Generalmente se ha usado sabiduria en los casos en que era necesaria la percepcion
 *  (buscar, etc) y carisma como suerte. Ahora la ficha es algo mas completa.
 * neverbot 3/2005
 *
 * Añadidas estadisticas relativas al oficio, antiguo /global/living/jobs.c, neverbot 10/08
 * Eliminamos el azar en los hps y gps que se ganan al subir de nivel, neverbot 06/12
 * 
 */

#include <common/properties.h>
#include <living/stats.h>

void recalc_stats(int i);
void calc();
void reset_carry_cap();
void reset_bon_stats();
void reset_all();
int set_ac(int i);
int set_extreme_str(int i);
int set_str(int i);
int set_con(int i);
int set_dex(int i);
int set_int(int i);
int set_wis(int i);
int set_cha(int i);
int set_per(int i); // percepcion (perception)
int set_wil(int i); // suerte (luck)
int adjust_str(int i);
int adjust_con(int i);
int adjust_dex(int i);
int adjust_int(int i);
int adjust_wis(int i);
int adjust_cha(int i);
int adjust_per(int i);
int adjust_wil(int i);
int adjust_tmp_str(int i);
int adjust_tmp_con(int i);
int adjust_tmp_dex(int i);
int adjust_tmp_int(int i);
int adjust_tmp_wis(int i);
int adjust_tmp_per(int i);
int adjust_tmp_wil(int i);
int adjust_bonus_str(int i);
int adjust_bonus_con(int i);
int adjust_bonus_dex(int i);
int adjust_bonus_int(int i);
int adjust_bonus_wis(int i);
int adjust_bonus_cha(int i);
int adjust_bonus_per(int i);
int adjust_bonus_wil(int i);

int adjust_tmp_ac_bon(int i);
int adjust_tmp_damage_bon(int i);
int adjust_tmp_tohit_bon(int i);
int adjust_thac0(int i);
void set_kill_xp(int bing);
int query_kill_xp();
void update_tmps();
int query_stat_bonus_to_con();
private int query_stat_bonus(int num); 
int query_gp_main_stat();

/* Cambio mas importante del archivo, neverbot Abril'04
 * Para reducir el numero de variables, cada lista de stats
 *  la introducimos en un unico array.
 * Si nos ponemos realistas este proceso sera mas lento que 
 *  tener una variable para cada stat, pero ahora mismo es necesario
 *  porque se ha alcanzado el maximo de variables para un player y
 *  se hace imposible programar nada nuevo :(
 *
 * int intbon, dexbon, conbon, strbon, wisbon, chabon;
 * int Dex, Int, Con, Str, Wis, Cha,
 *   inttmp, dextmp, contmp, wistmp, strtmp, chatmp;
 *
 */
int * stats_bon;
int * stats_base;
private int * stats_tmp;
// Bonificador temporales a los bonificadores de caracteristica :(
private int * bonus_to_stat_bonus; 

int damage_bonus, tohit_bonus, body_ac;
int extreme_str, kill_xp; 
private int tmp_ac_bon, tmp_damage_bon, tmp_tohit_bon, body_ac_bon;

// Añadidas resistencias como parte de los stats basicos
mapping resistances;
private mapping tmp_resistances;

void create()
{
  int i;
  damage_bonus = 0;
  tohit_bonus = 0;
  body_ac_bon = 0;
  body_ac = 0;
  extreme_str = 0;
  kill_xp = 0;
  tmp_ac_bon = 0;
  tmp_damage_bon = 0;
  tmp_tohit_bon = 0;
  
  resistances = ([ ]);
  tmp_resistances = ([ ]);
  
  stats_base = allocate_int(NUM_STATS);
  stats_bon = allocate_int(NUM_STATS);
  stats_tmp = allocate_int(NUM_STATS);
  bonus_to_stat_bonus = allocate_int(NUM_STATS);

  for (i = 0; i < NUM_STATS; i++)
  {
    stats_base[i] = 10;
    stats_bon[i] = 0;
    stats_tmp[i] = 0;
    bonus_to_stat_bonus[i] = 0;
  }  
}

int query_str() { return stats_base[STR_POS]+stats_tmp[STR_POS]+stats_bon[STR_POS]+extreme_str; }
int query_int() { return stats_base[INT_POS]+stats_tmp[INT_POS]+stats_bon[INT_POS]; }
int query_dex() { return stats_base[DEX_POS]+stats_tmp[DEX_POS]+stats_bon[DEX_POS]; }
int query_wis() { return stats_base[WIS_POS]+stats_tmp[WIS_POS]+stats_bon[WIS_POS]; }
int query_con() { return stats_base[CON_POS]+stats_tmp[CON_POS]+stats_bon[CON_POS]; }
int query_cha() { return stats_base[CHA_POS]+stats_tmp[CHA_POS]+stats_bon[CHA_POS]; }
int query_per() { return stats_base[PER_POS]+stats_tmp[PER_POS]+stats_bon[PER_POS]; }
int query_wil() { return stats_base[WIL_POS]+stats_tmp[WIL_POS]+stats_bon[WIL_POS]; }

int query_real_str() { return stats_base[STR_POS]; }
int query_real_int() { return stats_base[INT_POS]; }
int query_real_dex() { return stats_base[DEX_POS]; }
int query_real_wis() { return stats_base[WIS_POS]; }
int query_real_con() { return stats_base[CON_POS]; }
int query_real_cha() { return stats_base[CHA_POS]; }
int query_real_per() { return stats_base[PER_POS]; }
int query_real_wil() { return stats_base[WIL_POS]; }

int query_tmp_str() { return stats_tmp[STR_POS]; }
int query_tmp_int() { return stats_tmp[INT_POS]; }
int query_tmp_dex() { return stats_tmp[DEX_POS]; }
int query_tmp_wis() { return stats_tmp[WIS_POS]; }
int query_tmp_con() { return stats_tmp[CON_POS]; }
int query_tmp_cha() { return stats_tmp[CHA_POS]; }
int query_tmp_per() { return stats_tmp[PER_POS]; }
int query_tmp_wil() { return stats_tmp[WIL_POS]; }

int query_bonus_str() { return stats_bon[STR_POS]; }
int query_bonus_int() { return stats_bon[INT_POS]; }
int query_bonus_dex() { return stats_bon[DEX_POS]; }
int query_bonus_wis() { return stats_bon[WIS_POS]; }
int query_bonus_con() { return stats_bon[CON_POS]; }
int query_bonus_cha() { return stats_bon[CHA_POS]; }
int query_bonus_per() { return stats_bon[PER_POS]; }
int query_bonus_wil() { return stats_bon[WIL_POS]; }

int query_extreme_str()   { return extreme_str;   }
int query_tmp_tohit_bon() { return tmp_tohit_bon; }
int query_tmp_damage_bon(){ return tmp_damage_bon;}
int query_tmp_ac_bon()  { return tmp_ac_bon;  }
int query_damage_bonus()  { return damage_bonus + tmp_damage_bon; } 
int query_tohit_bonus()   { return tohit_bonus + tmp_tohit_bon; } 

int query_real_body_ac()  { return body_ac;   }
int query_body_ac()     { return body_ac + body_ac_bon + tmp_ac_bon; }

string stat_string(int i) 
{
  if (i<19)
    return i +"";
  if (i>28)
    return i-10+"";
  if (i==28)
    return "18/00";
  return "18/"+(i-18)+"0";
}

/* This is ugly.. 
 * But a test right now..
 * This does calculate the adjustments of max_gp & max_hp 
 * Baldrick march '95 (last change)
 */
void recalc_stats(int i)
{
  object class_ob;
  int aux, aux2, e;
  
  int hp_bonus, gp_bonus;
  
  class_ob = (object)this_object()->query_class_ob();
  
  // Cambiado, ahora hp_bonus y gp_bonus funcionan por los bonificadores segun
  //  caracteristica, neverbot 7/03
  hp_bonus = query_stat_bonus_to_con() + 
       (class_ob?class_ob->query_hp_bonus():0);
  gp_bonus = query_stat_bonus(query_gp_main_stat()) + 
       (class_ob?class_ob->query_gp_bonus():0);

  calc();
  for (e = 0; e < i; e++)
  {
    if(class_ob)
    {
      // Eliminamos el azar en los puntos de vida y gps ganados por nivel, neverbot 06/12
  
      // En el primer nivel se obtienen todos los dados de golpe
      // if (this_object()->query_level() == 0)
      // {
        aux = class_ob->query_hit_dice();
        aux2 = class_ob->query_gp_dice();
      // }
      // else 
      // {
      //   aux = roll(1, class_ob->query_hit_dice()/2) + class_ob->query_hit_dice()/2;
      //   aux2 = roll(1, class_ob->query_gp_dice()/2) + class_ob->query_gp_dice()/2;
      // }
    }
    else // Newbies y/o NPCs sin clase
    {
      if(interactive(this_object()))
        aux = aux2 = roll(1, 6); // + 4; 
      else
        aux = aux2 = roll(1, 8);
    }

    aux += hp_bonus;
    aux2 += gp_bonus;
    if (aux <= 0) 
      aux = 1;
    if (aux2 <= 0) 
      aux2 = 1;

    this_object()->set_max_hp(this_object()->query_max_hp() + aux);
    this_object()->set_max_gp(this_object()->query_max_gp() + aux2);
    
    // Logeamos en /secure, para que nadie pueda ver estas cosas
    // Argh! pero solo players!!!
    if (interactive(this_object()))
    log_file("recalc_stats", this_object()->query_cap_name() + 
         " sube: "+aux+" pvs y "+aux2+" pgs ["+
         ctime(time()) + "].\n");
  }
}

void reset_carry_cap()
{
  int old_cap, cap, st;
  object ob, ob1;

  old_cap = (int)this_object()->query_loc_weight();
  st = this_object()->query_str(); // fix encumb bug with extreme strength

  if (st < 8)
    cap = 500;
  else if (st > 28)
    cap = 3800+(st-28)*300;
  else
    cap = ({ 500, 550, 600, 650, 750, 850, 950, 1150, 1300, 1450, 1600,
      1750,1900,2100,2300,2500,2700,2950,3200,3500,3800})[st-8];

  this_object()->set_max_weight(cap);

  if (cap >= old_cap)
    return ;

  ob = first_inventory(this_object());

  while (ob) 
  {
    ob1 = next_inventory(ob);
    if(ob && ob->query_weight() && !ob->query_property(CURSED_PROP))
      if(!ob->drop() && ob->move(environment())) 
      {
        old_cap -= (int)ob->query_weight();
        tell_room(environment(this_object()),
            this_object()->query_cap_name()+" deja caer su "+ob->short()+
            ".\n",({this_object()}));
        tell_object(this_object(), "Tu debilitada de fuerza hace que dejes caer tu "+
            ob->short()+".\n");
      }
    if (cap >= old_cap)
      return ;
    ob = ob1;
  }
}

void reset_bon_stats()
{
  stats_bon[STR_POS] = 0;
  stats_bon[DEX_POS] = 0;
  stats_bon[CON_POS] = 0;
  stats_bon[INT_POS] = 0;
  stats_bon[WIS_POS] = 0;
  stats_bon[CHA_POS] = 0;
  stats_bon[PER_POS] = 0;
  stats_bon[WIL_POS] = 0;
}

void reset_all()
{
  int i;

  stats_tmp[STR_POS] = 0;
  stats_tmp[DEX_POS] = 0;
  stats_tmp[CON_POS] = 0;
  stats_tmp[WIS_POS] = 0;
  stats_tmp[INT_POS] = 0;
  stats_tmp[CHA_POS] = 0;
  stats_tmp[PER_POS] = 0;
  stats_tmp[WIL_POS] = 0;

  bonus_to_stat_bonus[STR_POS] = 0;
  bonus_to_stat_bonus[DEX_POS] = 0;
  bonus_to_stat_bonus[CON_POS] = 0;
  bonus_to_stat_bonus[WIS_POS] = 0;
  bonus_to_stat_bonus[INT_POS] = 0;
  bonus_to_stat_bonus[CHA_POS] = 0;
  bonus_to_stat_bonus[PER_POS] = 0;
  bonus_to_stat_bonus[WIL_POS] = 0;
  
  // Eliminamos resistencias temporales
  for (i = 0; i < sizeof(keys(tmp_resistances)); i++)
    map_delete(tmp_resistances, keys(tmp_resistances)[i]);

  reset_carry_cap();
  calc();
}

int set_str(int i)
{
  stats_base[STR_POS] = i;
  reset_carry_cap();
  calc();
  return 1;
}

int set_con(int i)
{
  stats_base[CON_POS] = i;
  calc();
  return 1;
}

int set_int(int i)
{
  stats_base[INT_POS] = i;
  calc();
  return 1;
}

int set_dex(int i)
{
  stats_base[DEX_POS] = i;
  calc();
  return 1;
}

int set_wis(int i)
{
  stats_base[WIS_POS] = i;
  calc();
  return 1;
}

int set_cha(int i)
{
  stats_base[CHA_POS] = i;
  calc();
  return 1;
}

int set_per(int i)
{
  stats_base[PER_POS] = i;
  calc();
  return 1;
}

int set_wil(int i)
{
  stats_base[WIL_POS] = i;
  calc();
  return 1;
}

int set_extreme_str(int i)
{
  extreme_str = i;
  calc();
  return 1;
}

int set_body_ac(int i)
{
  body_ac = i;
  return body_ac;
}

int adjust_dex(int i)
{
  stats_base[DEX_POS] += i;
  if (query_dex() < 1)
  {
    write("Sientes que tu destreza es muy baja y, siendo imposible moverte, "+ 
      "acabas muriendo.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  calc();
  return stats_base[DEX_POS];
}

int adjust_con(int i)
{
  stats_base[CON_POS] += i;
  if (query_con() < 1)
  {
    write("Tu debilitado corazón provoca tu muerte.\n");
    this_object()->do_death(this_object());
    return 0;
  }

  calc();
  return stats_base[CON_POS];
}

int adjust_str(int i)
{
  stats_base[STR_POS] += i;
  if (query_str() < 1)
  {
    write("Te sientes tan débil que ni siquiera puedes seguir viviendo.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  reset_carry_cap();
  calc();
  return stats_base[STR_POS];
}

int adjust_wis(int i)
{
  stats_base[WIS_POS] += i;
  if (query_wis() < 1)
  {
    write("Tu alma es separada de tu cuerpo al desaparecer tu energía vital.\n");
    this_object()->do_death(this_object());
    return 0;
  }

  calc();
  return stats_base[WIS_POS];
}

int adjust_int(int i)
{
  stats_base[INT_POS] += i;
  if (query_int() < 1)
  {
    write("Eres demasiado estúpido para seguir viviendo.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  calc();
  return stats_base[INT_POS];
}

int adjust_cha(int i)

{
  stats_base[CHA_POS] += i;
  if (query_cha() < 1)
  {
    write("Eres tan feo que necesitas morir para remediarlo.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  calc();
  return stats_base[CHA_POS];
}

int adjust_per(int i)
{
  stats_base[PER_POS] += i;
  if (query_per() < 1)
  {
    write("Te has muerto y no te has dado cuenta.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  calc();
  return stats_base[PER_POS];
}

int adjust_wil(int i)
{
  stats_base[WIL_POS] += i;
  if (query_wil() < 1)
  {
    write("Tienes tan poca fuerza de voluntad que acabas muriendo sin poder evitarlo.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  calc();
  return stats_base[WIL_POS];
}

int adjust_tmp_int(int i)
{
  stats_tmp[INT_POS] += i;
  if (query_int() < 1)
  {
    write("Eres demasido estúpido para seguir viviendo.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  //reset_gp();
  calc();
  return stats_tmp[INT_POS];
}

int adjust_tmp_dex(int i)
{
  stats_tmp[DEX_POS] += i;
  if (query_dex() < 1)
  {
    write("Sientes que tu destreza muy es baja y, siendo imposible moverte, " +
      "acabas muriendo.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  calc();
  return stats_tmp[DEX_POS];
}

int adjust_tmp_wis(int i) 
{
  stats_tmp[WIS_POS] += i;
  if (query_wis() < 1)
  {
    write("Tu alma es separada de tu cuerpo al desaparecer tu energía vital.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  calc();
  return stats_tmp[WIS_POS];
}

int adjust_tmp_str(int i)
{
  stats_tmp[STR_POS] += i;
  if (query_str() < 1)
  {
    write("Te sientes tan débil que ni siquiera puedes seguir viviendo.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  reset_carry_cap();
  calc();
  return stats_tmp[STR_POS];
}

int adjust_tmp_con(int i)
{
  stats_tmp[CON_POS] += i;
  if (query_con() < 1)
  {
    write("Tu debilitado corazón provoca tu muerte.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  calc();
  return stats_tmp[CON_POS];
}

int adjust_tmp_cha(int i) 
{
  stats_tmp[CHA_POS] += i;
  if (query_cha() < 1)
  {
    write("Eres tan feo que necesitas morir para remediarlo.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  calc();
  return stats_tmp[CHA_POS];
}

int adjust_tmp_per(int i)
{
  stats_tmp[PER_POS] += i;
  if (query_per() < 1)
  {
    write("Te has muerto y no te has dado cuenta.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  calc();
  return stats_tmp[PER_POS];
}

int adjust_tmp_wil(int i) 
{
  stats_tmp[WIL_POS] += i;
  if (query_wil() < 1)
  {
    write("Tienes tan poca fuerza de voluntad que acabas muriendo sin poder evitarlo.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  calc();
  return stats_tmp[WIL_POS];
}

int adjust_bonus_int(int i) 
{
  stats_bon[INT_POS] += i;
  if (query_int() < 1)
  {
    write("Eres demasiado estúpido para seguir viviendo.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  calc();
  return stats_bon[INT_POS];
}

int adjust_bonus_dex(int i) 
{
  stats_bon[DEX_POS] += i;
  if (query_dex() < 1)
  {
    write("Sientes que tu destreza es muy baja y, siendo imposible moverte, " +
      "acabas muriendo.\n");
    this_object()->do_death(this_object());
    return 0;
  }

  calc();
  return stats_bon[DEX_POS];
}

int adjust_bonus_wis(int i) 
{
  stats_bon[WIS_POS] += i;
  if (query_wis() < 1)
  {
    write("Tu alma es separada de tu cuerpo al desaparecer tu energía vital.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  calc();
  return stats_bon[WIS_POS];
}

int adjust_bonus_str(int i) 
{
  stats_bon[STR_POS] += i;
  if (query_str() < 1)
  {
    write("Te sientes tan débil que ni siquiera puedes seguir viviendo.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  reset_carry_cap();
  calc();
  return stats_bon[STR_POS];
}

int adjust_bonus_con(int i) 
{
  stats_bon[CON_POS] += i;
  if (query_con() < 1)
  {
    write("Tu debilitado corazón provoca tu muerte.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  calc();
  return stats_bon[CON_POS];
}

int adjust_bonus_cha(int i) 
{
  stats_bon[CHA_POS] += i;
  if (query_cha() < 1)
  {
    write("Eres tan feo que necesitas morir para remediarlo.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  calc();
  return stats_bon[CHA_POS];
}

int adjust_bonus_per(int i){
  stats_bon[PER_POS] += i;
  if (query_per() < 1)
  {
    write("Te has muerto y no te has dado cuenta.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  calc();
  return stats_bon[PER_POS];
}

int adjust_bonus_wil(int i) 
{
  stats_bon[WIL_POS] += i;
  if (query_wil() < 1)
  {
    write("Tienes tan poca fuerza de voluntad que acabas muriendo sin poder evitarlo.\n");
    this_object()->do_death(this_object());
    return 0;
  }
  calc();
  return stats_bon[WIL_POS];
}

void update_tmps(){
  if ( (stats_tmp[STR_POS] || 
    stats_tmp[CON_POS] ||
    stats_tmp[WIS_POS] ||
    stats_tmp[INT_POS] || 
    stats_tmp[CHA_POS] || 
    stats_tmp[DEX_POS]) !=0)
  {
    write("Sientes como tus habilidades vuelven a su estado normal.\n");
  }

  stats_tmp[STR_POS] = stats_tmp[STR_POS]/2;
  stats_tmp[CON_POS] = stats_tmp[CON_POS]/2;
  stats_tmp[WIS_POS] = stats_tmp[WIS_POS]/2;
  stats_tmp[INT_POS] = stats_tmp[INT_POS]/2;
  stats_tmp[DEX_POS] = stats_tmp[DEX_POS]/2;
  stats_tmp[CHA_POS] = stats_tmp[CHA_POS]/2;
  calc();
  reset_carry_cap();
}

int adjust_tmp_ac_bon(int i)
{
  tmp_ac_bon += i;
  return tmp_ac_bon;
}

int adjust_tmp_damage_bon(int i)
{
  tmp_damage_bon += i;
  return tmp_damage_bon;
}

int adjust_tmp_tohit_bon(int i)
{
  tmp_tohit_bon += i;
  return tmp_tohit_bon;
}

int adjust_body_ac_bon(int i)
{
  body_ac_bon += i;
  return body_ac_bon;
}

// Para evitar trampas no hace nada
void set_kill_xp(int bing)
{
  // kill_xp = bing;
  if (bing > 0)
    kill_xp = 0;
  return;
}

int query_kill_xp()
{
  int xp;
  int lvl;

  if (kill_xp)
    return kill_xp;

  if (!(lvl = this_object()->query_level())) 
    lvl = 1;

  xp = lvl * 60;
  return xp;
}

/* **********  Funciones para el nuevo sistema de resistencias ************ */

mapping query_resistances() { return resistances; } 
mapping query_tmp_resistances() { return tmp_resistances; } 

int set_resistance(string type, int value)
{ 
  resistances[type] = value;
  return 1;
}

int remove_resistance(string type)
{
  if (member_array(type, keys(resistances)) != -1)
    map_delete(resistances, type);
  if (member_array(type, keys(tmp_resistances)) != -1)
    map_delete(tmp_resistances, type);
  return 1;
}

int adjust_resistance(string type, int value)
{
  if (resistances[type])
     resistances[type] += value;
  else
     resistances[type] = value;   
  return resistances[type];
}

int reset_tmp_resistance(string type)
{
  tmp_resistances[type] = 0;
  return 1;
}

int adjust_tmp_resistance(string type, int value)
{
  if (tmp_resistances[type])
    tmp_resistances[type] += value;
  else
    tmp_resistances[type] = value;   
  return tmp_resistances[type];
}

int query_resistance(string type)
{
  int res;
  res = 0;

  if (resistances[type])
    res += resistances[type];
  if (tmp_resistances[type])
    res += tmp_resistances[type];

  return res;
}
int query_real_resistance(string type)
{
  int res;
  res = 0;

  if (resistances[type])
    res += resistances[type];

  return res;
}
int query_tmp_resistance(string type)
{
  int res;
  res = 0;

  if (tmp_resistances[type])
    res += tmp_resistances[type];

  return res;
}

/* **********  Final nuevo sistema de resistencias ************ */

void calc()
{
  // object class_ob;

  /* This will make sure the players die when they reach negative stats..
   * Baldrick, added dec '94
   * the charisma should be able to be negative, but right now it will
   * get rid of some bugabusers.
   */ 
  /* has to be smarter, or in another place 
  if (query_con() < 1 || query_dex() < 1 || query_str() < 1 || query_int() < 1
  || query_wis() < 1 || query_cha() < 1)
    {
    write("Sientes que eres demasiado débil y la vida se te escapa...\n");
    this_object()->do_death(this_object());
    }*/ /* if stats negative, then die..*/ 
   
  // class_ob   = this_object()->query_class_ob();

  /*
  // Cambiado, ahora hp_bonus y gp_bonus funcionan por los bonificadores segun
  //  caracteristica, neverbot 7/03
  hp_bonus = query_stat_bonus_to_con() + (class_ob?class_ob->query_hp_bonus():0);
  gp_bonus = query_stat_bonus(query_gp_main_stat()) + 
       (class_ob?class_ob->query_gp_bonus():0);
  */

  // El bono de daño ya no funciona asi, neverbot 6/03
  // damage_bonus = calc_damage_bonus(query_str());
  /* Estas funciones han desaparecido??
  body_ac_bon  = calc_body_ac(query_dex());
  tohit_bonus  = calc_tohit_bonus(query_str());
  */
  body_ac_bon = 0;
  tmp_ac_bon = 0;
  tmp_tohit_bon = 0;
  tmp_damage_bon = 0;
}

// Sistema de bonificaciones por caracteristica añadido, neverbot 6/03
private int query_stat_bonus(int num)
{
  switch(num)
  {
    case 0:
    case 1: return -5;
    case 2:
    case 3: return -4;
    case 4:
    case 5: return -3;
    case 6:
    case 7: return -2;
    case 8:
    case 9: return -1;
    case 10:
    case 11: return 0;
    case 12:
    case 13: return 1;
    case 14:
    case 15: return 2;
    case 16:
    case 17: return 3;
    case 18:
    case 19: return 4;
    case 20:
    case 21: return 5;
    case 22:
    case 23: return 6;
    case 24:
    case 25: return 7;
    case 26:
    case 27: return 8;
    case 28:
    case 29: return 9;
    case 30:
    case 31: 
    default: return 10;
  }
}

int query_stat_bonus_to_str() { return bonus_to_stat_bonus[STR_POS] + 
             query_stat_bonus(query_str()); }
int query_stat_bonus_to_int() { return bonus_to_stat_bonus[INT_POS] + 
             query_stat_bonus(query_int()); }
int query_stat_bonus_to_wis() { return bonus_to_stat_bonus[WIS_POS] + 
             query_stat_bonus(query_wis()); }
int query_stat_bonus_to_con() { return bonus_to_stat_bonus[CON_POS] + 
             query_stat_bonus(query_con()); }
int query_stat_bonus_to_cha() { return bonus_to_stat_bonus[CHA_POS] + 
             query_stat_bonus(query_cha()); }
int query_stat_bonus_to_per() { return bonus_to_stat_bonus[PER_POS] + 
             query_stat_bonus(query_per()); }
int query_stat_bonus_to_wil() { return bonus_to_stat_bonus[WIL_POS] + 
             query_stat_bonus(query_wil()); }
// La destreza puede ser modificada por las armaduras que se lleven
int query_stat_bonus_to_dex() 
{ 
  int total;
  total = bonus_to_stat_bonus[DEX_POS] + 
          query_stat_bonus(query_dex()); 

  if (this_object()->query_max_dex_bon() != -1)
    if (total > this_object()->query_max_dex_bon())
      return this_object()->query_max_dex_bon();

  return total;
}

// Bonificadores temporales a los bonificadores por caracteristica
int query_bonus_to_stat_bonus_str() { return bonus_to_stat_bonus[STR_POS]; }
int query_bonus_to_stat_bonus_int() { return bonus_to_stat_bonus[INT_POS]; }
int query_bonus_to_stat_bonus_dex() { return bonus_to_stat_bonus[DEX_POS]; }
int query_bonus_to_stat_bonus_wis() { return bonus_to_stat_bonus[WIS_POS]; }
int query_bonus_to_stat_bonus_con() { return bonus_to_stat_bonus[CON_POS]; }
int query_bonus_to_stat_bonus_cha() { return bonus_to_stat_bonus[CHA_POS]; }
int query_bonus_to_stat_bonus_per() { return bonus_to_stat_bonus[PER_POS]; }
int query_bonus_to_stat_bonus_wil() { return bonus_to_stat_bonus[WIL_POS]; }

int adjust_bonus_to_stat_bonus_str(int num) { return bonus_to_stat_bonus[STR_POS] += num; }
int adjust_bonus_to_stat_bonus_int(int num) { return bonus_to_stat_bonus[INT_POS] += num; }
int adjust_bonus_to_stat_bonus_dex(int num) { return bonus_to_stat_bonus[DEX_POS] += num; }
int adjust_bonus_to_stat_bonus_wis(int num) { return bonus_to_stat_bonus[WIS_POS] += num; }
int adjust_bonus_to_stat_bonus_con(int num) { return bonus_to_stat_bonus[CON_POS] += num; }
int adjust_bonus_to_stat_bonus_cha(int num) { return bonus_to_stat_bonus[CHA_POS] += num; }
int adjust_bonus_to_stat_bonus_per(int num) { return bonus_to_stat_bonus[PER_POS] += num; }
int adjust_bonus_to_stat_bonus_wil(int num) { return bonus_to_stat_bonus[WIL_POS] += num; }

// Añadida para ayudar un poco, neverbot 13/7/03
int query_gp_main_stat()
{
  object class_ob;
  class_ob = this_object()->query_class_ob();

  if (class_ob)
    return class_ob->query_gp_main_stat(this_object());

  return ((query_wis() + query_int()) / 2);
}

// stats añadido, algunos archivos con stats no llegan a devolver nada
mixed stats() 
{
  return ({ 
     ({"Stats Base", stats_base, }),
     ({"Stats Bonus", stats_bon, }),
     ({"Stats Tmp (nosave)", stats_tmp, }),
     ({"Bonus To Stat Bonus (nosave)", bonus_to_stat_bonus, }),
     ({"Extreme Str", extreme_str, }),
     ({"Damage Bonus", damage_bonus, }),
     ({"Tmp Damage Bon (nosave)", tmp_damage_bon, }),
     ({"ToHit Bonus", tohit_bonus, }),
     ({"Tmp Tohit Bon (nosave)", tmp_tohit_bon, }),
     ({"Tmp Ac Bon (nosave)", tmp_ac_bon, }),
     ({"Body Ac Bon (nosave)", body_ac_bon, }),
     ({"Body Ac", body_ac, }),
     ({"Kill Xp", kill_xp, }),
     ({"Resistances", resistances, }),
     ({"Tmp Resistances (nosave)", tmp_resistances, }),
       });
}
