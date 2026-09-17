// Post office behaviour, inherited both by the post office room
// (/lib/ventures/post-office.c) and by the post office component for
// locations (/lib/location/components/post-office.c).

#include <files/postal.h>
#include <user/player.h>
#include <language.h>

void init()
{
  ::init();

  add_action("do_mail", _LANG_POST_OFFICE_MAIL_CMDS);
}

int do_mail(string str)
{
  object mailer;

  if (this_player()->query_property(GUEST_PROP))
  {
    notify_fail(_LANG_POST_OFFICE_NO_GUESTS);
    return 0;
  }

  if (this_player()->user() && this_player()->user()->query_input_to())
  {
    notify_fail(_LANG_POST_OFFICE_BUSY);
    return 0;
  }

  mailer = clone_object(MAILER_OB);
  mailer->start(this_player(), str);
  return 1;
}
