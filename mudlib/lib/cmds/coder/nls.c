/*
 * this does an ls, now that get_dir has been written 
 * Modified by Turrican 13-7-95 with nicked code from TMI-2's ftpd.
 * - Added different options.
 * Modified by Dyraen@Rod, made external. 5-Nov-95
 */

#include <mud/cmd.h>
#include <mud/secure.h>
#include <files/file.h>

// #define FPERM "/secure/fperm"
#define CREATOR (SECURE->author_file(sprintf("%s/%s", str, direc[i][0]))? \
                SECURE->author_file(sprintf("%s/%s", str, direc[i][0])): \
                "mud")
#define DOMAIN  (SECURE->domain_file(sprintf("%s/%s", str, direc[i][0]))? \
                SECURE->domain_file(sprintf("%s/%s", str, direc[i][0])): \
                "mud")
#define CREATOR_D (SECURE->author_file(sprintf("%s/%s/.", str, direc[i][0]))? \
                SECURE->author_file(sprintf("%s/%s/.", str, direc[i][0])): \
                "mud")
#define DOMAIN_D (SECURE->domain_file(sprintf("%s/%s/.", str, direc[i][0]))? \
                SECURE->domain_file(sprintf("%s/%s/.", str, direc[i][0])): \
                "mud")

inherit CMD_BASE;

object tp;

void setup()
{
  set_aliases(({ "nls" }));
  set_usage("nls -acdflopCF [directory|file]");
  set_help("List files in current or given directory.\n" +
         "Options: -a includes hidden files.\n" +
         "         -c -C list in columns (by default).\n" +
         "         -d show only directories.\n" +
         "         -f -F extended info in file/directory names.\n" +
         "         -l list full data and permissions for every file and directory.\n" +
         "         -o use colors.\n" +
         "         -p paginated (for long listings).");
}

string dir_entry(string path, string name, int mask, object me)
{
  int size; 
  string *obs, tmp;

  size = file_size(get_path(path+name));
  if (size == -2) 
  {
    obs = get_dir((tmp = get_path(path+name))+(tmp == "/"?"":"/"));
    if (!obs)
      size = -1;
    else
      size = sizeof(obs);
    if (mask & MASK_F)
      name += "/";
  }
  else
  {
    if (find_object(path+name))
    {
      if (mask & MASK_F)
        name += "*";
    }
    size = (size / 1024) + 1;
  }

  if (size < 1000)
    return "    "[0..3-strlen(size+"")]+size+" "+name;

  return size+" "+name;
} /* dir_entry() */
 
