// Comando resistencias para extraer parte de la miga de la ficha, Folken 3/2002

#include <cmd.h>

inherit CMD_BASE;

void setup()
{
    position = 0;
}

string query_usage()
{
    return "resistencias";
}

string query_short_help()
{
    return "Devuelve informacion y caracteristicas magicas de un jugador.";
}

protected int cmd (string name, object quien, string verb)
{
    string info = "";
    int debug = quien->query_coder();
    object me, *tmp;

    if (debug)
    {
	if (!name) me = quien;
	else
	{
	    name = quien->expand_nickname(name);
	    me = find_living(name);
	    if (!objectp(me))
	    {
		tmp = find_match(name, environment(quien));
		if (sizeof(tmp)) me = tmp[0];
	    }
	    if (!objectp(me)) me = find_player(name);
	    if (!objectp(me)) me = find_object(name);
	    if (!objectp(me))
	    {
		notify_fail("Lo siento, no encuentro a '"+name+"'.\n");
		return 0;
	    }
	}
	if (!quien->query_coordinator() && !me->query_coder())
	    me = quien;
    }
    else me = quien;

    if (me->query_dead() && !debug)
    {
	notify_fail("Estas en forma espiritual, no necesitas conocer eso.\n");
	return 0;
    }

    if ((quien->query_coordinator()) || (debug && me->query_coder()) )
    {
	// info+="\nThac0:\t\t"+me->query_thac0()+"\t\t\t";
	// info+="AC total:\t"+me->query_total_ac()+"\n";
        info+="Características mágicas de '"+me->query_cap_name()+"':\n\n";
	info+="Res Acido\t(acid):\t\t"+me->query_res("acid")+" %\t";
	info+="Res Agua\t(water):\t"+me->query_res("water")+" %\n";
	info+="Res Aire\t(air):\t\t"+me->query_res("air")+" %\t";
	info+="Res Bien\t(good):\t\t"+me->query_res("good")+" %\n";
	info+="Res Electrico\t(electrical):\t"+me->query_res("electrical")+" %\t";
	info+="Res Enfermedad\t(disease):\t"+me->query_res("disease")+" %\n";
	info+="Res Frio\t(cold):\t\t"+me->query_res("cold")+" %\t";
	info+="Res Fuego\t(fire):\t\t"+me->query_res("fire")+" %\n";
	info+="Res Magico\t(magical):\t"+me->query_res("magical")+" %\t";
	info+="Res Mal\t\t(evil):\t\t"+me->query_res("evil")+" %\n";
	info+="Res Mental\t(psionic):\t"+me->query_res("psionic")+" %\t";
	info+="Res Muerte\t(death):\t"+me->query_res("death")+" %\n";
	info+="Res Proyectiles\t(missile):\t"+me->query_res("missile")+" %\t";
	info+="Res Tierra\t(stone):\t"+me->query_res("stone")+" %\n";
	info+="Res Veneno\t(poison):\t"+me->query_res("poison")+" %\t";
	info+="Res Caos\t(chaos):\t"+me->query_res("chaos")+" %\n";
	info+="Res Ley\t\t(law):\t\t"+me->query_res("law")+" %\t";
	info+="Invulnerable hechizos:\t\t"+me->query_property("invulnerable_hechizos")+"\n";
    }
    // printf("%-#*s\n", me->query_cols(), info);
    // printf("%s\n",info);
    tell_object(quien, info);
    return 1;
}
