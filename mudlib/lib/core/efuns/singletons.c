
// auxiliar functions to get common singleton objects like
// tables and handlers

#include <kernel.h>
#include <mud/translations.h>

static nomask object table(string name) 
{
  object ob;

  // ob = SINGLETON_HANDLER->get_table(name);

  // if (objectp(ob))
  //   return ob;

  // first the game specific table, it will override the lib one
  catch 
  {
    if (ob = load_object(game_root(this_object()) + "tables/" + name))
    {
      SINGLETON_HANDLER->set_handler(game_name(this_object()) + "-" + name, ob);
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

  // ob = SINGLETON_HANDLER->get_handler(name);

  // if (objectp(ob))
  //   return ob;

  // first the game specific handler, it will override the lib one
  catch 
  {
    if (ob = load_object(game_root(this_object()) + "handlers/" + name))
    {
      SINGLETON_HANDLER->set_handler(game_name(this_object()) + "-" + name, ob);
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
    if (file_exists("/docs/" + GLOBAL_COMPILE_LANG + "/" + name))
    {
      return "/docs/" + GLOBAL_COMPILE_LANG + "/" + name;
    }

    // if we do not have a document in the current language but it does
    // exist in the english documentation, use it
    if ((GLOBAL_COMPILE_LANG != "en") && file_exists("/docs/en/" + name))
    {
      return "/docs/en/" + name;
    }

    if (file_exists(game_root(this_object()) + "docs/" + name))
    {
      return game_root(this_object()) + "docs/" + name;
    }
  }

  return "";
}
