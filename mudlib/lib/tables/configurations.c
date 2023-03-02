// neverbot 10/2004
 
#include <user/configuration.h>
#include <language.h>

// Every entry in the mapping is a new mapping with all the pairs
// ( subcategory : ({ type, query_function, set_function, parameter, command }) )
// where:
//  type           : (string, int, bool) is the type to return and pass to each function
//  query_function : is the function to call to get the value
//  set_function   : is the function to call to set the value
//  parameter      : is an optional parameter to pass to the query/set functions
//  command        : command that can be used outside the configuration menu


// string * categories = ({ "terminal", "info", "environment" });

mapping query_config_data() 
{ 
  return ([   
      "terminal": ([ 
          _LANG_CONFIG_TOPIC_TERM:    ({ "string", "query_term_name", "set_term_type", "", "term", }),
          _LANG_CONFIG_TOPIC_COLS:    ({ "int", "query_cols", "set_cols", "", "columnas", }),
          _LANG_CONFIG_TOPIC_ROWS:    ({ "int", "query_rows", "set_rows", "", "filas", }),
        ]),
      "info": ([
          _LANG_CONFIG_TOPIC_MONITOR: ({ "bool", "query_monitor", "set_monitor", "", "monitor", }),
          _LANG_CONFIG_TOPIC_VERBOSE: ({ "bool", "query_verbose", "set_verbose", "", "modo", }),
          _LANG_CONFIG_TOPIC_LOGON:   ({ "bool", "query_inform", "set_inform", "logon", "informar", }),
        ]),
      "environment": ([
          _LANG_CONFIG_TOPIC_HUD:     ({ "string", "query_hud", "set_hud", "", "apuntador", }),
          _LANG_CONFIG_TOPIC_STUN:    ({ "bool", "query_combat_mode", "set_combat_mode", "", "combate", }),
        ]),                        
    ]); 
}

mapping query_config_translations() 
{ 
  return ([
    "terminal" :    _LANG_CONFIG_TERMINAL,
    "info" :        _LANG_CONFIG_INFO,
    "environment" : _LANG_CONFIG_ENVIRONMENT,
  ]);
}

string query_category_from_name(string name)
{
  mapping data;
  string * categories;
  int i;

  data = query_config_translations();
  categories = keys(data);

  for (i = 0; i < sizeof(categories); i++)
    if (member_array(name, data[categories[i]]) != -1)
      return categories[i];

  return "";
}

int query_topic_in_category(string topic, string category)
{
  mapping data;
  string * categories;

  data = query_config_data();
  
  if (undefinedp(data[category]))
    return 0;

  return (member_array(topic, keys(data[category])) != -1);
}