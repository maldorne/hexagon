/*
 * New doors for  CcMud, neverbot 6/03
 * Modification of an original idea from Iolo@Rl
 *
 */

inherit "/lib/core/object.c";

#include <language.h>
#include <mud/translations.h>
#include <room/room.h>

string dest,           // door direction
       dir_other_side, // direction we come from the other side (north -> south)
       s_dest;         // door direction reduced, if able (north -> n)

int status,            // 0 - closed, 1 - open
    init_status,       // initial status (0 - closed, 1 - open)
    breakable,         // can be broken
    lockable,          // can be locked
    lock_status,       // 0 - locked, 1 - unlocked (can be open or closed!!)
    init_lock_status,  // initial lock status (0 - closed, 1 - open)    
    max_hps, hps;      // door health (same as the other side)

int number,       // 0 == single, 1 == many)
    reset_msg;    // ignore the message with gender and number (if the current language
                  // has such things) and use a generic one like 'you open the door to xxx'

string * keys;    // names of the keys than can lock/unlock the door

object query_other_side_door(int flag);

// added to regenerate the health of the doors, ie after an update 
// we need this to avoid the two doors having inconsistent info
// neverbot, 7/03
void dest_me()
{
  object other_door;

  if (environment(this_object()))
  {
    other_door = query_other_side_door(0);
    if (other_door)
      other_door->repop();
  }
  ::dest_me();
}

void create()
{
  dest = "";
  s_dest = "";
  dir_other_side = "";
  reset_msg = 0;

  status = 1;
  init_status = 1;

  lockable = 0;
  lock_status = 1;
  init_lock_status = 1;
  keys = ({ });

  breakable = 1;
  max_hps = 1000;
  hps = 1000;

  number = 0; // single
  ::create();
}

void setup()
{
  set_name("");
  reset_get();
  reset_drop();
  set_long(_LANG_DOOR_LONG);
}

// query_hidden_object() returns 1, so we avoid find_match, neverbot 2/06
int query_hidden_object() { return 1; }

void init()
{
  ::init();
  add_action("do_open", _LANG_DOOR_OPEN_ACTIONS);  
  add_action("do_close", _LANG_DOOR_CLOSE_ACTIONS);
  add_action("do_lock", _LANG_DOOR_LOCK_ACTIONS);  
  add_action("do_unlock", _LANG_DOOR_UNLOCK_ACTIONS);
}

int query_status() { return status; }
// 0 - closed, 1 - open
void set_status(int i) 
{ 
  if (i == 0)
    status = 0; 
  else
  {
    status = 1;
    lock_status = 1;
  }

  // force an update the next time the exit_string is queried
  // (we have change the door from closed to open or open to closed)
  environment(this_object())->reset_short_exit_string();
  environment(this_object())->query_dirs_string();
}

int query_init_status() { return init_status; }
void set_init_status(int i)
{ 
  if (i == 0) 
    init_status = 0; 
  else 
    init_status = 1;
  set_status(init_status);
}

int query_breakable() { return breakable; }
void set_breakable(int i)
{
  if (i == 0) 
    breakable = 0;
  else 
    breakable = 1;
}

int query_lockable() { return lockable; }
void set_lockable(int i)
{
  if (i == 0){
    lockable = 0;
    lock_status = 0;
  }
  else {
    lockable = 1;
    lock_status = 1;
  }
}

int query_lock_status() { return lock_status; }
void set_lock_status(int i)
{
  if (i == 0)
  {
    status = 0;
    lock_status = 0;
  }
  else
    lock_status = 1;
}

int query_init_lock_status() { return init_lock_status; }
void set_init_lock_status(int i)
{ 
  if (i == 0) 
    init_lock_status = 0; 
  else 
    init_lock_status = 1;
  set_lock_status(init_lock_status);
}
      
void set_number(int i)
{ 
  if (i == 0)
    number = i;
  else
    number = 1; 
}
int query_number() { return number; }
void reset_message() { reset_msg = 1; }
int query_reset_message() { return reset_msg; }

