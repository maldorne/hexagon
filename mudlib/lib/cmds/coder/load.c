#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;

void setup(){
  position = 1;
}

protected int cmd(string str, object me, string verb) {
 string *filenames, err;
 int loop;

  if (!str) {
    notify_fail("¿Cargar qué?\n");
    return 0;
  }
  filenames = get_cfiles(str);
  if (!sizeof(filenames)) {
    notify_fail("No existe ese archivo.\n");
    return 0;
  }
  for(loop = 0; loop < sizeof(filenames); loop++) {
    str = filenames[loop];
    if (file_size(str)<0) {
      write(str + ": No existe ese archivo.\n");
       continue;
    }
    if (err = catch(str->load_up_with_yellow()))
      write("No se ha podido cargar el archivo '"+str+"'.\nError: "+err+"\n");
    else
      write("Archivo '"+str+"' cargado.\n");
  }
  // write("Ok.\n");
  return 1;
} /* load() */
 
