// wimpy cmd from player.c, now external. Morgoth 7/Mar/95
// Changed: if you type "wimpy" without a number, or with an out of range
//          number, it will now show you the actual setting, without changing
//          it, after warning you. The old version would not show it in either
//          case, and would set it to 0 in the latter case.

#include <standard.h>
#include <cmd.h>
//#include <player.h>

inherit CMD_BASE;

int wimpy;

void setup()
{
        position = 0;
}

string query_usage()
{
        return "cobardia <num>";
}

string query_short_help()
{
        return "Establece el porcentaje de vida a partir de la cual tu personaje intentará huir.";
}

string wimpy_str(object me)
{
        return "Actualmente seleccionado a ("+me->query_wimpy()+")%.";
}

protected int cmd (string str, object me, string verb)
{
        if (!str || (str == ""))
        {
                // notify_fail("Uso: "+query_usage()+". "+wimpy_str(me)+"\n");
                tell_object(me, "Uso: "+query_usage()+".\n"+wimpy_str(me)+"\n");
                return 1;
        }
        else
        {
                if (sscanf(str, "%d", wimpy) != 1)
                {
                        notify_fail("Debes introducir un numero.\n");
                        return 0;
                }
                if (wimpy < 0 || wimpy > 100)
                {
                        notify_fail("Debes poner un numero entre 0 y 100. "+
                                        wimpy_str(me)+"\n");
                        return 0;
                }
        }
        if (wimpy)
                tell_object (me, "Estas en modo cobarde y huiras cuanto llegues "+
                        "a un "+wimpy+"% de tu de tus puntos de vida.\n");
        else
                tell_object (me, "Estas en modo valiente.\n");
        me->set_wimpy(wimpy);
        return 1;
}

