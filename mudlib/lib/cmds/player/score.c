// Score para Ciudad capital, basado en el de Fradess para RlMud
// pero muy simplificado
//  neverbot 06/03

#include <mud/cmd.h>

#define RESET "%^RESET%^"

inherit CMD_BASE;

void setup()
{
  position = 0;
}

string query_usage()
{
  return "puntos [-d]";
}

string query_short_help()
{
  return "Muestra algunas estadísticas sobre tu ficha.";
}

static int cmd (string str, object me, string verb)
{
  int points, max_points, percentage;
  int age, aux;
  object ob;
  string points_string, max_points_string, color;
  string res;

  res = "";
    
  if (me->query_dead())
  {
    notify_fail("Estás en forma espiritual, no necesitas conocer eso.\n");
    return 0;
  }

  if (strlen(str) && (str != "-d"))
  {
    notify_fail("Sintaxis: puntos [-d]\n"+
          "      (Utiliza -d o el modo extendido para las estadísticas detalladas).\n");
    return 0;
  }
    
  color = "%^BOLD%^";
    
  points = me->query_hp();
  max_points = me->query_max_hp();

  if (points >= (max_points/2))
    color += "%^GREEN%^";
  else
  {
    if(points >= (max_points/4))
       color += "%^MAGENTA%^";
     else
       color += "%^RED%^";
  }

  if ( (str == "-d") || me->query_verbose() ) 
  {
    res += "Puntos de vida:       ";
    percentage = (points * 100) / max_points;
    points_string = color + points + RESET; 
    max_points_string = "%^BOLD%^%^GREEN%^" + max_points + RESET;      
    res += "["+percentage_bar(percentage)+"] ("+points_string+" de "+max_points_string+")"+
      (max_points>99?"\t":"\t\t")+"("+percentage+" %)\n";
  
    points = me->query_gp();
    max_points = me->query_max_gp();

    res += "Puntos de energía:    ";
    percentage = (points * 100) / max_points;
    points_string = "%^BOLD%^" + points + RESET; 
    max_points_string = "%^BOLD%^" + max_points + RESET; 
    res += "["+percentage_bar(percentage)+"] ("+points_string+" de "+max_points_string+")"+
      (max_points>99?"\t":"\t\t")+"("+percentage+" %)\n";
    
    points = me->query_social_points();
    max_points = me->query_max_social_points();

    res += "Puntos sociales:      ";
    percentage = (points * 100) / max_points;
    points_string = "%^BOLD%^" + points  + RESET; 
    max_points_string = "%^BOLD%^" + max_points  + RESET; 
    res += "["+percentage_bar(percentage)+"] ("+points_string+" de "+max_points_string+")"+
      (max_points>99?"\t":"\t\t")+"("+percentage+" %)\n";

    ob = load_object(me->query_class_ob());
    if (ob)
    {
      points = me->query_xp();
      max_points = ob->query_next_level_xp(me);

      res += "Experiencia (clase):  ";
      percentage = (points * 100) / max_points;
      points_string = "%^BOLD%^" + points  + RESET; 
      max_points_string = "%^BOLD%^" + max_points  + RESET; 
      res += "["+percentage_bar(percentage)+"] ("+points_string+" de "+max_points_string+")"+
        (max_points>99?"\t":"\t\t")+"("+percentage+" %)\n";
     }    

    ob = load_object(me->query_guild_ob());
    if (ob)
    {
      points = me->query_xp();
      max_points = ob->query_next_level_xp(me);

      res += "Experiencia (gremio): ";
      percentage = (points * 100) / max_points;
      points_string = "%^BOLD%^" + points  + RESET; 
      max_points_string = "%^BOLD%^" + max_points  + RESET; 
      res += "["+percentage_bar(percentage)+"] ("+points_string+" de "+max_points_string+")"+
        (max_points>99?"\t":"\t\t")+"("+percentage+" %)\n";
    }    


    ob = load_object(me->query_job_ob());
    if (ob)
    {
      points = me->query_job_xp();
      max_points = ob->query_next_level_xp(me);

      res += "Experiencia (oficio): ";
      percentage = (points * 100) / max_points;
      points_string = "%^BOLD%^" + points  + RESET; 
      max_points_string = "%^BOLD%^" + max_points  + RESET; 
      res += "["+percentage_bar(percentage)+"] ("+points_string+" de "+max_points_string+")"+
        (max_points>99?"\t":"\t\t")+"("+percentage+" %)\n";
    }    

    res += "Tienes ";
    age = me->query_time_on();
    age = -age;

    if (age > 31536000)
    {
      aux = (age / 31536000) % 365;
      if (aux == 1)
        res += "un año, ";
      else
        res += ""+ aux + " años, ";
      age = age - 31536000;
    }

    if (age > 2592000)
    {
      aux = (age / 2592000) % 12;
      if (aux == 1)
        res += "un mes, ";
      else
        res += ""+ aux + " meses, ";
    }

    if (age > 86400)
    {
      aux = (age / 86400) % 30;
      if (aux == 1)
        res += "un día, ";
      else
        res += ""+ aux + " días, ";
    }

    if (age > 3600)
    {
      aux = (age / 3600) % 24;
      if (aux == 1)
        res += "una hora, ";
      else
        res += ""+ aux + " horas, ";
    }

    if (age > 60)
    {
      aux = (age / 60) % 60;
      if (aux == 1)
        res += "un minuto y ";
      else
        res += ""+ aux + " minutos y ";
    }

    if (age % 60 == 1)
      res += "un segundo de antigüedad.\n";
    else
      res += ""+ age%60 + " segundos de antigüedad.\n";

    /*   
    if (me->query_wimpy())
     tell_object(me, "Cobardía puesta a "+me->query_wimpy()+"%\n");
    else
     tell_object(me, "Estás en modo valiente.\n");
    */
     
  }
  else
  {
    res = "Pv: " + color + points + RESET + " (" + color + max_points + RESET + ")";

    points = me->query_gp();
    max_points = me->query_max_gp();

    res += "  Pe: %^BOLD%^" + points + RESET + " (%^BOLD%^" + max_points + RESET + ")";
    res += "  Pexp: %^BOLD%^" + me->query_xp() + RESET;

    points = me->query_social_points();
    max_points = me->query_max_social_points();
    
    res += "  Psoc: "+ points + " (" + max_points + ")\n";
  }

  write(res);
   
  return 1;
}
