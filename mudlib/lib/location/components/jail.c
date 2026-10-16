// Jail component. The location is a cell: whoever is serving a sentence in the
// crimes handler (see /lib/handlers/crimes.md) cannot leave it before the time
// is up, and may try to bribe the jailer.
//
// Nothing is kept here and nothing is watched: the sentence belongs to the
// crimes handler, and a sentence that has run out is ended the first time
// somebody asks about it -- when the prisoner tries an exit, or logs in -- so
// there is no heart beat. The citizenship's prison (`crimes set <citizenship>
// prison <file>`) is the location prisoners are taken to; this component is what
// keeps them there.

#include <room/location.h>
#include <common/properties.h>
#include <language.h>

inherit component "/lib/location/component.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_JAIL);
}

void dest_me() {}

void init()
{
  string * verbs;
  int i;

  verbs = _LANG_JAIL_CMDS_BRIBE;
  for (i = 0; i < sizeof(verbs); i++)
    add_action("do_bribe", verbs[i]);
}

/**
 * Why a living cannot take an exit: it is serving a sentence that has not ended.
 * Asked by the location when an exit is typed.
 *
 * @param dir the exit
 * @param who the living leaving
 * @return the refusal, nil when it may go
 */
string query_exit_refusal(string dir, object who)
{
  int left;

  if (!who || !query_entity())
    return nil;

  left = (int)handler("crimes", query_entity())->query_jail_left(who);
  if (left <= 0)
    return nil;

  return replace_string(_LANG_JAIL_LOCKED, "$time$",
                        seconds_to_time_string(left));
}

/**
 * The bribe command: the price is paid win or lose, see the crimes handler.
 */
int do_bribe(string str)
{
  object who, crimes;
  int result, cost;

  who = this_player();
  if (!who || !query_entity())
    return 0;

  crimes = handler("crimes", query_entity());
  if ((int)crimes->query_jail_left(who) <= 0)
  {
    notify_fail(_LANG_JAIL_NOT_PRISONER);
    return 0;
  }

  if (who->query_property(PASSED_OUT_PROP))
  {
    notify_fail(_LANG_JAIL_PASSED_OUT);
    return 0;
  }

  cost = (int)crimes->query_bribe_cost(who);
  result = (int)crimes->bribe(who);

  switch (result)
  {
    case -2:
      write(replace_string(_LANG_JAIL_BRIBE_NO_MONEY, "$cost$", "" + cost));
      break;
    case 1:
      write(replace_string(_LANG_JAIL_BRIBE_OK, "$cost$", "" + cost));
      break;
    case 0:
      write(_LANG_JAIL_BRIBE_FAILED);
      break;
    default:
      return 0;
  }

  return 1;
}

string query_info()
{
  return "cell";
}
