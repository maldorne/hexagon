/*
 * Base of anything that can be eaten or drunk, neverbot 09/2026
 *
 * A piece of food is described like an entry of a pub menu: what it is,
 * how much it heals, how much room it takes in the stomach and how strong
 * it is. It is consumed in bites, and its condition falls with the time it
 * spends in the world until it is fit for nobody and rots away.
 */

#include <living/food.h>
#include <basic/condition.h>
#include <basic/move.h>
#include <language.h>

inherit "/lib/item.c";

// prototypes
void rot_away();
int query_spoiled();
int query_rot_time();

// what is served: FOOD, DRINK or ALCOHOL
private string food_type;
// the whole piece: every bite gives its share
private int heal, volume, intox;
private int bites, bites_left;
// seconds this takes to go from whole to inedible, and to rot away after
// that; with no decay time it never goes off
private int decay_time, rot_time;
// what is left of each of those two, counted down as the piece sits around
private int decay_left, rot_left;
// the moment the countdown was last charged. A piece of food only exists
// while somebody is around: it is destroyed when its owner logs out and made
// again on the way back, so the clock starts afresh and the hours nobody
// played never count.
static int charged_at;
// when and where it was made, kept as provenance
private int created_at;
// where it was made: the file name of the place, and what kind of place
private string origin_path, origin_kind;
private string consume_mess_me, consume_mess_others;

void create()
{
  food_type = FOOD;
  heal = 0;
  volume = 0;
  intox = 0;
  bites = 1;
  bites_left = 1;
  decay_time = 0;
  rot_time = 0;
  decay_left = 0;
  rot_left = 0;
  charged_at = time();
  created_at = time();
  origin_path = "";
  origin_kind = FOOD_ORIGIN_UNKNOWN;
  consume_mess_me = "";
  consume_mess_others = "";

  ::create();

  set_cond(MAX_COND);
}

void set_food_type(string type) { food_type = type; }
string query_food_type() { return food_type; }

int query_food() { return food_type == FOOD; }
int query_drink() { return food_type == DRINK || food_type == ALCOHOL; }

void set_heal(int amount) { heal = amount; }
int query_heal() { return heal; }

void set_volume(int amount) { volume = amount; }
int query_volume() { return volume; }

void set_intox(int amount) { intox = amount; }
int query_intox() { return intox; }

// how many bites the whole piece lasts
void set_bites(int number)
{
  bites = (number > 0) ? number : 1;
  bites_left = bites;
}

int query_bites() { return bites; }
int query_bites_left() { return bites_left; }

void set_decay_time(int seconds)
{
  decay_time = seconds;
  decay_left = seconds;
  rot_left = query_rot_time();
  charged_at = time();
}

int query_decay_time() { return decay_time; }

// How long a piece that has gone off lies around before it rots away. Unless
// it says otherwise, it lasts as long again as it took to go off.
void set_rot_time(int seconds)
{
  rot_time = seconds;
  rot_left = seconds;
}

int query_rot_time() { return (rot_time > 0) ? rot_time : decay_time; }

// What is left of its life, in seconds of being around
int query_decay_left() { return decay_left; }
int query_rot_left() { return rot_left; }

// Made in another moment than now: an older piece has already lost part of
// its life, a fresh one has all of it.
void set_created_at(int when)
{
  created_at = when;
  charged_at = time();
}

int query_created_at() { return created_at; }

void set_origin_path(string path) { origin_path = path; }
string query_origin_path() { return origin_path; }

void set_origin_kind(string kind) { origin_kind = kind; }
string query_origin_kind() { return origin_kind; }

// Who made this piece and when: the place it came out of, what kind of place
// that was, and the moment it was made.
void set_origin(object where, string kind)
{
  if (where)
    origin_path = where->query_file_name() ? where->query_file_name() :
                                             base_name(where);

  origin_kind = kind;
  created_at = time();
  decay_left = decay_time;
  rot_left = query_rot_time();
  charged_at = time();
  set_cond(MAX_COND);
}

// Each kind of food says how it is eaten: one message for whoever eats it,
// another for the rest of the room.
void set_consume_mess_me(string mine) { consume_mess_me = mine; }
void set_consume_mess_others(string others) { consume_mess_others = others; }

void set_consume_mess(string mine, string others)
{
  consume_mess_me = mine;
  consume_mess_others = others;
}

string query_consume_mess_me() { return consume_mess_me; }
string query_consume_mess_others() { return consume_mess_others; }

// Time is what spoils food. Nothing ticks: the seconds a piece has spent in
// the world are charged to it whenever somebody reads its state, looks at it,
// moves it or takes a bite, and its condition follows what is left. Answers
// whether the piece is done for, and books its end when it is: never in the
// middle of whatever is reading it.
private int apply_decay()
{
  int elapsed;

  if (decay_time <= 0)
    return 0;

  elapsed = time() - charged_at;
  charged_at = time();

  if (elapsed <= 0)
    return (decay_left <= 0 && rot_left <= 0);

  // freshness goes first; once there is none left the piece is on its way to
  // rotting away
  if (decay_left > 0)
  {
    if (elapsed >= decay_left)
    {
      elapsed -= decay_left;
      decay_left = 0;
    }
    else
    {
      decay_left -= elapsed;
      elapsed = 0;
    }
  }

  rot_left -= elapsed;

  set_cond(MAX_COND * decay_left / decay_time);

  if (decay_left > 0 || rot_left > 0)
    return 0;

  // the driver drops the pending call_outs of a destroyed object, so booking
  // this twice costs nothing
  call_out("rot_away", 0);
  return 1;
}

