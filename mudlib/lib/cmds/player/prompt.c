
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  position = 0;
}

static int cmd(string str, object me, string verb) 
{
  string ret;
  object user;

  user = me->user();

  if (!strlen(str))
  {
    ret =  "Tu prompt actual es: '" + user->query_prompt_string()+"'\n";
    ret += "Puedes cambiarlo usando 'prompt <mensaje>'. Dentro de <mensaje>, se " + 
           "aceptan los siguientes accesos rápidos:\n" + 
           "     $n -> tu nombre\n" + 
           "     $h -> tus puntos de vida actuales\n" + 
           "     $g -> tus puntos de energía actuales\n" + 
           "     $B -> comenzar negrita\n" + 
           "     $m -> el nombre del mud\n" + 
           (user->query_coder() ? 
           "     $~ -> path actual\n"
             : "");
    ret += "Ejemplo: 'prompt $n ($h)' mostrará en cada línea:\n" + 
           "     " + me->query_name()+" (" + me->query_hp()+") >\n";
    tell_object(me, ret);
    return 1;
  }

  user->set_prompt(str);
  tell_object(me, "Ok, prompt cambiado.\n");

  me->set_trivial_action();
  return 1;
}

