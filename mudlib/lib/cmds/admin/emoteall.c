
// A line to everybody connected, with your name in front of it. The anonymous
// half of the pair is 'echoall'.

#include <std.h>
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "emoteall" }));
  set_usage("emoteall <action>");
  set_help(
    "Emote to everybody connected.\n" +
    "\n" +
    "Your name goes in front of the text, so write it as an action: " +
    "'emoteall smiles at the world' reaches everybody as 'You smile at the " +
    "world'. To send a line nobody can trace back to you, use 'echoall'. " +
    "Every use is logged.");
}

static int cmd(string str, object me, string verb)
{
  object * connections;
  string text;
  int i;

  if (!str || !strlen(str))
  {
    notify_fail("Say what to emote: emoteall <action>\n");
    return 0;
  }

  log_file("echoes", "[" + ctime(time(), 4) + "] " + me->query_cap_name() +
           " emotealls: " + str + "\n");

  str += "%^RESET%^";
  text = me->query_cap_name() + " " + str + "\n";

  // the event carries who emoted, and event() would name this command
  // instead, so the connections are told one by one
  connections = users();
  for (i = 0; i < sizeof(connections); i++)
    if (connections[i] != me->user())
      connections[i]->event_player_emote_all(me, text);

  write("Sent: " + text);
  return 1;
}
