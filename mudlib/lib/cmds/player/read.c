// read / leer -- what is written on something.
//
// The reading used to be an add_action on /lib/core/basic/read_desc.c, which
// every item inherits, so a room with four items registered the verb four
// times. The writing still belongs to the object (set_read_mess and its
// family, and query_read_text() renders it for whoever is reading); only the
// verb moved here.

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_READ_ALIAS);
  set_usage(_LANG_CMD_READ_SYNTAX);
  set_help(_LANG_CMD_READ_HELP);
}

static int cmd(string str, object me, string verb)
{
  object * obs, ob;
  string text, what;
  mixed written;
  int i, found;

  if (!str || !strlen(str))
  {
    notify_fail(_LANG_CMD_READ_WHAT);
    return 0;
  }

  // a number is a book of the library where you stand: the place itself, or
  // one of its components
  if (sscanf(str, "%d", i) == 1 && environment(me))
  {
    obs = ({ environment(me) }) +
          (environment(me)->query_location() ?
           environment(me)->query_components() : ({ }));
    for (i = 0; i < sizeof(obs); i++)
      if (obs[i]->query_library() && obs[i]->do_read_book(str))
        return 1;
  }

  // what you carry and what is here, the way every other verb looks
  obs = find_match(str, ({ me, environment(me) }));

  if (!sizeof(obs))
  {
    notify_fail(_LANG_CMD_READ_CANNOT_FIND);
    return 0;
  }

  for (i = 0; i < sizeof(obs); i++)
  {
    ob = obs[i];

    // a book is read page by page, and only one at a time
    if (ob->query_book())
    {
      handler("books")->start_reading(me, ob->query_text_file());
      return 1;
    }

    // not everything answers with a string: what has nothing written says so
    // in its own way, or not at all
    written = ob->query_read_text();
    if (!stringp(written) || !strlen(written))
      continue;
    text = written;

    write(text);
    found = 1;
  }

  if (!found)
  {
    // a prop answers inside a component with no name of its own: say it the
    // way the player called it
    what = obs[0]->query_short();
    if (!what || !strlen(what))
      what = str;
    notify_fail(_LANG_CMD_READ_NOTHING_WRITTEN);
    return 0;
  }

  return 1;
}
