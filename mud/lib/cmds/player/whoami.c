
/*
 * whoami command, separated from who again
 * and translated for hexagon mudlib
 *   neverbot, 09/2019
 */

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  position = 0;
}

string query_usage()
{
  return "whoami";
}

string query_short_help()
{
  return _LANG_WHOAMI_HELP;
}

static int cmd (string str, object me, string verb)
{
  mapping guilds, jobs;
  string ret;
  string *g_names, *j_names;
  int i;

  guilds = me->query_guild_joined();
  jobs = me->query_job_joined();
  g_names = keys(guilds);
  j_names = keys(jobs);

  ret = me->query_cap_name() + " " +
    me->query_article() + " " +
    me->query_race_name() + ":\n";

  if (me->query_class_ob())
  {
    ret += _LANG_WHOAMI_CLASS_FIRST;
    ret += _LANG_WHOAMI_CLASS_EACH;
  }

  for (i = 0; i < sizeof(g_names); i++)
  {
    if (i == 0)
      ret += _LANG_WHOAMI_GUILD_FIRST;

    if (!load_object(g_names[i]))
      continue;

    ret += _LANG_WHOAMI_GUILD_EACH;
  }

  for (i = 0; i < sizeof(j_names); i++)
  {
    if (i == 0)
      ret += _LANG_WHOAMI_JOB_FIRST;

    if (!load_object(j_names[i]))
      continue;

    ret += _LANG_WHOAMI_JOB_EACH;
  }

  // ret += ".\n";
  write(ret);

  // me->set_trivial_action();
  return 1;
}