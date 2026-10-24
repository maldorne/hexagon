// reinforcements.c -- help that turns up when a fight starts, as an NPC
// component.
//
// While the owner or one of its helpers fights in its place, each blow has a
// chance of calling one more helper, up to a maximum answering at once. The
// helpers are people of the owner's community (the census of its area's root)
// made from any of the sources: the nearest free one comes walking, with the
// responder component, after whoever fights the owner, and goes back to its
// round when that fight is over. Nobody is created: when no helper is free, no
// help comes. Ported from the escolta of the old muds.
//
// Attributes:
//   "sources" - template ids of the helpers (areas/<area>/npcs/<type>), people
//               of the owner's community
//   "chance"  - percent per blow that help is called (default 20)
//   "max"     - helpers answering at once (default 3)
//   "arrive"  - what the room is told when one reaches the owner
//   "leave"   - what the room is told when they go, once nobody fights

#include <areas/area.h>

inherit component "/lib/npc/component.c";

#define DEFAULT_CHANCE 20
#define DEFAULT_MAX 3
// seconds a helper keeps after an attacker who has gone
#define HELP_SECONDS 120
// seconds between the checks of whether the fight is over
#define CHECK_DELAY 20

private string * helper_sources;
private string arrive_message, leave_message;
private int helper_chance, helper_max;
// the helpers called, not saved: they are not part of the owner
static object * helpers;
static int call_pending, check_handle;

void create()
{
  component::create();
  helper_sources = ({ });
  arrive_message = "";
  leave_message = "";
  helper_chance = DEFAULT_CHANCE;
  helper_max = DEFAULT_MAX;
  helpers = ({ });
  call_pending = 0;
  check_handle = 0;
}

string query_info()
{
  return implode(helper_sources, ", ") + " " + helper_chance + "% up to " +
         helper_max;
}

// The helpers still on their errand.
private object * answering_helpers()
{
  int i;
  object * out;

  out = ({ });
  for (i = 0; i < sizeof(helpers); i++)
    if (helpers[i] && !helpers[i]->query_dead() &&
        helpers[i]->has_component("responder"))
      out += ({ helpers[i] });

  return out;
}

// Whether somebody is one of the owner's helpers.
private int is_helper(object who)
{
  return who && member_array(who, answering_helpers()) != -1;
}

// A blow in the owner's place: maybe call for help.
void event_fight_in_progress(object ob, object * fighters)
{
  object me;

  me = query_entity();
  if (!me || !sizeof(helper_sources) || !pointerp(fighters) ||
      sizeof(fighters) < 2 || call_pending)
    return;

  if (member_array(me, fighters) == -1 &&
      !is_helper(fighters[0]) && !is_helper(fighters[1]))
    return;

  if (random(100) >= helper_chance || sizeof(answering_helpers()) >= helper_max)
    return;

  // outside the blow: calling may load locations
  call_pending = 1;
  call_out("call_help", 0);
}

// Every living thing fighting the owner or one of its helpers, here.
private object * enemies_here()
{
  object me;
  object * who, * list;
  int i;

  me = query_entity();
  who = ({ me }) + answering_helpers();
  list = ({ });
  for (i = 0; i < sizeof(who); i++)
    if (who[i])
      list |= who[i]->query_attacker_list();

  list -= who + ({ nil });
  for (i = 0; i < sizeof(list); i++)
    if (environment(list[i]) != environment(me))
      list[i] = nil;

  return list - ({ nil });
}

// The owner's community: the root of its area's parent chain.
private object community()
{
  object me, area, root;

  me = query_entity();
  area = (object)load_object(AREA_HANDLER)->query_area(me->query_npc_area_path());
  if (!area)
    return nil;

  root = (object)area->query_root_area();
  return root ? root : area;
}

// The nearest free helper is sent after one of the owner's enemies.
void call_help()
{
  object me, area, here, helper, comp;
  object * enemies, * picked;
  string * uuids;

  call_pending = 0;
  me = query_entity();
  if (!me || me->query_dead() || !(here = environment(me)))
    return;

  enemies = enemies_here();
  area = community();
  if (!sizeof(enemies) || !area)
    return;

  uuids = (string *)area->query_census_uuids_of(helper_sources) -
          ({ me->query_uuid() });
  picked = (object *)area->pick_nearest_npcs(here, uuids, 1);
  if (!sizeof(picked))
    return;

  helper = picked[0];
  helper->add_component("responder", ([ ]));
  comp = (object)helper->query_component_by_type("responder");
  if (!comp)
    return;

  helpers = answering_helpers() + ({ helper });
  comp->respond(enemies[random(sizeof(enemies))], here, HELP_SECONDS);

  if (!check_handle)
    check_handle = call_out("check_fight", CHECK_DELAY);
}

/**
 * Somebody has come in: a helper reaching the owner is announced, after the
 * room has seen it come in.
 */
void event_enter(object ob, varargs string mess, object from, mixed avoid)
{
  if (strlen(arrive_message) && is_helper(ob))
    call_out("announce_arrival", 0, ob);
}

void announce_arrival(object helper)
{
  object me;

  me = query_entity();
  if (me && helper && environment(helper) == environment(me))
    tell_room(environment(me), arrive_message, ({ helper }));
}

// While somebody still fights, the helpers stay; once nobody does, those beside
// the owner take their leave (their errand takes them back).
void check_fight()
{
  object me;
  object * list;
  int i;

  check_handle = 0;
  me = query_entity();
  if (!me || me->query_dead())
  {
    helpers = ({ });
    return;
  }

  if (sizeof(enemies_here()) || sizeof(me->query_attacker_list()))
  {
    check_handle = call_out("check_fight", CHECK_DELAY);
    return;
  }

  list = answering_helpers();
  helpers = ({ });
  if (!strlen(leave_message))
    return;

  for (i = 0; i < sizeof(list); i++)
    if (environment(list[i]) == environment(me))
      tell_room(environment(me), leave_message, ({ list[i] }));
}

void dest_me()
{
  if (check_handle)
    remove_call_out(check_handle);
  helpers = ({ });
  component::dest_me();
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() + ([
    "sources" : helper_sources,
    "chance"  : helper_chance,
    "max"     : helper_max,
    "arrive"  : arrive_message,
    "leave"   : leave_message,
  ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;

  if (pointerp(args["sources"]))
    helper_sources = ({ }) + args["sources"];
  if (intp(args["chance"]))
    helper_chance = args["chance"];
  if (intp(args["max"]))
    helper_max = args["max"];
  if (stringp(args["arrive"]))
    arrive_message = args["arrive"];
  if (stringp(args["leave"]))
    leave_message = args["leave"];
}

mixed * stats()
{
  return component::stats() + ({
    ({ "Sources", implode(helper_sources, ", ") }),
    ({ "Chance", helper_chance }),
    ({ "Max", helper_max }),
    ({ "Helpers", sizeof(answering_helpers()) }),
  });
}
