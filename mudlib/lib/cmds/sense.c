// sense.c -- what the four sense commands (smell, listen, taste, feel) share.
//
// Every object can smell, sound, taste and feel of something (see
// /lib/core/basic/senses.c); these commands only ask. Without a word they ask
// the place. With one they look for that thing here or carried and ask it,
// first by the word itself (a prop answers for its own pieces) and then for
// what it is like as a whole; and if nothing here answers to the word, they ask
// the place for that word (the air, the flowers, a smell of sewers).
//
// An object or a room with a verb of its own for the same thing (a pendant you
// touch) still wins: actions are tried before commands.

#include <mud/cmd.h>

inherit CMD_BASE;

// "smell", "sound", "taste" or "feel": set by each command.
private string sense;

void set_sense(string str) { sense = str; }
string query_sense_kind() { return sense; }

// Messages each command gives when there is nothing to tell.
string query_nothing_here() { return ""; }
string query_nothing_in(object ob) { return ""; }

private void tell(string text)
{
  if (text[strlen(text) - 1] != '\n')
    text += "\n";
  write(text);
}

static int cmd(string str, object me, string verb)
{
  object place;
  object * obs;
  string text;
  int i;

  place = environment(me);
  if (!place)
    return 0;

  // no word: the place itself
  if (!str || !strlen(str))
  {
    text = place->query_sense(sense);
    if (!text)
    {
      notify_fail(query_nothing_here());
      return 0;
    }
    tell(text);
    return 1;
  }

  // a thing here or carried
  obs = find_match(str, ({ me, place }), 1);
  for (i = 0; i < sizeof(obs); i++)
  {
    text = obs[i]->query_sense(sense, str);
    if (!text)
      text = obs[i]->query_sense(sense);
    if (text)
    {
      tell(text);
      return 1;
    }
  }

  // a word of the place
  text = place->query_sense(sense, str);
  if (text)
  {
    tell(text);
    return 1;
  }

  notify_fail(sizeof(obs) ? query_nothing_in(obs[0]) : query_nothing_here());
  return 0;
}
