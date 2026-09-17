// Post office behaviour, inherited both by the post office room
// (/lib/ventures/post-office.c) and by the post office component for
// locations (/lib/location/components/post-office.c).

#include <files/postal.h>
#include <user/player.h>
#include <language.h>

// The sign this post office last put up, so putting it up again replaces it.
static object mail_sign;

object create_sign(varargs object where)
{
  if (!where)
    where = this_object();

  if (mail_sign)
    mail_sign->dest_me();

  mail_sign = where->add_sign(_LANG_POST_OFFICE_SIGN_DESC, _LANG_POST_OFFICE_SIGN_TEXT,
                              _LANG_POST_OFFICE_SIGN_NAME);
  mail_sign->add_alias(_LANG_POST_OFFICE_SIGN_ALIASES);
  return mail_sign;
}

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