int query_door() { return 1; }
int is_open() { return status; }

// with the new custom messages, some directions are not so common,
// we manage them here, neverbot 10/03
void set_dest(string str)
{ 
  // with these directions, we force the message to reset always
  // so we won't see 'open the door to down', etc
  if ((str == DIR_IN) || (str == DIR_OUT) ||
      (str == DIR_UP) || (str == DIR_DOWN))
  {
    reset_msg = 1;
  }

  // doors can be open/close with the reduced direction (north -> n, etc)
  s_dest = EXIT_HAND->query_reduced_exit_name(str);
  dest = str; 
}

string query_short_dest() { return s_dest; }
string query_dest() { return dest; }
void set_dir_other_side(string str) { dir_other_side = str; }
string query_dir_other_side() { return dir_other_side; }
string * query_keys() { return keys; }
void set_keys(string * list)
{ 
   keys = list; 
   set_lockable(1);
}
void add_key(string str)
{ 
  keys += ({ str }); 
  set_lockable(1);
}

int query_known_exit(string name)
{
  switch(name){
    case DIR_DOWN:
    case DIR_UP:
    case DIR_IN:
    case DIR_OUT:
    case DIR_NORTH:
    case DIR_SOUTH:
    case DIR_EAST:
    case DIR_WEST:
    case DIR_SOUTHWEST:
    case DIR_SOUTHEAST:
    case DIR_NORTHEAST:
    case DIR_NORTHWEST:
      return 1;
    default:
      return 0;
  }
}

void open_msg(string door, object ob, int flag);
void close_msg(string door, object ob, int flag);

int do_unlock(varargs string str)
{
  int find, i, aux;
  object * obs;
  string key;

  if (this_player()->query_dead())
  {
    notify_fail(_LANG_DOOR_NOT_DEAD);
    return 0;
  }

  // if we are not opening THIS door
  if (!strlen(str) || ((str != dest) && (str != s_dest)))
    return 0;

  if (lock_status == 1) 
  {
    notify_fail(_LANG_DOOR_NOT_LOCKED);
    return 0;    
  }

  find = -1;
  obs = deep_inventory(this_player());

  // auto search for the right key
  for (i = 0; i < sizeof(obs); i++)
  {
    if (obs[i]->query_key())
    {
      key = file_name(obs[i]);
      // this should not happen...
      aux = strsrch(key, "#");
      if (aux != -1)
        key = extract(key, 0, aux - 1);
      if (member_array(key, keys) != -1)
      {
        find = i;
        break;
      }
    }
  }

  if (find == -1)
  {
    if (reset_msg)
    {
      notify_fail(_LANG_DOOR_IS_LOCKED);
    }
    else if (query_known_exit(dest))
    {
      notify_fail(_LANG_DOOR_IS_LOCKED_KNOWN);
    }
    else
    {
      notify_fail(_LANG_DOOR_IS_LOCKED_CUSTOM);
    }

    return 0;
  } 

  tell_object(this_player(), _LANG_DOOR_USE_TO_UNLOCK);
  return 1;
}

int do_open(varargs string str) 
{
  object other_door;

  if (this_player()->query_dead())
  {
    notify_fail(_LANG_DOOR_NOT_DEAD);
    return 0;
  }

  // if we are not opening THIS door
  if (!strlen(str) || ((str != dest) && (str != s_dest)))
    return 0;

  // already open
  if (query_status() == 1)
  {
    if (reset_msg)
    {
      notify_fail(_LANG_DOOR_OPEN_ALREADY);
    }
    else if (query_known_exit(dest))
    {
      notify_fail(_LANG_DOOR_OPEN_ALREADY_KNOWN);
    }
    else 
    {
      notify_fail(_LANG_DOOR_OPEN_ALREADY_CUSTOM);
    }

    return 0;
  }
  else
  {
    // can be locked and IS locked
    if ((lockable) && (lock_status == 0))
    {
      // try to auto unlock
      if (!do_unlock(dest))
        return 0;
    }

    set_status(1);
    
    open_msg(dest, this_player(), 0); 

    other_door = query_other_side_door(1);    
    if (other_door && !other_door->is_open())
    {
      other_door->set_status(1);
      open_msg(dir_other_side, other_door, 1);
    }
    return 1; 
  }
}

