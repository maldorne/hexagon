#include <standard.h>
#include <cmd.h>


inherit CMD_BASE;

void setup()
{
        position = 0;
}

protected int cmd(string str, object me, string verb) 
{
    string ret = "Estado de tu equipo:\n";
	string weapons = "  Armas:\n";
	string shields = "  Escudos:\n";
	string armours = "  Otros:\n";
	object * obs = all_inventory(me);
	int i;
	int count_weap = 0;
	int count_arm = 0;
	int count_shi = 0;
	int complete = 0;

	if (me->query_dead())
	{
		notify_fail("Estando muerto no puedes hacer eso.\n");
		return 0;
	}

	if (str == "todo")
		complete = 1;

	for (i = 0; i < sizeof(obs); i++)
	{
		if (!obs[i]->cond_string())
			continue;
		if (complete || obs[i]->query_in_use())
			if (obs[i]->query_armour())
			{
				armours += sprintf("    %-30.30s : %s", capitalize(obs[i]->query_name()), obs[i]->cond_string());
				count_arm++;
			}
			else if (obs[i]->query_weapon())
			{
				weapons += sprintf("    %-30.30s : %s", capitalize(obs[i]->query_name()), obs[i]->cond_string());
				count_weap++;
			}
			else if (obs[i]->query_shield())
			{
				shields += sprintf("    %-30.30s : %s", capitalize(obs[i]->query_name()), obs[i]->cond_string());
				count_shi++;
			}
	}

	if (count_weap)
		ret += weapons;
	if (count_shi)
		ret += shields;
	if (count_arm)
		ret += armours;
	
	ret += "\n";

	if (!complete)
		ret += "Utiliza 'equipo todo' para ver el estado de todo lo que estás cargando.\n";

    tell_object(me, ret);
    
    me->set_trivial_action();
    return 1;
}

