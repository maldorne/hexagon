
// item to arrange initial character stats

#include <language.h>
#include <common/properties.h>
#include <translations/common.h>
#include "../path.h"

inherit "/lib/item.c";

#define LOG_FILE "adjust"
#define ORB_TAG_FOR_FIND "orb_tag_for_find"

// prototypes
int intro(object ob);

string * thestats, spew;
int * statorder;
int * scores;
int * fixed_scores;

// so it's never saved in the player's inventory
int query_auto_load(){ return 1; }
int query_no_save_object(){ return 1; }

void create()
{
  ::create();
  thestats = _LANG_ATTRIBUTES;
  statorder = ({ -1, -1, -1, -1, -1, -1, -1, -1 });
  scores = ({ 0, 0, 0, 0, 0, 0, 0, 0 });
  fixed_scores = ({ 18, 16, 15, 13, 13, 11, 10, 10, });
}

void setup() 
{
  set_name(_LANG_ORB_NAME);
  set_short(capitalize(_LANG_ORB_NAME));
  set_main_plural(capitalize(pluralize(_LANG_ORB_NAME)));
  add_plural(pluralize(_LANG_ORB_NAME));

  add_alias(ORB_TAG_FOR_FIND);

  // do not reset_get, or the orb could not be moved to player inventory
  // reset_get();
  set_weight(1);

  set_long(_LANG_ORB_DESC);
}

void init()
{
  ::init();
  add_action("do_adjust", _LANG_ADJUST_VERBS);
}

int do_adjust(string str)
{
  object orb;

  if (this_player()->query_property(ORIGINAL_ADJUST_PROP))
  {
    log_file(LOG_FILE, this_player()->query_cap_name() + " has already adjusted their character, " +
      ctime(time(),4) + ".\n");
    tell_object(this_player(), _LANG_ORB_ALREADY_ADJUSTED);

    this_player()->move_living("X", NEWBIE_ROOM);

    // if for any reason the player has already adjusted but still has 
    // the object, we destroy it (maybe the mud crashed?)
    if (environment(this_object()) == this_player())
      call_out("dest_me", 0);

    return 1;
  }

  // if we are trying to adjust when we already have an orb, use the action
  // from the orb in your inventory
  if ((orb = present(ORB_TAG_FOR_FIND, this_player())) && 
      (orb != this_object()))
  {
    return 0;
  }

  // to avoid problems with several players adjusting, we clone a new orb 
  // for each player
  if (environment(this_object()) != this_player())
  {
    orb = clone_object(base_name(this_object()));
    if (!orb)
      return 0;

    orb->move(this_player());
  }

  orb->intro(this_player());
  return 1;
}

int intro(object ob) 
{
  string racial_bonuses, ret;

  racial_bonuses = sprintf(_LANG_ORB_RACIAL_BONUSES,
    this_player()->query_bonus_str(),
    this_player()->query_bonus_dex(),
    this_player()->query_bonus_con(),
    this_player()->query_bonus_int(),
    this_player()->query_bonus_wis(),
    this_player()->query_bonus_cha(),
    this_player()->query_bonus_per(),
    this_player()->query_bonus_wil() );

  ret = _LANG_ORB_ADJUST_TXT;

  write("\n" + handler("frames")->frame(wrap(ret)));
  write(_LANG_ORB_ADJUST_ORDER);

  input_to("choose_order", 0);
}