int do_lock(varargs string str)
{
  int find, i, aux;
  object * obs;
  string key;
  object other_door;

  // cannot be locked
  if (!lockable)
  {
    notify_fail(_LANG_DOOR_CANNOT_BE_LOCKED);
    return 0;
  }

  // if can be locked and IS locked 
  if (lock_status == 0)
  {
    if (reset_msg)
    {
      notify_fail(_LANG_DOOR_LOCK_ALREADY);
    }
    else if (query_known_exit(dest))
    {
      notify_fail(_LANG_DOOR_LOCK_ALREADY_KNOWN);
    }
    else
    {
      notify_fail(_LANG_DOOR_LOCK_ALREADY_CUSTOM);
    }

    return 0;
  }

  // was not locked
  if (lock_status == 1)
  {
    find = -1;
    obs = all_inventory(this_player());
  
    // auto search for the right key
    for (i = 0; i < sizeof(obs); i++)
    {
      if (obs[i]->query_key())
      {
        key = file_name(obs[i]);
        // this should not happen...
        aux = strsrch(key, "#");
        if (aux != -1)
           key = extract(key, 0, aux - 1);
        if (member_array(key, keys) != -1)
        {
           find = i;
           break;
        }
      }
    }

    if (find == -1)
    {
      notify_fail(_LANG_DOOR_NEED_A_KEY);
      return 0;
    }
    
    set_lock_status(0);

    tell_object(this_player(), _LANG_DOOR_USE_TO_LOCK);

    // message to the room, to all players including this_player()
    if (reset_msg)
    {
      tell_room(environment(this_object()), _LANG_DOOR_OTHERS_LOCK);
    }
    else if (query_known_exit(dest))
    {
      tell_room(environment(this_object()), _LANG_DOOR_OTHERS_LOCK_KNOWN);
    }
    else 
    {
      tell_room(environment(this_object()), _LANG_DOOR_OTHERS_LOCK_CUSTOM);
    }

    // other side messages
    other_door = query_other_side_door(1);

    if (other_door) 
    {
      other_door->set_lock_status(0);
      
      if (reset_msg)
      {
        tell_room(environment(other_door), _LANG_DOOR_OTHER_SIDE_LOCK);
      }
      else if (query_known_exit(dir_other_side))
      {
        tell_room(environment(other_door), _LANG_DOOR_OTHER_SIDE_LOCK_KNOWN);
      }
      else 
      {
        tell_room(environment(other_door), _LANG_DOOR_OTHER_SIDE_LOCK_CUSTOM);
      }
    }

    return 1;
  }

  return 0;
}

int do_close(varargs string str) 
{ 
  object other_door;
  string * list;
  int i, find, aux;
  string key;
  object * obs;
  string ex;
  
  if (this_player()->query_dead())
  {
    notify_fail(_LANG_DOOR_NOT_DEAD);
    return 0;
  }

  if (!strlen(str))
    return 0;

  // if we are not closing THIS door
  if ((str != dest) && (str != s_dest))
    return 0;

  if (this_object()->is_broken())
  {
    if (reset_msg)
    {
      notify_fail(_LANG_DOOR_BROKEN);
    }
    else if (query_known_exit(dest))
    {
      notify_fail(_LANG_DOOR_BROKEN_KNOWN);
    }
    else
    {
      notify_fail(_LANG_DOOR_BROKEN_CUSTOM);
    }

    return 0;
  }

  // initially closed
  if (!query_status())
  {
    if (reset_msg)
    {
      tell_object(this_player(), _LANG_DOOR_CLOSED_ALREADY);
    }
    else if (query_known_exit(dest))
    {
      tell_object(this_player(), _LANG_DOOR_CLOSED_ALREADY_KNOWN);
    }
    else 
    {
      tell_object(this_player(), _LANG_DOOR_CLOSED_ALREADY_CUSTOM);
    }
  }
  // initially open
  else 
  {
    set_status(0);
    close_msg(dest, this_player(), 0);

    other_door = query_other_side_door(1);

    if (other_door && other_door->is_open()) 
    {
      other_door->set_status(0);
      close_msg(dir_other_side, other_door, 1);
    }
  }

  list = explode(str, " ");
  str = list[0];

  if (_LANG_DOOR_VERB_TO_LOCK_USED)
  {
    if (!do_lock(dest))
      return 0;
  }

  return 1;
}

