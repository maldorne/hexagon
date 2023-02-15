
#include <living/consents.h>
#include <mud/secure.h>

mapping consents;

void create() 
{
  // consents = allocate_mapping(sizeof(CONSENT_TYPES));
  consents = ([ ]);
}

void consent_commands() 
{ 
  add_private_action("change_consent", "consentir"); 
}

mixed query_valid_consents() { return CONSENT_TYPES; }
mixed query_consent(string tipo) { return consents[tipo]; }
mapping query_consents() { return consents; }

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
  string tipo, ret;
  int value;

  ret = "";

  if (!strlen(str))
  {
    ret += "Tus consentimientos actuales son:\n";

    for (value = sizeof(CONSENT_TYPES); value--;)
      ret += sprintf("%17s: %s\n", CONSENT_TYPES[value], (consents[CONSENT_TYPES[value]] ? "sí":"no"));
            
    ret += "\nPara cambiarlos, utiliza 'consentir <nombre> <sí|no>'.\n";

    tell_object(this_player(), ret);
    return 1;
  }

  if ((sscanf(str, "%s si", tipo) == 1) || (sscanf(str, "%s sí", tipo) == 1))
    value = 1;
  else if (sscanf(str, "%s no", tipo) == 1)
    value = 0;
  else
  {
    notify_fail("Tipo inválido de consentimiento.\nLos tipos válidos son: " +
      query_multiple_short(CONSENT_TYPES) + ".\n");
    return 0;
  }

  consents[tipo] = value;

  tell_object(this_object(), "Ok. Consentir "+tipo+" cambiado a "+(value ? "sí":"no")+".\n");
  return 1;
}

mixed stats() 
{
  return ({ 
      ({"Consents", consents, }),
         });
}
