// Translated by neverbot 01, again in 09/2019

#include <mud/cmd.h>
#include <language.h>

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
    notify_fail(_LANG_CLONE_WHAT);
    return 0;
  }

  filenames = get_files(str);
  if (!sizeof(filenames))
  {
    notify_fail(_LANG_CMD_NO_FILES);
    return 0;
  }

  for(loop = 0; loop < sizeof(filenames); loop++)
  {
    str = filenames[loop];
    if (file_size(str) < 0 && file_size(str + ".c") < 0)
    {
      notify_fail(_LANG_CMD_NO_FILES);
      return 0;
    }

    // err = catch(ob = clone_object(str));
    // if (err)
    //   write(_LANG_CLONE_ERROR_IN + " clone_object():\n   " + err + "\n");

    ob = clone_object(str);

    if (ob)
    {
      err = catch((mov = (int)ob->move(this_player())));
      if (err)
        write(_LANG_CLONE_ERROR_IN + " move(this_player()):\n   " + err + "\n");

      if (err || mov)
      {
        err = catch(ob->move(environment(this_player())));
        if (err)
          write(_LANG_CLONE_ERROR_IN + " move(environment()):\n   " + err + "\n");
      }

      // added by neverbot, unique objects could be destroyed during move
      if (!ob)
      {
        write(_LANG_CLONE_ERROR_MAYBE_UNIQUE);
        return 1;
      }

      write(_LANG_CLONE_OK_YOU);
      tell_room(environment(this_player()), _LANG_CLONE_OK_ENV, ({ this_player() }));
    }
    else
    {
      write(_LANG_CLONE_ERROR);
    }
  }
  return 1;
}
