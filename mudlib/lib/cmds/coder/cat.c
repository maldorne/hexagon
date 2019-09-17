
#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  position = 1;
}

static int cmd(string str, object me, string verb)
{
  string *filenames;
  int loop;

  if (!strlen(str))
  {
    notify_fail(_LANG_CAT_WHAT);
    return 0;
  }

  if (str == "*")
  {
    notify_fail(_LANG_CAT_ASTERISK);
    return 0;
  }

  filenames = get_files(str);

  if (!sizeof(filenames))
  {
    notify_fail(str + ": " + _LANG_CMD_FILE_DOES_NOT_EXIST);
    return 0;
  }

  for (loop = 0; loop < sizeof(filenames); loop++)
  {
    int result;
    str = filenames[loop];

    if (sizeof(filenames) > 1)
     write(_LANG_CMD_FILE + ": " + str + "\n\n");

    if (file_size(str) < 8196)
    {
      result = cat(str);
      if (!result)
        write(_LANG_CMD_CANNOT_READ_FILE);
    }
    else
    {
      write(read_bytes(str,0,8195));
      write(_LANG_CAT_TRUNCATED);
    }
  }

  return 1;
}
