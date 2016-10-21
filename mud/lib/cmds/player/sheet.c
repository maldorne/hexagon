// Converted to the new command handler system by Morgoth 1/Mar/95
// - cleaned the code a bit :)
// - fixed 'money carried' to show the right amount
// - changed it to use query_al instead of query_alignment, was
//   showing wrong values.
// - Moved back to FR, added the FR'ish stuff to this one.

// Radix added Group name, Jan 1996
// Radix added Race group name, Apr 1996
// Radix removed the spell and command lists, July 1996

// Nueva ficha para CcMud, neverbot 6/03
// Añadido sistema de clases (gremio generico), neverbot Jun'03
// Añadido nuevo sistema de oficios y deidades, neverbot Mar'03
// Ficha modificada para Devel por neverbot Jul'01

// Luck and Perception added, neverbot 03/05
// Luck changed to Willpower, neverbot 08/09
// Removed alignment, neverbot 01/2013

#include <mud/cmd.h>
#include <item/armour.h>
#include <living/combat.h>

inherit CMD_BASE;

#define LOG_SHEET "sheet"

void setup()
{
  position = 0;
}

string query_usage()
{
  return "ficha";
}

string query_short_help()
{
  return "Devuelve información y características de tu personaje.";
}

static int cmd (string name, object who, string verb)
{
  int debug;
  int i, aux, diff_ac;
  string info;
  
  object guild, job, group, race, race_group, me, deity, guild_class, city;

  int * stats;
  int * tmp_stats;
  // Estos son los bonificadores a las habilidades de una caracteristica
  // dex 18 -> bonificador +4, etc
  int * bonus_to_stats;
  string * str_stats;

  stats = allocate(7);
  tmp_stats = allocate(8);
  bonus_to_stats = allocate(8);
  str_stats = allocate(8);

  debug = who->query_coder();
  
  if (debug)
  {
    if (!strlen(name)) 
      me = who;
    else
    {
      name = who->expand_nickname(name);
      me = find_living(name);

      if (!objectp(me)) 
        me = find_player(name);
      if (!objectp(me)) 
        me = find_object(name);

      if (!objectp(me))
      {
        notify_fail("Lo siento, no encuentro a '"+name+"'.\n");
        return 0;
      }

      if (me->query_player())
        log_file(LOG_SHEET, who->query_cap_name() + " looked through "+
           me->query_cap_name() + "'s sheet [" + ctime(time()) + "]\n");
    }

    if (!who->query_coordinator() && !me->query_coder())
      me = who;
  }
  else 
    me = who;

  if (me->query_dead() && !debug)
  {
    notify_fail("Estás en forma espiritual, no necesitas conocer eso.\n");
    return 0;
  }

  race = me->query_race_ob();
  guild = me->query_guild_ob();
  guild_class = me->query_class_ob();
  job = me->query_job_ob();
  group = me->query_group_ob();
  race_group = me->query_race_group_ob();
  deity = me->query_deity_ob();
  city = me->query_city_ob();

  // Obtenemos los datos del personaje:
  str_stats[0] = me->stat_string(me->query_str()); // Para dar el 18/xx
  stats[0] = me->query_dex();
  stats[1] = me->query_con();
  stats[2] = me->query_wis();
  stats[3] = me->query_int();
  stats[4] = me->query_cha();
  stats[5] = me->query_per();
  stats[6] = me->query_wil();

  tmp_stats[0] = me->query_tmp_str();
  tmp_stats[1] = me->query_tmp_dex();
  tmp_stats[2] = me->query_tmp_con();
  tmp_stats[4] = me->query_tmp_int();
  tmp_stats[3] = me->query_tmp_wis();
  tmp_stats[5] = me->query_tmp_cha();
  tmp_stats[6] = me->query_tmp_per();
  tmp_stats[7] = me->query_tmp_wil();

  // Nuevo sistema de bonificadores por caracteristicas 
  // basicas para CcMud, neverbot 6/03
  bonus_to_stats[0] = me->query_bonus_to_stat_bonus_str();
  bonus_to_stats[1] = me->query_bonus_to_stat_bonus_dex();
  bonus_to_stats[2] = me->query_bonus_to_stat_bonus_con();
  bonus_to_stats[3] = me->query_bonus_to_stat_bonus_int();
  bonus_to_stats[4] = me->query_bonus_to_stat_bonus_wis();
  bonus_to_stats[5] = me->query_bonus_to_stat_bonus_cha();
  bonus_to_stats[6] = me->query_bonus_to_stat_bonus_per();
  bonus_to_stats[7] = me->query_bonus_to_stat_bonus_wil();

  // Si tenemos algun modificador a las caracteristicas basicas, 
  // mostramos esa caracteristica en color rojo en la ficha:
  // Fuerza
  if (tmp_stats[0] != 0)
    str_stats[0] = "%^BOLD%^%^RED%^"+str_stats[0]+"%^RESET%^";
  else
    str_stats[0] = ""+str_stats[0]+"";   
  if (bonus_to_stats[0] != 0)
    str_stats[0] += " (%^BOLD%^RED%^" +
            ((me->query_stat_bonus_to_str()>=0)?"+":"") + 
            me->query_stat_bonus_to_str()+"%^RESET%^)";
  else
    str_stats[0] += " (" +
            ((me->query_stat_bonus_to_str()>=0)?"+":"") + 
            me->query_stat_bonus_to_str()+")";

  // Destreza
  if (tmp_stats[1] != 0)
    str_stats[1] = "%^BOLD%^%^RED%^"+stats[0]+"%^RESET%^";
  else
    str_stats[1] = ""+stats[0]+"";   
  if (bonus_to_stats[1] != 0)
    str_stats[1] += " (%^BOLD%^RED%^" +
            ((me->query_stat_bonus_to_dex()>=0)?"+":"") + 
            me->query_stat_bonus_to_dex()+"%^RESET%^)";
  else
    str_stats[1] += " (" +
            ((me->query_stat_bonus_to_dex()>=0)?"+":"") + 
            me->query_stat_bonus_to_dex()+")";

  // Constitucion
  if (tmp_stats[2] != 0)
    str_stats[2] = "%^BOLD%^%^RED%^"+stats[1]+"%^RESET%^";
  else
    str_stats[2] = ""+stats[1]+"";   
  if (bonus_to_stats[2] != 0)
    str_stats[2] += " (%^BOLD%^RED%^" +
            ((me->query_stat_bonus_to_con()>=0)?"+":"") + 
            me->query_stat_bonus_to_con()+"%^RESET%^)";
  else
    str_stats[2] += " (" +
            ((me->query_stat_bonus_to_con()>=0)?"+":"") + 
            me->query_stat_bonus_to_con()+")";

  // Inteligencia   
  if (tmp_stats[4] != 0)
    str_stats[4] = "%^BOLD%^%^RED%^"+stats[3]+"%^RESET%^";
  else
    str_stats[4] = ""+stats[3]+"";   
  if (bonus_to_stats[4] != 0)
    str_stats[4] += " (%^BOLD%^RED%^" +
            ((me->query_stat_bonus_to_int()>=0)?"+":"") + 
            me->query_stat_bonus_to_int()+"%^RESET%^)";
  else
    str_stats[4] += " (" +
            ((me->query_stat_bonus_to_int()>=0)?"+":"") + 
            me->query_stat_bonus_to_int()+")";

  // Sabiduria   
  if (tmp_stats[3] != 0)
    str_stats[3] = "%^BOLD%^%^RED%^"+stats[2]+"%^RESET%^";
  else
    str_stats[3] = ""+stats[2]+"";   
  if (bonus_to_stats[3] != 0)
    str_stats[3] += " (%^BOLD%^RED%^" +
            ((me->query_stat_bonus_to_wis()>=0)?"+":"") + 
            me->query_stat_bonus_to_wis()+"%^RESET%^)";
  else
    str_stats[3] += " (" +
            ((me->query_stat_bonus_to_wis()>=0)?"+":"") + 
            me->query_stat_bonus_to_wis()+")";

  // Carisma
  if (tmp_stats[5] != 0)
    str_stats[5] = "%^BOLD%^%^RED%^"+stats[4]+"%^RESET%^";
  else
     str_stats[5] = ""+stats[4]+"";   
  if (bonus_to_stats[5] != 0)
    str_stats[5] += " (%^BOLD%^RED%^" +
            ((me->query_stat_bonus_to_cha()>=0)?"+":"") + 
            me->query_stat_bonus_to_cha()+"%^RESET%^)";
  else
    str_stats[5] += " (" +
            ((me->query_stat_bonus_to_cha()>=0)?"+":"") + 
            me->query_stat_bonus_to_cha()+")";

  // Percepcion
  if (tmp_stats[6] != 0)
     str_stats[6] = "%^BOLD%^%^RED%^"+stats[5]+"%^RESET%^";
  else
     str_stats[6] = ""+stats[5]+"";   
  if (bonus_to_stats[6] != 0)
    str_stats[6] += " (%^BOLD%^RED%^" +
            ((me->query_stat_bonus_to_per()>=0)?"+":"") + 
            me->query_stat_bonus_to_per()+"%^RESET%^)";
  else
    str_stats[6] += " (" +
            ((me->query_stat_bonus_to_per()>=0)?"+":"") + 
            me->query_stat_bonus_to_per()+")";

  // Voluntad
  if (tmp_stats[7] != 0)
    str_stats[7] = "%^BOLD%^%^RED%^"+stats[6]+"%^RESET%^";
  else
    str_stats[7] = ""+stats[6]+"";   
  if (bonus_to_stats[7] != 0)
    str_stats[7] += " (%^BOLD%^RED%^" +
            ((me->query_stat_bonus_to_wil()>=0)?"+":"") + 
            me->query_stat_bonus_to_wil()+"%^RESET%^)";
  else
    str_stats[7] += " (" +
            ((me->query_stat_bonus_to_wil()>=0)?"+":"") + 
            me->query_stat_bonus_to_wil()+")";

  if (me->query_str() < 10) str_stats[0] = " "+str_stats[0];
  if (stats[0] < 10) str_stats[1] = " "+str_stats[1];
  if (stats[1] < 10) str_stats[2] = " "+str_stats[2];
  if (stats[2] < 10) str_stats[3] = " "+str_stats[3];
  if (stats[3] < 10) str_stats[4] = " "+str_stats[4];
  if (stats[4] < 10) str_stats[5] = " "+str_stats[5];
  if (stats[5] < 10) str_stats[6] = " "+str_stats[6];
  if (stats[6] < 10) str_stats[7] = " "+str_stats[7];

  info="\t\t\tNombre:\t%^BOLD%^"+me->query_cap_name()+"%^RESET%^\n";

  info+="Características básicas:\t";
  info+="Raza:\t\t"+(race?race->query_short():"Sin raza")+"\n";

  // Si hay algun modificador a fuerza petan las tabulaciones:
  if (me->query_str() > 18)
     info+="Fuerza:\t\t"+str_stats[0]+"\t";
  else
     info+="Fuerza:\t\t"+str_stats[0]+"\t\t";
  info+="Sexo:\t\t"+capitalize(me->query_gender_string())+"\n";
  
  info+="Constitución:\t"+str_stats[2]+"\t\t";
  info+="Clase:\t\t"+(guild_class?capitalize(guild_class->query_short()+" ("+
    me->query_class_level()+")"):"Sin Clase")+"\n";

  info+="Destreza:\t"+str_stats[1]+"\t\t";
  info+="Gremio:\t\t"+(guild?capitalize(guild->query_short()+" ("+
    me->query_guild_level()+")"):"Sin Gremio")+"\n";
  
  info+="Inteligencia:\t"+str_stats[4]+"\t\t";
  info+="Oficio:\t\t"+(job?capitalize(job->query_short()+(me->query_job_level()?(" ("+
    me->query_job_level()+")"):"")):"Sin Oficio")+"\n";

  info+="Sabiduría:\t"+str_stats[3]+"\t\t";
  info+="Deidad:\t\t"+(deity?deity->query_short():"Sin Deidad")+"\n";
  
  info+="Carisma:\t"+str_stats[5]+"\t\t";  
  // Casa en vez de Clan para las razas con grupo racial. Oskuro Nov'99
  // Hago generico el sistema de casas, no solo drows, neverbot 7/01
  if (race_group) 
    info+="Casa:\t\t"+(race_group?race_group->query_short():"Sin casa")+"\n";
  else
    info+="Clan:\t\t"+(group?me->query_group_name():"Sin Clan")+"\n";

  // Nueva caracteristica de la ficha, neverbot 03/05
  info+="Percepción:\t"+str_stats[6]+"\t\t";  
  info+="Ciudadanía:\t"+(city?city->query_short():"Sin Ciudadanía")+"\n";

  // Nueva caracteristica de la ficha, neverbot 03/05
  info+="Voluntad:\t"+str_stats[7]+"\t\t";  
  info+="Estado:\t\t"+capitalize(me->volume_string())+"\n";

  info+="Cobardía:\t( "+me->query_wimpy()+" )%\t\t";    
  info+="Carga:\t\t"+me->query_loc_weight()+" ( "+me->query_max_weight()+" )\n";

  info+="Combate:\t"+capitalize(me->query_combat_mode_string())+
        ((strlen(me->query_combat_mode_string()) > 5)?"\t":"\t\t");    
  info+="Actitud:\t"+capitalize(me->query_combat_role_string())+"\n";

  // Comprobamos si todas las categorias son iguales
  diff_ac = 0;
  for(i = 0; i < sizeof(AC_TYPES); i++)
  {
    for (aux = 0; aux < i; aux++)
    {
      if (me->query_total_ac(AC_TYPES[i]) != me->query_total_ac(AC_TYPES[aux]) )
      {
        diff_ac = 1;
        break;
      }
    }
  }
  if (diff_ac)
  {
    info += "Capacidad defensiva, contra armas:\n\t";
    for(i = 0; i < sizeof(AC_TYPES); i++)
      info += "" + capitalize(AC_TYPES[i])+ "s ("+me->query_total_ac(AC_TYPES[i])+") ";
    info += "\n";
  } 
  else 
  {
    info += "Capacidad defensiva:\t\t"+me->query_total_ac(AC_TYPES[0])+"\n";
  }
  info+="Habilidad de combate:\t\t"+me->query_total_wc()+"\n";

  if (me->query_skill_malus() != 0)
    info+="Penalizador a las habilidades:\t"+me->query_skill_malus()+"\n";

  // neverbot 6/03, new alignment system
  // Conditional definition 01/13
#ifdef USE_ALIGNMENT  
  info+=me->query_align_string(); 
#endif

  info+="\n";

  if ((me->query_value()/100)>0)
  {
  if ((me->query_value()/100)==1)
    info+="Dinero total:\t\t\t1 Moneda de Oro\n";
  else   
    info+="Dinero total:\t\t\t"+me->query_value()/100+" Monedas de Oro\n";
  }

  if (me->query_xp() > 0)
    info+="Experiencia (Gremio/Clase):\t"+me->query_xp()+"\n"; // Añadir coste de avanzar??
  if (me->query_job_ob() && (me->query_job_xp() > 0))
    info+="Experiencia (Oficio):\t\t"+me->query_job_xp()+"\n"; // Añadir coste de avanzar??

  // if (me->query_status_points() > 0)
  //   info+="Puntos de Prestigio:\t\t"+me->query_status_points()+"\n";
// TODO comm
  // if (me->query_max_social_points() > 0)
  //   info+="Puntos Sociales:\t\t"+me->query_social_points()+" ( "+me->query_max_social_points()+" )\n";
  info+="Puntos de Energía:\t\t"+me->query_gp()+" ( "+me->query_max_gp()+" )\n";
  info+="Puntos de Vida:\t\t\t"+me->query_hp()+" ( "+me->query_max_hp()+" )\n";

  info+="\n";

  info+="También puedes usar: 'dotes', 'maestrías', 'estilos' y 'conocimientos'.\n\n";

  // printf("%-#*s\n", me->query_cols(), info);
  // printf("%s\n",info);

  tell_object(who, info);

  // tell_object(who,"\nEscribe \"hechizos\" o \"comandos\" para ver tus conocimientos "
  //   "o habilidades.\n");
  return 1;
}
