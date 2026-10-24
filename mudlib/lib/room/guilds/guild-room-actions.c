// Guild room: the main hall of a guild's house, where its members gather.
// Shared by the guild room (/lib/room/guilds/guild-room.c) and the guild room
// location component (/lib/location/components/guild-room.c). Its members
// advance a level of the guild here (avanzar), spending the experience the
// guild asks for it, which coste shows them here (the general coste command,
// elsewhere, shows every kind of level), learn what their level allows
// (aprender) and read its rules (normas).
//
// Ported from the CcMud guild room, neverbot 12/2006 and 07/2010.

#include <language.h>

// the guild this is the hall of, as the path of its object
private string room_guild;
// who may advance here: the class levels (0 for no limit) and the teacher that
// has to be present (nil for none)
private int teach_min_level, teach_max_level;
private string teacher;
static object room_sign;

int query_guild_room() { return 1; }

// Where the hall stands. The component answers with its location.
object query_guild_room_place() { return this_object(); }

void create()
{
  room_guild = "";
  teach_min_level = 0;
  teach_max_level = 0;
  teacher = nil;
}

string query_room_guild() { return room_guild; }
void set_room_guild(string path) { room_guild = path ? path : ""; }

/**
 * Limits who advances in this hall: only members whose class level is within
 * the range (0 for no limit on that side) and, when a teacher is named, only
 * while a living thing answering to that id stands in the hall.
 */
void set_teaching(int min_level, int max_level, varargs string id)
{
  teach_min_level = min_level;
  teach_max_level = max_level;
  teacher = id;
}
int query_teach_min_level() { return teach_min_level; }
int query_teach_max_level() { return teach_max_level; }
string query_teacher() { return teacher; }

// The refusal for a member who may not advance here, nil when they may.
private string teaching_refusal(object me)
{
  if (teacher && !present(teacher, query_guild_room_place()))
    return _LANG_GUILD_ROOM_NO_TEACHER;
  if (teach_min_level && me->query_level() < teach_min_level)
    return _LANG_GUILD_ROOM_TOO_YOUNG;
  if (teach_max_level && me->query_level() > teach_max_level)
    return _LANG_GUILD_ROOM_TOO_ADVANCED;

  return nil;
}

// Put up the sign that says what can be done in the hall, replacing any earlier
// one, so a place never has two.
object create_guild_room_sign(object where)
{
  if (room_sign)
    room_sign->dest_me();

  room_sign = where->add_sign(_LANG_GUILD_ROOM_SIGN_LONG,
                              _LANG_GUILD_ROOM_SIGN_TEXT);
  return room_sign;
}

// Take the sign down again, when the hall leaves its place.
void take_down_guild_room_sign()
{
  if (room_sign)
    room_sign->dest_me();
  room_sign = nil;
}

void init()
{
  add_action("do_advance", _LANG_GUILD_ROOM_CMDS_ADVANCE);
  add_action("do_cost", _LANG_GUILD_ROOM_CMDS_COST);
  add_action("do_learn", _LANG_GUILD_ROOM_CMDS_LEARN);
  add_action("do_rules", _LANG_GUILD_CMDS_RULES);
}

// normas: the rules of the guild, read from its object and its rules book, for
// its members.
int do_rules(string str)
{
  string text;

  if (!this_player())
    return 0;

  if (!strlen(room_guild) || this_player()->query_guild_ob() != room_guild)
  {
    notify_fail(strlen(room_guild) ? _LANG_GUILD_ROOM_RULES_NOT_MEMBER
                                   : _LANG_GUILD_ROOM_NO_GUILD);
    return 0;
  }

  text = handler("rules")->query_rules(room_guild, this_player(), query_guild_room_place());
  if (!text)
  {
    notify_fail(_LANG_GUILD_ROOM_NO_GUILD);
    return 0;
  }

  tell_object(this_player(), text);
  return 1;
}

