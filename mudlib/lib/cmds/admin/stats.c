
#include <mud/cmd.h>

inherit CMD_BASE;

string query_usage()
{
  return "stats <nombre or id> [filter-1 filter-2 ... filter-n]";
}

string query_help()
{
  return "Shows all stats of given object\n" + 
         "(or only those which name contains some of the filter key words).\n";
}

static int cmd(string str, object me, string verb) 
{
  object ob;
  mixed * my_stats;
  string ret;
  int i;
  string * params;
  string * words;

  ret = "";

  if (!this_player())
    return 0;

  if (this_player(1) != this_user())
    return 0;

  if (!strlen(str))
  {
    write(query_help());
    return 1;
  }

  params = explode(str, " ");
  str = params[0];
  params -= ({ str });

  ob = find_object(str);

  if (!ob)
    ob = find_living(str);

  if (!ob && sizeof(find_match(str, me)))
    ob = find_match(str, me)[0];

  if (!ob && sizeof(find_match(str, environment(me))))
    ob = find_match(str, environment(me))[0];

  if (!ob)
  {
    tell_object(me, "Cannot find object or player '" + str + "'.\n");
    return 1;
  }

  my_stats = ob->stats();

  for (i = 0; i < sizeof(my_stats); i++)
  {
    int j, found;
    found = 0;

    words = explode(my_stats[i][0], " ");

    if (sizeof(params))
    {
      // filter stats by key words
      for (j = 0; j < sizeof(words); j++)
        if (member_array(lower_case(words[j]), params) != -1)
          found = 1;
    }

    if (!sizeof(params) || found)
    {
      ret += "[" + i + "] " + my_stats[i][0];
  
      if (intp(my_stats[i][1]) ||
          stringp(my_stats[i][1]) || 
         (arrayp(my_stats[i][1]) && sizeof(my_stats[i][1]) == 0) || 
         (mappingp(my_stats[i][1]) && m_sizeof(my_stats[i][1]) == 0) ||
         (undefinedp(my_stats[i][1]))
         )
        ret += ": ";
      else
        ret += ":\n";

      ret += to_string(my_stats[i][1]); // + "\n";
    }
  }

  tell_object(me, ret);
  tell_object(me, "\nUse exec for more information, i.e.:\n  exec return find_living(\"" + 
    me->query_name() + "\")->stats()[ <index> ]\n");
  return 1;
}

