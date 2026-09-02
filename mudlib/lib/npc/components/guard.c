// guard.c -- the guard-role behaviour, as an NPC component.
//
// Watching an exit and stopping enemy citizenships is a role, not a race or a
// class, so it is a component an NPC can be given (a citizenship's guards get it
// when the area posts them) rather than a base class. This replaces the old
// /lib/npc/guardian.c mixin and its /lib/guard.c base: a guard is now a plain
// generic NPC carrying this component.
//
// The room side is unchanged -- the exit handler still calls
// guard_npc->guardian_check(mover) / guardian_message(); npc.c proxies those to
// this component. The composing NPC supplies query_city_ob() (its citizenship),
// which this component reads through query_owner().

#include <areas/diplomacy.h>
#include <areas/poi.h>
#include <room/location.h>

inherit component "/lib/npc/component.c";

// The exit this guard watches, read off the post it works at rather than stored:
// a town_entrance POI names the way in, and a guard is posted to a place before
// it is posted to a direction. Empty at a square, where the watch is presence
// only and nobody is stopped.
private string _watched_direction()
{
  object me, post;
  mapping poi;
  mixed work;

  me = query_owner();
  work = me ? me->query_work() : nil;
  if (!stringp(work) || !strlen(work))
    return "";

  post = (object)load_object(LOCATION_HANDLER)->load_location(work);
  if (!post || !post->query_area())
    return "";

  poi = ((mapping)post->query_area()->query_pois())[work];
  if (!poi || poi[POI_FIELD_KIND] != POI_KIND_TOWN_ENTRANCE)
    return "";

  return poi[POI_FIELD_GUARD_DIR] ? poi[POI_FIELD_GUARD_DIR] : "";
}

string query_direction() { return _watched_direction(); }

// Called once the NPC is standing in the world. Register on the exit so the
// exit handler consults us; it checks we are present before asking, so this
// holds across the walk to the barracks and back.
void placed()
{
  object me, post;
  string dir;

  dir = _watched_direction();
  me = query_owner();
  if (!strlen(dir) || !me)
    return;

  post = (object)load_object(LOCATION_HANDLER)->load_location(
           (string)me->query_work());
  if (post)
    post->register_guard(me, dir);
}

// Let `mover` through the watched exit? Blocks only citizenships this guard's
// citizenship is at war with, so a diplomacy change flips who is stopped without
// touching the guard. With no direction posted, or either side lacking a
// citizenship, the mover passes.
int check(object mover)
{
  string my_city_ob, mover_city_ob;
  object me, my_citizenship, mover_citizenship;

  me = query_owner();
  if (!strlen(_watched_direction()) || !mover || !me)
    return 1;

  // a mover with no citizenship is neutral and passes
  my_city_ob = me->query_city_ob();
  mover_city_ob = mover->query_city_ob();
  if (!my_city_ob || !mover_city_ob)
    return 1;

  // city_ob is the citizenship object's path; load it and read its name and
  // game -- diplomacy works in names, and resolves the parent cascade
  my_citizenship = load_object(my_city_ob);
  mover_citizenship = load_object(mover_city_ob);
  if (!my_citizenship || !mover_citizenship)
    return 1;

  return !handler("diplomacy", my_citizenship)->is_enemy(
             my_citizenship->query_name(),
             mover_citizenship->query_name());
}

// Message shown when the guard stops someone. nil falls back to the exit
// handler's default.
string message() { return nil; }
