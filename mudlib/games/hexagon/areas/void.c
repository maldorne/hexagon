
// Radix - fixes from new driver .. void was an unfriendly place
// after we moved to Aurora...
// Sept 28, 1995
// This got COMPLICATED .. argh
// Translated for CcMud, neverbot 7/03
// Player now go to ENTRY_POS

// TODO this should be a different destination for every game
// should check the character's game and find it in its master.c
#define ENTRY_POS "/games/ciudad-capital/areas/ciudad-capital/barrio-antiguo/rooms/calzada02"

#include <language.h>
#include <areas/common.h>

inherit "/lib/room";

int do_update(string str)
{
  int i;
  object *all, where;

  all = all_inventory(this_object());
  // if (!stringp(str) || str == "here")
  // {
    for (i = 0; i < sizeof(all); i++)
    {
      if (interactive(all[i])) 
      {
        if (all[i]->query_coder())
        {
          tell_object(all[i], _LANG_VOID_MOVING_CREATOR);
          where = load_object(CODER_COMMON_ROOM);
        }
        else
        {
          tell_object(all[i], _LANG_VOID_MOVING_PLAYER);
          where = load_object(ENTRY_POS);
        }

        if (!where)
        {
          tell_object(all[i], _LANG_VOID_ERROR);
          return 1;
        }

        all[i]->move(where);
        all[i]->do_look();
      }
      else
      {
        // dest those NPCs and items
        all[i]->dest_me();
      }
    }
  // }
  return 1; 
}

void setup() {
  set_light(40);
  set_short(_LANG_VOID_SHORT);
  set_long(_LANG_VOID_LONG);

  add_exit(_LANG_VOID_EXIT_NAME, ENTRY_POS, "standard");
}

void init()
{
  add_action("do_update", _LANG_VOID_COMMANDS);
  ::init();
}

