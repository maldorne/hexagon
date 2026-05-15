
#include <mud/cmd.h>
#include <room/location.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "room2loc" }));
  set_usage("room2loc < filename | dirname | here >");
  set_help("Convert one or more legacy .c rooms into .o locations.\n" +
           "The target may be an absolute file or directory, a path\n" +
           "relative to your coder path, or 'here' for your current\n" +
           "room. Coordinates are guessed from the exit graph after\n" +
           "the batch finishes.\n");
}

static int _filter_dot_c(string file)
{
  return file[strlen(file) - 2..] == ".c";
}

static int cmd(string str, object me, string verb)
{
  string * files;

  if (!str || !strlen(str))
  {
    notify_fail("Convert what?\n");
    return 0;
  }

  if (str == "here")
  {
    if (!environment(me))
    {
      notify_fail("You are nowhere.\n");
      return 0;
    }
    files = ({ base_name(environment(me)) });
  }
  else
  {
    files = load_object(LOCATION_HANDLER)->resolve_targets(str);
    if (!sizeof(files))
    {
      notify_fail("No such file or directory.\n");
      return 0;
    }
  }

  // only .c files (rooms); skip already-converted locations
  files = filter_array(files, "_filter_dot_c");

  if (!sizeof(files))
  {
    write("No room files to convert.\n");
    return 1;
  }

  load_object(LOCATION_HANDLER)->batch_convert(files);

  return 1;
}
