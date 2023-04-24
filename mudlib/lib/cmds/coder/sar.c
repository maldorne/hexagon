/*
  Nanvaent Industries International

  LPC: Search and Replace

  Written by  : Weazel@Nanvaent Nov 92
  Modified by : Bill@Nanvaent Jan 93
  Modified by : Bill@Nanvaent+@Discworld Mar 93
  Modified by : Dimmak@RlMud Apr 99 to allow more complex things
  Modified by : neverbot@CcMud Feb 10 to allow silent mode
*/

#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "sar" }));
  set_usage("sar -s $<text to look for>$ $<text to exchange with>$ <file(s)>");
  set_help("Exchanges a piece of text with another, in one or multiple files.\n" + 
         "Use -s for silent output.");
}

static int cmd(string str, object me, string verb)
{
  int i;
  string *files;
  string s1, s2, s3, s4, s5;
  int silent;

  if (!strlen(str))
  {
    write("Syntax: " + query_usage() + "\n");
    return 1;
  }

  silent = 0;
  
  // if (sscanf(str, s4+"%s"+s4+" "+s4+"%s"+s4+" %s", s1, s2, s3) != 3)
  if (!stringp(str) || (sscanf(str, "$%s$ $%s$ %s", s1, s2, s3) != 3))
  {
    if ((sscanf(str, "%s $%s$ $%s$ %s", s5, s1, s2, s3) == 4) && (s5 == "-s"))
    {
      silent = 1;
    }
    else
    {
      notify_fail("Syntax: sar [-s] $<text to look for>$ $<text to exchange with>$ <file(s)>\n");
      return 0;
    }
  }
  
  s4 = str[0..0];
  
  // No hacemos log de lo que se haga con el sar
  // log_file("SAR",me->query_name()+" : "+str+"\n");
  files = (string *)get_files(s3);
  
  if (!sizeof(files))
  {
    notify_fail("Files(s) " + s3 + " not found.\n");
    return 0;
  }
  
  if (silent)
    write("Searching files...\n");
  
  for (i = 0; i < sizeof(files); i++)
  {
    if (file_size(files[i]) <= 0) 
      continue;
      
    if (!silent)
      write("Searching in file '"+files[i]+"'.\n");
      
    s4 = read_file(files[i]);
    
    if (s4)
    {
      s4 = replace(s4, s1, s2);
      rm(files[i]);
      write_file(files[i], s4);
    }
    else
      write("... fail: The file '"+files[i]+"' could not be read.\n");
  }
  
  if (silent)
    write("\t... finished.\n");
    
  return 1;
}
