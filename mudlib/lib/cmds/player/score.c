// Score for CcMud, based on the one by Fradess for RlMud
// but very simplified
//  neverbot 06/03

#include <mud/cmd.h>
#include <translations/stats.h>
#include <translations/common.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_SCORE_ALIAS);
  set_usage(_LANG_CMD_SCORE_SYNTAX);
  set_help(_LANG_CMD_SCORE_HELP);
}

#define RESET "%^RESET%^"

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
    notify_fail(_LANG_CMD_DEAD);
    return 0;
  }

  if (strlen(str) && (str != "-d"))
  {
    write(_LANG_SYNTAX + ": " +_LANG_CMD_SCORE_SYNTAX + "\n");
    return 1;
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
    percentage = (points * 100) / max_points;
    res += sprintf("%-19s ( %3d %% )", capitalize(_LANG_HEALTH_POINTS_LONG) + ":", percentage);
    points_string = color + points + RESET; 
    max_points_string = "%^BOLD%^%^GREEN%^" + max_points + RESET;      
    res += " [ " + bar(percentage, 15) + " ] (" + points_string + " " + 
      _LANG_OF + " " + max_points_string + ")\n";
  
    points = me->query_gp();
    max_points = me->query_max_gp();
    percentage = (points * 100) / max_points;

    res += sprintf("%-19s ( %3d %% )", capitalize(_LANG_GUILD_POINTS_LONG) + ":", percentage);
    points_string = "%^BOLD%^" + points + RESET; 
    max_points_string = "%^BOLD%^" + max_points + RESET; 
    res += " [ " + bar(percentage, 15) + " ] (" + points_string + " " + 
      _LANG_OF + " " + max_points_string + ")\n";
    
    points = me->query_social_points();
    max_points = me->query_max_social_points();
    percentage = (points * 100) / max_points;

    res += sprintf("%-19s ( %3d %% )", capitalize(_LANG_SOCIAL_POINTS_LONG) + ":", percentage);
    points_string = "%^BOLD%^" + points  + RESET; 
    max_points_string = "%^BOLD%^" + max_points  + RESET; 
    res += " [ " + bar(percentage, 15) + " ] (" + points_string + " " + 
      _LANG_OF + " " + max_points_string + ")\n";

    ob = load_object(me->query_class_ob());
    if (ob)
    {
      points = me->query_xp();
      max_points = ob->query_next_level_xp(me);
      percentage = (points * 100) / max_points;

      res += sprintf("%-19s ( %3d %% )", capitalize(_LANG_XP_CLASS_LONG) + ":", percentage);
      points_string = "%^BOLD%^" + points  + RESET; 
      max_points_string = "%^BOLD%^" + max_points  + RESET; 
      res += " [ " + bar(percentage, 15) + " ] (" + points_string + " " + 
        _LANG_OF + " " + max_points_string + ")\n";
    }    

    ob = load_object(me->query_guild_ob());
    if (ob)
    {
      points = me->query_xp();
      max_points = ob->query_next_level_xp(me);
      percentage = (points * 100) / max_points;

      res += sprintf("%-19s ( %3d %% )", capitalize(_LANG_XP_GUILD_LONG) + ":", percentage);
      points_string = "%^BOLD%^" + points  + RESET; 
      max_points_string = "%^BOLD%^" + max_points  + RESET; 
      res += " [ " + bar(percentage, 15) + " ] (" + points_string + " " + 
        _LANG_OF + " " + max_points_string + ")\n";
    }    

    ob = load_object(me->query_job_ob());
    if (ob)
    {
      points = me->query_job_xp();
      max_points = ob->query_next_level_xp(me);
      percentage = (points * 100) / max_points;

      res += sprintf("%-19s ( %3d %% )", capitalize(_LANG_XP_JOB_LONG) + ":", percentage);
      points_string = "%^BOLD%^" + points  + RESET; 
      max_points_string = "%^BOLD%^" + max_points  + RESET; 
      res += " [ " + bar(percentage, 15) + " ] (" + points_string + " " + 
        _LANG_OF + " " + max_points_string + ")\n";
    }    

    age = me->query_time_on();
    age = -age;

    res += _LANG_CMD_SCORE_AGE;
  }
  else
  {
    res = _LANG_HEALTH_POINTS_SHORT + ": " + color + points + RESET + 
          " (" + color + max_points + RESET + ")";

    points = me->query_gp();
    max_points = me->query_max_gp();

    res += "  " + _LANG_GUILD_POINTS_SHORT + ": %^BOLD%^" + points + RESET + 
           " (%^BOLD%^" + max_points + RESET + ")";
    
    res += "  " + _LANG_XP_CLASS_SHORT + ": %^BOLD%^" + me->query_xp() + RESET;

    points = me->query_social_points();
    max_points = me->query_max_social_points();
    
    res += "  " + _LANG_SOCIAL_POINTS_SHORT + ": "+ points + " (" + max_points + ")\n";
  }

  write(res);
  return 1;
}
