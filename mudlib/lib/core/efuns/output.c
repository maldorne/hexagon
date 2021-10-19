
#include "output/fix_string.c"
#include "output/percentage_bar.c"
#include "output/write.c"
#include "output/say.c"
#include "output/message.c"

static nomask void shout(string str, varargs object avoid)
{
  object *ob;
  int i;

  ob = users();

  for (i = 0; i < sizeof(ob); i++)
    if (ob[i] != avoid && !ob[i]->query_earmuffs())

  if(ob[i] != this_player())
    ob[i]->event_say(previous_object(), str, ({ }));
}

static nomask void tell_room(object ob, string str, varargs mixed avoid)
{
  if (!ob || !objectp(ob))
    return;

  event(ob, "say", str, avoid);
}

static nomask void tell_object(object ob, string str)
{
  if (undefinedp(ob))
    return;

  if (ob->query_player())
    ob = ob->user();

  ob->event_write(previous_object(), str);
}

static nomask void tell_player(object player, string text)
{
  tell_object(player, "\n\t%^BOLD%^" + text + "%^RESET%^\n");
}

