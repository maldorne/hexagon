/* ********************************************************************
 *
 *   Command to show and change the player configuration
 *   neverbot 04
 *
 * ********************************************************************
 */

#include <mud/cmd.h>
#include <user/configuration.h>
#include <user/terminal.h>
#include <translations/common.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_CONFIG_ALIAS);
  set_usage(_LANG_CMD_CONFIG_SYNTAX);
  set_help(_LANG_CMD_CONFIG_HELP);
}

private string show_topic_value(string category, string topic)
{
  mapping config, current_map;
  string func_name;
  mixed result;

  config = CONFIG_TABLE->query_config_data();
  current_map = config[category];
  func_name = current_map[topic][CONFIG_POS_QUERY_FUNC];
  
  if (function_exists(func_name, this_user()))
  {
    if (current_map[topic][CONFIG_POS_PARAM] != "")
      result = call_other(this_user(), func_name, current_map[topic][CONFIG_POS_PARAM]);
    else
      result = call_other(this_user(), func_name);
    
    if (current_map[topic][CONFIG_POS_TYPE] == "bool")
      result = (result ? _LANG_YES : _LANG_NO);

    return _LANG_CMD_CONFIG_CURRENT_CONFIG_FOR + category + " / " + topic + 
            ": [ " + result + " ]\n";
  }
  else
    return _LANG_CMD_CONFIG_CURRENT_CONFIG_FOR + category + " / " + topic + 
          ": " + _LANG_CMD_CONFIG_UNKNOWN + "\n";
}

private string set_topic_value(string category, string topic, mixed value)
{
  mapping config, current_map;
  string func_name;
  mixed result;

  config = CONFIG_TABLE->query_config_data();
  current_map = config[category];
  write(to_string(topic));
  write(to_string(category));
  write(to_string(current_map));
  write(to_string(current_map[topic]));
  func_name = current_map[topic][CONFIG_POS_SET_FUNC];
  
  if (function_exists(func_name, this_user()))
  {
    switch(current_map[topic][CONFIG_POS_PARAM])
    {
      case "string":
        if (current_map[topic][CONFIG_POS_PARAM] != "")
          result = call_other(this_user(), func_name, 
                              current_map[topic][CONFIG_POS_PARAM], (string)value);
        else
          result = call_other(this_user(), func_name, (string)value);
        break;
      case "int":
        if (current_map[topic][CONFIG_POS_PARAM] != "")
          result = call_other(this_user(), func_name, 
                              current_map[topic][CONFIG_POS_PARAM], to_int(value));
        else
          result = call_other(this_user(), func_name, to_int(value));
        break;
      case "bool":
        if (current_map[topic][CONFIG_POS_PARAM] != "")
          result = call_other(this_user(), func_name, 
                              current_map[topic][CONFIG_POS_PARAM], affirmative(value));
        else
          result = call_other(this_user(), func_name, affirmative(value));
        break;
    }
  }

  // result will probably be just 1 or 0, we need
  // to call again the query func to return the real value
  return show_topic_value(category, topic);
}

private string show_category(string title, string category)
{
  string ret, func_name;
  mapping config, current_map;
  string * topics;
  int i;
  mixed result;

  config = CONFIG_TABLE->query_config_data();
  current_map = config[category];
  topics = keys(current_map);

  ret = "\n" + _LANG_CMD_CONFIG_OPTIONS_IN_CATEGORY + "\n";

  for (i = 0; i < sizeof(topics); i++)
  {
    func_name = current_map[topics[i]][CONFIG_POS_QUERY_FUNC];
    
    if (function_exists(func_name, this_user()))
    {
      if (current_map[topics[i]][CONFIG_POS_PARAM] != "")
        result = call_other(this_user(), func_name, current_map[topics[i]][CONFIG_POS_PARAM]);
      else
        result = call_other(this_user(), func_name);
      
      if (current_map[topics[i]][CONFIG_POS_TYPE] == "bool")
        result = (result ? _LANG_YES : _LANG_NO);
      
      ret += sprintf("    %18s : %15s", (string)topics[i], (string)result);
    }
    else
      ret += sprintf("    %18s : %15s", (string)topics[i], _LANG_CMD_CONFIG_UNKNOWN);

    ret += "    [ %^GREEN%^" + current_map[topics[i]][CONFIG_POS_COMMAND] + "%^BOLD%^ ]\n";
  }

  return ret;
}

int show_all_config()
{
  mapping config;
  string * categories;
  int i;
  string ret;

  config = CONFIG_TABLE->query_config_data();
  categories = keys(config);
  ret = "";

  for (i = 0; i < sizeof(categories); i++)
  {
    // the config indices in the mapping are just indices, translate them 
    // before showing them to the user
    ret += show_category(CONFIG_TABLE->query_config_translations()[categories[i]][0], 
        categories[i]);
  }

  ret += _LANG_CMD_CONFIG_USE_COMMANDS;

  ret = handler("frames")->frame(ret, _LANG_CMD_CONFIG_FOR_USER, 
                                 this_user()->query_cols());
  write(ret);
  return 1;
}  

static int cmd(string str, object me, string verb)
{
  string category, topic, value;

  // if we do not have the three arguments category, topic, value,
  // show all the configuration
  if (sscanf(str, "%s %s %s", category, topic, value) == 3)
  {
    // set new value
    category = CONFIG_TABLE->query_category_from_name(category);
    
    // whatever we wrote is not an existing category
    if (strlen(category) == 0)
    {
      write(_LANG_CMD_CONFIG_NOT_VALID_CATEGORY);
      return 1;
    }

    // whatever we wrote is not an existing topic
    if (!CONFIG_TABLE->query_topic_in_category(topic, category))
    {
      write(_LANG_CMD_CONFIG_NOT_VALID_TOPIC);
      return 1;
    }

    write(set_topic_value(category, topic, value));
    return 1;
  }
  else if (sscanf(str, "%s %s", category, topic) == 2)
  {
    // show current value
    category = CONFIG_TABLE->query_category_from_name(category);

    // whatever we wrote is not an existing category
    if (strlen(category) == 0)
    {
      write(_LANG_CMD_CONFIG_NOT_VALID_CATEGORY);
      return 1;
    }

    // whatever we wrote is not an existing topic
    if (!CONFIG_TABLE->query_topic_in_category(topic, category))
    {
      write(_LANG_CMD_CONFIG_NOT_VALID_TOPIC);
      return 1;
    }

    write(show_topic_value(category, topic));
    return 1;
  }
  else if (strlen(str) > 0)
  {
    // we enter just a category name
    // show all topics and values for the category
    category = CONFIG_TABLE->query_category_from_name(str);

    // whatever we wrote is not an existing category
    if (strlen(category) == 0)
    {
      write(_LANG_CMD_CONFIG_NOT_VALID_CATEGORY);
      return 1;
    }

    write(handler("frames")->frame(show_category(
              CONFIG_TABLE->query_config_translations()[category][0], 
              category), 
        _LANG_CMD_CONFIG_FOR_USER, this_user()->query_cols()));;
    return 1;
  }
  else
  {
    // show all configuration
    return show_all_config();
  }
}