// Gone for good, said where it can be heard: in the hands of whoever carries
// it, or on the floor of the room it lies in.
void rot_away()
{
  object env;

  if (!query_spoiled())
    return;

  env = environment(this_object());

  if (env && living(env))
  {
    tell_object(env, _LANG_FOOD_ROTTED_HOLDER);
    if (environment(env))
      tell_room(environment(env), env->query_cap_name() + " " +
        _LANG_FOOD_ROTTED_HOLDER_ROOM, ({ env }));
  }
  else if (env)
    tell_room(env, _LANG_FOOD_ROTTED_ROOM);

  dest_me();
}

// Changing hands is another moment somebody deals with the piece, so the time
// it has spent around is charged first. A piece that turns out to be done for
// is refused rather than moved: it falls apart where it lies, and the message
// that says so follows this refusal.
int move(mixed dest, varargs mixed messin, mixed messout)
{
  if (apply_decay())
    return MOVE_NO_GET;

  return ::move(dest, messin, messout);
}

int query_cond()
{
  apply_decay();
  return ::query_cond();
}

int query_condition() { return query_cond(); }

// Food does not wear out like a blade does: it goes off. The generic
// condition words do not fit, so it says it in its own.
string cond_string()
{
  int left;

  apply_decay();

  left = (100 * ::query_cond()) / MAX_COND;

  if (left <= 0)
    return _LANG_FOOD_COND_SPOILED;
  if (left <= 25)
    return _LANG_FOOD_COND_STALE;
  if (left <= 60)
    return _LANG_FOOD_COND_PASSABLE;

  return _LANG_FOOD_COND_FRESH;
}

int query_spoiled() { return query_cond() <= 0; }

string long(varargs string str, int dark)
{
  string ret;

  ret = ::long(str, dark);

  if (decay_time > 0)
    ret += _LANG_FOOD_LONG_CONDITION;

  if (bites > 1)
    ret += _LANG_FOOD_LONG_BITES;

  return ret;
}

void init()
{
  ::init();

  add_action("do_consume", _LANG_FOOD_EAT_VERBS);
  add_action("do_consume", _LANG_FOOD_DRINK_VERBS);
}

// The messages of a bite: whatever this piece was given, or the default
// ones for what it is.
private string bite_mess_me()
{
  if (strlen(consume_mess_me))
    return consume_mess_me;

  return query_food() ? _LANG_FOOD_DEFAULT_EAT_ME : _LANG_FOOD_DEFAULT_DRINK_ME;
}

private string bite_mess_others()
{
  if (strlen(consume_mess_others))
    return consume_mess_others;

  return query_food() ? _LANG_FOOD_DEFAULT_EAT_OTHERS : _LANG_FOOD_DEFAULT_DRINK_OTHERS;
}

// One bite: its share of everything the whole piece carries, worth as much as
// the piece itself still is. Food nobody should eat turns the stomach instead.
void consume_bite(object eater)
{
  int spoiled;

  spoiled = query_spoiled();

  tell_object(eater, bite_mess_me());
  if (environment(eater))
    tell_room(environment(eater), eater->query_cap_name() + " " +
      bite_mess_others(), ({ eater }));

  if (spoiled)
  {
    tell_object(eater, _LANG_FOOD_SPOILED_ME);
    eater->adjust_hp(-(1 + random(3)), eater);
  }
  else
  {
    eater->adjust_volume(query_food() ? D_FOOD : D_DRINK, volume / bites);
    eater->adjust_hp(heal * query_cond() / (bites * MAX_COND), eater);
    eater->adjust_intox(intox / bites);
  }

  bites_left -= 1;

  if (bites_left > 0)
  {
    // what is left of the piece weighs less
    set_weight(query_weight() * bites_left / (bites_left + 1));
    return;
  }

  tell_object(eater, _LANG_FOOD_FINISHED);
  call_out("dest_me", 0);
}

int do_consume(string str)
{
  if (!str || !strlen(str) || !id(str))
  {
    notify_fail(_LANG_FOOD_CONSUME_WHAT);
    return 0;
  }

  if (!living(this_player()))
    return 0;

  consume_bite(this_player());
  return 1;
}

mapping query_auto_load_attributes()
{
  // what is written down is what the piece has left right now, not what it
  // had the last time somebody dealt with it. A piece about to fall apart is
  // not written down at all: nothing that is being destroyed belongs in a
  // character sheet it would come back from.
  if (apply_decay())
    return nil;

  return ::query_auto_load_attributes() +
    ([ "food_bites_left" : bites_left,
       "food_decay_left" : decay_left,
       "food_rot_left" : rot_left,
       "food_created_at" : created_at,
       "food_origin_path" : origin_path,
       "food_origin_kind" : origin_kind,
    ]);
}

void init_auto_load_attributes(mapping args)
{
  if (!undefinedp(args["food_bites_left"]))
    bites_left = args["food_bites_left"];
  if (!undefinedp(args["food_decay_left"]))
    decay_left = args["food_decay_left"];
  if (!undefinedp(args["food_rot_left"]))
    rot_left = args["food_rot_left"];
  if (!undefinedp(args["food_created_at"]))
    created_at = args["food_created_at"];
  if (!undefinedp(args["food_origin_path"]))
    origin_path = args["food_origin_path"];
  if (!undefinedp(args["food_origin_kind"]))
    origin_kind = args["food_origin_kind"];

  ::init_auto_load_attributes(args);

  // back in the world with whatever life it had left, and its clock starting
  // now: the time its owner spent logged out does not count
  charged_at = time();

  if (decay_time > 0)
    set_cond(MAX_COND * decay_left / decay_time);
}
