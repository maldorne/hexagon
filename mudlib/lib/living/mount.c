
// Variables necesarias para tener una montura
// neverbot 7/2005

// #define SAVE_DIR "/save/mounts/"

static object mount;
// string mount_save_name;
string mount_file_name;

// default for mounts
int query_riding() { return 0; }

string query_mount_file_name() { return mount_file_name; }
void set_mount(object ob)
{ 
  mount = ob; 
  mount_file_name = base_name(mount);
  if (mount)
    mount->set_owner(this_object()->query_name());
}
object query_mount() { return mount; }

void create()
{
  mount = nil;
  // mount_save_name = "";
  mount_file_name = "";
}

void save_mount()
{
  if (mount)
  {
    mount_file_name = base_name(mount);
    mount->save_me();
  }
}

void load_mount()
{
  if (!mount_file_name)
    return;

  mount = clone_object(mount_file_name);
  if (!mount)
  {
    return;
  }

  mount->set_owner(this_object()->query_name());
  mount->load_me();
  mount->move_to_saved();
}

mixed * stats() 
{
  return ({ 
    ({"Mount (nosave)", mount, }),
    // ({"Mount Save Name", mount_save_name, }),
    ({"Mount File Name", mount_file_name, }),
    });
}