/*
 * shows the open message
 *  if flag == 0:
 *   - You open the door + 
 *   - ob opens the door
 *  if flag == 1:
 *   - Somebody opens the door from the other side
 */
void open_msg(string door, object ob, int flag)
{
  if (flag == 0)
  {
    if (reset_msg)
    {
      tell_object(ob, _LANG_DOOR_YOU_OPEN);
      tell_room(environment(ob), _LANG_DOOR_PLAYER_OPENS, ({ ob }));
      return;
    }
    
    if (query_known_exit(door))
    {
      tell_object(ob, _LANG_DOOR_YOU_OPEN_KNOWN);
      tell_room(environment(ob), _LANG_DOOR_PLAYER_OPENS_KNOWN, ({ ob }));
      return;
    }

    tell_object(ob, _LANG_DOOR_YOU_OPEN_CUSTOM);
    tell_room(environment(ob), _LANG_DOOR_PLAYER_OPENS_CUSTOM, ({ ob }) );
  }
  else
  {
    if (reset_msg)
    {
      tell_room(environment(ob), _LANG_DOOR_SOMEBODY_OPENS);
      return;
    }

    if (query_known_exit(door))
    {
      tell_room(environment(ob), _LANG_DOOR_SOMEBODY_OPENS_KNOWN);
      return;
    }
    
    tell_room(environment(ob), _LANG_DOOR_SOMEBODY_OPENS_CUSTOM);
  }
}

void close_msg(string door, object ob, int flag)
{
  if (flag == 0)
  {
    if (reset_msg)
    {
      tell_object(ob, _LANG_DOOR_YOU_CLOSE);
      tell_room(environment(ob), _LANG_DOOR_PLAYER_CLOSES, ({ ob }));
      return;
    }

    if (query_known_exit(door))
    {
      tell_object(ob, _LANG_DOOR_YOU_CLOSE_KNOWN);
      tell_room(environment(ob), _LANG_DOOR_PLAYER_CLOSES_KNOWN, ({ ob }));
      return;
    }

    tell_object(ob, _LANG_DOOR_YOU_CLOSE_CUSTOM);
    tell_room(environment(ob), _LANG_DOOR_PLAYER_CLOSES_CUSTOM, ({ ob }) );
  }
  else
  {
    if (reset_msg){
      tell_room(environment(ob), _LANG_DOOR_SOMEBODY_CLOSES);
      return;
    }

    if (query_known_exit(door))
    {
      tell_room(environment(ob), _LANG_DOOR_SOMEBODY_CLOSES_KNOWN);
      return;
    }

    tell_room(environment(ob), _LANG_DOOR_SOMEBODY_CLOSES_CUSTOM);
  }
}

int query_max_hps() { return max_hps; }
void set_max_hps(int i)
{
  object other_door;

  max_hps = i;
  other_door = query_other_side_door(1);
  
  if (other_door && 
     (other_door->query_max_hps() != this_object()->query_max_hps()) )  
    other_door->set_max_hps(i);

  this_object()->set_hps(i);
}