// What the next level of the guild costs a member. Anybody else gets the
// general coste command.
int do_cost(string str)
{
  object me, guild;
  int cost, have, next;

  me = this_player();
  if (!me || !strlen(room_guild) ||
      me->query_guild_ob() != room_guild ||
      !(guild = load_object(room_guild)))
    return 0;

  if ((int)me->query_guild_level() >= (int)guild->query_max_level())
  {
    write(_LANG_GUILD_ROOM_MAX_LEVEL);
    return 1;
  }

  next = (int)me->query_guild_level() + 1;
  cost = (int)guild->query_next_level_xp(me);
  have = (int)me->query_xp();
  write(_LANG_GUILD_ROOM_COST);
  return 1;
}

// A member of the guild goes up a level of it, paying the experience the guild
// asks for the next one.
int do_advance(string str)
{
  object me, guild;
  string refusal;
  int cost, have;

  me = this_player();
  if (!me || !strlen(room_guild) ||
      me->query_guild_ob() != room_guild)
  {
    notify_fail(_LANG_GUILD_ROOM_NOT_MEMBER);
    return 0;
  }

  guild = load_object(room_guild);
  if (!guild)
  {
    notify_fail(_LANG_GUILD_ROOM_NOT_MEMBER);
    return 0;
  }

  if ((int)me->query_guild_level() >= (int)guild->query_max_level())
  {
    notify_fail(_LANG_GUILD_ROOM_MAX_LEVEL);
    return 0;
  }

  refusal = teaching_refusal(me);
  if (refusal)
  {
    notify_fail(refusal);
    return 0;
  }

  cost = (int)guild->query_next_level_xp(me);
  have = (int)me->query_xp();
  if (have < cost)
  {
    notify_fail(_LANG_GUILD_ROOM_NOT_ENOUGH_XP);
    return 0;
  }

  me->adjust_xp(-cost);
  me->adjust_guild_level(1);

  log_file("guilds", "[" + ctime(time(), 4) + "] " + me->query_cap_name() +
           " advances to level " + me->query_guild_level() + " of " +
           room_guild + "\n");

  tell_room(environment(me), _LANG_GUILD_ROOM_ADVANCED_OTHERS, ({ me }));
  me->save();
  return 1;
}

// aprender: a member learns every spell and skill of the guild up to their
// guild level that they do not know yet, so whatever moved to a lower level
// is learned as well. With nothing new, it says what comes next.
int do_learn(string str)
{
  object me, guild, ob;
  string * learned, * names;
  mixed * next;
  string next_name;
  int i, next_level;

  me = this_player();
  if (!me || !strlen(room_guild) || me->query_guild_ob() != room_guild ||
      !(guild = load_object(room_guild)))
  {
    notify_fail(_LANG_GUILD_ROOM_LEARN_NOT_MEMBER);
    return 0;
  }

  guild->grant_guild_skills(me);
  learned = guild->teach_guild_spells(me);

  if (sizeof(learned))
  {
    names = ({ });
    for (i = 0; i < sizeof(learned); i++)
    {
      ob = nil;
      catch(ob = load_object(learned[i]));
      names += ({ ob ? capitalize(ob->query_effect_name()) : learned[i] });
    }

    write(_LANG_GUILD_ROOM_LEARNED);
    tell_room(environment(me), _LANG_GUILD_ROOM_LEARNED_OTHERS, ({ me }));
    me->save();
    return 1;
  }

  // the lowest level above the member's that teaches a spell
  next = guild->query_next_ability_spell((int)me->query_guild_level());
  next_level = 0;
  next_name = nil;
  if (next)
  {
    ob = nil;
    catch(ob = load_object(next[0]));
    next_level = next[1];
    next_name = ob ? capitalize(ob->query_effect_name()) : next[0];
  }

  write(next_name ? _LANG_GUILD_ROOM_LEARN_NEXT : _LANG_GUILD_ROOM_LEARN_NOTHING);
  return 1;
}

mixed * stats()
{
  return ({ ({ "Room Guild", room_guild }),
            ({ "Teaching class levels", teach_min_level + " to " + teach_max_level }),
            ({ "Teacher", teacher ? teacher : "-" }) });
}
