// neverbot 04, idea taken from the same command by Iolo@RlMud
// (but with a different combat system, they are not that similar... )

#include <mud/cmd.h>
#include <translations/combat.h> // ac types

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "armours" }));
  set_usage("armours [<who>]");
  set_help("Shows information about all the armours worn by given player.");
}

static int cmd (string str, object me, string verb)
{
  int i;
  object *equip;
  object who;
  string ret;
  
  if (strlen(str))
  {
    who = find_living(str);
    if (!who)
    {
      notify_fail("Cannot find '"+str+"'.\n");
      return 0;
    }
  }
  else
  {
    who = me;
  }
  
  equip = who->query_worn_ob();

  ret  = "--------------------------------------------------------------------------\n";
  ret += "  Info about the armours worn by "+who->query_cap_name()+"\n";
  ret += "--------------------------------------------------------------------------\n";
  
  ret += sprintf("  %-18s %|10s %|15s %|4s %|4s %|4s %|4s\n",
           "Name", "Protects", "Type", "Ac +", "Slash", "Blunt", "Piercing");
  ret += "--------------------------------------------------------------------------\n";
  
  for (i = 0; i < sizeof(equip); i++) {
    
    if (equip[i]->query_localization()) {

       ret += sprintf("  %-18s %|10s %|15s |%4d |%4d |%4d |%4d\n",
           equip[i]->query_name(), 
           equip[i]->query_localization(),
           equip[i]->query_armour_type_name(),
           equip[i]->query_ac(), 
           equip[i]->query_ac_bon_against(SLASHING), 
           equip[i]->query_ac_bon_against(BLUNT), 
           equip[i]->query_ac_bon_against(PIERCING));
    }
  }
  ret += "--------------------------------------------------------------------------\n";
  
  write(ret);
  return 1;
}

