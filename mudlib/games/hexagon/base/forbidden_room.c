
// base tmp room, will be inherited by a lot of special rooms in /games/hexagon
// where players and coders should not be
// log every player/coder enter

#include <room/room.h>

inherit "/lib/room";

string log_filename;

void create()
{
  ::create();
  add_property(NO_CLEAN_UP_PROP, 1);
}

void setup()
{
  set_short("Temporal room, you should not be here");
  set_long("This is a temporal room, you should not be here. Your presence " +
    "has been logged.\n");
}

void set_log_filename(string name) { log_filename = name; }
string query_log_filename() { return log_filename; }

void event_enter(object who, varargs string msg, object from, mixed avoid)
{
  if (living(who))
  {
    log_file(log_filename, who->query_cap_name() + " has entered " + file_name(this_object()) +
           " from: "+ file_name(from) + " [" + ctime(time(),4) + "]\n");
  }

  ::event_enter(who, msg, from, avoid);
}
