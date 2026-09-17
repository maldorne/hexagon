
#include <mud/secure.h>
#include <user/editor.h>

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

  if (this_player(1) != this_user())
    return 0;

  if (!strlen(str))
  {
    // in_editor = "(hidden)";
    // ed("frog", "fini_editor");
    notify_fail("Syntax: ed <file>\n");
    return 0;
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
          write("That file does not exist.\n");
          return 0;
        }
        else 
        {
          str = filenames[loop];
        }

        write("Ambiguous name, using: " + str + "\n");
      }
    }
  }

  if (file_size(str) == -2) 
  {
    write("Is a directory.\n");
    return 1;
  }

  in_editor = str;
  
  if (!SECURE->valid_write(str, geteuid(this_player()), "frog"))
    write("[read only]\n");
  
  // ed(str, "fini_editor");
  clone_object(EDITOR_OB)->start_file(this_player(), str);
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

mixed * stats() 
{
  return ({ 
            ({"(role) Ed Setup", ed_setup, }),
          });
}
            
