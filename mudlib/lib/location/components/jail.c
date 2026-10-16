// Jail component. The location is a cell: the same behaviour as the jail cell
// room (/lib/room/justice/jail-cell.c), both from jail-actions.c. Whoever is
// serving a sentence in the crimes handler (see /lib/handlers/crimes.md)
// cannot leave it before the time is up, and may try to bribe the jailer.

#include <room/location.h>

inherit component "/lib/location/component.c";
inherit actions   "/lib/room/justice/jail-actions.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_JAIL);
}

void dest_me() {}

object query_jail_place() { return query_entity(); }

void init()
{
  actions::init();
}

/**
 * Asked by the location when an exit is typed.
 *
 * @param dir the exit
 * @param who the living leaving
 * @return the refusal, nil when it may go
 */
string query_exit_refusal(string dir, object who)
{
  return query_jail_refusal(dir, who);
}

string query_info()
{
  return "cell";
}
