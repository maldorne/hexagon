// A scroll: a parchment with a spell written on it. Whoever knows enough of
// the spell's magic reads it aloud (read <scroll> [on <target>]) and the spell
// is cast once, at the level and power it was written with and for no gp; the
// parchment crumbles. A scroll with no spell is blank.
//
// Who can read it is decided by the spell: someone who knows at least one of
// its categories (a sphere or a school) through their guild, and whose guild
// does not withhold its gifts. Anyone else sees runes. A reader who knows the
// category only a little may botch it, and the scroll burns.
//
// An item (or a shop) makes a scroll in its setup:
//
//   inherit "/lib/obj/scroll.c";
//   void setup() { set_scroll_spell("/games/<game>/obj/spells/.../<spell>.c");
//                  set_scroll_level(3); }

#include <living/effects.h>
#include <language.h>

inherit "/lib/item.c";

// the spell cast (the path of its effect), nil when the scroll is blank
private string spell_path;
// the guild level (1 to 20) the spell runs at
private int scroll_level;
// the level of the spell in its categories, which prices the scroll
private int spell_level;
// the power the spell runs with (1 minor, 2 neutral, 3 major), the writer's
// knowledge of its magic instead of the reader's
private int scroll_power;

// the chance in a hundred that a reader who knows the category only a little
// botches the reading
#define BOTCH_CHANCE 20
// the power a scroll is written with unless its setup says otherwise: whoever
// wrote it knew the magic well enough (2 neutral)
#define DEFAULT_SCROLL_POWER 2
// the price of a scroll for each level of the spell and of the scroll
#define VALUE_PER_LEVEL 40

void create()
{
  scroll_level = 1;
  scroll_power = DEFAULT_SCROLL_POWER;
  ::create();

  // a blank scroll, unless the setup of an item has written it already
  if (!spell_path)
  {
    set_name(_LANG_SCROLL_NAME);
    add_alias(_LANG_SCROLL_ALIASES);
    set_main_plural(_LANG_SCROLL_PLURAL);
    add_plural(_LANG_SCROLL_PLURALS);
    set_short(_LANG_SCROLL_SHORT);
    set_long(_LANG_SCROLL_LONG);
    set_gender(1);
    set_weight(1);
    set_value(VALUE_PER_LEVEL);
  }
}

int query_scroll() { return 1; }

/**
 * The same word without accents, for the aliases of players whose client
 * mangles them.
 */
private string plain_ascii(string str)
{
  string * from, * to;
  int i;

  // the accents are several bytes each, so they are replaced whole
  from = ({ "á", "é", "í", "ó", "ú", "ü", "ñ" });
  to = ({ "a", "e", "i", "o", "u", "u", "ny" });
  for (i = 0; i < sizeof(from); i++)
    str = replace_string(str, from[i], to[i]);

  return str;
}

/**
 * Writes a spell on the scroll (nil leaves it blank). The scroll is named
 * after the spell and priced by its level.
 *
 * @param  path  the file of the spell's effect
 */
void set_scroll_spell(string path)
{
  object spell;
  string name, spell_name;
  string * names;
  int i;

  spell_path = path;
  if (!path)
    return;

  spell = nil;
  catch(spell = load_object(path));
  if (!spell)
  {
    spell_path = nil;
    return;
  }

  spell_name = spell->query_effect_name();
  names = ({ spell_name }) + spell->query_aliases();

  set_name(_LANG_SCROLL_NAME);
  add_alias(_LANG_SCROLL_ALIASES);
  for (i = 0; i < sizeof(names); i++)
  {
    name = _LANG_SCROLL_NAME + " " + names[i];
    add_alias(({ name, plain_ascii(name) }));
    // "scroll of <spell>", as the shop and the player type it
    name = _LANG_SCROLL_NAME + " " + _LANG_SCROLL_OF + " " + names[i];
    add_alias(({ name, plain_ascii(name) }));
  }
  set_main_plural(_LANG_SCROLL_PLURAL);
  add_plural(_LANG_SCROLL_PLURALS);
  set_gender(1);
  set_weight(1);

  set_short(_LANG_SCROLL_SHORT_WRITTEN);
  set_long(_LANG_SCROLL_LONG_WRITTEN);
  spell_level = spell->query_lowest_level();
  set_value(VALUE_PER_LEVEL * scroll_level * spell_level);
}

