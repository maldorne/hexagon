// raise.c -- bringing the dead back, as an NPC component.
//
// Some settlements keep somebody who can return a ghost to its body: a priest
// at a shrine, a healer at a temple. That is a service the person offers, not
// a fact about their race or their trade, so it is a component an NPC can be
// given rather than a base class -- the same shape as the guard role.
//
// The component asks the NPC for two verbs (see query_component_actions): one
// to explain the service, one to request it. Being dead is the only condition,
// plus a cooldown so the service cannot be leaned on repeatedly.

#include <language.h>

inherit component "/lib/npc/component.c";

// How long, in heart beats, before the same player may ask again.
#define RAISE_COOLDOWN 100
// The timed property stamped on the player for that long.
#define RAISE_COOLDOWN_PROP "raise_cooldown"

void create()
{
  component::create();
}

// The service is a fact about the component, not state to carry: nothing here
// is persisted beyond what the base already stores.
mapping query_component_actions()
{
  mapping out;
  string * verbs;
  int i;

  out = ([ ]);

  verbs = _LANG_RAISE_INFO_VERBS;
  for (i = 0; i < sizeof(verbs); i++)
    out[verbs[i]] = "do_info";

  verbs = _LANG_RAISE_VERBS;
  for (i = 0; i < sizeof(verbs); i++)
    out[verbs[i]] = "do_raise";

  return out;
}

int do_info(string str)
{
  if (!query_owner())
    return 0;

  query_owner()->do_say(_LANG_RAISE_INFO);
  return 1;
}

int do_raise(string str)
{
  object who;

  who = this_player();
  if (!who || !query_owner())
    return 0;

  if (who->query_timed_property(RAISE_COOLDOWN_PROP))
  {
    query_owner()->do_say(_LANG_RAISE_TOO_SOON);
    return 1;
  }

  if (!who->query_dead())
  {
    query_owner()->do_say(_LANG_RAISE_NOT_DEAD);
    return 1;
  }

  tell_object(who, _LANG_RAISE_DONE_ME);
  tell_room(environment(who), _LANG_RAISE_DONE_ROOM, who);

  who->remove_ghost();
  who->add_timed_property(RAISE_COOLDOWN_PROP, 1, RAISE_COOLDOWN);
  return 1;
}
