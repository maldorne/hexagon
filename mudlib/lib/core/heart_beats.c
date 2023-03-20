
#include <mud/mudos.h>
#include <mud/config.h>

// number of seconds between calls to the heart_beat function
// do not touch this
#define HEART_BEAT_TIME 2

// every item in the _hb_object_list is an array of three elements
// ({ 
//   object, // the object that has a heart beat
//   int,    // how many heart_beats to wait before calling its heart_beat
//   int     // how many heart_beats have passed since the last call
// })
#define POS_OBJECT 0
#define POS_INTERVAL 1
#define POS_COUNTER 2


private static mixed * _hb_object_list;
private static int _hb_handle;

// prototypes
int set_initiator_object(object ob);
int set_initiator_player(object player);
int set_initiator_user(object user);
void set_current_verb(string v);
void set_current_command(string v);
void set_notify_fail_msg(string msg);

void create()
{
  _hb_object_list = ({ });
  _hb_handle = -1;

  debug("hbs", " ───> heart_beat creation\n");

  _hb_handle = call_out("_heart_beat", HEART_BEAT_TIME);
}

nomask void _heart_beat()
{
  int i;

  if (!mudlib_privileges())
  {
    debug("hbs", "Illegal _heart_beat\n");
    return;
  }

  // first thing in business, 
  // queue the next _heart_beat execution, just in case there is 
  // some error during this execution
  _hb_handle = call_out("_heart_beat", HEART_BEAT_TIME);

  if (CONFIG_LOG_HEART_BEATS)
  {
    debug("hbs", " ───> new heart_beat cycle\n");
    debug("hbs", to_string(_hb_object_list));
  }

  for (i = 0; i < sizeof(_hb_object_list); i++)
  {
    object ob;
    string result;

    ob = _hb_object_list[i][POS_OBJECT];

    // remove the items that have been destructed
    if (undefinedp(ob))
    {
      // _hb_object_list -= ({ ob });
      _hb_object_list = delete(_hb_object_list, i, 1);
      i--;
      continue;
    }

    // if the current item has not reached the interval yet, skip it
    if (++_hb_object_list[i][POS_COUNTER] % _hb_object_list[i][POS_INTERVAL] != 0)
      continue;

    // reset the counter
    _hb_object_list[i][POS_COUNTER] = 0;

    if (CONFIG_LOG_HEART_BEATS)
      stderr(" ~~~ mudos::_heart_beat() for <"+object_name(ob)+">\n");

    // purge the current execution context before every heart_beat
    if (living(ob))
      set_initiator_object(ob);
    else
      set_initiator_object(nil);

    if (interactive(ob))
      set_initiator_user(ob->user());
    else
      set_initiator_user(nil);

    set_current_verb("");
    set_current_command("");
    set_notify_fail_msg("");
    // end purging

    rlimits(MAX_HB_DEPTH ; MAX_HB_TICKS)
    {
      result = catch(call_other(ob, "heart_beat"));
    }
  
    set_initiator_object(nil);
    set_initiator_user(nil);

    if (CONFIG_LOG_HEART_BEATS)
      stderr(" ~~~ end mudos::_heart_beat() for "+(ob ? ("<"+object_name(ob)+">") : "nil")+"\n");

    // turn off heart beat in the object
    if (ob && result)
    {
      debug("hbs", "  └─> heart_beat error in " + object_name(ob) + ":\n      " +
                                            result + "\n");
      // _hb_object_list -= ({ ob });
      _hb_object_list = delete(_hb_object_list, i, 1);
      i--;
      // will try to remove the object from _hb_object_list, better safe than sorry
      call_other(ob, "set_heart_beat", 0);
    }
  }
}

nomask int hb_object_index(object ob)
{
  int i;

  for (i = 0; i < sizeof(_hb_object_list); i++)
    if (_hb_object_list[i][POS_OBJECT] == ob)
      return i;

  return -1;
}

nomask int add_hb_object(object ob, int time)
{
  if (hb_object_index(ob) == -1)
  {
    _hb_object_list += ({ ({ ob, time, 0 }) });
    return 1;
  }

  return 0;
}

nomask int remove_hb_object(object ob)
{
  int i;

  i = hb_object_index(ob);

  if (i == -1)
    return 0;

  // _hb_object_list -= ({ ob });
  _hb_object_list = delete(_hb_object_list, i, 1);

  return 1;
}

// nomask void perform_heart_beat()
// {
//   string error;

//   if (previous_object() && previous_object() != this_object())
//     return;

//   error = catch(this_object()->heart_beat());

//   if (error)
//   {
//     set_heart_beat(0);
//     return;
//   }

//   _hb_handle = call_out("perform_heart_beat", HEART_BEAT_PERIOD);
// }
