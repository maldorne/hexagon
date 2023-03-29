#include <mud/cmd.h>
#include <common/properties.h>
#include <translations/common.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_EQUIPMENT_ALIAS);
  set_usage(_LANG_CMD_EQUIPMENT_SYNTAX);
  set_help(_LANG_CMD_EQUIPMENT_HELP);
}

static int cmd(string str, object me, string verb) 
{
  string ret, weapons, shields, armours;
  object * obs;
  int i, count_weap, count_arm, count_shi, complete, using;
  
  ret = _LANG_CMD_EQUIPMENT_HEADER;
  weapons = _LANG_CMD_EQUIPMENT_WEAP;
  shields = _LANG_CMD_EQUIPMENT_SHIE;
  armours = _LANG_CMD_EQUIPMENT_ARMO;
  
  obs = all_inventory(me);
  count_weap = 0;
  count_arm = 0;
  count_shi = 0;
  complete = 0;
  using = 0;

  if (me->query_dead())
  {
    notify_fail(_LANG_CMD_DEAD);
    return 0;
  }

  // check if we have to show all the equipment
  if (member_array(str, _LANG_ALL_OPTIONS) != -1)
    complete = 1;

  for (i = 0; i < sizeof(obs); i++)
  {
    if (!obs[i]->cond_string())
      continue;

    if (complete || obs[i]->query_in_use())
    {
      using++;

      if (obs[i]->query_armour())
      {
        armours += sprintf("    %-30.30s : %s", capitalize(obs[i]->query_name()), obs[i]->cond_string());
        count_arm++;
      }
      else if (obs[i]->query_weapon())
      {
        weapons += sprintf("    %-30.30s : %s", capitalize(obs[i]->query_name()), obs[i]->cond_string());
        count_weap++;
      }
      else if (obs[i]->query_shield())
      {
        shields += sprintf("    %-30.30s : %s", capitalize(obs[i]->query_name()), obs[i]->cond_string());
        count_shi++;
      }
    }
  }

  if (!complete && !using)
    ret += _LANG_CMD_EQUIPMENT_NOTHING;
  else
  {
    if (count_weap)
      ret += weapons;
    if (count_shi)
      ret += shields;
    if (count_arm)
      ret += armours;
  }

  ret += "\n";

  if (!complete)
    ret += _LANG_CMD_EQUIPMENT_FOOTER;

  write(ret);
  
  me->set_trivial_action();
  return 1;
}

