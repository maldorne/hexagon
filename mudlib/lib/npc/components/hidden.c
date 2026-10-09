// hidden.c -- lying in wait, as an NPC component.
//
// The NPC is hidden as soon as it stands in the world, with the same shadow the
// hide skill puts on a player, so it does not show in the room and cannot be
// targeted. A search that finds it (the search skill against the hider) brings
// it out at once against whoever found it, opening with a skill when the
// template names one. Once nobody is fighting it, it hides again.
//
// Attributes: "skill", the id of the skill it opens with (optional).

#include <living/skills.h>

inherit component "/lib/npc/component.c";

#define HIDE_SHADOW "/lib/obj/skills/hide_sh.c"
// seconds between tries at hiding again after being found
#define REHIDE_DELAY 60

private string opening_skill;
// the pending try at hiding again, if any
static int rehide_handle;

void create()
{
  component::create();
  opening_skill = nil;
}

// Hide the owner, unless it is already hidden, dead or in a fight.
private void hide_owner()
{
  object npc;

  npc = query_owner();
  if (!npc || npc->query_dead() || npc->query_hide_shadow())
    return;
  if (sizeof(npc->query_attacker_list()))
    return;

  clone_object(HIDE_SHADOW)->setup_shadow(npc);
}

void placed()
{
  hide_owner();
}

// Somebody's search found the owner: go for them.
void revealed(mixed * args)
{
  object npc, finder;
  mixed data;
  int opened;

  npc = query_owner();
  finder = sizeof(args) ? args[0] : nil;

  if (npc && finder && !finder->query_dead() &&
      environment(finder) == environment(npc))
  {
    if (opening_skill)
    {
      data = table("skills", npc)->query_skill_data(opening_skill);
      if (pointerp(data))
        opened = load_object(data[SKILL_DATA_PATH])->cast_effect(
                   finder->query_name(), npc, 0);
    }

    // with no skill, or one it could not use, a plain attack
    if (!opened)
      npc->attack_ob(finder);
  }

  if (rehide_handle)
    remove_call_out(rehide_handle);
  rehide_handle = call_out("try_to_hide", REHIDE_DELAY);
}

// Back into hiding once the fight is over.
void try_to_hide()
{
  object npc;

  rehide_handle = 0;
  npc = query_owner();
  if (!npc || npc->query_hide_shadow())
    return;

  if (npc->query_dead() || sizeof(npc->query_attacker_list()))
  {
    rehide_handle = call_out("try_to_hide", REHIDE_DELAY);
    return;
  }

  hide_owner();
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         (opening_skill ? ([ "skill" : opening_skill ]) : ([ ]));
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (args && stringp(args["skill"]))
    opening_skill = args["skill"];
}

mixed * stats()
{
  return component::stats() + ({ ({ "Opening skill", opening_skill }) });
}
