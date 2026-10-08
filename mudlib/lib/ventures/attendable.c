/* 
 * functions to say if a venture is open for business, Dimmak@Rl '99
 * reviewed for CcMud, neverbot '03
 * reviewed for Hexagon, neverbot '21
 */

#include <language.h>

// Where the opening conditions live, and the one every venture starts with.
#define CONDITIONS_DIR "/lib/ventures/conditions/"
#define DEFAULT_CONDITIONS ({ ({ CONDITIONS_DIR + "attended", ([ ]) }) })

string open_condition;
string attender_name;

// The conditions the venture opens under, all of which must hold:
// ({ ({ condition object path, ([ argument : value ]) }) }). See
// /lib/ventures/conditions/readme.md.
mixed * open_conditions;

// from shop.c
static int only_sell;

void create() 
{
  open_condition = "";
  attender_name = "";
  only_sell = 0;
  open_conditions = DEFAULT_CONDITIONS;
}

void set_open_condition(string str) { open_condition = str; }
string query_open_condition() { return open_condition; }

// The location the venture stands in: a location component's location, or the
// venture itself when it is a room.
object query_venue()
{
  object loc;

  loc = function_exists("query_my_location", this_object())
          ? (object)this_object()->query_my_location() : nil;
  return loc ? loc : this_object();
}

mixed * query_open_conditions()
{
  return open_conditions ? open_conditions : ({ });
}

// Add a condition, or replace the arguments of one already there.
void add_open_condition(string path, varargs mapping args)
{
  int i;

  if (!open_conditions)
    open_conditions = ({ });

  for (i = 0; i < sizeof(open_conditions); i++)
    if (open_conditions[i][0] == path)
    {
      open_conditions[i] = ({ path, args ? args : ([ ]) });
      return;
    }

  open_conditions += ({ ({ path, args ? args : ([ ]) }) });
}

int remove_open_condition(string path)
{
  int i;

  for (i = 0; i < sizeof(open_conditions); i++)
    if (open_conditions[i][0] == path)
    {
      open_conditions = open_conditions[0..i - 1] + open_conditions[i + 1..];
      return 1;
    }

  return 0;
}

// Why the venture cannot serve right now, or 0 when it can: open when the
// legacy open_condition function (if any) and every condition say so. The first
// condition that does not hold gives the reason the customer reads.
mixed query_closed_reason()
{
  mixed answer;
  int i;

  if (open_condition && strlen(open_condition))
    if (!call_other(this_object(), open_condition))
      return _LANG_ATT_NON_ATTENDABLE;

  for (i = 0; i < sizeof(open_conditions); i++)
  {
    answer = call_other(open_conditions[i][0], "check_open", this_object(),
                        query_venue(), this_player(), open_conditions[i][1]);
    if (stringp(answer))
      return answer;
    if (!answer)
      return _LANG_ATT_NON_ATTENDABLE;
  }

  return 0;
}

// Whether the venture serves right now; if not, its reason is the pending
// notify_fail.
int check_open_condition()
{
  mixed reason;

  reason = query_closed_reason();
  if (reason)
  {
    notify_fail(reason);
    return 0;
  }

  return 1;
}

void set_attender(string str)
{
  attender_name = str;
  set_open_condition("standard_open_condition");
}

int standard_open_condition()
{
  object *list;
  object who;
  int i;
   
  // TODO check in the location if this is a component

  list = find_match(attender_name, this_object());
  if (!sizeof(list)) 
    return 0;

  who = list[0];
  list = who->query_attacker_list();
  for (i = 0; i < sizeof(list); i++)
    if (environment(list[i]) == this_object()) 
      return 0;
  
  return 1;
}

// from shop.c

void sell_only() { only_sell = 1; }
int query_only_sell() { return only_sell; }

// Autoload contract. Only consulted by location components — the legacy
// room version of shop/pub persists these fields through its own
// save_object on the blueprint. See /lib/location/component.c for the
// pull-on-save flow.
mapping query_auto_load_attributes()
{
  return ([
    "attendable_open_condition" : open_condition,
    "attendable_attender_name"  : attender_name,
    "attendable_only_sell"      : only_sell,
    "attendable_open_conditions" : query_open_conditions(),
  ]);
}

void init_auto_load_attributes(mapping args)
{
  if (!undefinedp(args["attendable_open_condition"]))
    open_condition = args["attendable_open_condition"];
  if (!undefinedp(args["attendable_attender_name"]))
    attender_name = args["attendable_attender_name"];
  if (!undefinedp(args["attendable_only_sell"]))
    only_sell = args["attendable_only_sell"];
  if (arrayp(args["attendable_open_conditions"]))
    open_conditions = args["attendable_open_conditions"];
}

mixed * stats()
{
  return ({
    ({ "Open Condition", open_condition }),
    ({ "Attender Name", attender_name }),
    ({ "Only Sell", only_sell }),
    ({ "Open Conditions", query_open_conditions() }),
  });
}

