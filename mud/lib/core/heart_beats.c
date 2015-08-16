
#include <mud/mudos.h>

// number of seconds between calls to the heart_beat function
// do not touch this
#define HEART_BEAT_TIME 2

private static object * _hb_object_list;
private static int _hb_handle;

// prototypes
int set_initiator_object(object ob);
int set_initiator_player(object player);

void create()
{
  _hb_object_list = ({ });
  _hb_handle = -1;

  stderr("@ __heart_beat create\n");    

  _hb_handle = call_out("__heart_beat", HEART_BEAT_TIME);
}

nomask void __heart_beat()
{
  int i;

  if (!mudlib_privileges()) 
  {
    stderr("Illegal __heart_beat\n");
    return;
  }

  stderr("@ __heart_beat cicle\n");

  for (i = 0; i < sizeof(_hb_object_list); i++)
  {
    object ob;
    string result;

    ob = _hb_object_list[i];

    if (!ob)
    {
      _hb_object_list -= ({ ob });
      i--;
      continue;
    }

    set_initiator_player(nil);

    if (living(ob))
      set_initiator_object(ob);
    else
      set_initiator_object(nil);

    rlimits(MAX_HB_DEPTH ; MAX_HB_TICKS) 
    {
      result = catch(call_other(ob, "__call_other", "heart_beat"));
    }

    set_initiator_object(nil);

    // turn off heart beat in the object
    if (result)
    {
      _hb_object_list -= ({ _hb_object_list[i] });
      i--;
      call_other(ob, "set_heart_beat", 0);
    }
  }

  // queue the next __heart_beat execution
  _hb_handle = call_out("__heart_beat", HEART_BEAT_TIME);
}

nomask int hb_object_index(object ob)
{
  int i;

  for (i = 0; i < sizeof(_hb_object_list); i++)
    if (_hb_object_list[i] == ob)
      return i;

  return -1;
}

nomask int add_hb_object(object ob)
{
  if (hb_object_index(ob) == -1)
  {
    _hb_object_list += ({ ob });
    return 1;
  }

  return 0;
}

nomask int remove_hb_object(object ob)
{
  // int i;

  // i = hb_object_index(ob);

  // if (i == -1)
  //   return 0;

  _hb_object_list -= ({ ob });

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