string query_scroll_spell() { return spell_path; }

/**
 * The guild level (1 to 20) the spell was written with, the one it uses for
 * its amounts and durations instead of the reader's.
 */
void set_scroll_level(int level)
{
  scroll_level = level < 1 ? 1 : level;

  // the price follows the level
  if (spell_path)
    set_value(VALUE_PER_LEVEL * scroll_level * spell_level);
}

int query_scroll_level() { return scroll_level; }

/**
 * The power (1 minor, 2 neutral, 3 major) the spell was written with, the one
 * its formulas use instead of how well the reader knows its magic.
 */
void set_scroll_power(int power)
{
  scroll_power = power < 1 ? 1 : (power > 3 ? 3 : power);
}

int query_scroll_power() { return scroll_power; }

/**
 * Whether a reader can make sense of the writing: they know a category of the
 * spell through their guild, and their guild does not withhold its gifts.
 *
 * @return nil when they can, else what to tell them
 */
mixed query_scroll_refusal(object reader)
{
  object spell;
  string refusal;

  if (!spell_path)
    return _LANG_SCROLL_BLANK;

  spell = nil;
  catch(spell = load_object(spell_path));
  if (!spell)
    return _LANG_SCROLL_RUNES;

  refusal = spell->query_guild_refusal(reader);
  if (stringp(refusal))
    return refusal;

  if (!spell->query_effect_power(reader))
    return _LANG_SCROLL_RUNES;

  return nil;
}

string query_long()
{
  object reader;
  string spell_name, level;
  mixed refusal;
  object ob;

  if (!spell_path)
    return ::query_long();

  reader = this_player();
  if (!reader)
    return ::query_long();

  refusal = query_scroll_refusal(reader);
  if (refusal)
    return ::query_long() + _LANG_SCROLL_LONG_RUNES;

  ob = load_object(spell_path);
  spell_name = ob->query_effect_name();
  level = "" + scroll_level;
  return ::query_long() + _LANG_SCROLL_LONG_LEGIBLE;
}

/**
 * The reader reads the scroll aloud: the spell is cast through its effect with
 * the scroll as the source, at the level and power it was written with and
 * for no gp.
 * The scroll crumbles when the cast starts, and stays when it does not (no
 * target, the guild refuses...); the reason is left for notify_fail.
 *
 * @param  reader  who reads it
 * @param  rest    what the reader named after the scroll (the target)
 * @return 1 when the scroll was read (even if it burned), 0 when not
 */
int read_scroll(object reader, string rest)
{
  object spell;
  mixed refusal;

  refusal = query_scroll_refusal(reader);
  if (refusal)
  {
    notify_fail(refusal);
    return 0;
  }

  // the reader must see the writing, and hold the scroll
  if (reader->check_dark(environment(reader) ? (int)environment(reader)->query_light() : 0))
  {
    notify_fail(_LANG_SCROLL_DARK);
    return 0;
  }

  if (environment(this_object()) != reader)
  {
    notify_fail(_LANG_SCROLL_NOT_CARRIED);
    return 0;
  }

  spell = load_object(spell_path);
  if (!rest)
    rest = "";

  // knowing the magic only a little, the reading may go wrong
  if (spell->query_effect_power(reader) == 1 && random(100) < BOTCH_CHANCE)
  {
    tell_object(reader, _LANG_SCROLL_BURNS_ME);
    tell_room(environment(reader), reader->query_cap_name() +
      _LANG_SCROLL_BURNS_ROOM, reader);
    dest_me();
    return 1;
  }

  if (!spell->cast_effect_at(rest, reader, scroll_level, scroll_power))
    return 0;

  tell_object(reader, _LANG_SCROLL_CRUMBLES_ME);
  tell_room(environment(reader), reader->query_cap_name() +
    _LANG_SCROLL_CRUMBLES_ROOM, reader);
  dest_me();
  return 1;
}
