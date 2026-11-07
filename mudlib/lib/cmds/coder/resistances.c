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
                  "Res. acid:", who->query_resistance(DAMAGE_ACID),
                  "Res. water:", who->query_resistance(DAMAGE_WATER));
  info += sprintf("%-17s ( %3d )%%      %-17s ( %3d )%%\n", 
                  "Res. air:", who->query_resistance(DAMAGE_AIR),
                  "Res. good:", who->query_resistance(DAMAGE_GOOD));
  info += sprintf("%-17s ( %3d )%%      %-17s ( %3d )%%\n", 
                  "Res. electrical:", who->query_resistance(DAMAGE_ELECTRICAL),
                  "Res. disease:", who->query_resistance(DAMAGE_DISEASE));
  info += sprintf("%-17s ( %3d )%%      %-17s ( %3d )%%\n", 
                  "Res. cold:", who->query_resistance(DAMAGE_COLD),
                  "Res. fire:", who->query_resistance(DAMAGE_FIRE));
  info += sprintf("%-17s ( %3d )%%      %-17s ( %3d )%%\n", 
                  "Res. magical:", who->query_resistance(DAMAGE_MAGIC),
                  "Res. evil:", who->query_resistance(DAMAGE_EVIL));
  info += sprintf("%-17s ( %3d )%%      %-17s ( %3d )%%\n", 
                  "Res. psionic:", who->query_resistance(DAMAGE_PSIONIC),
                  "Res. death:", who->query_resistance(DAMAGE_DEATH));
  info += sprintf("%-17s ( %3d )%%      %-17s ( %3d )%%\n", 
                  "Res. missile:", who->query_resistance(DAMAGE_MISSILES),
                  "Res. stone:", who->query_resistance(DAMAGE_STONE));
  info += sprintf("%-17s ( %3d )%%      %-17s ( %3d )%%\n", 
                  "Res. poison:", who->query_resistance(DAMAGE_POISON),
                  "Res. chaos:", who->query_resistance(DAMAGE_CHAOS));
  info += sprintf("%-17s ( %3d )%%      %-17s %d\n", 
                  "Res. law:", who->query_resistance(DAMAGE_LAW),
                  "Spell invul.:", who->query_property(PROPERTY_SPELL_INVUL));

  info = handler("frames")->frame(info, 
                                  "Magical characteristics of '" + who->query_cap_name() + "'",
                                  this_user()->query_cols());
  tell_object(me, info);
  return 1;
}
