// Converted to the new command handler system by Morgoth 1/Mar/95
// - cleaned the code a bit :)
// - fixed 'money carried' to show the right amount
// - changed it to use query_al instead of query_alignment, was
//   showing wrong values.
// - Moved back to FR, added the FR'ish stuff to this one.

// Radix added Group name, Jan 1996
// Radix added Race group name, Apr 1996
// Radix removed the spell and command lists, July 1996

// new sheet for CcMud, neverbot 6/03
// new class system added (generic guild), neverbot Jun'03
// new job and deity system added, neverbot Mar'03
// sheet modified for RL-Devel by neverbot Jul'01

// Perception added, neverbot 03/05
// Willpower, neverbot 08/09
// Removed alignment, neverbot 01/2013

#include <mud/cmd.h>
#include <mud/config.h>
#include <translations/armour.h>
#include <translations/combat.h>
#include <translations/stats.h>
#include <language.h>

inherit CMD_BASE;

#define LOG_SHEET "sheet"

void setup()
{
  set_aliases(_LANG_CMD_SHEET_ALIAS);
  set_usage(_LANG_CMD_SHEET_SYNTAX);
  set_help(_LANG_CMD_SHEET_HELP);
}

static int cmd (string name, object me, string verb)
{
  int debug;
  int i, aux, diff_ac;
  string info;
  
  // social objects
  object guild, job, guild_class, 
         group, race, race_group, 
         deity, citizenship;
  object target;

  int * stats;
  int * tmp_stats;
  // these are the bonus to characteristics
  // dex 18 -> bonus +4, etc
  int * bonus_to_stats;
  string * str_stats;

  stats = allocate(8);
  tmp_stats = allocate(8);
  bonus_to_stats = allocate(8);
  str_stats = allocate(8);
  info = "";

  debug = me->query_coder();
  
  if (debug)
  {
    if (!strlen(name)) 
      target = me;
    else
    {
      name = me->expand_nickname(name);
      target = find_living(name);

      if (!objectp(target)) 
        target = find_player(name);
      if (!objectp(target)) 
        target = find_object(name);

      if (!objectp(target))
      {
        notify_fail(_LANG_CMD_SHEET_CANNOT_FIND);
        return 0;
      }

      if (target->query_player())
        log_file(LOG_SHEET, me->query_cap_name() + " looked through "+
           target->query_cap_name() + "'s sheet [" + ctime(time()) + "]\n");
    }
  }
  else 
    target = me;

  if (me->query_dead() && !debug)
  {
    notify_fail(_LANG_CMD_DEAD);
    return 0;
  }

  race =        load_object(target->query_race_ob());
  guild =       load_object(target->query_guild_ob());
  guild_class = load_object(target->query_class_ob());
  job =         load_object(target->query_job_ob());
  group =       load_object(target->query_group_ob());
  race_group =  load_object(target->query_race_group_ob());
  deity =       load_object(target->query_deity_ob());
  citizenship = load_object(target->query_city_ob());

  // get character data
  stats[0] = target->query_str();
  stats[1] = target->query_dex();
  stats[2] = target->query_con();
  stats[3] = target->query_wis();
  stats[4] = target->query_int();
  stats[5] = target->query_cha();
  stats[6] = target->query_per();
  stats[7] = target->query_wil();

  tmp_stats[0] = target->query_tmp_str();
  tmp_stats[1] = target->query_tmp_dex();
  tmp_stats[2] = target->query_tmp_con();
  tmp_stats[4] = target->query_tmp_int();
  tmp_stats[3] = target->query_tmp_wis();
  tmp_stats[5] = target->query_tmp_cha();
  tmp_stats[6] = target->query_tmp_per();
  tmp_stats[7] = target->query_tmp_wil();

  // new system of bonuses to characteristics for CcMud, neverbot 6/03
  bonus_to_stats[0] = target->query_bonus_to_stat_bonus_str();
  bonus_to_stats[1] = target->query_bonus_to_stat_bonus_dex();
  bonus_to_stats[2] = target->query_bonus_to_stat_bonus_con();
  bonus_to_stats[3] = target->query_bonus_to_stat_bonus_int();
  bonus_to_stats[4] = target->query_bonus_to_stat_bonus_wis();
  bonus_to_stats[5] = target->query_bonus_to_stat_bonus_cha();
  bonus_to_stats[6] = target->query_bonus_to_stat_bonus_per();
  bonus_to_stats[7] = target->query_bonus_to_stat_bonus_wil();

  // if we have any modifier to the basic characteristics, show it in red
  // strenght
  if (tmp_stats[0] != 0)
    str_stats[0] = "%^BOLD%^%^RED%^"+str_stats[0]+"%^RESET%^";
  else
    str_stats[0] = "" + stats[0] + "";   
  if (bonus_to_stats[0] != 0)
    str_stats[0] += " (%^BOLD%^RED%^" +
            ((target->query_stat_bonus_to_str() >= 0) ? "+" : "") + 
            target->query_stat_bonus_to_str()+"%^RESET%^)";
  else
    str_stats[0] += " (" +
            ((target->query_stat_bonus_to_str() >= 0) ? "+" : "") + 
            target->query_stat_bonus_to_str()+")";

  // dexterity
  if (tmp_stats[1] != 0)
    str_stats[1] = "%^BOLD%^%^RED%^"+stats[0]+"%^RESET%^";
  else
    str_stats[1] = "" + stats[0] + "";   
  if (bonus_to_stats[1] != 0)
    str_stats[1] += " (%^BOLD%^RED%^" +
            ((target->query_stat_bonus_to_dex() >= 0) ? "+" : "") + 
            target->query_stat_bonus_to_dex()+"%^RESET%^)";
  else
    str_stats[1] += " (" +
            ((target->query_stat_bonus_to_dex() >= 0) ? "+" : "") + 
            target->query_stat_bonus_to_dex()+")";

  // constitution
  if (tmp_stats[2] != 0)
    str_stats[2] = "%^BOLD%^%^RED%^"+stats[1]+"%^RESET%^";
  else
    str_stats[2] = "" + stats[1] + "";   
  if (bonus_to_stats[2] != 0)
    str_stats[2] += " (%^BOLD%^RED%^" +
            ((target->query_stat_bonus_to_con() >= 0) ? "+" : "") + 
            target->query_stat_bonus_to_con()+"%^RESET%^)";
  else
    str_stats[2] += " (" +
            ((target->query_stat_bonus_to_con() >= 0) ? "+" : "") + 
            target->query_stat_bonus_to_con()+")";

  // intelligence
  if (tmp_stats[4] != 0)
    str_stats[4] = "%^BOLD%^%^RED%^"+stats[3]+"%^RESET%^";
  else
    str_stats[4] = "" + stats[3] + "";   
  if (bonus_to_stats[4] != 0)
    str_stats[4] += " (%^BOLD%^RED%^" +
            ((target->query_stat_bonus_to_int() >= 0) ? "+" : "") + 
            target->query_stat_bonus_to_int()+"%^RESET%^)";
  else
    str_stats[4] += " (" +
            ((target->query_stat_bonus_to_int() >= 0) ? "+" : "") + 
            target->query_stat_bonus_to_int()+")";

  // wisdom   
  if (tmp_stats[3] != 0)
    str_stats[3] = "%^BOLD%^%^RED%^"+stats[2]+"%^RESET%^";
  else
    str_stats[3] = "" + stats[2] + "";   
  if (bonus_to_stats[3] != 0)
    str_stats[3] += " (%^BOLD%^RED%^" +
            ((target->query_stat_bonus_to_wis() >= 0) ? "+" : "") + 
            target->query_stat_bonus_to_wis()+"%^RESET%^)";
  else
    str_stats[3] += " (" +
            ((target->query_stat_bonus_to_wis() >= 0) ? "+" : "") + 
            target->query_stat_bonus_to_wis()+")";

  // charisma
  if (tmp_stats[5] != 0)
    str_stats[5] = "%^BOLD%^%^RED%^"+stats[4]+"%^RESET%^";
  else
     str_stats[5] = "" + stats[4] + "";   
  if (bonus_to_stats[5] != 0)
    str_stats[5] += " (%^BOLD%^RED%^" +
            ((target->query_stat_bonus_to_cha() >= 0) ? "+" : "") + 
            target->query_stat_bonus_to_cha()+"%^RESET%^)";
  else
    str_stats[5] += " (" +
            ((target->query_stat_bonus_to_cha() >= 0) ? "+" : "") + 
            target->query_stat_bonus_to_cha()+")";

  // perception
  if (tmp_stats[6] != 0)
     str_stats[6] = "%^BOLD%^%^RED%^"+stats[5]+"%^RESET%^";
  else
     str_stats[6] = "" + stats[5] + "";   
  if (bonus_to_stats[6] != 0)
    str_stats[6] += " (%^BOLD%^RED%^" +
            ((target->query_stat_bonus_to_per() >= 0) ? "+" : "") + 
            target->query_stat_bonus_to_per()+"%^RESET%^)";
  else
    str_stats[6] += " (" +
            ((target->query_stat_bonus_to_per() >= 0) ? "+" : "") + 
            target->query_stat_bonus_to_per()+")";

  // willpower
  if (tmp_stats[7] != 0)
    str_stats[7] = "%^BOLD%^%^RED%^"+stats[6]+"%^RESET%^";
  else
    str_stats[7] = "" + stats[6] + "";   
  if (bonus_to_stats[7] != 0)
    str_stats[7] += " (%^BOLD%^RED%^" +
            ((target->query_stat_bonus_to_wil() >= 0) ? "+" : "") + 
            target->query_stat_bonus_to_wil()+"%^RESET%^)";
  else
    str_stats[7] += " (" +
            ((target->query_stat_bonus_to_wil() >= 0) ? "+" : "") + 
            target->query_stat_bonus_to_wil()+")";

  if (target->query_str() < 10) str_stats[0] = " " + str_stats[0];
  if (stats[0] < 10) str_stats[1] = " " + str_stats[1];
  if (stats[1] < 10) str_stats[2] = " " + str_stats[2];
  if (stats[2] < 10) str_stats[3] = " " + str_stats[3];
  if (stats[3] < 10) str_stats[4] = " " + str_stats[4];
  if (stats[4] < 10) str_stats[5] = " " + str_stats[5];
  if (stats[5] < 10) str_stats[6] = " " + str_stats[6];
  if (stats[6] < 10) str_stats[7] = " " + str_stats[7];

  info += sprintf("%-32s", _LANG_CMD_SHEET_BASIC_CHARACTERISTICS + ":");
  info += sprintf("%-15s %s", capitalize(_LANG_STATS_RACE) + ":", 
                  (race ? race->query_short() : capitalize(_LANG_STATS_NO_RACE)));
  info += "\n";

  // to show 18/xx
  if (target->query_str() > 18 && target->query_str() < 29)
    info += sprintf("%-15s %-5s (%-+1d) %5s", capitalize(_LANG_STATS_STR) + ":",
                    target->stat_string(target->query_str()),
                    target->query_stat_bonus_to_str(), "");
  else
    info += sprintf("%-15s %-2s (%-+1d) %8s", capitalize(_LANG_STATS_STR) + ":",
                    target->stat_string(target->query_str()),
                    target->query_stat_bonus_to_str(), "");
  info += sprintf("%-15s %s", capitalize(_LANG_STATS_GENDER) + ":", 
                  capitalize(target->query_gender_string()));
  info += "\n";

  info += sprintf("%-15s %-2s (%-+1d) %8s", capitalize(_LANG_STATS_CON) + ":",
                  target->query_con(),
                  target->query_stat_bonus_to_con(), "");
  if (!guild_class)
    info += sprintf("%-15s %s", capitalize(_LANG_STATS_CLASS) + ":", 
                    capitalize(_LANG_STATS_NO_CLASS));
  else    
    info += sprintf("%-15s %s (%d)", capitalize(_LANG_STATS_CLASS) + ":", 
                    capitalize(guild_class->query_short()), 
                    target->query_class_level());
  info += "\n";

  info += sprintf("%-15s %-2s (%-+1d) %8s", capitalize(_LANG_STATS_DEX) + ":",
                  target->query_dex(),
                  target->query_stat_bonus_to_dex(), "");
  if (!guild)
    info += sprintf("%-15s %s", capitalize(_LANG_STATS_GUILD) + ":", 
                    capitalize(_LANG_STATS_NO_GUILD));
  else    
    info += sprintf("%-15s %s (%d)", capitalize(_LANG_STATS_GUILD) + ":", 
                    capitalize(guild->query_short()), 
                    target->query_guild_level());
  info += "\n";

  info += sprintf("%-15s %-2s (%-+1d) %8s", capitalize(_LANG_STATS_INT) + ":",
                  target->query_int(),
                  target->query_stat_bonus_to_int(), "");
  if (!job)
    info += sprintf("%-15s %s", capitalize(_LANG_STATS_JOB) + ":", 
                    capitalize(_LANG_STATS_NO_JOB));
  else    
    info += sprintf("%-15s %s (%d)", capitalize(_LANG_STATS_JOB) + ":", 
                    capitalize(job->query_short()), 
                    target->query_job_level());
  info += "\n";

  info += sprintf("%-15s %-2s (%-+1d) %8s", capitalize(_LANG_STATS_WIS) + ":",
                  target->query_wis(),
                  target->query_stat_bonus_to_wis(), "");
  if (!deity)
    info += sprintf("%-15s %s", capitalize(_LANG_STATS_DEITY) + ":", 
                    capitalize(_LANG_STATS_NO_DEITY));
  else    
    info += sprintf("%-15s %s", capitalize(_LANG_STATS_DEITY) + ":", 
                    capitalize(deity->query_short()));
  info += "\n";

  info += sprintf("%-15s %-2s (%-+1d) %8s", capitalize(_LANG_STATS_CHA) + ":",
                  target->query_cha(),
                  target->query_stat_bonus_to_cha(), "");
  if (!group) // clan, race group, etc
    info += sprintf("%-15s %s", capitalize(_LANG_STATS_GROUP) + ":", 
                    capitalize(_LANG_STATS_NO_GROUP));
  else    
    info += sprintf("%-15s %s", capitalize(_LANG_STATS_GROUP) + ":", 
                    capitalize(group->query_short()));
  info += "\n";

  // new characteristic, neverbot 03/05
  info += sprintf("%-15s %-2s (%-+1d) %8s", capitalize(_LANG_STATS_PER) + ":",
                  target->query_per(),
                  target->query_stat_bonus_to_per(), "");
  if (!citizenship)
    info += sprintf("%-15s %s", capitalize(_LANG_STATS_CITIZENSHIP) + ":", 
                    capitalize(_LANG_STATS_NO_CITIZENSHIP));
  else    
    info += sprintf("%-15s %s", capitalize(_LANG_STATS_CITIZENSHIP) + ":", 
                    capitalize(citizenship->query_short()));
  info += "\n";

  // new characteristic, neverbot 03/05
  info += sprintf("%-15s %-2s (%-+1d) %8s", capitalize(_LANG_STATS_WIL) + ":",
                  target->query_wil(),
                  target->query_stat_bonus_to_wil(), "");
  // info += sprintf("%-15s %s", capitalize(_LANG_STATS_CITIZENSHIP) + ":", 
  //                 capitalize(_LANG_STATS_NO_CITIZENSHIP));
  // info += "Drinks:\t\t"+capitalize(target->volume_string())+"\n";
  info += "\n";


  info += sprintf("%-15s ( %-3d%% ) %7s", capitalize(_LANG_STATS_BRAVERY) + ":",
                  100 - target->query_wimpy(),
                  "");
  info += sprintf("%-15s %d (of %d)", capitalize(_LANG_STATS_ENCUMBRANCE) + ":",
                  target->query_contents_weight(),
                  target->query_max_weight());
  info += "\n";

  info += sprintf("%-15s %-16s", capitalize(_LANG_STATS_COMBAT_MODE) + ":",
                  capitalize(target->query_combat_mode_string()));
  info += sprintf("%-15s %s", capitalize(_LANG_STATS_COMBAT_ROLE) + ":",
                  capitalize(target->query_combat_role_string()));
  info += "\n";

  info += "\n";

  // cheack if all ac are the same
  diff_ac = 0;
  for(i = 0; i < sizeof(AC_TYPES); i++)
  {
    for (aux = 0; aux < i; aux++)
    {
      if (target->query_total_ac(AC_TYPES[i]) != target->query_total_ac(AC_TYPES[aux]) )
      {
        diff_ac = 1;
        break;
      }
    }
  }

  if (diff_ac)
  {
    info += capitalize(_LANG_STATS_AC_AGAINST);
    for (i = 0; i < sizeof(AC_TYPES); i++)
      info += "\n - " + capitalize(AC_TYPES_STR[i]) + " ("+target->query_total_ac(AC_TYPES[i])+")";
    info += "\n";
  } 
  else 
    info += sprintf("%-32s %-16s\n", capitalize(_LANG_STATS_AC) + ":",
                    target->query_total_ac(AC_TYPES[0]));


  info += sprintf("%-32s %-16s\n", capitalize(_LANG_STATS_COMBAT_ABILITY) + ":",
                  target->query_total_wc());

  if (target->query_skill_malus() != 0)
  {
    info += sprintf("%-32s %-16s\n", capitalize(_LANG_STATS_SKILL_MALUS) + ":",
                    target->query_skill_malus());
  }

  // neverbot 6/03, new alignment system
  // Conditional definition 01/13
#ifdef USE_ALIGNMENT  

  info += sprintf("%-32s %s\n", capitalize(_LANG_STATS_EXT_ALIGN) + ":",
                  capitalize(target->query_real_align_string()));
  info += sprintf("%-32s %s\n", capitalize(_LANG_STATS_REAL_ALIGN) + ":",
                  capitalize(target->query_ext_align_string()));

#endif

  if ((target->query_value()/100) > 0)
  {
    info += sprintf("%-32s %d %s\n", capitalize(_LANG_STATS_MONEY) + ":",
                    target->query_value() / 100,
                    ((target->query_value()/100) == 1) ? _LANG_STATS_COIN : _LANG_STATS_COINS);
  }

  info += "\n";

  if (target->query_xp() > 0)
    info += sprintf("%-32s %d\n", capitalize(_LANG_XP_CLASS_LONG) + ":",
                    target->query_xp());

  if (target->query_job_ob() && (target->query_job_xp() > 0))
    info += sprintf("%-32s %d\n", capitalize(_LANG_XP_JOB_LONG) + ":",
                    target->query_job_xp());

  if (target->query_max_social_points() > 0)
    info += sprintf("%-32s %3d ( %3d )\n", capitalize(_LANG_SOCIAL_POINTS_LONG) + ":",
                    target->query_social_points(),
                    target->query_max_social_points());

  info += sprintf("%-32s %3d ( %3d )\n", capitalize(_LANG_GUILD_POINTS_LONG) + ":",
                  target->query_gp(),
                  target->query_max_gp());
  info += sprintf("%-32s %3d ( %3d )\n", capitalize(_LANG_HEALTH_POINTS_LONG) + ":",
                  target->query_hp(),
                  target->query_max_hp());

  info += "\n";
  info += _LANG_CMD_SHEET_CAN_ALSO_USE;

  info = handler("frames")->frame(info, 
                                  _LANG_CMD_SHEET_NAME + ": %^BOLD%^" + target->query_cap_name() + "%^RESET%^",
                                  this_user()->query_cols());

  write(info);

  // tell_object(me,"\nEscribe \"hechizos\" o \"comandos\" para ver tus conocimientos "
  //   "o habilidades.\n");
  return 1;
}
