// discard.c traduccion neverbot 01/01

#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  position = 1;
}

/* This is for querying about objects who don't want to be destructed */
static object discard_obj;

static int cmd(string str, object me, string verb) 
{
  string *file_names;
  int loop;
  string err;

  if (!strlen(str)) 
  {
    notify_fail("¿Descartar el qué?\n");
    return 0;
  }

  file_names = get_files(str);

  for(loop = 0; loop < sizeof(file_names); loop++) 
  {
    str = file_names[loop];
    discard_obj = find_object(str);

    if (!discard_obj) 
    {
      if (file_size(str) < 0) 
        notify_fail("No existe el fichero " + str + "\n");
      else 
      {
        tell_object(me, str + " no esta cargado.\n");
      }
      continue;
    }

    /* Hamlet's addition */
    if(environment(discard_obj)) 
    {
      tell_object(me,str + " tiene un entorno(environment). No es posible "+
        "descartarlo.\n");
      continue;
    }

    err = catch(discard_obj->dest_me());
    this_player()->handle_error(err, "dest_me");

    if (discard_obj) 
    {
      tell_object(me, "Este objeto ha sido creado con un destino importante.\n");
      tell_object(me, "¿Estás seguro de querer hacer esto? ");
      input_to("no_discard");
      return 1;
    }
  }

  tell_object(me, "Ok.\n");
  return 1;
} /* discard() */

void no_discard(string s) 
{
  string err;

  if (this_player()->affirmative(s)) 
  {
    err = catch(discard_obj->dwep());
    this_player()->handle_error(err, "DWEP");
    if (discard_obj) 
    {
      tell_object(this_player(), "Este objeto NO quiere ser descartado.\n");
      destruct(discard_obj);

      if (discard_obj) 
      {
        tell_object(this_player(), "No ha sido destruido.\n");
        return;
      }
    }
  }
  tell_object(this_player(),"Ok.\n");
  discard_obj = nil;
} /* no_discard() */
