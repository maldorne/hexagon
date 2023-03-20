
#include <language.h>

inherit "/lib/monster";

static object myfriend;

void set_friend(object friend) { myfriend = friend; }
object query_friend() { return myfriend; }

void event_fight_in_progress(object ob1, object ob2)
{
  if (ob1 && ob1 != myfriend)
    ::attack_by(ob1);
  if (ob2 && ob2 != myfriend)
    ::attack_by(ob2);
}

void attack_by(object ob)
{
  if (ob == myfriend)
  {
    tell_room(environment(this_object()), _LANG_UNDEAD_FRIEND_MSG);
    call_out("dest_me", 0);
  }

  ::attack_by(ob);
}

void attack_ob(object ob)
{
  if (ob == myfriend) 
    return;

  ::attack_ob(ob);
}

// just in case
int query_kill_xp() { return 0; }
