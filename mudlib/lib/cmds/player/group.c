
// group / party -- a handful of people who band together for a while.
//
// The party itself is kept by the parties handler; this is the verb that makes
// and unmakes one. The living answers query_adventurer() and its family
// (/lib/living/party.c), which is what the party channel and the map read.

#include <mud/cmd.h>
#include <living/party.h>
#include <living/consents.h>
#include <basic/communicate.h>
#include <language.h>

inherit CMD_BASE;

private object find_member(object me, string who);

void setup()
{
  set_aliases(_LANG_CMD_GROUP_ALIAS);
  set_usage(_LANG_CMD_GROUP_SYNTAX);
  set_help(_LANG_CMD_GROUP_HELP);
}

// Somebody in my party, by name or by the nickname I call them.
private object find_member(object me, string who)
{
  object * members;
  int i;

  members = me->query_adventurer_list();
  who = me->expand_nickname(lower_case(who));

  for (i = 0; i < sizeof(members); i++)
    if (members[i]->query_name() == who)
      return members[i];

  return nil;
}

static int cmd(string str, object me, string verb)
{
  object who, leader;
  object * members;
  string option, rest, name, out;
  int id, i;

  if (!str || !strlen(str))
  {
    if (me->query_adventurer())
    {
      name = me->query_adventurer_group_name();
      write(_LANG_CMD_GROUP_IN_PARTY);
      return 1;
    }

    notify_fail(_LANG_CMD_GROUP_UNKNOWN_OPTION);
    return 0;
  }

  if (sscanf(str, "%s %s", option, rest) != 2)
  {
    option = str;
    rest = "";
  }

  option = lower_case(option);
  id = PARTY_HANDLER->query_party(me);
  name = me->query_adventurer_group_name();

  // ---- starting one -------------------------------------------------------

  if (member_array(option, _LANG_CMD_GROUP_CREATE) != -1)
  {
    if (id)
    {
      notify_fail(_LANG_CMD_GROUP_ALREADY_IN_PARTY);
      return 0;
    }

    // query_time_on() counts back from now, so a played character is negative
    if (-me->query_time_on() < PARTY_MIN_PLAY_TIME)
    {
      notify_fail(_LANG_CMD_GROUP_TOO_YOUNG);
      return 0;
    }

    name = strlen(rest) ? rest : _LANG_CMD_GROUP_DEFAULT_NAME;

    if (strlen(name) > PARTY_NAME_LENGTH)
    {
      notify_fail(_LANG_CMD_GROUP_NAME_TOO_LONG);
      return 0;
    }

    PARTY_HANDLER->create_party(me, name);

    write(_LANG_CMD_GROUP_CREATED_ME);
    tell_room(environment(me), _LANG_CMD_GROUP_CREATED_ROOM, ({ me }));
    return 1;
  }

  // ---- taking an invitation ----------------------------------------------

  if (member_array(option, _LANG_CMD_GROUP_JOIN) != -1)
  {
    if (id)
    {
      notify_fail(_LANG_CMD_GROUP_ALREADY_IN_PARTY);
      return 0;
    }

    if (!me->query_timed_property_exists(PARTY_INVITE_PROP))
    {
      notify_fail(_LANG_CMD_GROUP_NOT_INVITED);
      return 0;
    }

    id = me->query_timed_property(PARTY_INVITE_PROP);
    me->remove_timed_property(PARTY_INVITE_PROP);

    if (!PARTY_HANDLER->query_party_leader(id))
    {
      notify_fail(_LANG_CMD_GROUP_INVITE_GONE);
      return 0;
    }

    if (!PARTY_HANDLER->add_member(id, me))
    {
      notify_fail(_LANG_CMD_GROUP_FULL);
      return 0;
    }

    name = PARTY_HANDLER->query_party_name(id);
    members = PARTY_HANDLER->query_party_members(id) - ({ me });

    write(_LANG_CMD_GROUP_JOINED_ME);
    for (i = 0; i < sizeof(members); i++)
      tell_object(members[i], _LANG_CMD_GROUP_JOINED_THEM);

    return 1;
  }

  // everything else asks for a party

  if (!id)
  {
    notify_fail(_LANG_CMD_GROUP_NOT_IN_PARTY);
    return 0;
  }

  members = me->query_adventurer_list();

  // ---- what anybody in a party may do ------------------------------------

  if (member_array(option, _LANG_CMD_GROUP_CHANNEL) != -1)
  {
    CHAT_HANDLER->do_channel(me->query_adventurer_channel(), rest,
                             me->query_cap_name());
    return 1;
  }

  if (member_array(option, _LANG_CMD_GROUP_LIST) != -1 ||
      member_array(option, _LANG_CMD_GROUP_STATUS) != -1)
  {
    out = _LANG_CMD_GROUP_LIST_HEADER;
    leader = me->query_adventurer_leader();

    for (i = 0; i < sizeof(members); i++)
    {
      out += "  " + members[i]->query_cap_name();

      if (members[i] == leader)
        out += _LANG_CMD_GROUP_LIST_LEADER;

      if (member_array(option, _LANG_CMD_GROUP_STATUS) != -1 &&
          !members[i]->query_dead())
        out += ", " + members[i]->health_string(members[i] == me);

      out += "\n";
    }

    write(out);
    return 1;
  }

  if (member_array(option, _LANG_CMD_GROUP_LEAVE) != -1)
  {
    if (me->query_adventurer_leading())
    {
      notify_fail(_LANG_CMD_GROUP_LEADER_CANNOT_LEAVE);
      return 0;
    }

    members -= ({ me });
    write(_LANG_CMD_GROUP_LEFT_ME);

    PARTY_HANDLER->remove_member(me);

    for (i = 0; i < sizeof(members); i++)
      tell_object(members[i], _LANG_CMD_GROUP_LEFT_THEM);

    return 1;
  }

  // ---- what only the leader may do ---------------------------------------

  if (!me->query_adventurer_leading())
  {
    notify_fail(_LANG_CMD_GROUP_NOT_LEADER);
    return 0;
  }

  if (member_array(option, _LANG_CMD_GROUP_END) != -1)
  {
    members -= ({ me });
    write(_LANG_CMD_GROUP_ENDED_ME);

    for (i = 0; i < sizeof(members); i++)
      tell_object(members[i], _LANG_CMD_GROUP_ENDED_THEM);

    PARTY_HANDLER->end_party(id);
    return 1;
  }

  if (member_array(option, _LANG_CMD_GROUP_NAME) != -1)
  {
    if (!strlen(rest))
    {
      notify_fail(_LANG_CMD_GROUP_NEEDS_NAME);
      return 0;
    }

    if (strlen(rest) > PARTY_NAME_LENGTH)
    {
      notify_fail(_LANG_CMD_GROUP_NAME_TOO_LONG);
      return 0;
    }

    PARTY_HANDLER->set_party_name(id, rest);
    name = rest;

    write(_LANG_CMD_GROUP_RENAMED_ME);
    members -= ({ me });

    for (i = 0; i < sizeof(members); i++)
      tell_object(members[i], _LANG_CMD_GROUP_RENAMED_THEM);

    return 1;
  }

  if (member_array(option, _LANG_CMD_GROUP_INVITE) != -1)
  {
    if (!strlen(rest))
    {
      notify_fail(_LANG_CMD_GROUP_NEEDS_PLAYER);
      return 0;
    }

    if (sizeof(members) >= MAX_PARTY_MEMBERS)
    {
      notify_fail(_LANG_CMD_GROUP_FULL);
      return 0;
    }

    who = find_player(me->expand_nickname(lower_case(rest)));

    if (!who || !interactive(who))
    {
      notify_fail(_LANG_CMD_GROUP_NO_SUCH_PLAYER);
      return 0;
    }

    if (who == me)
    {
      notify_fail(_LANG_CMD_GROUP_NOT_YOURSELF);
      return 0;
    }

    if (who->query_adventurer())
    {
      notify_fail(_LANG_CMD_GROUP_THEY_HAVE_PARTY);
      return 0;
    }

    if (!who->query_consent(GROUP_CONSENT))
    {
      notify_fail(_LANG_CMD_GROUP_THEY_REFUSE);
      return 0;
    }

    who->add_timed_property(PARTY_INVITE_PROP, id, PARTY_INVITE_TIME);

    write(_LANG_CMD_GROUP_INVITED_ME);
    tell_object(who, _LANG_CMD_GROUP_INVITED_THEM);
    return 1;
  }

  if (member_array(option, _LANG_CMD_GROUP_KICK) != -1)
  {
    if (!strlen(rest))
    {
      notify_fail(_LANG_CMD_GROUP_NEEDS_PLAYER);
      return 0;
    }

    who = find_member(me, rest);

    if (!who)
    {
      notify_fail(_LANG_CMD_GROUP_NOT_A_MEMBER);
      return 0;
    }

    if (who == me)
    {
      notify_fail(_LANG_CMD_GROUP_NOT_YOURSELF);
      return 0;
    }

    PARTY_HANDLER->remove_member(who);

    write(_LANG_CMD_GROUP_KICKED_ME);
    tell_object(who, _LANG_CMD_GROUP_KICKED_THEM);

    members -= ({ me, who });
    for (i = 0; i < sizeof(members); i++)
      tell_object(members[i], _LANG_CMD_GROUP_KICKED_OTHERS);

    return 1;
  }

  if (member_array(option, _LANG_CMD_GROUP_LEADER) != -1)
  {
    if (!strlen(rest))
    {
      notify_fail(_LANG_CMD_GROUP_NEEDS_PLAYER);
      return 0;
    }

    who = find_member(me, rest);

    if (!who)
    {
      notify_fail(_LANG_CMD_GROUP_NOT_A_MEMBER);
      return 0;
    }

    if (who == me)
    {
      notify_fail(_LANG_CMD_GROUP_ALREADY_LEADER);
      return 0;
    }

    PARTY_HANDLER->set_party_leader(id, who);

    write(_LANG_CMD_GROUP_HANDED_ME);
    tell_object(who, _LANG_CMD_GROUP_HANDED_THEM);

    members -= ({ me, who });
    for (i = 0; i < sizeof(members); i++)
      tell_object(members[i], _LANG_CMD_GROUP_HANDED_OTHERS);

    return 1;
  }

  notify_fail(_LANG_CMD_GROUP_UNKNOWN_OPTION);
  return 0;
}
