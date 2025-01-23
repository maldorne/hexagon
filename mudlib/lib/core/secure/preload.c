
/*
 * This is all the autoloading stuff.
 * please don't remove again - lynscar.
 */

void add_preload(string file) 
{
  if (previous_object() == this_object() ||
      is_administrator(geteuid(initial_object()))) 
  {
    if (!file_exists(file))
      return;

    if (!sizeof(preload)) 
    {
      preload = ({ file });
    } 
    else if (member_array(file, preload) == -1) 
    {
      preload += ({ file });
    }

    save_object(SECURE_SAVE_PATH);
  }
}

string * query_preload() { return preload; }
string * query_call_out_preload() { return call_out_preload; }

void remove_preload(string file)
{
  int i;

  if (previous_object() == this_object() ||
      is_administrator(geteuid(initial_object()))) 
  {
    if (sizeof(preload)) 
    {
      if (member_array(file, preload) != -1)
      {
        preload -= ({ file });
        save_object(SECURE_SAVE_PATH);
      }
    }
  }
}

void add_call_out_preload(string file) 
{
  if (previous_object() == this_object() ||
      is_administrator(geteuid(initial_object()))) 
  {
    if (!file_exists(file))
      return;

    if (!sizeof(call_out_preload)) 
    {
      call_out_preload = ({ file });
    } 
    else if (member_array(file, call_out_preload) == -1) 
    {
      call_out_preload += ({ file });
    }

    save_object(SECURE_SAVE_PATH);
  }
}

void remove_call_out_preload(string file) 
{
  int i;

  if (previous_object() == this_object() ||
      is_administrator(geteuid(initial_object()))) 
  {
    if (sizeof(call_out_preload)) 
    {
      if (member_array(file, call_out_preload) != -1)
      {
        call_out_preload -= ({ file });
        save_object(SECURE_SAVE_PATH);
      }
    }
  }
}

nomask void _preload(string file, varargs int delayed) 
{
  string e, d;

  d = (delayed ? " (delayed)" : "");

  if (!file_exists(file))
  {
    stderr(" ~ preloading" + d +": " + file + " (file does not exist).\n");
    return;
  }

  stderr(" ~ preloading" + d +": " + file + ".\n");

  if ((e = catch(load_object(file)->dummy()))) 
    write("      "+e+"\n");
}

nomask void load_secure_object() 
{
  if (!done) 
  {
    int i;
  
    done = TRUE;
    seteuid(ROOT);
    restore_object(SECURE_SAVE_PATH);

    for (i = 0; i < sizeof(preload); i++)
      call_out("_preload", 0, preload[i]);

    for (i = 0; i < sizeof(call_out_preload); i++)
      call_out("_preload", 2, call_out_preload[i], TRUE);
  }
}