int ls(string str, int mask, object me)
{
  mixed *direc;
  string *bit, *bing, bong, path;
  int i, j, size;
  // int k;
 
  seteuid(geteuid(me));
  path = str; 

  if (file_size(str) == -2 && str[strlen(str)-1] != '/' && !(mask & MASK_D))
    path += "/";

  if (mask & MASK_A)
    path += "*";

  direc = get_dir(path);

  if (!direc)
  {
    notify_fail("No files.\n");
    return 0;
  }

  if (!sizeof(direc))
  {
    notify_fail("No files.\n");
    return 0;
  }

  if (file_size(path) == -2)
  {
    if (path[strlen(path)-1] != '/')
      path += "/";
  }
  else
  {
    bit = explode(path, "/");
    bit = bit[0..sizeof(bit)-2];
    path = "/"+implode(bit,"/")+"/";
  }

  if(path == "//")
    path = "/";

  write("Directory: "+path+"\n");

  if (!(mask & MASK_A))
    direc = filter_array(direc, "check_dots", this_object(), 0);

  bing = allocate(sizeof(direc));
  j = sizeof(direc);

  if (!(mask & MASK_C) && !(mask & MASK_L))
  {
    if (!(mask & MASK_F) && !(mask & MASK_O))
    {
      bong = sprintf("%-*s\n", me->query_cols(), implode(direc, "\n"));
      bing = explode(bong, "\n");
    }
    else for (i = 0; i < j; i++)
    {
      if (file_size(path+direc[i]) == -2 || direc[i] == "..")
        if (mask & MASK_O)
          bing[i] = sprintf("%s%-*s", "%^GREEN%^",
            (int)me->query_cols()+8, direc[i]+"%^RESET%^"+
            (mask & MASK_F?"/":""));
        else
          bing[i] = sprintf("%-*s", me->query_cols(),
            direc[i]+"/");
      else 
      if (find_object(path+direc[i]))
        if (mask & MASK_O)
          bing[i] = sprintf("%s%-*s", "%^MAGENTA%^",
            (int)me->query_cols()-1, direc[i]+"%^RESET%^"+
            (mask & MASK_F?"*":""));
        else
          bing[i] = sprintf("%-*s", me->query_cols(),
            direc[i]+"*");
      else
        bing[i] = sprintf("%-*s", me->query_cols(), direc[i]);
    }
    bong = implode(bing, "\n");
  } 
  else if (!(mask & MASK_L))
  {
    j = sizeof(direc);

    for (i = 0; i < j; i++)
    {
      if (strlen(direc[i]) > 35)
      {
        write(dir_entry(path, direc[i], mask, me)+"\n");
        bing = delete(bing, i, 1);
        direc = delete(direc, i--, 1);
        j--;
        continue;
      } 
      else 
        bing[i] = dir_entry(path, direc[i], mask, me) + "\n";
    }

    bong = sprintf("%#-*s\n", me->query_cols(), implode(bing - ({ nil }), ""));

    if (mask & MASK_O)
    {
      /* Boy, it's hard to work with colors and sprintf. *sigh* 
         *especially* in column mode :-( */
      i = j;

      while (i--)
      {
        if (file_size(path+direc[i]) == -2 || direc[i] == "..")
          bong = replace_string(bong, " "+direc[i], 
            sprintf(" %s%s%s", "%^GREEN%^", direc[i], "%^RESET%^"));
        else if (find_object(path+direc[i]))
          bong = replace_string(bong, " "+direc[i], 
            sprintf(" %s%s%s", "%^MAGENTA%^", direc[i], "%^RESET%^"));
        else
          /* Believe me, it's needed. */
          bong = replace_string(bong, " "+direc[i],
            sprintf(" %s%s", "%^WHITE%^", direc[i]));
      }
    }
  }
  else
  {
    string tmp, tmp2, fname;
    // mixed *stats;
    int *count;
    object loaded;

    /* if path is a directory get contents */
    if (file_size( str ) == -2 && !(mask & MASK_D))
    {
      if ( str[ strlen( str ) - 1 ] == '/' )
        str += "*";
      else
        str += "/*";
    }

      /* begin long "list" */
    direc = get_dir(str, -1);

    if ( !(mask & MASK_A) || str == "/*" )  // Marziac 99
      direc = filter_array(direc, "check_dots", this_object(), 1);
    if (!direc || !(size = sizeof(direc)))
        return 0;

    bit = allocate(size);
    count = allocate_int(size);

    // initialize values
    for (i = 0; i < size; i++)
    {
      bit[i] = "";
    }

    i = strsrch(str, '/', -1);
    if (i >= 0)
      str = str[0..i];

    for (i = 0; i < size; i++)
    {
      // reset_eval_cost();
      /* process timestamp */
      tmp2 = ctime((direc[i])[2], 4); /* get last modified timestamp */

      if ((direc[i])[2] + (604800) < time()) 
        tmp = tmp2;
      else 
        tmp = sprintf("%s", ctime(direc[i][2], 4)); // shorter date string      
        // tmp = sprintf("%s %s", ctime(direc[i][2], 4), tmp2[10..16]);

      j = (direc[i])[1];   /* get filesize */

      if (j == -2)
      {
        count[i] = 1;
        /* directory */
        bit[i] = sprintf("%-*s", (me->query_cols()+
          (mask & MASK_O?17:0)),
          sprintf("drwxr%cx%c%c%c %3d %-8.8s %-8.8s [%4d] %12s %s%s%s%s",
          (SECURE->valid_write(sprintf("%s%s/fl.uff",str,direc[i][0]),
          DOMAIN_D, "get_dir")?'w':'-'),
          (SECURE->valid_read(sprintf("%s%s",str,direc[i][0]),
          "NOBODY", "get_dir")?'r':'-'),
          (SECURE->valid_write(sprintf("%s%s/fl.uff",str,direc[i][0]),
          "NOBODY", "get_dir")?'w':'-'),
          (SECURE->valid_read(sprintf("%s%s",str,direc[i][0]),
          "NOBODY", "get_dir")?'x':'-'),
          sizeof(filter_array((get_dir(get_path(
          sprintf("%s%s/*", str, direc[i][0])))?get_dir(
          get_path(sprintf("%s%s/*", str, direc[i][0]))):({ })),
          "is_dir", this_object(),
          sprintf("%s%s", str, direc[i][0])))+(direc[i][0..1] == ".." &&
          str == "/"?2:0),
          /*
          (FPERM->query_fperms(str+direc[i][0])?
          FPERM->query_fperms(str+direc[i][0])[0]:CREATOR_D), DOMAIN_D,
          */
          CREATOR,DOMAIN,
          sizeof(get_dir(get_path(sprintf("%s%s/*", str, direc[i][0])))),
          tmp, (mask & MASK_O?"%^GREEN%^":""),
          (direc[i][0]), (mask & MASK_O?"%^RESET%^":""),
          (mask & MASK_F?"/":"")));
      }
      else
      {
        /* file */
        count[i] = 0;
        fname = (strlen(direc[i][0]) > 20) ? 
                ("" + direc[i][0])[0..19] : 
                ("" + direc[i][0]);
        loaded = find_object(str+direc[i][0]);

        bit[i] = sprintf("%-*s", (me->query_cols()+
          ((mask & MASK_O) && loaded?19:0)), 
          sprintf("-rw%c%c%c-%c%c-   1 %-8.8s %-8.8s %6d %12s %s%s%s%s",
          (loaded ? 'x' : '-'),
          (SECURE->valid_read(sprintf("%s/%s",str,direc[i][0]),
          DOMAIN, "get_dir")?'r':'-'),
          (SECURE->valid_write(sprintf("%s/%s",str,direc[i][0]),
          DOMAIN, "get_dir")?'w':'-'),
          (SECURE->valid_read(sprintf("%s/%s",str,direc[i][0]),
          "NOBODY", "get_dir")?'r':'-'),
          (SECURE->valid_write(sprintf("%s/%s",str,direc[i][0]),
          "NOBODY", "get_dir")?'w':'-'),
          /*
          (FPERM->query_fperms(str+direc[i][0])?
          FPERM->query_fperms(str+direc[i][0])[0]:CREATOR),DOMAIN, j, tmp, 
          */
          CREATOR,DOMAIN, j, tmp,
          ((mask & MASK_O) && loaded?"%^MAGENTA%^":""),
          fname, ((mask & MASK_O) && 
          loaded?"%^RESET%^":""),
          (loaded?(mask & MASK_F?"*":""):"")));
      }

      bong = implode(bit, "\n");
    }
  }

  if (mask & MASK_P)
    me->more_string(bong, str);
  else
    write(bong + "\n");

  return 1;
} /* ls() */

