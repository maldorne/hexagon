// Folken 04, idea sacada del mismo comando de Iolo@RlMud
//  (aunque con distinto sistema de combate en poco se parecen... )

#include <standard.h>
#include <cmd.h>

inherit CMD_BASE;

protected int cmd (string str, object me, string verb)
{
  int i;
  object *equip;
  object who;
  string ret;
  
  if (str)
  {
    who = find_living(str);
    if (!who)
    {
      notify_fail("No puedo encontrar a '"+str+"'.\n");
      return 0;
    }
  }
  else
  {
      who = me;
  }	
  
  equip = who->query_worn_ob();

  ret = "--------------------------------------------------------------------------\n";
  ret += "  Datos sobre las armaduras que lleva: "+who->query_cap_name()+"\n";
  ret += "--------------------------------------------------------------------------\n";
  
  ret += sprintf("%20-s %10|s %15|s %4|s %4|s %4|s %4|s\n",
           "Nombre", "Protege", "Tipo", "Ac +", "Slash", "Blunt", "Piercing");
  ret += "--------------------------------------------------------------------------\n";
  
  for (i = 0; i < sizeof(equip); i++) {
    
    if (equip[i]->query_localization()) {

       ret += sprintf("%20-s %10|s %15|s %4|d %4|d %4|d %4|d\n",
           equip[i]->query_name()[0..19], equip[i]->query_localization()[0..9], 
           equip[i]->query_armour_type_name()[0..14], equip[i]->query_ac(), 
           equip[i]->query_slashing_bon(), equip[i]->query_blunt_bon(), equip[i]->query_piercing_bon());

      /*      
      tell_object(me, equip[i]->query_short() +
			" - Protege: '" + equip[i]->query_localization() + 
			"' Tipo: '"+ equip[i]->query_armour_type_name() + 
			"' Ac: '"+ equip[i]->query_ac() + 
			"' Bonus (cort, cont, perf): (" + equip[i]->query_slashing_bon() + ", " +
							  equip[i]->query_blunt_bon() + ", " +
							  equip[i]->query_piercing_bon() + ")\n");
      */
    }
  }
  ret += "--------------------------------------------------------------------------\n";
  
  tell_object(me, ret);
    
  return 1;
}

