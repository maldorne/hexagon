
// cmd that shows a map of the current zone, neverbot@CcMud, 02/2010

#include <mud/cmd.h>
#include <translations/exits.h>
#include <room/location.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_MAP_ALIAS);
  set_usage(_LANG_CMD_MAP_SYNTAX);
  set_help(_LANG_CMD_MAP_HELP);
}

// each position is an array of three elements
// x coord, y coord, room object
#define POSITION_X_COORD 0
#define POSITION_Y_COORD 1
#define POSITION_ROOM_OB 2

// class position
// {
//   int fil;
//   int col;
//   object room;
// }

// valid: 3, 7, 11, 15, etc (4 by 4)
// but!!! if you change this you have to change the drawing of the map, i leave
// it pending to build the strings with the exact width for another day with more
// energy
#define MAX_X 15
#define MAX_Y 15

#define ROOM 1
#define VERTICAL_EXIT 2
#define HORIZONTAL_EXIT 3
#define BACKSLASH_EXIT 4
#define SLASH_EXIT 5
#define QUEST_ROOM 6
#define FINISH_QUEST_ROOM 7
#define DOOR_ROOM 8
#define UP_ROOM 9
#define DOWN_ROOM 10
#define COAST_ROOM 11
#define ADVENTURER_ROOM 12
#define GUARD_ROOM 13
#define ENEMY_ROOM 14

string draw_map(mixed * map);

private int query_room_type(object room, object player)
{
  object * inv;
	object * enemies;
  int i;
  string * exits;

  inv = all_inventory(room);
  enemies = player->query_attacker_list() + player->query_call_outed();
  
  for (i = 0; i < sizeof(inv); i++)
    if ((member_array(inv[i], enemies) != -1) && !inv[i]->query_hidden())
    	return ENEMY_ROOM;
    else if (inv[i]->query_quest_object() && inv[i]->check_player_finished(player))
      return FINISH_QUEST_ROOM;
    else if (inv[i]->query_quest_object() && inv[i]->check_player(player))
      return QUEST_ROOM;
    else if (player->query_adventurer() && (player->query_adventurer_group_name() == inv[i]->query_adventurer_group_name()))
      return ADVENTURER_ROOM;
    else if (inv[i]->query_guard() && (inv[i]->query_citizenship() == player->query_citizenship()))
      return GUARD_ROOM;

  exits = room->query_direc();

  for (i = 0; i < sizeof(exits); i++)
  {
    if (member_array(room->query_ex_type(exits[i]), ({ "door", "gate" })) != -1)
      return DOOR_ROOM;
    if (exits[i] == DIR_UP)
      return UP_ROOM;
    if (exits[i] == DIR_DOWN)
      return DOWN_ROOM;
  }

  if (room->query_coast())
    return COAST_ROOM;
  
  return ROOM;
}