void choose_order(string order)
{
  string * stuff;
  int i, * slots;
  
  slots = ({ 0, 0, 0, 0, 0, 0, 0, 0 });

  if (!strlen(order))
  {
    write(_LANG_ORB_ADJUST_INVALID + _LANG_ORB_ADJUST_TRY_AGAIN + _LANG_ORB_ADJUST_ORDER);
    input_to("choose_order", 0);
    return; 
  }

  // we input a cancel option
  if (member_array(order, _LANG_CANCEL_OPTIONS) != -1)
  {
    write(_LANG_ORB_ADJUST_CANCELLED);
    return;
  }

  stuff = explode(order, " ");

  if (sizeof(stuff) < 8)
  {
    write(_LANG_ORB_ADJUST_EIGHT + _LANG_ORB_ADJUST_TRY_AGAIN + _LANG_ORB_ADJUST_ORDER);
    input_to("choose_order", 0);
    return;
  }

  for (i = 0; i < 8; i++)
  {
    if ((statorder[i] = member_array(lower_case(stuff[i]), thestats)) == -1)
    {
      write(_LANG_ORB_ADJUST_INVALID + _LANG_ORB_ADJUST_TRY_AGAIN + _LANG_ORB_ADJUST_ORDER);
      input_to("choose_order", 0);
      return;
    }

    slots[statorder[i]] = 1;
  }

  for (i = 0; i < 8; i++)
    if (!slots[i])
    {
      write(_LANG_ORB_ADJUST_EIGHT + _LANG_ORB_ADJUST_TRY_AGAIN + _LANG_ORB_ADJUST_ORDER);
      input_to("choose_order", 0);
      return;
    }

  // We now have their desired order stored in statorder
  spew = implode(({ thestats[statorder[0]], thestats[statorder[1]],
    thestats[statorder[2]], thestats[statorder[3]],
    thestats[statorder[4]], thestats[statorder[5]],
    thestats[statorder[6]], thestats[statorder[7]] }), " ");

  write(_LANG_ORB_ADJUST_CONFIRM);
  input_to("confirm_order", 0);
}

void confirm_order(string answer)
{
  // the answer if affirmative
  if (member_array(answer, _LANG_AFFIRMATIVE_OPTIONS) != -1)
  {
    mapping stats_prop;
    int i;

    // set the attributes
    for (i = 0; i < 8; i++)
      scores[statorder[i]] = fixed_scores[i];

    stats_prop = ([
      thestats[statorder[0]] : scores[0],
      thestats[statorder[1]] : scores[1],
      thestats[statorder[2]] : scores[2],
      thestats[statorder[3]] : scores[3],
      thestats[statorder[4]] : scores[4],
      thestats[statorder[5]] : scores[5],
      thestats[statorder[6]] : scores[6],
      thestats[statorder[7]] : scores[7],
    ]);

    write(_LANG_ORB_ADJUST_OK + "\n\t" + 
      capitalize(thestats[statorder[0]]) + ": "+ fixed_scores[0]+"\t\t" + 
      capitalize(thestats[statorder[1]]) + ": "+ fixed_scores[1]+"\t\t" + 
      capitalize(thestats[statorder[2]]) + ": "+ fixed_scores[2]+"\t\t" + 
      capitalize(thestats[statorder[3]]) + ": "+ fixed_scores[3]+"\n\t" + 
      capitalize(thestats[statorder[4]]) + ": "+ fixed_scores[4]+"\t\t" + 
      capitalize(thestats[statorder[5]]) + ": "+ fixed_scores[5]+"\t\t" + 
      capitalize(thestats[statorder[6]]) + ": "+ fixed_scores[6]+"\t\t" + 
      capitalize(thestats[statorder[7]]) + ": "+ fixed_scores[7]+"\n\n");
    
    this_player()->add_property(ORIGINAL_ADJUST_PROP, stats_prop);
    
    this_player()->set_str(scores[0]);
    this_player()->set_dex(scores[1]);
    this_player()->set_con(scores[2]);
    this_player()->set_int(scores[3]);
    this_player()->set_wis(scores[4]);
    this_player()->set_cha(scores[5]);
    this_player()->set_per(scores[6]);
    this_player()->set_wil(scores[7]);

    write(handler("frames")->frame(wrap(_LANG_ORB_ADJUST_CONGRATS), 
                                   "", 0, 0, "notifications") + "\n\n");

    this_player()->move_living("X", NEWBIE_ROOM);
    this_player()->save_me();
    this_user()->save_me();

    // destroy the orb
    call_out("dest_me", 0);

    return;
  }
  // the answer if negative
  else if (member_array(answer, _LANG_NEGATIVE_OPTIONS) != -1)
  {
    write(_LANG_ORB_ADJUST_TRY_AGAIN + _LANG_ORB_ADJUST_ORDER);
    input_to("choose_order", 0);
    return;
  }
  // we cancel the adjust
  else if (member_array(answer, _LANG_CANCEL_OPTIONS) != -1)
  {
    write(_LANG_ORB_ADJUST_CANCELLED);
    return;
  }

  // any other text
  write(_LANG_ORB_ADJUST_CONFIRM);
  input_to("confirm_order", 0);
}
