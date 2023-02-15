#include <standard.h>
#include <cmd.h>


inherit CMD_BASE;

void setup()
{
        position = 0;
}

protected int cmd(string str, object me, string verb) 
{
    string ret = "Avanzarás tu nivel de:\n";

    object ob = me->query_class_ob();
    if (ob) 
        ret += "   - Clase  con: %^BOLD%^" + ob->query_next_level_xp(me) + "%^RESET%^ puntos (llevas "+me->query_xp()+").\n";

    ob = me->query_guild_ob();
    if (ob) 
        ret += "   - Gremio con: %^BOLD%^" + ob->query_next_level_xp(me) + "%^RESET%^ puntos (llevas "+me->query_xp()+").\n";

    ob = me->query_job_ob();
    if (ob) 
        ret += "   - Oficio con: %^BOLD%^" + ob->query_next_level_xp(me) + "%^RESET%^ puntos de oficio (llevas "+me->query_job_xp()+").\n";


    tell_object(me, ret);
    
    me->set_trivial_action();
    return 1;
}

