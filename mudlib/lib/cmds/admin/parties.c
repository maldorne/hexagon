
// Every adventuring party in the game right now. Parties are not saved, so
// this is the only way to look at them from outside.

#include <std.h>
#include <mud/cmd.h>
#include <living/party.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "parties" }));
  set_usage("parties");
  set_help(
    "List every adventuring party there is at the moment.\n" +
    "\n" +
    "One block per party: its id, the name its members read, the channel it " +
    "talks on, and who is in it, the leader first. A party lives only in " +
    "memory, so this list is empty again after a reboot.");
}

static int cmd(string str, object me, string verb)
{
  object leader;
  object * members;
  int * ids;
  string out;
  int i, j;

  ids = (int *)PARTY_HANDLER->query_parties();

  if (!sizeof(ids))
  {
    notify_fail("There is no party right now.\n");
    return 0;
  }

  out = "";

  for (i = 0; i < sizeof(ids); i++)
  {
    leader = (object)PARTY_HANDLER->query_party_leader(ids[i]);
    members = (object *)PARTY_HANDLER->query_party_members(ids[i]);

    out += sprintf("%3d  %-30s %s\n", ids[i],
                   (string)PARTY_HANDLER->query_party_name(ids[i]),
                   (string)PARTY_HANDLER->query_party_channel(ids[i]));

    for (j = 0; j < sizeof(members); j++)
      out += "       " + members[j]->query_cap_name() +
             (members[j] == leader ? "  (leader)" : "") +
             (interactive(members[j]) ? "" : "  (not connected)") + "\n";
  }

  write("" + sizeof(ids) + " part" + (sizeof(ids) == 1 ? "y" : "ies") +
        ":\n" + out);
  return 1;
}
