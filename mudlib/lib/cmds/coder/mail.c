// mail - read and send mail from anywhere. Players have to go to a post
// office for the same thing.

#include <mud/cmd.h>
#include <files/postal.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "mail" }));
  set_usage("mail [names]");
  set_help("Opens your mailbox, or writes a letter straight away to the given names.");
}

static int cmd(string str, object me, string verb)
{
  object mailer;

  if (me->user() && me->user()->query_input_to())
  {
    notify_fail("Finish what you are doing first.\n");
    return 0;
  }

  mailer = clone_object(MAILER_OB);
  mailer->start(me, str);
  return 1;
}
