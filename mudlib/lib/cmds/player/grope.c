// Grope (around in the dark)
// For blind players to find their way.
// Quark, April 97
// Translated by neverbot@RL

#include <mud/cmd.h>
#include <common/properties.h>
#include <room/room.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_GROPE_ALIAS);
  set_usage(_LANG_CMD_GROPE_SYNTAX);
  set_help(_LANG_CMD_GROPE_HELP);
}

static int cmd(string arg, object me, string verb)
{
  mixed * exits, exit;
  object * contents;
  int i, j, dark;

  dark = me->check_dark((int)environment(me)->query_light());

  if (!me->query_property(BLIND_PROP) && !dark) 
  {
    notify_fail(_LANG_CMD_GROPE_NO_DARK);
    return 0;
  }   

  if (me->query_property(SEARCH_PROP))
  {
    notify_fail(_LANG_CMD_GROPE_LOCKOUT);
    return 0;
  } 
  
  me->add_timed_property(SEARCH_PROP, 1, 1);

  i = random(10);

  // 30% chance of not finding an exit
  if (i < 3)
  {
    contents = all_inventory(environment(me)); 
    j = random(sizeof(contents));
    
    if (contents[j] == me)
    {
      tell_object(me, _LANG_CMD_GROPE_NOTHING);
      return 1;
    }
    else if (interactive(contents[j]))
    {
      tell_object(me, _LANG_CMD_GROPE_PLAYER_ME);
      tell_object(contents[j], _LANG_CMD_GROPE_PLAYER_THEM);
      tell_room(environment(me), _LANG_CMD_GROPE_PLAYER_ROOM, ({ me, contents[j] }));
    }
    else if (contents[j]->query_npc())
    {
      tell_object(me, _LANG_CMD_GROPE_NPC_ME);
      tell_room(environment(me), _LANG_CMD_GROPE_NPC_ROOM, ({ me }));
    }
    // neverbot, 21/02/04
    else if (contents[j]->query_door())
    {
      tell_object(me, _LANG_CMD_GROPE_DOOR_ME);
      tell_room(environment(me), _LANG_CMD_GROPE_DOOR_ROOM, ({ me }));
    }
    else
    {
      tell_object(me, _LANG_CMD_GROPE_OTHER_ME);
      tell_room(environment(me), _LANG_CMD_GROPE_OTHER_ROOM, ({ me }));
    }

    return 1;
  }
  else
  {
    exits = environment(me)->query_dest_dir();  
    
    if (sizeof(exits))
    {
      string dir;
      
      exit = exits[(random(sizeof(exits)/2))*2];
      dir = ROOM_HAND->query_exit_dir(exit);

      tell_object(me, _LANG_CMD_GROPE_MSG);
      return 1;
    }
  }
  
  notify_fail(_LANG_CMD_GROPE_NOTHING);
  return 0;
}
