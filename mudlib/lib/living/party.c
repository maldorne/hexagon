
// party.c -- the living's side of an adventuring party.
//
// The party itself is kept by the parties handler, which is where the state
// lives and dies; this is only the living asking about its own. It sits on
// every living, not only on players, so a hired NPC can walk in a party too.

#include <living/party.h>
#include <language.h>

int query_adventurer()
{
  return PARTY_HANDLER->query_party(this_object()) != 0;
}

object query_adventurer_leader()
{
  return PARTY_HANDLER->query_party_leader(
    PARTY_HANDLER->query_party(this_object()));
}

object * query_adventurer_list()
{
  return PARTY_HANDLER->query_party_members(
    PARTY_HANDLER->query_party(this_object()));
}

string query_adventurer_group_name()
{
  return PARTY_HANDLER->query_party_name(
    PARTY_HANDLER->query_party(this_object()));
}

string query_adventurer_channel()
{
  return PARTY_HANDLER->query_party_channel(
    PARTY_HANDLER->query_party(this_object()));
}

// Whether this living leads the party it is in.
int query_adventurer_leading()
{
  return query_adventurer() && query_adventurer_leader() == this_object();
}

// Leaving the world: out of the party, handing the lead over if it was ours.
// Called from dest_me, so it covers quitting, a lost link and a destruct.
void leave_party()
{
  object heir;

  if (!query_adventurer())
    return;

  heir = PARTY_HANDLER->leave_party(this_object());

  if (heir)
    tell_object(heir, _LANG_PARTY_NOW_LEADING);
}