int query_hps() { return hps; }
void adjust_hps(int i)
{ 
  this_object()->set_hps(this_object()->query_hps() + i);
}

void set_hps(int i)
{ 
  object other_door;
 
  if (this_object()->query_hps() < 0) 
    return;
  if (i <= max_hps) 
    hps = i; 
  else 
    return;
  
  other_door = query_other_side_door(1);
  if (other_door && 
     (other_door->query_hps() != this_object()->query_hps()) )
    other_door->set_hps(i);   
  
  if (this_object()->query_hps() < 0) 
    this_object()->do_break();
}

void do_damage(int i)
{ 
  if ((i > 0) && 
      (!this_object()->is_open()) && 
      this_object()->query_breakable()) 
    this_object()->adjust_hps(-i);
}

void do_break()
{
  if (reset_msg)
  {
     tell_room(environment(this_object()), _LANG_DOOR_BREAKS);
  }
  else if (query_known_exit(dest))
  {
    tell_room(environment(this_object()), _LANG_DOOR_BREAKS_KNOWN);
  }
  else
  {
    tell_room(environment(this_object()), _LANG_DOOR_BREAKS_CUSTOM);
  }

  // open it
  set_status(1);
}

int is_broken()
{ 
  if (this_object()->query_hps() < 0) 
    return 1; 
  else 
    return 0; 
}

string health_string()
{
  int intervals, our_interval;

  intervals = sizeof(_LANG_DOOR_HEALTH_STATUSES);
  our_interval = (query_hps() * intervals) / query_max_hps();

  return _LANG_DOOR_HEALTH_STATUSES[our_interval >= intervals ? intervals - 1 : our_interval];
}

// if flag==1 we load the other room to be sure to have the door object
// if flag==0 we do not load (we are destroying things)
object query_other_side_door(int flag)
{  
  mixed *exits;
  object other_room, other_door;
  int i;
  
  if (!environment(this_object()))
    return nil;
  
  exits = environment(this_object())->query_dest_dir();
  i = 0;
  while (i < sizeof(exits)) {
    if (exits[i] == dest) {
      other_room = find_object(exits[i+1]);
      if (!other_room && flag) {
        load_object(exits[i+1]);
        other_room = find_object(exits[i+1]);
      }
      i = sizeof(exits);
    }
    i += 2;
  }
   
  if (other_room) {
    other_door = other_room->query_door_ob(dir_other_side);
  }
  return (other_door);
}
     
void repop()
{  
  if (!query_status() && (init_status == 1))
  {
    if (reset_msg)
    {
      tell_room(environment(this_object()), _LANG_DOOR_OPENS);
    }
    else if (query_known_exit(dest))
    {
      tell_room(environment(this_object()), _LANG_DOOR_OPENS_KNOWN);
    }
    else 
    {
      tell_room(environment(this_object()), _LANG_DOOR_OPENS_CUSTOM);
    }
  }

  if (query_status() && (init_status == 0))
  {
    if (reset_msg)
    {
      tell_room(environment(this_object()), _LANG_DOOR_CLOSES);
    }
    else if (query_known_exit(dest))
    {
      tell_room(environment(this_object()), _LANG_DOOR_CLOSES_KNOWN);
    }
    else 
    {
      tell_room(environment(this_object()), _LANG_DOOR_CLOSES_CUSTOM);
    }
  }
    
  set_status(init_status);
  set_hps(query_max_hps());

  if (!is_open() && !query_init_lock_status()) 
  {
    if (reset_msg)
    {
      tell_room(environment(this_object()), _LANG_DOOR_OTHERS_LOCK);
    }
    else if (query_known_exit(dest))
    {
      tell_room(environment(this_object()), _LANG_DOOR_OTHERS_LOCK_KNOWN);
    }
    else
    {
      tell_room(environment(this_object()), _LANG_DOOR_OTHERS_LOCK_CUSTOM);
    }
    
    set_lock_status(init_lock_status);
  }
}  
