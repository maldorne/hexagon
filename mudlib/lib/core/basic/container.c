// some translations, neverbot 10/2001
// addapted to ccmud, neverbot 6/03

#include <basic/move.h>
#include <language.h>

inherit obj      "/lib/core/object.c";
inherit contents "/lib/core/basic/contents.c"; // old cute_look.c

#define BASE_WEIGHT_REDUCTION 30
#define DEFAULT_MAX_WEIGHT 0

static int max_weight;       // maximum allowable weight, 0 = no limit
static int contents_weight;  // current contained weight
static int weight_reduction;


void create()
{
  weight_reduction = BASE_WEIGHT_REDUCTION;
  max_weight = DEFAULT_MAX_WEIGHT;
  obj::create();
}

void fix_my_contents_weight();
void set_max_weight(int arg) { max_weight = arg; }
int query_max_weight() { return max_weight; }
int query_contents_weight() { return contents_weight; }
void set_weight_reduction(int arg) { weight_reduction = arg; }
int query_weight_reduction() { return weight_reduction; }

int query_weight()
{
  int temp;

  if (living(this_object()))
    temp = this_object()->query_contents_weight();
  else
    temp = this_object()->query_contents_weight() * weight_reduction / 100;

  temp += ::query_weight();

  if (temp > 0)
    return temp;
  else
    return 0;
}

int add_weight(int n)
{
  if (!max_weight)
    return 1;

  if (n + contents_weight > max_weight)
    return 0;

  contents_weight += n;

  if (environment())
    environment()->fix_my_contents_weight();

  return 1;
}

// Oct 1995 -- Hamlet
// This is damn sloppy.  Zillions of things are bugging encumbrance,
// particularly money.  So I'm going to have money call this fcn every
// time something is transacted.  Should fix encumbrance, clearing up
// problems with ANYTHING that might be bugging it.  But boy it's stupid
// to have to do this.  Someone should figure out how to fix the real
// problems.  But not now.  No time.

void fix_my_contents_weight()
{
  object *stuff;
  int i;

  stuff = all_inventory(this_object());
  contents_weight = 0;

  for (i = 0; i < sizeof(stuff); i++)
    if (!add_weight(stuff[i]->query_weight()))
  {
	  // Let's be nice to the player and TELL them when they drop something :)
	  // Anirudh
    tell_object(this_object(), _LANG_CONTAINER_DROP);
    stuff[i]->move(environment(this_object()));
  }
}

void fix_my_contents_weight_later()
{
  call_out("fix_my_contents_weight",2);
  return;
}

int transfer_all_to(object dest)
{
  object *ob;
  int i;

  ob = all_inventory(this_object());
  ob -= ({ dest });
  for (i = 0; i < sizeof(ob); i++)
    ob[i]->move(dest);
  if (first_inventory(this_object()))
    return 0;
  return 1;
}

// ok modifyed back to using writes...
string long(string str, int dark)
{
  string ret;

  ret = ::long(str);

  // if we do not have a description, don't fill a line 
  // with blank characters
  if (strlen(ret))
    ret = sprintf("\n  %-=*s\n", (this_user()?this_user()->query_cols()-2:77),
          "   " + ::long(str));

  ret += calc_extra_look();

  if (this_object()->query_corpse())
    ret += (string)this_object()->query_living_contents(0);
  else
    ret += query_contents(short(dark) + _LANG_CONTAINER_CONTAINS, all_inventory());
  return ret;
}

// int test_remove(object ob, int flag) { return 1; }
// int test_add(object ob, int flag) {  return 1; }

object * find_inv_match(string str)
{
  return all_inventory(this_object());
}

mapping query_auto_load_attributes()
{
  return ([
      "::" : ::query_auto_load_attributes(),
      "inv" : "/lib/core/basic/auto_load"->create_auto_load(all_inventory(this_object())),
    ]) + 
    ((weight_reduction != BASE_WEIGHT_REDUCTION) ? ([ "weight reduction" : weight_reduction ]) : ([ ])) +
    ((max_weight != DEFAULT_MAX_WEIGHT) ? ([ "max weight" : max_weight ]) : ([ ]));
}

void init_auto_load_attributes(mapping attribute_map)
{
  if (!undefinedp(attribute_map["max weight"]))
    max_weight = attribute_map["max weight"];
  else
    max_weight = DEFAULT_MAX_WEIGHT;

  if (!undefinedp(attribute_map["weight reduction"]))
    weight_reduction = attribute_map["weight reduction"];
  else
    weight_reduction = BASE_WEIGHT_REDUCTION;

  if (!undefinedp(attribute_map["inv"]))
    "/lib/core/basic/auto_load"->load_auto_load(attribute_map["inv"], this_object());

  if (!undefinedp(attribute_map["::"]))
    ::init_auto_load_attributes(attribute_map["::"]);
} 

mixed * stats()
{
  return ::stats() + ({
      ({ "Contents Weight (nosave)", contents_weight, }),
      ({ "Max Weight (nosave)", max_weight, }),
      ({ "Weight Reduction (nosave)", weight_reduction, }),
    });
}
