
// Codigo extraido de monster.c, neverbot 04/2009

#include <living/combat.h>

mixed *attack_effects;      /* ({ chance, identifier, data, ... }) */
int protecting;             /* Am I protecting someone currently ? */
string * protect;           /* People that I am protecting */
int follow_speed;           /* how long before we follow someone */

void create()
{
	attack_effects = ({ });
	protect = ({ });
	protecting = 0;
	follow_speed = 3;
}

// Para facilitar la utilizacion de estilos de combate:
void set_unarmed_style(string name)
{
  // Siempre al 100%
  this_object()->add_known_unarmed_style(name, 100);
  this_object()->set_unarmed_combat_style(name);
}

void set_armed_style(string name)
{
  // Siempre al 100%
  this_object()->add_known_armed_style(name, 100);
  this_object()->set_armed_combat_style(name);
}

void add_attacks( string * names )
{
  int i;
  mixed * tmp;
  mixed * messages;

  messages = allocate(3);
  messages[0] = ({ });
  messages[1] = ({ });
  messages[2] = ({ });
  
  for (i = 0; i < sizeof(names); i++)
  {
    // Si el estilo existe
    if (UNARMED_BASE->style_exists(names[i]))
    {
      tmp = UNARMED_BASE->query_messages(names[i]);

      // Si es una lista, la añadimos
      if (arrayp(tmp[0]))
      {
        messages[0] += tmp[0];
        messages[1] += tmp[1];
        messages[2] += tmp[2];
      }
      else // En otro caso sera un string individual
      {
        messages[0] += ({ tmp[0] });
        messages[1] += ({ tmp[1] });
        messages[2] += ({ tmp[2] });
      }
    }
  }
  
  // si tenemos mensajes
  if (sizeof(messages[0]))
    this_object()->set_messages(messages);
}

mixed * query_attack_effects() { return attack_effects; }
int add_attack_effect(int chance, string name, mixed obj)
{
	int i;

	if ((i = member_array(name, attack_effects)) == -1)
    attack_effects += ({ chance, name, obj  });
	else
	{
		attack_effects[i-1] = chance;
		attack_effects[i+1] = obj;
	}
	return 1;
}

int remove_attack_effect(string name) 
{
	int i;

	if ((i = member_array(name, attack_effects)) == -1)
    return 0;

	attack_effects = delete(attack_effects, i-1, 3);
	return 1;
}

void add_protect(mixed shield)
{
	int i;

	if (pointerp(shield))
    for (i = 0; i < sizeof(shield); i++)
      add_protect(shield[i]);
	else if (member_array(shield, protect) != -1)
    return;
	else
    protect += ({ shield });
}

int query_protecting() { return protecting; }
int is_protecting() { return protecting; }

void set_protecting(int i) { protecting = i; }

string *query_protect() { return protect; }
void set_protect(string *shields)
{
	int i;

	for (i = 0; i < sizeof(shields); i++)
    add_protect(shields[i]);
}

void remove_protect(mixed shield) 
{
	int i;

	if (pointerp(shield))
    for(i = 0; i < sizeof(shield); i++)
      remove_protect(shield[i]);
	else
	{
		if ((i = member_array(shield, protect)) == -1)
	    return;
		protect = delete(protect, i, 1);
	}
}

void do_follow_move(string dir) 
{
	this_object()->queue_action(dir);
}

int query_follow_speed() { return follow_speed; }
void set_follow_speed(int f) { follow_speed = f; }

void log_death(string filename, object attacker)
{
	object * obs;
	int i;
	string log;

  obs = this_object()->query_attacker_list() + this_object()->query_call_outed();
	
	/*
	if (file_size(filename) == -1)
		return 0;
	*/
		
	log = "["+ctime(time(),4)+"] "+this_object()->query_cap_name() + " matado por " + attacker->query_cap_name() + 
		" ("+attacker->query_class_name()+" nivel "+attacker->query_level()+")\n";
		
	if (sizeof(obs) > 1)
		log += "\tayudado por ";
	
	for (i = 0; i < sizeof(obs); i++)
	{
		if (obs[i] == attacker)
			continue;
			
		log += obs[i]->query_cap_name() +" ("+obs[i]->query_class_name()+" "+obs[i]->query_level()+")";
		
		if (i<sizeof(obs)-1)
			log += ", ";
		else
			log += ".\n";
	}

#ifdef COMMON_SAVE	
	log_file("common/npcs/"+implode(explode(real_file_name(this_object()), "/"), "_") + "_log", log);	
#else	
	write_file(filename, log);	
#endif

}

mixed *stats() 
{
  int i;
  mixed * ret;
  mixed * shield;

  shield = ({ });

  for (i = 0; i < sizeof(protect); i++)
    shield += ({ ({ "Protect "+i, protect[i] }) });

  ret = ({
      ({ "Protecting (nosave)", protecting }),
      ({ "Attack effects", attack_effects }),
      ({ "Follow Speed", query_follow_speed() }),
    });

  return ret + shield;
}