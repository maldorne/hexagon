// Translated by neverbot 01, again in 09/2019

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "clone" }));
  set_usage("clone <file path> [" + _LANG_CMD_CLONE_IN[0] + " <target>]");
  set_help("Creates a clone given the path of a file.\n" +
     "If the object can be moved to your inventory, it will be placed there, " +
     "in your environment in other case.\n" +
     "\n" +
     "An admin may name where the clone goes instead: a player anywhere in " +
     "the mud, or anything present where you stand. The object is put " +
     "straight into it and the clone is logged.");
}

static int cmd(string str, object me, string verb)
{
  object ob, target;
  string err, dest, where, * filenames, * words;
  int loop, mov, i;

  if (!strlen(str))
  {
    write("Syntax: " + query_usage() + "\n");
    return 1;
  }

  // "<file> in <target>" splits the paths from where the clone is to go
  words = explode(str, " ") - ({ "" });
  for (i = 0; i < sizeof(words); i++)
    if (member_array(words[i], _LANG_CMD_CLONE_IN) != -1)
      break;

  if (i < sizeof(words))
  {
    if (i == sizeof(words) - 1)
    {
      notify_fail("Say what to clone it into.\n");
      return 0;
    }

    if (!me->query_admin())
    {
      notify_fail("Only an admin may say where a clone goes.\n");
      return 0;
    }

    dest = implode(words[i + 1 ..], " ");
    str = implode(words[.. i - 1], " ");

    target = find_player(lower_case(dest));
    if (!target)
      target = present(dest, environment(me));
    if (!target)
      target = present(dest, me);

    if (!target)
    {
      notify_fail("There is nothing called " + dest + " to clone it into.\n");
      return 0;
    }
  }

  filenames = get_files(str);

  if (!sizeof(filenames))
    filenames = get_files(str + ".c");

  if (!sizeof(filenames))
  {
    notify_fail("There are no files with that name.\n");
    return 0;
  }

  for (loop = 0; loop < sizeof(filenames); loop++)
  {
    str = filenames[loop];

    if (file_size(str) < 0)
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
      if (target)
      {
        err = catch((mov = (int)ob->move(target)));
        if (err)
          write("Error in move(target):\n   " + err + "\n");
      }
      else
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
      }

      // added by neverbot, unique objects could be destroyed during move
      if (!ob)
      {
        write("Error, could not clone object (maybe unique?).\n");
        return 1;
      }

      if (target)
        where = stringp(target->query_cap_name()) ? target->query_cap_name()
                                                  : file_name(target);
      else if (environment(ob) == this_player())
        where = "you";
      else if (environment(ob) == environment(this_player()))
        where = "this place";
      else
        where = file_name(environment(ob));

      write("Ok. Object " + file_name(ob) + " cloned in " + where + ".\n");

      // putting an object inside somebody else is worth a trace
      if (target)
        log_file("clone", "[" + ctime(time(), 4) + "] " +
                 me->query_cap_name() + " clones " + file_name(ob) +
                 " into " + where + ".\n");
      else
        tell_room(environment(this_player()), _LANG_CLONE_OK_ENV,
                  ({ this_player() }));
    }
    else
    {
      write("Error, the object could not be cloned.\n");
    }
  }
  return 1;
}
