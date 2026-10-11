// Following: a living that follows another goes wherever the other goes,
// through the same exit, right after it, whatever is happening to either of
// them. Started by the follow skill, or from code for an NPC (a pet, a
// mount, a creature that keeps up a fight), and ended by stop_following, the
// evade skill, or the other one going away.

#include <room/room.h>
#include <language.h>

static object following;    // whom this living follows
static object * followers;  // who follows this living

object query_following() { return following; }

object * query_followers()
{
  if (!followers)
    followers = ({ });
  followers -= ({ nil });
  return followers;
}

int add_follower(object ob)
{
  if (!ob || ob == this_object())
    return 0;
  if (member_array(ob, query_followers()) == -1)
    followers += ({ ob });
  return 1;
}

int remove_follower(object ob)
{
  if (!ob || member_array(ob, query_followers()) == -1)
    return 0;
  followers -= ({ ob });
  return 1;
}

// Stop following whoever this living follows. Both are told unless silent.
void stop_following(varargs int silent)
{
  object leader;

  leader = following;
  following = nil;
  if (!leader)
    return;

  leader->remove_follower(this_object());
  if (silent)
    return;

  tell_object(this_object(), _LANG_FOLLOW_STOP_ME);
  tell_object(leader, _LANG_FOLLOW_STOP_LEADER);
}

// Start following `leader`, with no roll: the follow skill rolls before
// calling this, and code that makes an NPC follow does not roll at all.
// Refused when it would close a circle of followers.
int start_following(object leader)
{
  object step;
  int i;

  if (!leader || leader == this_object() || !living(leader))
    return 0;

  // nobody follows somebody who, one way or another, follows them
  for (step = leader, i = 0; step && i < 20; step = step->query_following(), i++)
    if (step == this_object())
      return 0;

  if (following)
    stop_following(1);

  following = leader;
  leader->add_follower(this_object());
  return 1;
}

// After this living has gone out of `from` by `dir`, those following it who
// were still there go the same way. Each goes through the exit as anybody
// would (a closed door, a guard), so one may be left behind.
void drag_followers(string dir, object from)
{
  object * list;
  object follower;
  string name;
  int i;

  list = query_followers();
  if (!sizeof(list) || !from || !dir)
    return;

  name = this_object()->query_cap_name();

  for (i = 0; i < sizeof(list); i++)
  {
    follower = list[i];
    if (!follower || follower->query_following() != this_object())
    {
      remove_follower(follower);
      continue;
    }

    // left behind somewhere else: the trail is lost
    if (environment(follower) != from)
    {
      if (environment(follower) != environment(this_object()))
      {
        tell_object(follower, _LANG_FOLLOW_LOST_ME);
        follower->stop_following(1);
      }
      continue;
    }

    tell_object(follower, _LANG_FOLLOW_GO_ME);
    if (!from->force_exit(dir, follower, _LANG_FOLLOW_GO_LEAVE,
                          _LANG_FOLLOW_GO_ARRIVE))
      tell_object(follower, _LANG_FOLLOW_GO_FAILED);
  }
}

mixed * follow_stats()
{
  return ({
      ({ "Following (nosave)", following, }),
      ({ "Followers (nosave)", query_followers(), }),
    });
}
