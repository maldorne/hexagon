/*
 * consider taken from /std/commands so it takes less mem, Errante.
 * reviewed by neverbot, 20/04/2003, adapting to Cc.
 */

#include <mud/cmd.h>
#include <translations/common.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_CONSIDER_ALIAS);
  set_usage(_LANG_CMD_CONSIDER_SYNTAX);
  set_help(_LANG_CMD_CONSIDER_HELP);
}

static int cmd (string arg, object me, string verb)
{
  object * aux, * obs;
  int i, ok, level, dif;

  arg = me->expand_nickname(arg);

  if (!strlen(arg)) 
  {
    write(_LANG_SYNTAX + ": " +_LANG_CMD_CONSIDER_SYNTAX + "\n");
    return 1;
  }

  arg = lower_case(arg);
  // flag to ignore hidden
  aux = find_match(arg, environment(me), 1);
  obs = ({ });

  for (i = 0; i < sizeof(aux); i++) 
  {
    if ((aux[i]->query_player() && (aux[i] != me)) || aux[i]->query_npc())
      obs += ({ aux[i] });
  }

  if (!sizeof(obs)) 
  {
    write(_LANG_CMD_CONSIDER_EXIST);
    return 1;
  }

  level = (int)me->query_level();

  for (i = 0; i < sizeof(obs); i++) 
  {
    if ((int)obs[i]->query_coder() )
    {
      write(_LANG_CMD_CONSIDER_CODER);
      ok = 1;
      continue;
    }

    if (!living(obs[i]))
      continue;

    ok = 1;
    dif = (int)obs[i]->query_level() - level;
    // dif += random(3) - random(3);

    if (dif > 15)
    {
      write(_LANG_CMD_CONSIDER_15);
      continue;
    }

    if (dif > 10) 
    {
      write(_LANG_CMD_CONSIDER_10);
      continue;
    }

    if (dif < -10) 
    {
      write(_LANG_CMD_CONSIDER_M10);
      continue;
    }

    if (level > 13) 
    {
      // dif is now  -10...10
      dif += 10;   // dif is now between 0...20
      dif /= 2;  // dif is now between 0...10
  
      // Fix by Aragorn, capitalized the short
      write(obs[i]->query_cap_name() + _LANG_CMD_CONSIDER_14[dif]);
    } 
    else 
    {   // if level is less than 14
      dif += 10;  // 0...20
      dif /=  5;  // 0...4
  
      write(capitalize((string)obs[i]->short()) + _LANG_CMD_CONSIDER_NOT_14[dif]);
    }  // end else
  } // end for

  if (!ok) 
  {
    write(_LANG_CMD_CONSIDER_NOT_THAT);
    return 1;
  }

  return 1;
}
