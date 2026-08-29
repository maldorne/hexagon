
// auxiliar functions to get common singleton objects like
// tables and handlers

#include <kernel.h>
#include <mud/translations.h>

// `from` is what the game is resolved against, defaulting to the caller. Pass
// the player (or anything standing in the world) from a file that belongs to no
// game -- a command under /lib/cmds/ asking for its own game gets none, and
// would silently be answered by the lib copy instead of the one the player's
// game overrides it with.
static nomask object table(string name, varargs object from) 
{
  object ob;
  string game, key;

  if (!from)
    from = this_object();

  // Resolution is per game -- a game's own copy overrides the lib one -- so
  // the cache key carries the game, or a lookup made from inside one game
  // would hand back what a different game resolved. Objects that belong to
  // no game key by the bare name.
  game = game_name(from);
  key = (game && strlen(game)) ? game + "-" + name : name;

  ob = SINGLETON_HANDLER->get_table(key);

  if (objectp(ob))
    return ob;

  // first the game specific table, it will override the lib one
  catch 
  {
    if (ob = load_object(game_root(from) + "tables/" + name))
    {
      SINGLETON_HANDLER->set_table(key, ob);
      return ob;
    }

    if (ob = load_object("/lib/tables/" + name))
    {
      SINGLETON_HANDLER->set_table(key, ob);
      return ob;
    }
  }

  return nil;
}

// Same as table(): `from` is what the game is resolved against, defaulting to
// the caller.
static nomask object handler(string name, varargs object from) 
{
  object ob;
  string game, key;

  if (!from)
    from = this_object();

  // Resolution is per game -- a game's own copy overrides the lib one -- so
  // the cache key carries the game, or a lookup made from inside one game
  // would hand back what a different game resolved. Objects that belong to
  // no game key by the bare name.
  game = game_name(from);
  key = (game && strlen(game)) ? game + "-" + name : name;

  ob = SINGLETON_HANDLER->get_handler(key);

  if (objectp(ob))
    return ob;

  // first the game specific handler, it will override the lib one
  catch 
  {
    if (ob = load_object(game_root(from) + "handlers/" + name))
    {
      SINGLETON_HANDLER->set_handler(key, ob);
      return ob;
    }

    if (ob = load_object("/lib/handlers/" + name))
    {
      SINGLETON_HANDLER->set_handler(key, ob);
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

static nomask object package(string name) 
{
  object ob;

  catch 
  {
    if (ob = load_object("/packages/" + name + "/master.c"))
      return ob;
  }

  return nil;
}
