
inherit "/lib/core/object.c";

string last_wrote;
int daily_time, running_time;
int daily_coder_time, running_coder_time;

void save_timekeeper();
int check_write();

void setup()
{
  set_name("timekeeper");
  restore_object("/save/timekeeper.o");
}

int add_time(int time_on, varargs int is_coder)
{
  if (time_on > 0 && time_on < 86400)
  {
    if (is_coder)
    {
      daily_coder_time += time_on;
      running_coder_time += time_on;
    }
    else
    {
      daily_time += time_on;
      running_time += time_on;
    }
  }

  check_write();
  save_timekeeper();
  return daily_time;
}

int check_write()
{
  string * cur_time;
  cur_time = explode(ctime(time())," ");

  if (cur_time[0] != last_wrote)
  {
    log_file("time_on", "["+ctime(time())+
      "] daily time = "+daily_time+
      "; daily coder time = "+daily_coder_time+"\n");

    daily_time = 0;
    daily_coder_time = 0;
    last_wrote = cur_time[0];
    return 1;
  }
  return 0;
}

int query_daily_time()
{
  check_write();
  return daily_time;
}

int query_running_time()
{
  check_write();
  return running_time;
}

void dest_me()
{
  save_timekeeper();
  ::dest_me();
}

void set_last_wrote(string str)
{
  last_wrote = str;
}

void save_timekeeper()
{
  save_object("/save/timekeeper.o", 1);
}
