
#include <basic/move.h>
#include <common/properties.h>
#include <translations/souls.h>
#include <language.h>

/* The effects of being drunk.
 * It is not recomended for a good party :)
 */
void do_wander(int bing);

static int drunkness, missing;

void create()
{
  drunkness = 0;
  missing = 0;
}

int heart_beat() 
{
  int lvl, i, con;
  int intox;
  object *obs;

  intox = (int)this_object()->query_intoxication();

  if (intox <= 0)
    return 1;

  if (missing) 
  {
    missing--;
    return 0;
  }

  if (this_object()->query_property(PASSED_OUT_PROP))
    return 0;

  if (--drunkness > 0 && !missing)
    return 1;

  con = (int)this_object()->query_con();

  if (random((50 - con) * 10) > intox)
    return 1;

  drunkness = 20 + random(30);
  lvl = random(intox) / 1500;

  if (lvl > 3)
    lvl = 3;

  switch (lvl) 
  {
    case 0 :
      switch (random(5)) 
      {
        case 0 :
          this_object()->do_command(SOUL_CMD_HICCUP);
          break;
        case 1 :
          this_object()->do_command(SOUL_CMD_BURP);
          break;
        case 2 :
          this_object()->do_command(SOUL_CMD_PUKE);
          break;
        case 3 :
          this_object()->do_command(SOUL_CMD_TRIP);
          break;
        case 4 :
          this_object()->do_command(SOUL_CMD_STAGGER);
          break;
      }
      break;
    case 1 :
      switch (random(1)) 
      {
        case 0 :
          write(_LANG_DRUNK_TRIP_YOU);
          say(_LANG_DRUNK_TRIP_THEY);
          missing = 10 + random(10);
          return 0;
          break;
      }
      break;
    case 2 :
      switch (random(2)) 
      {
        case 0 :
          obs = all_inventory();
          while (sizeof(obs) &&
                 obs[(i=random(sizeof(obs)))]->move(environment())&&
                 !obs[i]->short() && !obs[i]->query_worn())
            obs = delete(obs, i, 1);

          if (!sizeof(obs))
            break;

          write(_LANG_DRUNK_DROP_YOU);
          say(_LANG_DRUNK_DROP_THEY);
          break;

        case 1 :
          write(_LANG_DRUNK_FALL_YOU);
          say(_LANG_DRUNK_FALL_THEY);
          this_player()->adjust_hp(-random(3), this_player());
          break;
      }
      break;
    case 3 :
      switch (random(2)) 
      {
        case 0 :
          this_player()->add_property(PASSED_OUT_PROP,1);
          call_out("remove_property", 20 + random(30), PASSED_OUT_PROP);
          write(_LANG_DRUNK_PASSED_OUT_YOU);
          say(_LANG_DRUNK_PASSED_OUT_THEY);
          return 0;
          break;
        case 1 : /* wandering fits */
          do_wander(5 + random(10));
          break;
      }
      break;
  }
  return 1;
}

void do_wander(int num) 
{
  string * direcs;
  int i;
  // int bong;

  if (num > 0)
    call_out("do_wander", 2 + random(5), --num);

  direcs = (mixed *)environment()->query_dest_dir();

  // while (!bong && sizeof(direcs)) 
  // {
    i = random(sizeof(direcs) / 2) * 2;
    // with do_command the command is enqueued, so we can't
    // check if it was executed or not, so there is no need
    // to add this property, it will be removed before the 
    // actual movement
    // this_object()->add_property(UNKNOWN_MOVE_PROP, 1);

    // bong = 
      this_object()->do_command(direcs[i]);
    
    // this_object()->remove_property(UNKNOWN_MOVE_PROP);

    // if (!bong)
    //   direcs = delete(direcs, i, 2);
  // }
}

mixed * stats() 
{
  return ({
    ({ "Drunkness (nosave)", drunkness }),
    ({ "Missing (nosave)", missing }),
          });
}