static int cmd(string str, object me, string verb)
{
  mixed * revised;
  mixed * pending;
  mixed current;
  int i, j;
  string * dest_dir;
  mixed * map;

  revised = ({ });
  pending = ({ });
 
  if (!environment(me)) 
  {
    notify_fail(_LANG_CMD_MAP_NO_ENV);
    return 0;
  }

  if (environment(me)->query_dungeon_room())
  {
    notify_fail(_LANG_CMD_MAP_INVALID);
    return 0;    
  }
  
  if (environment(me)->query_water_environment())
  {
    notify_fail(_LANG_CMD_MAP_NO_WATER);
    return 0;  
  }

  // prepare the structure that will hold the map (13x13 matrix)
  map = allocate(MAX_Y);
  for (i = 0; i < MAX_Y; i++)
  {
    // map[i] = ({ });
    map[i] = allocate(MAX_X);
  }
  
  // initialize with the central room (our current position)
  current = allocate(3);
  current[POSITION_Y_COORD] = MAX_Y/2;
  current[POSITION_X_COORD] = MAX_X/2;
  current[POSITION_ROOM_OB] = environment(me);
  pending += ({ current });
  map[current[POSITION_Y_COORD]][current[POSITION_X_COORD]] = query_room_type(environment(me), me);

  while (sizeof(pending))
  {
    current = pending[0];
    dest_dir = current[POSITION_ROOM_OB]->query_dest_dir(); // exits + destinations in current
    
    for (i = 0; i < sizeof(dest_dir); i += 2)
    {
      object new_room;
      mixed * new_position;
      int add;
      int x, y;
      
      new_room = nil;
      add = 0;

      switch (dest_dir[i])
      {
        case DIR_SOUTH:        
          // if we are still inside the map
          if (current[POSITION_Y_COORD] + 1 < MAX_Y)
            map[current[POSITION_Y_COORD]+1][current[POSITION_X_COORD]] = VERTICAL_EXIT;
          if (current[POSITION_Y_COORD] + 2 < MAX_Y)
          {
            add = 1;
            x = current[POSITION_X_COORD];
            y = current[POSITION_Y_COORD]+2;
          }
          break;
        case DIR_NORTH:
          // if we are still inside the map
          if (current[POSITION_Y_COORD] - 1 >= 0)
            map[current[POSITION_Y_COORD]-1][current[POSITION_X_COORD]] = VERTICAL_EXIT;
          if (current[POSITION_Y_COORD] - 2 >= 0)
          {
            add = 1;
            x = current[POSITION_X_COORD];
            y = current[POSITION_Y_COORD]-2;
          }
          break;
        case DIR_EAST:
          // if we are still inside the map
          if (current[POSITION_X_COORD] + 1 < MAX_X)
            map[current[POSITION_Y_COORD]][current[POSITION_X_COORD]+1] = HORIZONTAL_EXIT;
          if (current[POSITION_X_COORD] + 2 < MAX_X)
          {
            add = 1;
            x = current[POSITION_X_COORD]+2;
            y = current[POSITION_Y_COORD];
          }
          break;
        case DIR_WEST:
          // if we are still inside the map
          if (current[POSITION_X_COORD] - 1 >= 0)
            map[current[POSITION_Y_COORD]][current[POSITION_X_COORD]-1] = HORIZONTAL_EXIT;
          if (current[POSITION_X_COORD] - 2 >= 0)
          {
            add = 1;
            x = current[POSITION_X_COORD]-2;
            y = current[POSITION_Y_COORD];
          }
          break;
        case DIR_NORTHWEST:
          // if we are still inside the map
          if ((current[POSITION_X_COORD] - 1 >= 0) && (current[POSITION_Y_COORD] - 1 >= 0))
            map[current[POSITION_Y_COORD]-1][current[POSITION_X_COORD]-1] = BACKSLASH_EXIT;
          if ((current[POSITION_X_COORD] - 2 >= 0) && (current[POSITION_Y_COORD] - 2 >= 0))
          {
            add = 1;
            x = current[POSITION_X_COORD]-2;
            y = current[POSITION_Y_COORD]-2;
          }
          break;
        case DIR_NORTHEAST:
          // if we are still inside the map
          if ((current[POSITION_X_COORD] + 1 < MAX_X) && (current[POSITION_Y_COORD] - 1 >= 0))
            map[current[POSITION_Y_COORD]-1][current[POSITION_X_COORD]+1] = SLASH_EXIT;
          if ((current[POSITION_X_COORD] + 2 < MAX_X) && (current[POSITION_Y_COORD] - 2 >= 0))
          {
            add = 1;
            x = current[POSITION_X_COORD]+2;
            y = current[POSITION_Y_COORD]-2;
          }
          break;
        case DIR_SOUTHWEST:
          // if we are still inside the map
          if ((current[POSITION_X_COORD] - 1 >= 0) && (current[POSITION_Y_COORD] + 1 < MAX_Y))
            map[current[POSITION_Y_COORD]+1][current[POSITION_X_COORD]-1] = SLASH_EXIT;
          if ((current[POSITION_X_COORD] - 2 >= 0) && (current[POSITION_Y_COORD] + 2 < MAX_Y))
          {
            add = 1;
            x = current[POSITION_X_COORD]-2;
            y = current[POSITION_Y_COORD]+2;
          }
          break;
        case DIR_SOUTHEAST:
          // if we are still inside the map
          if ((current[POSITION_X_COORD] + 1 < MAX_X) && (current[POSITION_Y_COORD] + 1 < MAX_Y))
            map[current[POSITION_Y_COORD]+1][current[POSITION_X_COORD]+1] = BACKSLASH_EXIT;
          if ((current[POSITION_X_COORD] + 2 < MAX_X) && (current[POSITION_Y_COORD] + 2 < MAX_Y))
          {
            add = 1;
            x = current[POSITION_X_COORD]+2;
            y = current[POSITION_Y_COORD]+2;
          }
          break;
        default:
          break;
      }
      
      if (add)
      {
        // create a new pending

        // if the destination ends in .c, it is a room, just load it
        if (dest_dir[i+1][strlen(dest_dir[i+1])-2..strlen(dest_dir[i+1])-1] == ".o")
          new_room = load_object(LOCATION_HANDLER)->load_location(dest_dir[i+1]);
        else
          catch ( new_room = load_object(dest_dir[i+1]) );
        
        if (new_room)
        {
          int repeated;
          repeated = 0;
          // check if we have already been here
          for (j = 0; j < sizeof(revised); j++)
            if (revised[j][POSITION_ROOM_OB] == new_room)
            {
              repeated = 1;
              break;
            }

          if (!repeated)
          {
            new_position = allocate(3);
            new_position[POSITION_Y_COORD] = y; 
            new_position[POSITION_X_COORD] = x;
            new_position[POSITION_ROOM_OB] = new_room;
            pending += ({ new_position });
            map[y][x] = query_room_type(new_room, me);
            // map[y][x] = ROOM;
          }
        }
      }
    }
    
    // after checking all the exits, we mark the current room as revised
    revised += ({ current });
    pending -= ({ current });    
  }

  write(draw_map(map));
  
  return 1;
}

