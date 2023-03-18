/*
 * start room for the demo-fantasy game
 * here the new players have to choose a class
 */

inherit "/lib/room";

#include <living/races.h>
#include <common/properties.h>
#include <language.h>

#define RACES_PATH "/games/demo-fantasy/obj/races/"
#define NEXT_ROOM "/games/demo-fantasy/areas/start/begin3.c"

int i, num_races;
object ob;
mapping races;
// string * descs;
string * files;
string tmp, r;
object fake_item;

mapping test() { return races; }

void obtain_data();

void create()
{
  races = ([ ]);
  r = "<race>";
  obtain_data();

  ::create();
}

void setup()
{
  set_light((LIGHT_STD_HIGH - LIGHT_STD_LOW)/2);
  set_short(game_master_object(this_object())->query_game_name() + ": " + _LANG_START2_SHORT);
  set_long(_LANG_START2_LONG + " " + implode(_LANG_START2_HINTS, " ") + "\n");  

  add_property(NOFEAT_PROP, 1);
  add_property(NOKILL_PROP, 1);
}

int show_hints(object who)
{
  tell_object(who, "%^BOLD%^" + implode(_LANG_START2_HINTS, "\n") + "%^RESET%^\n");
  return 1;
}

void event_enter(object who, varargs string msg, object from, mixed avoid)
{
  if (living(who))
    call_out("show_hints", 1, who);

  ::event_enter(who, msg, from, avoid);
}

void obtain_data()
{
  i = 0;

  // get every race file in the game
  files = get_dir(RACES_PATH);

  for (i = 0; i < sizeof(files); i++)
    if (ob = clone_object(RACES_PATH + files[i]))
      if (ob->query_is_race_ob() && (ob->query_playable()))
        races[ob->query_name()] = ob->query_long();

  num_races = m_sizeof(races);  
}

void init()
{
  if (!fake_item)
  {
    fake_item = clone_object("/lib/item.c");
    fake_item->set_short(capitalize(pluralize(_LANG_START2_CHESTS_NAME)));
    fake_item->add_alias(_LANG_START2_CHESTS);
    fake_item->set_long(_LANG_START2_CHESTS_DESC);
    fake_item->reset_get();
    fake_item->move(this_object());
  }

  ::init();

  add_action("do_look", _LANG_START2_LOOK_VERBS);
  add_action("do_open", _LANG_START2_OPEN_VERBS);
}

void dest_me()
{
  if (fake_item)
    fake_item->dest_me();

  ::dest_me();
}

int do_look(string str)
{
  string a, r, ret;

  // if we look something that is not a chest, we return 0
  // so we use the normal look command
  if ((sscanf(str, "%s %s", a, r) != 2) || (a != _LANG_START2_CHESTS_NAME) ||
      (member_array(lower_case(r), keys(races)) == -1))
    return 0;

  ret = _LANG_START2_CHEST_RACE + "\n";
  ret += sprintf("\n   %-=*s", this_user()->query_cols()-3, "   " + 
                 (string)races[lower_case(r)] + " " + _LANG_START2_HINTS[1] + "\n\n");

  write(ret);
  return 1;
}

int do_open(string str)
{
  string a, r, ret;

  // if we try to open something that is not a chest
  if ((sscanf(str, "%s %s", a, r) != 2) || (a != _LANG_START2_CHESTS_NAME) ||
      (member_array(lower_case(r), keys(races)) == -1))
  {
    notify_fail(_LANG_START2_CHEST_NOT_FOUND);
    return 0;
  }

  if (!this_player()->query_race_ob())
  {
    this_player()->set_race_ob(RACES_PATH + lower_case(r) + ".c");
    log_file("races", this_player()->query_cap_name() + ": "+
        r + " " + ctime(time(),4) + "\n");
  }

  write(_LANG_START2_OPEN_MSG);

  this_player()->move_living("X", NEXT_ROOM);
  return 1;
}
