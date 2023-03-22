
#include <mud/cmd.h>
#include <translations/common.h>
#include <language.h>

inherit CMD_BASE;

#define MONEY_HAND "/lib/handlers/money.c"

void setup()
{
  set_aliases(_LANG_CMD_MONEY_ALIAS);
  set_usage(_LANG_CMD_MONEY_SYNTAX);
  set_help(_LANG_CMD_MONEY_HELP);
}

static int cmd (string arg, object me, string verb)
{
	string str, * props, * pieces;
	object bank;
	int i, value, total, inbank;
	
  str = "";
  props = keys(me->query_properties());
  total = 0;
  inbank = 0;
  value = me->query_value();

  if (value)
	{
		str = _LANG_CMD_MONEY_ON_YOU;
		total += value;
	}
	
	for (i = 0; i < sizeof(props); i++)
	{
		pieces = explode(props[i], " ");
		if (sizeof(pieces) == 2 && pieces[0] == "bank")
		{
			// check the bank still exists
			bank = load_object(pieces[1]);
			if (bank && bank->query_bank())
			{
        inbank = 1;
				value = me->query_property(props[i]);
				
        if (value)
				{
					str += _LANG_CMD_MONEY_ON_BANK;
					str += MONEY_HAND->money_value_string(value) + ".\n";
					total += value;
				}
				else
					str += _LANG_CMD_MONEY_ON_BANK_EMPTY;
			}
		}
	}
	
  if (inbank)
	  str += _LANG_CMD_MONEY_TOTAL;

	tell_object(me, str);
	return 1;
}
