#include <living/consents.h>
#include <mud/secure.h>
#include <language.h>

mapping consents;

private string consent_id(string word);

void create() 
{
  // consents = allocate_mapping(sizeof(CONSENT_TYPES));
  consents = ([ ]);
}

void consent_commands() 
{ 
  add_private_action("change_consent", _LANG_CONSENT_VERBS); 
}

mixed query_valid_consents() { return CONSENT_TYPES; }
mixed query_consent(string tipo) { return consents[tipo]; }
mapping query_consents() { return consents; }

// What a consent is called for the player who reads it.
string query_consent_name(string id)
{
  mapping names;

  names = _LANG_CONSENT_NAMES;
  return names[id] ? names[id] : id;
}

// The consent a typed word names, by its id or by the name it is shown under.
private string consent_id(string word)
{
  string * types;
  int i;

  types = CONSENT_TYPES;

  for (i = 0; i < sizeof(types); i++)
    if (types[i] == word || query_consent_name(types[i]) == word)
      return types[i];

  return nil;
}

void set_consents(mapping map)
{
  // allow the login process (and migrations) to change this
  if (!SECURE->valid_progname("/lib/core/login"))
    return;  

  consents = map;
}

int set_consent(string cons, int value)
{
  if (member_array(cons, CONSENT_TYPES) == -1)
    return 0;

  consents[cons] = value;
  
  return value;
}

int change_consent(string str)
{
  string id, name, word, value;
  string * names;
  int allowed, i;

  if (!strlen(str))
  {
    string ret;

    ret = _LANG_CONSENT_HEADER;

    for (i = sizeof(CONSENT_TYPES); i--;)
    {
      name = query_consent_name(CONSENT_TYPES[i]);
      ret += sprintf("%17s: %s\n", name,
        (consents[CONSENT_TYPES[i]] ? _LANG_CONSENT_ON : _LANG_CONSENT_OFF));
    }

    ret += _LANG_CONSENT_FOOTER;

    tell_object(this_player(), ret);
    return 1;
  }

  // the names to answer with, in the language the player reads
  names = ({ });
  for (i = 0; i < sizeof(CONSENT_TYPES); i++)
    names += ({ query_consent_name(CONSENT_TYPES[i]) });

  if (sscanf(str, "%s %s", word, value) != 2)
  {
    notify_fail(_LANG_CONSENT_UNKNOWN);
    return 0;
  }

  if (member_array(value, _LANG_CONSENT_YES) != -1)
    allowed = 1;
  else if (member_array(value, _LANG_CONSENT_NO) != -1)
    allowed = 0;
  else
  {
    notify_fail(_LANG_CONSENT_UNKNOWN);
    return 0;
  }

  id = consent_id(word);

  if (!id)
  {
    notify_fail(_LANG_CONSENT_UNKNOWN);
    return 0;
  }

  consents[id] = allowed;

  name = query_consent_name(id);
  value = allowed ? _LANG_CONSENT_ON : _LANG_CONSENT_OFF;
  tell_object(this_object(), _LANG_CONSENT_CHANGED);
  return 1;
}

mixed * stats() 
{
  return ({ 
      ({"Consents", consents, }),
         });
}