int check_dots(mixed arg, int flag)
{
  if (flag)
    return (arg[0][0..0] != ".");
  return (arg[0..0] != ".");
}

int is_dir(mixed arg, string path)
{
  if (!tp)
    return (file_size(get_path(sprintf("%s/%s", path, arg))) == -2);
  else
    return (file_size(get_path(sprintf("%s/%s", path, arg))) == -2);
}

static int cmd(string str, object me, string verb)
{
  string flags;
  int mask, i;

  tp = me;

  if (!strlen(str)) 
    str = "";

  if ((sscanf(str,"-%s %s",flags,str) == 2) ||
      (sscanf(str,"-%s", flags) == 1) )
    for (i = 0; i < strlen(flags); i++)
      switch(flags[i..i])
      {
        case "a": mask |= MASK_A;
                  break;
        case "C":
        case "c": mask |= MASK_C;
                  break;
        case "d": mask |= MASK_D;
                  break;
        case "F":
        case "f": mask |= MASK_F;
                  break;
        case "l": mask |= MASK_L;
                  break;
        case "o": mask |= MASK_O;
                  break;
        case "p": mask |= MASK_P;
                  break;
        default:
          break;
      }
  
  if (!mask) 
    mask = 42;

  if (strlen(str) >= 1 && str[0..0] == "-") 
    str = "";

  str = get_path(str);

  return ls(str, mask, me->user());
} /* cmd() */
