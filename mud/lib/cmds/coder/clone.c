// Traducido por neverbot 01

#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  position = 1;
}

static int cmd(string str, object me, string verb) 
{
  object ob;
  string err, *filenames;
  int loop, mov;

  if (!strlen(str)) 
  {
    notify_fail("¿Clonar que?\n");
    return 0;
  }

  filenames = get_files(str);
  if (!sizeof(filenames))  
  {
    notify_fail("No hay archivos coincidentes.\n");
    return 0;
  }

  for(loop = 0; loop < sizeof(filenames); loop++) 
  {
    str = filenames[loop];
    if (file_size(str) < 0 && file_size(str + ".c") < 0) 
    {
      notify_fail("No hay archivos coincidentes.\n");
      return 0;
    }
 
    err = catch(ob = clone_object(str));
    tell_object(this_player(), "Error in clone_object():\n   " + err + "\n"); 
 
    // ob = clone_object(str);

    if (ob) 
    {
      err = catch((mov = (int)ob->move(this_player())));
      tell_object(this_player(), "Error in move(this_player()):\n   " + err + "\n");

      if (err || mov) 
      {
        err = catch(ob -> move(environment(this_player())));
        tell_object(this_player(), "Error in move(environment()):\n   " + err + "\n");
      }

      // Añadido por neverbot, los objetos unicos pueden ser destruidos durante el move
      if (!ob)
      {
        tell_object(this_player(),"Error, no se ha podido clonar el objeto (¿objeto único?).\n");
        return 1;
      }

      tell_object(this_player(), "Ok.  Objeto "+file_name(ob)+" clonado en "+
        (environment(ob)==this_player() ? "ti" :
        (environment(ob)==environment(this_player()) ? "este lugar" :
          this_player()->desc_object(ob)))+
        ".\n");

      tell_room(environment(this_player()),this_player()->query_cap_name() + 
        // " busca " + ((int)ob->query_genero()?"una ":"un ") +
        " busca su "+
        ((string)ob->query_short()?(string)ob->query_short():"objeto") +
        " en otra dimensión.\n",({this_player()}));
    } 
    else 
    {
      tell_object(this_player(),"Error, no se ha podido clonar el objeto.\n");
    }
  }
  return 1;
} /* clone() */


