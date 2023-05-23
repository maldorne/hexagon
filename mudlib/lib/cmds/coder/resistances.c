// resistances, neverbot 3/2002

#include <mud/cmd.h>
// #include <language.h>
#include <living/spells.h>
#include <translations/combat.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "resistances" }));
  set_usage("resistances");
  set_help("Returns information and magical characteristics of a player.");
}

static int cmd(string name, object me, string verb)
{
  string info;
  object who, *tmp;

  info = "";

  // cmd moved to /coders directory, this should not happen anymore
  // if (me->query_coder())
  // {
    if (!strlen(name)) 
      who = me;
    else
    {
      name = me->expand_nickname(name);
      who = find_living(name);

      if (!objectp(who))
      {
        tmp = find_match(name, environment(me));
        if (sizeof(tmp)) 
          who = tmp[0];
      }

      if (!objectp(who)) who = find_player(name);
      if (!objectp(who)) who = find_object(name);
      if (!objectp(who))
      {
        notify_fail("Could not find '"+name+"'.\n");
        return 0;
      }
    }

    if (!me->query_admin())
      who = me;
  // }
  // else 
  //   who = me;

  // if (who->query_dead() && !me->query_coder())
  // {
  //   if (me == who)
  //     notify_fail("You are dead, don't need to know that.\n");
  //   else
  //     notify_fail(who->query_cap_name() + " is dead, you don't need to know that.\n");
  //   return 0;
  // }

  info += sprintf("%-17s ( %3d )%%      %-17s ( %3d )%%\n", 
                  "Res. acid:", who->query_resistance(ACID_STR),
                  "Res. water:", who->query_resistance(WATER_STR));
  info += sprintf("%-17s ( %3d )%%      %-17s ( %3d )%%\n", 
                  "Res. air:", who->query_resistance(AIR_STR),
                  "Res. good:", who->query_resistance(GOOD_STR));
  info += sprintf("%-17s ( %3d )%%      %-17s ( %3d )%%\n", 
                  "Res. electrical:", who->query_resistance(ELECTRICAL_STR),
                  "Res. disease:", who->query_resistance(DISEASE_STR));
  info += sprintf("%-17s ( %3d )%%      %-17s ( %3d )%%\n", 
                  "Res. cold:", who->query_resistance(COLD_STR),
                  "Res. fire:", who->query_resistance(FIRE_STR));
  info += sprintf("%-17s ( %3d )%%      %-17s ( %3d )%%\n", 
                  "Res. magical:", who->query_resistance(MAGIC_STR),
                  "Res. evil:", who->query_resistance(EVIL_STR));
  info += sprintf("%-17s ( %3d )%%      %-17s ( %3d )%%\n", 
                  "Res. psionic:", who->query_resistance(PSIONIC_STR),
                  "Res. death:", who->query_resistance(DEATH_STR));
  info += sprintf("%-17s ( %3d )%%      %-17s ( %3d )%%\n", 
                  "Res. missile:", who->query_resistance(MISSILES_STR),
                  "Res. stone:", who->query_resistance(STONE_STR));
  info += sprintf("%-17s ( %3d )%%      %-17s ( %3d )%%\n", 
                  "Res. poison:", who->query_resistance(POISON_STR),
                  "Res. chaos:", who->query_resistance(CHAOS_STR));
  info += sprintf("%-17s ( %3d )%%      %-17s %d\n", 
                  "Res. law:", who->query_resistance(LAW_STR),
                  "Spell invul.:", who->query_property(PROPERTY_SPELL_INVUL));

  info = handler("frames")->frame(info, 
                                  "Magical characteristics of '" + who->query_cap_name() + "'",
                                  who->user()->query_cols());
  tell_object(me, info);
  return 1;
}
