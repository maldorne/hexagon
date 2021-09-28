// - Añadido define para el numero maximo de lineas y subido de 
//   900 a 2000 y arreglos varios.
//   neverbot 08/2008

#include <mud/cmd.h>

inherit CMD_BASE;

#define NUMBER_OF_LINES 2000

string query_usage()
{
  return "grep [-in] <pattern> <file(s)>";
}

string query_help()
{
  return "Search for a patter in one or multiple files.\n" + 
         "Flags:  -n do not show the line where the pattern was found\n" + 
         "        -i ignore case";
}

static int cmd(mixed str, object me, string verb) 
{
  int i, j, num, ignore_case, found;
  string *files, *bit, *bit2;
  string s1, s2, s3, s4, s5;
  string err;

  if (!strlen(str)) 
  {
    notify_fail(query_help()); 
    return 0;
  }

  num = 1;
  ignore_case = 0;
  found = 0;

  if (sscanf(str, "-n %s", str) == 1)
  {
    num = 0;
  }

  if (sscanf(str, "-i %s", str) == 1)
  {
    ignore_case = 1;
  }

  if (sscanf(str, "%s -n %s", s1, s2) == 2) 
  {
    num = 0;
    str = s1 + " " + s2;
  }

  if (sscanf(str, "%s %s", s1, s2) != 2) 
  {
    notify_fail(query_help()); 
    return 0;
  }

  files = get_files(s2);

  if (ignore_case)
  {
    s5 = capitalize(s1);
  }

  if (!sizeof(files)) 
  {
    notify_fail("File(s) " + s2 + " not found.\n");
    return 0;
  }

  for (i = 0; i < sizeof(files); i++)
  {
    if (file_size(files[i]) > 0)
    {
      // read whole file
      catch (str = read_file(files[i], 0, NUMBER_OF_LINES) );

      if (!str)
      {
        tell_object(this_player(), "Empty file or too big to use grep (" + files[i] + ").\n");
        continue;
      }

      // search in the whole file
      if ( (sscanf(str, "%s" + s1 + "%s", s3, s4) == 2) ||
        (ignore_case && sscanf(str, "%s" + s5 + "%s", s3, s4) == 2) )
      {
        j = 0;
        found = 1;

        write("Found in file: " + files[i] + "\n");

        if (str && num) 
        {
          while( (sscanf(str, "%s" + s1 + "%s", s3, s4) == 2 ) ||
              (ignore_case && sscanf(str, "%s" + s5 + "%s", s3, s4) == 2) )
          {
            bit = explode(s3, "\n");
            bit2 = explode(s4, "\n");

            if (sizeof(bit))
              write(sprintf("%4d: %s\n", j + sizeof(bit), bit[sizeof(bit)-1] + s1 + 
                                                          (sizeof(bit2) ? bit2[0] : "")));

            j += sizeof(bit);

            str = implode(bit2[1..], "\n");
          }
        }
      }
    }
  }
  
  if (found == 0)
    write("'"+s1+"' was not found in any file.\n");

  return 1;
}

