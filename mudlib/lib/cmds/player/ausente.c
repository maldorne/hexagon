#include <standard.h>
#include <cmd.h>

#include <properties.h>

inherit CMD_BASE;

void setup()
{
        position = 0;
}

protected int cmd(string str, object me, string verb) 
{
    if (!str)
    {
        if (!me->query_static_property(AWAY_PROP))
           write("No has indicado ningún mensaje de ausencia.\n");
        else
        {   
           me->remove_static_property(AWAY_PROP);
           write("Mensaje de ausencia borrado.\n");
        }
    }
    else
    {
        me->add_static_property(AWAY_PROP, str);
        write("Seleccionado mensaje de ausencia como %^BOLD%^RED%^'" + str + "'%^RESET%^.\n");
    }
    me->set_trivial_action();
    return 1;
}

