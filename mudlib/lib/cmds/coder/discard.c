// discard.c translated by neverbot 01/01

#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "discard" }));
  set_usage("discard <file>");
  set_help("Discard a file from memory.");
}

// This is for querying about objects who don't want to be destructed
static object discard_obj;

static int cmd(string str, object me, string verb) 
{
  string *file_names;
  int loop;
  string err;

  if (!strlen(str)) 
  {
    notify_fail("Discard what?\n");
    return 0;
  }

  file_names = get_files(str);

  if (!sizeof(file_names))
  {
    notify_fail("File " + str + " cannot be found.\n");
    return 0;
  }

  for (loop = 0; loop < sizeof(file_names); loop++) 
  {
    str = file_names[loop];
    discard_obj = find_object(str);

    if (!discard_obj) 
    {
      if (file_size(str) < 0)
      {
        write("File " + str + " does not exist.\n");
      }
      else 
      {
        write(str + " is not loaded.\n");
      }

      continue;
    }

    /* Hamlet's addition */
    if (environment(discard_obj)) 
    {
      write(str + " has an environment. It's not possible to " +
        "discard it.\n");
      continue;
    }

    err = catch(discard_obj->dest_me());

    if (err)
      write("Error in dest_me():\n   " + err + "\n");

    if (discard_obj) 
    {
      write("This object seems to have been created with a special purpose.\n");
      write("Are you sure you want to discard it? ");
      input_to("no_discard");
      return 1;
    }
  }

  write("Ok.\n");
  return 1;
} /* discard() */

void no_discard(string s) 
{
  string err;

  if (this_player()->affirmative(s)) 
  {
    err = catch(discard_obj->dwep());
    
    if (err)
      write("Error in dwep():\n   " + err + "\n");

    if (discard_obj) 
    {
      write("This object does not want to be discarded.\n");
      destruct(discard_obj);

      if (discard_obj) 
      {
        write("Destroying it has not been possible.\n");
        return;
      }
    }
  }
  
  write("Ok.\n");
  discard_obj = nil;
} /* no_discard() */
