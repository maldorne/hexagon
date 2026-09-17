
#include <language.h>

inherit "/lib/monster";

static object myfriend;

void set_friend(object friend) { myfriend = friend; }
object query_friend() { return myfriend; }

// fighters holds the two sides of a blow traded in the room
void event_fight_in_progress(object ob, object * fighters)
{
  int i;

  for (i = 0; i < sizeof(fighters); i++)
    if (fighters[i] && fighters[i] != myfriend)
      ::attack_by(fighters[i]);
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
