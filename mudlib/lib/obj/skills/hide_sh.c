// Hide shadow. Cloned onto a character by the hide skill on success and
// shadowed over them, so call_other/-> calls to the character's name-producing
// methods return nil (invisible) while it is attached. It reveals itself (and
// self-destructs) when the character acts, or when a searcher wins the
// player_search contest. Ported from the old CcMud hide_sh, Folken 10/2009.

#include <common/properties.h>
#include <living/skills.h>
#include <language.h>

private object player;
private int found;

// A living carrying this shadow reports itself as hidden. query_hidden is
// what the effect system and other code check; query_hide_shadow marks that
// the hidden state comes from this shadow specifically.
int query_hide_shadow() { return 1; }
int query_hidden() { return !found; }

static void do_dest_hide_shadow() { destruct_object(this_object()); }

// Reveal the hidden character to the room and remove the shadow.
void dest_hide_shadow()
{
  if (found)
    return;

  found = 1;

  tell_room(environment(player),
    _LANG_HIDE_REVEALED_ROOM_PRE + player->query_short() +
    _LANG_HIDE_REVEALED_ROOM_POST, ({ player }));
  tell_player(player, _LANG_HIDE_REVEALED_YOU);

  call_out("do_dest_hide_shadow", 0);
}

void setup_shadow(object ob)
{
  // Already hidden -> nothing to do.
  if (ob->query_hide_shadow())
    return;

  player = ob;
  shadow(player, 1);
}

// The character is visible again once found, to a looker who senses life, or
// to the who/finger listings (which must still show everyone).
private int visible()
{
  return found ||
    (this_player() && this_player()->query_property(SENSE_LIFE_PROP)) ||
    (query_verb() == "who") || (query_verb() == "finger");
}

// Name-producing methods return nil while hidden, so the room's contents
// listing and look skip the character.
string query_short() { return visible() ? player->query_short() : nil; }
string short(varargs int dark) { return visible() ? player->short(dark) : nil; }
string pretty_short(varargs int dark) { return visible() ? player->pretty_short(dark) : nil; }
string query_plural() { return visible() ? player->query_plural() : nil; }
string pretty_plural() { return visible() ? player->pretty_plural() : nil; }

// Acting breaks the hiding.
int move_living(string dir, mixed dest, varargs mixed message, mixed enter)
{
  dest_hide_shadow();
  return player->move_living(dir, dest, message, enter);
}

void attack_by(object ob)
{
  dest_hide_shadow();
  player->attack_by(ob);
}

void attack_ob(object ob)
{
  dest_hide_shadow();
  player->attack_ob(ob);
}

int do_death(string str)
{
  dest_hide_shadow();
  return (int) player->do_death(str);
}

int do_say(string str)
{
  int x;
  x = player->do_say(str);
  if (x)
    dest_hide_shadow();
  return x;
}

int do_shout(string str)
{
  int x;
  x = player->do_shout(str);
  if (x)
    dest_hide_shadow();
  return x;
}

int do_emote(string str)
{
  int x;
  x = player->do_emote(str);
  if (x)
    dest_hide_shadow();
  return x;
}

// Someone searched the room (see the search skill). Roll the searcher's
// perception + search ability against the hider's evasion; on a win, reveal.
// The searcher is this_player() (the one who ran search).
int event_player_search(varargs mixed args)
{
  object searcher;
  int me, him;

  if (found)
    return 1;

  searcher = this_player();
  if (!searcher || searcher == player)
    return 0;

  him = player->query_int() + player->query_dex() +
        player->query_level() + player->query_per();
  me  = searcher->query_int() + searcher->query_dex() +
        searcher->query_level() + searcher->query_per() +
        searcher->query_skill_ability(SKILL_SEARCH);

  if (random(me) + me / 4 > random(him) + him / 4)
  {
    tell_player(searcher, _LANG_HIDE_SEARCH_FOUND_PRE +
      player->query_cap_name() + _LANG_HIDE_SEARCH_FOUND_POST);
    dest_hide_shadow();
    return 1;
  }

  return 0;
}
