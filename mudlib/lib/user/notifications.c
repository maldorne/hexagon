
#include <user/notifications.h>
#include <language.h>

#define MAX_NOTIFICATIONS 100

// notifications = ({ time, type, message })
static mixed * notifications;
// are there any notification pending?
static int pending_notifications;

mixed * query_notifications() { return notifications; }

void create()
{
  notifications = ({ });
  pending_notifications = false;
}

void notifications_commands()
{
  add_action("do_show_notifications", "notifications");
}

int query_pending_notifications() { return pending_notifications == true; }

void add_notification(string type, string message, varargs int silent)
{
  string time;

  // only store the last notifications, a player can be playing for a lot of time
  if (sizeof(notifications) > MAX_NOTIFICATIONS)
  {
    // remove the oldest notification
    notifications = notifications[1..];
  }

  time = itoa(time());
  
  // remove spaces, tabs and \n
  message = trim(message);

  notifications += ({ ({ time, type, message, (silent ? true : false) }) });
  
  if (!silent)
    pending_notifications = true;
}

// prototype
int show_notifications(varargs int only_pending);

// called from the action
int do_show_notifications(string str)
{
  return show_notifications(true);
}

// called from the player object, when the heart_beat finishes
int show_notifications(varargs int show_all)
{
  int i, max_length;
  string * times, * messages, text, title;

  if (!pending_notifications && !show_all)
    return 1;

  if (!arrayp(notifications))
    notifications = ({ });

  if (!sizeof(notifications))
  {
    write(_LANG_NO_NOTIFICATIONS);
    return 1;
  }

  messages = ({ });

  for (i = 0; i < sizeof(notifications); i++)
  {
    if (show_all)
    {
      int itime;
      itime = atoi(notifications[i][NOTIFICATION_TIME]);

      messages += ({ ctime(itime, 7) + " - " + notifications[i][NOTIFICATION_MSG]});
      notifications[i][NOTIFICATION_SENT] = true;
    }
    else if (!notifications[i][NOTIFICATION_SENT])
    {
      messages += ({ notifications[i][NOTIFICATION_MSG] });
      notifications[i][NOTIFICATION_SENT] = true;
    }
  }

  text = "";
  title = (show_all ? _LANG_PAST_NOTIFICATIONS : "");

  for (i = 0; i < sizeof(messages); i++)
    text += (sizeof(messages) > 1 ? "* " : "") + messages[i] + "\n";

  text = handler("frames")->frame(text, title, 0, 0, "notifications");
  
  this_user()->more_string("\n\n" + text + "\n");

  pending_notifications = false;
  return 1;
}

mixed * stats()
{
  return ({
    ({ "Notifications (nosave)", notifications, }),
          });
}