string format(string str) { return sprintf("%-*s", 24, str); }

string draw_map(mixed * map)
{
  string result;
  string line;
  int i, j;
  int quests, fquests, doors, ups, downs, coasts, adventurers, guards, enemies;

  // show or not in the legend:
  quests = 0;
  fquests = 0;
  doors = 0;
  ups = 0;
  downs = 0;
  coasts = 0;
  adventurers = 0;
  guards = 0;
  enemies = 0;

  result = "";
  
  result+="\n";
  result+="    ______________________________________________________________\n";
  result+="   /\\                                                             \\\n";
  result+="   \\_|                                                             |\n";
  
  for (i = 0; i < sizeof(map); i++)
  {
    line = "";
    for (j = 0; j < sizeof(map[i]); j++)
    {

      if ((i == MAX_Y/2) && (j == MAX_X/2))
      {
        if (map[i][j] == COAST_ROOM)
        { 
          line += "%^BLUE%^[%^ORANGE%^*%^BLUE%^]%^RESET%^"; 
          coasts = 1; 
        }
        else
          line += "[%^ORANGE%^*%^RESET%^]";
        continue;
      }
      
      switch(map[i][j])
      {
        case ROOM:               
                    line += "[ ]"; break;
        case COAST_ROOM:         
                    line += "%^BLUE%^[ ]%^RESET%^"; coasts = 1; break;
        case DOOR_ROOM:          
                    line += "["+_LANG_CMD_MAP_DOOR_LETTER+"]"; doors = 1; break;
        case UP_ROOM:            
                    line += "[^]"; ups = 1; break;
        case DOWN_ROOM:          
                    line += "[v]"; downs = 1; break;
        case FINISH_QUEST_ROOM:  
                    line += "[%^BOLD%^YELLOW%^?%^RESET%^]"; fquests = 1; break;
        case QUEST_ROOM:         
                    line += "[%^BOLD%^YELLOW%^!%^RESET%^]"; quests = 1; break;
        case ADVENTURER_ROOM:    
                    line += "[%^BOLD%^CYAN%^*%^RESET%^]"; adventurers = 1; break;
        case GUARD_ROOM:         
                    line += "[%^BOLD%^GREEN%^*%^RESET%^]"; guards = 1; break;
        case ENEMY_ROOM:         
                    line += "[%^BOLD%^RED%^*%^RESET%^]"; enemies = 1; break;
        case HORIZONTAL_EXIT:    
                    line += "---"; break;
        case VERTICAL_EXIT:      
                    line += " | "; break;
        case SLASH_EXIT:         
                    line += " / "; break;
        case BACKSLASH_EXIT:     
                    line += " \\ "; break;
        default:                 
                    line += "   "; break;      
      }
    }
    
    result+="     |        " + line + "        |\n";
  }

  result += "     |                                                             |\n";
  result += "     |                            " + format(_LANG_CMD_MAP_LEGEND + " :") + "         |\n";
  result += "     |                               %^ORANGE%^*%^RESET%^   : " + format(_LANG_CMD_MAP_YOUR_POS) + "|\n";

  if (enemies)
  result += "     |                               %^BOLD%^RED%^*%^RESET%^   : " + format(_LANG_CMD_MAP_ENEMIES) + "|\n";
  if (adventurers)
  result += "     |                               %^BOLD%^CYAN%^*%^RESET%^   : " + format(_LANG_CMD_MAP_FRIENDS) + "|\n";
  if (guards)
  result += "     |                               %^BOLD%^GREEN%^*%^RESET%^   : " + format(_LANG_CMD_MAP_GUARDS) + "|\n";
  if (doors)
  result += "     |                               "+_LANG_CMD_MAP_DOOR_LETTER+"   : " + format(_LANG_CMD_MAP_DOORS) + "|\n";
  if (ups)
  result += "     |                               ^   : " + format(_LANG_CMD_MAP_UP_STAIRS) + "|\n";
  if (downs)
  result += "     |                               v   : " + format(_LANG_CMD_MAP_DOWN_STAIRS) + "|\n";
  if (quests)
  result += "     |                               %^BOLD%^YELLOW%^!%^RESET%^   : " + format(_LANG_CMD_MAP_NEW_QUESTS) + "|\n";
  if (fquests)
  result += "     |                               %^BOLD%^YELLOW%^?%^RESET%^   : " + format(_LANG_CMD_MAP_FINISHED_QUESTS) + "|\n";
  if (coasts)
  result += "     |                               %^BLUE%^[ ]%^RESET%^ : " + format(_LANG_CMD_MAP_COAST) + "|\n";
  result += "     |                                                             |\n";
  result += "     |   __________________________________________________________|___\n";
  result += "      \\_/_____________________________________________________________/\n\n";
  
  return result;
  
}