// neverbot 10/2004
// Rewritten for Hexagon, neverbot 09/2026: settings described by mappings,
// with their bounds and options, some of them built for the player asking
// (the informs their role can receive, the languages they know, ...).

#include <user/configuration.h>
#include <translations/inform.h>
#include <translations/combat.h>
#include <translations/pov.h>
#include <living/consents.h>
#include <language.h>

#define TERM_HANDLER "/lib/handlers/terminal"

// Categories in the order they are shown.
string * query_categories()
{
  return ({ "terminal", "info", "communication", "character", "consents" });
}

// The words a player types for each category; the first one is shown.
mapping query_config_translations()
{
  return ([
    "terminal"      : _LANG_CONFIG_TERMINAL,
    "info"          : _LANG_CONFIG_INFO,
    "communication" : _LANG_CONFIG_COMMUNICATION,
    "character"     : _LANG_CONFIG_CHARACTER,
    "consents"      : _LANG_CONFIG_CONSENTS,
  ]);
}

private mapping entry(string type, string owner, string query, string set,
                      varargs mixed param, string command)
{
  mapping ret;

  ret = ([ CONFIG_TYPE : type, CONFIG_OWNER : owner, CONFIG_QUERY : query,
           CONFIG_SET : set ]);

  if (!undefinedp(param) && param != nil)
    ret[CONFIG_PARAM] = param;
  if (strlen(command))
    ret[CONFIG_COMMAND] = command;

  return ret;
}

private string * informs_for(object user)
{
  if (user->query_admin())
    return INFORM_EVENTS_ADMINS;
  if (user->query_coder())
    return INFORM_EVENTS_CODERS;
  return INFORM_EVENTS_PLAYERS;
}

// category -> ([ topic : entry ]) for this player.
mapping query_config_data(object player)
{
  object user;
  mapping data, topics, e;
  string * list, * names;
  int i;

  user = player->user();
  data = ([ ]);

  // terminal
  topics = ([ ]);
  e = entry(CONFIG_CHOICE, CONFIG_ON_USER, "query_term_name", "set_terminal", nil,
            _LANG_CONFIG_CMD_TERM);
  e[CONFIG_OPTIONS] = TERM_HANDLER->query_term_types();
  topics[_LANG_CONFIG_TOPIC_TERM] = e;
  e = entry(CONFIG_INT, CONFIG_ON_USER, "query_cols", "set_cols", nil, _LANG_CONFIG_CMD_COLS);
  e[CONFIG_MIN] = 35;
  e[CONFIG_MAX] = 200;
  topics[_LANG_CONFIG_TOPIC_COLS] = e;
  e = entry(CONFIG_INT, CONFIG_ON_USER, "query_rows", "set_rows", nil, _LANG_CONFIG_CMD_ROWS);
  e[CONFIG_MIN] = 11;
  e[CONFIG_MAX] = 200;
  topics[_LANG_CONFIG_TOPIC_ROWS] = e;
  data["terminal"] = topics;

  // info: how much the game tells, and which informs this role receives
  topics = ([ ]);
  topics[_LANG_CONFIG_TOPIC_VERBOSE] = entry(CONFIG_BOOL, CONFIG_ON_USER, "query_verbose",
                                             "set_verbose", nil, _LANG_CONFIG_CMD_VERBOSE);
  list = informs_for(user);
  for (i = 0; i < sizeof(list); i++)
    topics[list[i]] = entry(CONFIG_BOOL, CONFIG_ON_USER, "query_inform", "set_inform",
                            list[i], _LANG_CONFIG_CMD_INFORM);
  data["info"] = topics;

  // communication
  topics = ([ ]);
  topics[_LANG_CONFIG_TOPIC_EARMUFFS] = entry(CONFIG_BOOL, CONFIG_ON_USER, "query_earmuffs",
                                              "set_earmuffs", nil, _LANG_CONFIG_CMD_EARMUFFS);

  e = entry(CONFIG_CHOICE, CONFIG_ON_USER, "query_editor_name", "set_editor_name", nil,
            _LANG_CONFIG_CMD_EDITOR);
  e[CONFIG_OPTIONS] = user->query_editor_names();
  topics[_LANG_CONFIG_TOPIC_EDITOR] = e;

  e = entry(CONFIG_CHOICE, CONFIG_ON_USER, "query_pov_name", "set_pov_name", nil,
            _LANG_CONFIG_CMD_POV);
  e[CONFIG_OPTIONS] = POV_TYPES;
  topics[_LANG_CONFIG_TOPIC_POV] = e;

  topics[_LANG_CONFIG_TOPIC_PROMPT] = entry(CONFIG_STRING, CONFIG_ON_USER,
    "query_prompt_string", "set_prompt", nil, _LANG_CONFIG_CMD_PROMPT);
  data["communication"] = topics;

  // character
  topics = ([ ]);
  e = entry(CONFIG_CHOICE, CONFIG_ON_PLAYER, "query_pretty_combat_mode",
            "set_pretty_combat_mode", nil, _LANG_CONFIG_CMD_COMBAT);
  e[CONFIG_OPTIONS] = COMBAT_MODE_STRINGS;
  topics[_LANG_CONFIG_TOPIC_COMBAT] = e;

  e = entry(CONFIG_CHOICE, CONFIG_ON_PLAYER, "query_combat_role_string",
            "set_combat_role_name", nil, _LANG_CONFIG_CMD_ATTITUDE);
  e[CONFIG_OPTIONS] = COMBAT_ROLE_STRINGS;
  e[CONFIG_REFUSES] = 1;
  topics[_LANG_CONFIG_TOPIC_ATTITUDE] = e;

  e = entry(CONFIG_INT, CONFIG_ON_PLAYER, "query_wimpy", "set_wimpy", nil,
            _LANG_CONFIG_CMD_WIMPY);
  e[CONFIG_MIN] = 0;
  e[CONFIG_MAX] = 99;
  topics[_LANG_CONFIG_TOPIC_WIMPY] = e;

  names = ({ });
  list = player->query_languages();
  for (i = 0; i < sizeof(list); i++)
    names += ({ handler("languages")->query_language_display(list[i]) });
  e = entry(CONFIG_CHOICE, CONFIG_ON_PLAYER, "query_current_language_display",
            "set_language", nil, _LANG_CONFIG_CMD_SPEAK);
  e[CONFIG_OPTIONS] = names;
  topics[_LANG_CONFIG_TOPIC_LANGUAGE] = e;
  data["character"] = topics;

  // consents, by the name each one is shown under
  topics = ([ ]);
  list = CONSENT_TYPES;
  for (i = 0; i < sizeof(list); i++)
    topics[player->query_consent_name(list[i])] = entry(CONFIG_BOOL, CONFIG_ON_PLAYER,
      "query_consent", "set_consent", list[i], _LANG_CONFIG_CMD_CONSENT);
  data["consents"] = topics;

  return data;
}

string query_category_from_name(string name)
{
  mapping data;
  string * categories;
  int i;

  data = query_config_translations();
  categories = query_categories();

  for (i = 0; i < sizeof(categories); i++)
    if (member_array(name, data[categories[i]]) != -1)
      return categories[i];

  return "";
}

string query_category_name(string category)
{
  return query_config_translations()[category][0];
}
