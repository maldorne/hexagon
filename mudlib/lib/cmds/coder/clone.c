// Translated by neverbot 01, again in 09/2019

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "clone" }));
  set_usage("clone <file path>");
  set_help("Creates a clone given the path of a file.\n" +
     "If the object can be moved to your inventory, it will be placed there, " +
     "in your environment in other case.");
}

static int cmd(string str, object me, string verb)
{
  object ob;
  string err, *filenames;
  int loop, mov;

  if (!strlen(str))
  {
    write("Syntax: " + query_usage() + "\n");
    return 1;
  }

  filenames = get_files(str);
  if (!sizeof(filenames))
  {
    notify_fail("There are no files with that name.\n");
    return 0;
  }

  for (loop = 0; loop < sizeof(filenames); loop++)
  {
    str = filenames[loop];

    if (file_size(str) < 0 && file_size(str + ".c") < 0)
    {
      notify_fail("There are no files with that name.\n");
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
        write("Error in move(this_player()):\n   " + err + "\n");

      if (err || mov)
      {
        err = catch(ob->move(environment(this_player())));
        if (err)
          write("Error in move(environment()):\n   " + err + "\n");
      }

      // added by neverbot, unique objects could be destroyed during move
      if (!ob)
      {
        write("Error, could not clone object (maybe unique?).\n");
        return 1;
      }

      write("Ok. Object " + file_name(ob) + " cloned in " +
        (environment(ob) == this_player() ? "you" :
        (environment(ob) == environment(this_player()) ? "this place" :
          this_player()->desc_object(ob))) +
        ".\n");
      tell_room(environment(this_player()), _LANG_CLONE_OK_ENV, ({ this_player() }));
    }
    else
    {
      write("Error, the object could not be cloned.\n");
    }
  }
  return 1;
}
