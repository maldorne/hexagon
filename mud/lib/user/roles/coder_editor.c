
#include <mud/secure.h>

static int ed_setup;
static mixed in_editor;

// prototypes
object *wiz_present(string str, object onobj, varargs int nogoout);


void create()
{
}

int query_ed_setup() { return ed_setup; }
void set_ed_setup(int i) { ed_setup = i; }

static void role_commands()
{
  add_action("edit", "ed");
}

int edit(string str) 
{
  string *filenames, spam;
  object *things;
  int egg;

  if (this_player(1) != this_object()->query_player()) 
    return 0;

  if (!str) 
  {
    in_editor = "(hidden)";
    // ed("frog", "fini_editor");
    editor("frog"); // TODO
    return 1;
  }

   // dodgy idea, but allows 'ed here' or 'ed strawberry'
  if (sizeof(things = wiz_present(str, this_player()))) 
  {
    spam = file_name(things[0]);
    sscanf(spam, "%s#%d", spam, egg);
    if(spam[0..6] == "/global")
    filenames = get_files(str);
    else
    if (file_size(spam) < 0)
    filenames = ({ spam+".c" });
    else
    filenames = ({ spam });
  } 
  else
    filenames = get_files(str);

  if (!sizeof(filenames)) 
  {
    str = get_path(str);
  } 
  else 
  {
    if (sizeof(filenames) > 0) 
    {
      str = filenames[0];
      if (sizeof(filenames) > 1) 
      {
        int loop;

        loop = 0;
        while(loop < sizeof(filenames) && file_size(filenames[loop]) < 0)
          loop++;
        
        if(loop >= sizeof(filenames)) 
        {
          write("El archivo no existe.\n");
          return 0;
        }
        else 
        {
          str = filenames[loop];
        }

        write("Ambiguo, usando: " + str + "\n");
      }
    }
  }

  if (file_size(str) == -2) 
  {
    write("Es un directorio.\n");
    return 1;
  }

  in_editor = str;
  
  if (!SECURE->valid_write(str, geteuid(), "frog"))
    write("[sólo lectura] ");
  
  // ed(str, "fini_editor");
  editor(str); // TODO
  return 1;
} 

void set_in_editor(mixed funcname) 
{
  int fsize3;
  fsize3 = strlen(file_name(previous_object()));
  
  if (file_name(previous_object())[fsize3-8..fsize3-1] == "/cmds/ed")
    in_editor = funcname;
}

mixed query_in_editor() { return in_editor; }

void fini_editor() 
{
  in_editor = 0;
}

mixed stats() 
{
  return ({ 
            ({"(role) Ed Setup", ed_setup, }),
          });
}
            
