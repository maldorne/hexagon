
// auxiliar functions to get common singleton objects like
// tables and handlers

#include <kernel.h>
#include <mud/config.h>

static nomask object table(string name) 
{
  object ob;

  ob = SINGLETON_HANDLER->get_table(name);

  if (objectp(ob))
    return ob;

  // first the game specific table, it will override the lib one
  catch 
  {
    if (ob = load_object("/game/tables/" + name))
    {
      SINGLETON_HANDLER->set_table(name, ob);
      return ob;
    }

    if (ob = load_object("/lib/tables/" + name))
    {
      SINGLETON_HANDLER->set_table(name, ob);
      return ob;
    }
  }

  return nil;
}

static nomask object handler(string name) 
{
  object ob;

  ob = SINGLETON_HANDLER->get_handler(name);

  if (objectp(ob))
    return ob;

  // first the game specific handler, it will override the lib one
  catch 
  {
    if (ob = load_object("/game/handlers/" + name))
    {
      SINGLETON_HANDLER->set_handler(name, ob);
      return ob;
    }

    if (ob = load_object("/lib/handlers/" + name))
    {
      SINGLETON_HANDLER->set_handler(name, ob);
      return ob;
    }
  }

  return nil;
}

// analog to the previous ones, but returns a document
// important: the documents are checked in the lib directory _first_
// if not found then the game directory will be checked
static nomask string doc(string name)
{
  string path;

  // first int the lib docs directory, it overrides the game one
  catch 
  {
    if (file_exists("/lib/docs/" + GLOBAL_COMPILE_LANG + "/" + name))
    {
      return "/lib/docs/" + GLOBAL_COMPILE_LANG + "/" + name;
    }

    if ((GLOBAL_COMPILE_LANG != "en") && file_exists("/lib/docs/en/" + name))
    {
      return "/lib/docs/en/" + name;
    }

    if (file_exists("/game/docs/" + name))
    {
      return "/game/docs/" + name;
    }
  }

  return "";
}
