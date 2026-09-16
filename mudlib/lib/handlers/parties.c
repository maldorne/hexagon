
// parties.c -- who is adventuring with whom, right now.
//
// A party is a handful of livings who band together for a while: they share a
// channel of their own and see each other on the map. One of them leads it,
// and may take people in, put them out, rename it or hand the lead over.
//
// Nothing here is saved. A party lasts as long as the mud is up and the
// people in it are around; when the last one leaves, it is gone.
//
// The channel is named "#<id>#<name>": the leading # marks it as temporary
// (the channel handler drops those on reconnecting), the id keeps it apart
// from every other party, and the name is what the members read.

#include <living/party.h>
#include <basic/communicate.h>

int end_party(int id);

// id -> ([ "leader": ob, "members": ({ ob, ... }), "name": str ])
static mapping parties;
// the id each living belongs to
static mapping belongs_to;
// the last id handed out
static int last_id;

void create()
{
  parties = ([ ]);
  belongs_to = ([ ]);
  last_id = 0;
}

// The party somebody is in, or 0.
int query_party(object who)
{
  if (!who || !belongs_to[who])
    return 0;

  return belongs_to[who];
}

string query_party_name(int id)
{
  return parties[id] ? parties[id]["name"] : "";
}

object query_party_leader(int id)
{
  return parties[id] ? parties[id]["leader"] : nil;
}

// The members of a party, the leader among them, without whoever is gone.
object * query_party_members(int id)
{
  object * members;

  if (!parties[id])
    return ({ });

  members = parties[id]["members"] - ({ nil });
  parties[id]["members"] = members;

  return members + ({ });
}

// What the party's channel is called.
string query_party_channel(int id)
{
  if (!parties[id])
    return "";

  return "#" + id + "#" + parties[id]["name"];
}

// Every party there is, for whoever is looking from outside.
int * query_parties()
{
  return map_indices(parties);
}

// ---------------------------------------------------------------------------
// Making and unmaking
// ---------------------------------------------------------------------------

// Start a party led by somebody who is in none. Returns its id, or 0.
int create_party(object leader, string name)
{
  int id;

  if (!leader || belongs_to[leader])
    return 0;

  if (!stringp(name) || !strlen(name))
    return 0;

  id = ++last_id;
  parties[id] = ([ "leader": leader,
                   "members": ({ leader }),
                   "name": name ]);
  belongs_to[leader] = id;

  CHAT_HANDLER->open_channel(query_party_channel(id), leader);
  return id;
}

// Take somebody into a party. Returns false if it is full, or they are in one.
int add_member(int id, object who)
{
  if (!parties[id] || !who || belongs_to[who])
    return FALSE;

  if (sizeof(query_party_members(id)) >= MAX_PARTY_MEMBERS)
    return FALSE;

  parties[id]["members"] += ({ who });
  belongs_to[who] = id;

  CHAT_HANDLER->open_channel(query_party_channel(id), who);
  return TRUE;
}

// Take somebody out. The party's own leader is handled by hand_over_lead().
int remove_member(object who)
{
  int id;

  id = query_party(who);
  if (!id)
    return FALSE;

  CHAT_HANDLER->close_channel(query_party_channel(id), who);

  parties[id]["members"] -= ({ who });
  belongs_to[who] = nil;

  // a party of one is no party
  if (sizeof(query_party_members(id)) < 2)
    end_party(id);

  return TRUE;
}

// Hand the lead to somebody already in the party.
int set_party_leader(int id, object who)
{
  if (!parties[id] || !who || query_party(who) != id)
    return FALSE;

  parties[id]["leader"] = who;
  return TRUE;
}

// Rename a party. Everybody's channel is named after it, so it is closed and
// opened again under the new name.
int set_party_name(int id, string name)
{
  object * members;
  string channel;
  int i;

  if (!parties[id] || !stringp(name) || !strlen(name))
    return FALSE;

  members = query_party_members(id);
  channel = query_party_channel(id);

  for (i = 0; i < sizeof(members); i++)
    CHAT_HANDLER->close_channel(channel, members[i]);

  parties[id]["name"] = name;
  channel = query_party_channel(id);

  for (i = 0; i < sizeof(members); i++)
    CHAT_HANDLER->open_channel(channel, members[i]);

  return TRUE;
}

// End a party, whoever is still in it.
int end_party(int id)
{
  object * members;
  string channel;
  int i;

  if (!parties[id])
    return FALSE;

  members = query_party_members(id);
  channel = query_party_channel(id);

  for (i = 0; i < sizeof(members); i++)
  {
    CHAT_HANDLER->close_channel(channel, members[i]);
    belongs_to[members[i]] = nil;
  }

  parties = m_delete(parties, id);
  return TRUE;
}

// Somebody is leaving the world: out of the party, and if they were leading
// it, the next one in takes over. Returns the new leader, when there is one.
object leave_party(object who)
{
  object leader;
  int id;

  id = query_party(who);
  if (!id)
    return nil;

  leader = query_party_leader(id);

  // not the leader, or the last one standing: nothing to hand over
  if (leader != who || sizeof(query_party_members(id)) < 3)
  {
    remove_member(who);
    return nil;
  }

  remove_member(who);

  leader = query_party_members(id)[0];
  set_party_leader(id, leader);

  return leader;
}

mixed * stats()
{
  return ({
    ({ "Parties (nosave)", map_sizeof(parties), }),
    ({ "Last id (nosave)", last_id, }),
  });
}
