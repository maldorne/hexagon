
// Singletons handler
// Keeps tracking of game tables and handlers

static mapping _tables;
static mapping _handlers;

void create() 
{
  // anti-cloning
  if (file_name(this_object()) != "/lib/core/singletons") 
  {
    write("This object cannot be cloned.\n");
    destruct(this_object());
    return;
  }

  ::create();
  _tables = ([ ]);
  _handlers = ([ ]);
}

mapping handlers() { return _handlers; }
mapping tables() { return _tables; }

object get_handler(string name)
{
  if (_handlers[name])
    return _handlers[name];

  return nil;
}

object get_table(string name)
{
  if (_tables[name])
    return _tables[name];

  return nil;
}

object set_handler(string name, object ob)
{
  _handlers[name] = ob;
}

object set_table(string name, object ob)
{
  _tables[name] = ob;
}
