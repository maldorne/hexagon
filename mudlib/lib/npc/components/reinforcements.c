// reinforcements.c -- help that turns up when a fight starts, as an NPC
// component.
//
// While the owner, one of its helpers or an NPC of the helpers' type fights in
// its place, each blow has a chance of calling one more helper, up to a maximum
// alive at once. The helper is born from a bestiary template, joins the fight
// against everybody who fights the owner and its helpers, and the helpers leave
// when nobody fights any more. Ported from the escolta of the old muds.
//
// Attributes:
//   "source" - template id of the helper (areas/<area>/npcs/<type>), of the
//              owner's game
//   "chance" - percent per blow that help is called (default 20)
//   "max"    - helpers alive at once (default 3)
//   "arrive" - what the room is told when one arrives
//   "leave"  - what the room is told when they leave

#include <living/persisted.h>

inherit component "/lib/npc/component.c";

#define DEFAULT_CHANCE 20
#define DEFAULT_MAX 3
// seconds between the call for help and the arrival, and between the checks
// of whether the fight is over
#define ARRIVAL_DELAY 3
#define CHECK_DELAY 20
// seconds the helpers wait, at most, before they leave once it is over
#define LEAVE_DELAY 10

private string helper_source, arrive_message, leave_message;
private int helper_chance, helper_max;
// the helpers alive, not saved: they are not part of the owner
static object * helpers;
static int arrival_pending, check_handle;

void dismiss();

void create()
{
  component::create();
  helper_source = nil;
  arrive_message = "";
  leave_message = "";
  helper_chance = DEFAULT_CHANCE;
  helper_max = DEFAULT_MAX;
  helpers = ({ });
  arrival_pending = 0;
  check_handle = 0;
}

string query_info()
{
  return (helper_source ? helper_source : "-") + " " + helper_chance +
         "% up to " + helper_max;
}

// The helpers still standing.
private object * living_helpers()
{
  helpers -= ({ nil });
  return helpers;
}

// Whether somebody is one of the owner's helpers or of their kind.
private int is_helper_kind(object who)
{
  return who && (member_array(who, helpers) != -1 ||
                 (who->query_npc() && who->query_npc_source() == helper_source));
}

// A blow in the owner's place: maybe call for help.
void event_fight_in_progress(object ob, object * fighters)
{
  object me;

  me = query_entity();
  if (!me || !helper_source || !pointerp(fighters) || sizeof(fighters) < 2 ||
      arrival_pending)
    return;

  if (member_array(me, fighters) == -1 &&
      !is_helper_kind(fighters[0]) && !is_helper_kind(fighters[1]))
    return;

  if (random(100) >= helper_chance || sizeof(living_helpers()) >= helper_max)
    return;

  arrival_pending = 1;
  call_out("arrive", ARRIVAL_DELAY);
}

// Every living thing fighting the owner or one of its helpers.
private object * enemies_of_all()
{
  object me;
  object * who, * list;
  int i;

  me = query_entity();
  who = ({ me }) + living_helpers();
  list = ({ });
  for (i = 0; i < sizeof(who); i++)
    if (who[i])
      list += who[i]->query_attacker_list();

  return list - who - ({ nil });
}

// The helper turns up and joins the fight.
void arrive()
{
  object me, helper, here;
  object * enemies;
  mapping t;
  mixed kit;
  string game;
  int i;

  arrival_pending = 0;
  me = query_entity();
  if (!me || me->query_dead() || !helper_source)
    return;

  here = environment(me);
  game = me->query_npc_game();
  if (!here || !game || sizeof(living_helpers()) >= helper_max)
    return;

  helper = BESTIARY_HANDLER->spawn_from_template(game, helper_source);
  if (!helper)
    return;

  // the kit of the type, one alternative per slot, as the sweep hands it over
  t = BESTIARY_HANDLER->query_template(game, helper_source);
  kit = t ? t["equipment"] : nil;
  if (pointerp(kit))
    for (i = 0; i < sizeof(kit); i++)
      if (pointerp(kit[i]) && sizeof(kit[i]))
        helper->add_clone(kit[i][random(sizeof(kit[i]))], 1);

  helper->move(here);
  helper->do_equip("");
  helper->components_placed();
  helpers += ({ helper });

  if (strlen(arrive_message))
    tell_room(here, arrive_message, ({ helper }));

  enemies = enemies_of_all();
  for (i = 0; i < sizeof(enemies); i++)
    if (environment(enemies[i]) == here)
      helper->attack_ob(enemies[i]);

  if (!check_handle)
    check_handle = call_out("check_fight", CHECK_DELAY);
}

// Keep the helpers while somebody still fights; send them off when nobody does.
void check_fight()
{
  object me;

  check_handle = 0;
  me = query_entity();

  if (!me || me->query_dead())
  {
    dismiss();
    return;
  }

  if (sizeof(enemies_of_all()) || sizeof(me->query_attacker_list()))
  {
    check_handle = call_out("check_fight", CHECK_DELAY);
    return;
  }

  call_out("dismiss", 1 + random(LEAVE_DELAY));
}

// The helpers go.
void dismiss()
{
  object * list;
  int i;

  list = living_helpers() + ({ });
  helpers = ({ });

  for (i = 0; i < sizeof(list); i++)
  {
    if (strlen(leave_message) && environment(list[i]))
      tell_room(environment(list[i]), leave_message, ({ list[i] }));
    list[i]->dest_me();
  }
}

void dest_me()
{
  if (check_handle)
    remove_call_out(check_handle);
  dismiss();
  component::dest_me();
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() + ([
    "source" : helper_source,
    "chance" : helper_chance,
    "max"    : helper_max,
    "arrive" : arrive_message,
    "leave"  : leave_message,
  ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;

  if (stringp(args["source"]))
    helper_source = args["source"];
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
    ({ "Source", helper_source }),
    ({ "Chance", helper_chance }),
    ({ "Max", helper_max }),
    ({ "Helpers", sizeof(living_helpers()) }),
  });
}
