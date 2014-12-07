
// number of seconds between calls to the heart_beat function
// do not touch this

#define HEART_BEAT_PERIOD 2

private static int hb_handle;

// Passing 'flag' as 0 disables the object's heart beat.  Passing a 'flag' of
// 1 will cause heart_beat() to be called in the object once each heart beat
// (a variable number defined by your local administrator, usually 2 seconds).
// Passing a 'flag' of greater than 1 will usually set the number of heart beats
// in between calls to heart_beat(), however your local administrator may have
// the system configured to treat any 'flag' above 1 as 1.

#include <status.h>

static nomask void set_heart_beat(int flag) 
{
  if (flag == 0)
  {
    remove_call_out(hb_handle);
    hb_handle = 0;
    return;
  }

  hb_handle = call_out("perform_heart_beat", HEART_BEAT_PERIOD);
}

nomask void perform_heart_beat() 
{
  string error;

  if (previous_object() && previous_object() != this_object()) 
    return;

  error = catch(this_object()->heart_beat());
  
  if (error) 
  {
    set_heart_beat(0);
    return;
  }

  hb_handle = call_out("perform_heart_beat", HEART_BEAT_PERIOD);
}

static nomask int query_heart_beat(varargs object ob)
{
  if (ob && (ob != this_object()))
    return ob->query_heart_beat();

  if (hb_handle != 0)
    return 1;

  return 0;
}
