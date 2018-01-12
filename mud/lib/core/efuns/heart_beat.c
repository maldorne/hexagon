
// Passing 'flag' as 0 disables the object's heart beat.  Passing a 'flag' of
// 1 will cause heart_beat() to be called in the object once each heart beat
// (a variable number defined by your local administrator, usually 2 seconds).
// Passing a 'flag' of greater than 1 will usually set the number of heart beats
// in between calls to heart_beat(), however your local administrator may have
// the system configured to treat any 'flag' above 1 as 1.

private static int _hb_status;

nomask void set_heart_beat(int flag) 
{
  debug("hbs", " ~ set_heart_beat " + flag + " in " + object_name(this_object()) + "\n");

  if (flag == 0)
  {
    _hb_status = 0;
    MUDOS->remove_hb_object(this_object());
    return;
  }

  _hb_status = 1;
  MUDOS->add_hb_object(this_object());
}

nomask int query_heart_beat()
{
  return _hb_status;
}

void heart_beat()
{
  debug("hbs", "  └─> heart_beat in " + object_name(this_object()) + "\n");
  // debug("hbs", " ~ sizeof call_out stack: " + (MUDOS->query_call_out_stack_size()) + " time: "+time()+"\n");
  // debug("hbs", " ~ call_out stack: " + to_string(MUDOS->query_call_out_stack()) + "\n");
}
