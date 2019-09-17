#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;

void setup(){

position = 1;
}



int rec_du(string path) {
  string *files;
  int i, size, tot;
  if (path[strlen(path)-1] != '/')
    path += "/";
  files = get_dir(path+"*");
  for (i=0;i<sizeof(files);i++) {
    if (files[i] == "." || files[i] == "..")
      continue;
    size = file_size(path+files[i]);
    if (size > 0)
      tot += size;
    else if (size == -2) {
      printf("%-30s %5d\n", path+files[i], (size = rec_du(path+files[i])));
      tot += size*1024;
     }
  }
  return (tot+1023)/1024;
} /* rec_du() */

protected int cmd(string str, object me, string verb) {
  notify_fail("Must have write access to be allowed to use du on a dir.\n");
  if (!str) {
    if(!"secure/master"->valid_write(this_player()->query_current_path(),
                geteuid(this_player())))

     return 0;
    printf("%-30s %5d\n", "Total:", rec_du((string)this_player()->query_current_path()));
    return 1;
  }
/* forget about type 2 for now ;) */
  return 0;
} /* du() */
 
