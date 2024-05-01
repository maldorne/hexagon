

// The given object has just been cloned.  Called just before the object
// is initialized with create(1).
// void clone(string owner, object obj) 
// {

// }

// The given object is about to be destructed.
// void destruct(string owner, object obj) 
// {

// }

// The given inheritable object is about to be destructed.
// void destruct_lib(string owner, string path) 
// {
  
// }


void create()
{
  // anti-cloning
  if (file_name(this_object()) != "/lib/core/objects") 
  {
    write("This object cannot be cloned.\n");
    destruct(this_object());
    return;
  }

  ::create();
}

// avoid discard or destructions
void dest_me()
{
  error("This object does not allow you to destroy it.\n");
}

void dwep()
{
  error("This object does not allow you to destroy it.\n");
}
