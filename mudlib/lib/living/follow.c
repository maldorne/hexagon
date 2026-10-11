// Following: a living that follows another goes wherever the other goes,
// through the same exit, right after it, whatever is happening to either of
// them. Started by the follow skill, or from code for an NPC (a pet, a
// mount, a creature that keeps up a fight), and ended by stop_following, the
// evade skill, or the other one going away.

#include <room/room.h>
#include <translations/pov.h>
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

// Those following this living who are here with it.
object * query_followers_here()
{
  object * list, * here;
  int i;

  list = query_followers();
  here = ({ });
  for (i = 0; i < sizeof(list); i++)
    if (list[i]->query_following() == this_object() &&
        environment(list[i]) == environment(this_object()))
      here += ({ list[i] });

  return here;
}

// Set while this living goes through an exit behind its leader: the move
// tells nothing, the leader tells one line for the whole group.
static int coming_along;
int query_coming_along() { return coming_along; }
void set_coming_along(int i) { coming_along = i; }

// A living as `viewer` sees them, coloured.
private string name_seen_by(object viewer, object who)
{
  string color, name;

  // change player appereance from /handler/pov.c, neverbot 10/04
  color = (string)POV->query_color(viewer, who);
  name = (string)POV->query_name_changed(viewer, who);
  return strlen(color) ? color + name + "%^RESET%^" : name;
}

// One line to everybody in `room` but the group, the names as each sees them:
// the leader's movement line with those who came along added at its end.
private void tell_group_line(object room, string tpl, object * came,
                             string in_the_dark)
{
  object * obs, * seen;
  string line, * names;
  int i, j;

  if (!room || !tpl)
    return;

  // the leader's lines start with the marker move_living strips
  if (strlen(tpl) && tpl[0] == '@')
    tpl = tpl[1..];

  obs = all_inventory(room) - came - ({ this_object() });
  for (i = 0; i < sizeof(obs); i++)
  {
    if (!interactive(obs[i]))
      continue;

    if (obs[i]->check_dark(room->query_light()))
    {
      tell_object(obs[i], in_the_dark);
      continue;
    }

    line = replace_string(tpl, "$N", name_seen_by(obs[i], this_object()));

    names = ({ });
    seen = came - ({ obs[i] });
    for (j = 0; j < sizeof(seen); j++)
      if (!seen[j]->query_invis())
        names += ({ name_seen_by(obs[i], seen[j]) });

    // the followers go before the closing full stop of the leader's line
    if (sizeof(names))
    {
      if (strlen(line) && line[strlen(line) - 1] == '.')
        line = line[0 .. strlen(line) - 2];
      line += _LANG_FOLLOW_FOLLOWED_BY + query_multiple_short(names) + ".";
    }

    tell_object(obs[i], wrap(fix_string(line + "\n"), obs[i]->user() ?
                obs[i]->user()->query_cols() : 0));
  }
}

// After this living has gone out of `from` by `dir`, take the ones following
// it who were there through the same exit, each as anybody would (a closed
// door, a guard), so one may be left behind. Then tell each room one line for
// all who made it. With no line of the leader's own (it went unseen), each
// follower is seen going on their own.
void move_group(string dir, object from, object * group, string leave_tpl,
                string arrive_tpl)
{
  object * came, * list;
  string name;
  int i;

  name = this_object()->query_cap_name();
  came = ({ });

  for (i = 0; i < sizeof(group); i++)
  {
    if (!group[i])
      continue;

    tell_object(group[i], _LANG_FOLLOW_GO_ME);
    group[i]->set_coming_along(leave_tpl ? 1 : 0);
    from->force_exit(dir, group[i]);
    group[i]->set_coming_along(0);

    if (environment(group[i]) == environment(this_object()))
      came += ({ group[i] });
    else
      tell_object(group[i], _LANG_FOLLOW_GO_FAILED);
  }

  // those following who were somewhere else have lost the trail
  list = query_followers() - group;
  for (i = 0; i < sizeof(list); i++)
    if (environment(list[i]) != environment(this_object()))
    {
      tell_object(list[i], _LANG_FOLLOW_LOST_ME);
      list[i]->stop_following(1);
    }

  if (!leave_tpl)
    return;

  tell_group_line(from, leave_tpl, came, _LANG_FOLLOW_SOMEBODY_LEAVES);
  tell_group_line(environment(this_object()), arrive_tpl, came,
                  _LANG_FOLLOW_SOMEBODY_ARRIVES);
}

mixed * follow_stats()
{
  return ({
      ({ "Following (nosave)", following, }),
      ({ "Followers (nosave)", query_followers(), }),
    });
}
