// Base deity file...
// neverbot -  March 18, 2002
// I do not think channels for the deity group are needed, nor commands
// of your own for having a particular deity, but I leave the code commented just in case
//    neverbot 3/2002
// 10/2026 - A deity now says who may worship it, what it takes as an offering
//           and what it can be prayed for (see /lib/room/temple/temple.md).
//
// A deity is chosen once and kept forever. Who may worship it:
//   - races: allowed and forbidden race ids; a race admits all its lineages
//   - lineages: allowed and forbidden lineage ids (a lineage is a race object
//     with a base race, see /lib/lineage.c)
//   - alignment: a band of the real alignment
//   - guilds: required (any of them) and forbidden guild paths
//   - citizenship: the worshipper's own citizenship must accept the deity
// Offerings are rules, first match wins (add_offering): coins, items, and
// corpses and body parts filtered by whose body it was; prayers are ids with
// the words that ask for them (add_prayer). A deity changes the defaults in its
// setup, and masks grant_prayer for a prayer of its own.

#include <basic/gender.h>
#include <living/skills.h>
#include <deities.h>
#include <language.h>

inherit "/lib/core/object.c";

// mixed *job_commands = ({ });

private string * races, * forbidden_races;
private string * lineages, * forbidden_lineages;
private string * guilds, * forbidden_guilds;
private int min_align, max_align;
private mapping * offerings;
private mapping prayers;

void create()
{
  string * ids;
  mapping words;
  int i;

  races = ({ });
  forbidden_races = ({ });
  lineages = ({ });
  forbidden_lineages = ({ });
  guilds = ({ });
  forbidden_guilds = ({ });
  min_align = -MAX_ALIGN_INT;
  max_align = MAX_ALIGN_INT;

  // coins, and anything worth something
  offerings = ({
    ([ DEITY_OFFER_KIND : DEITY_OFFER_COINS ]),
    ([ DEITY_OFFER_KIND : DEITY_OFFER_ITEM, DEITY_OFFER_MIN : DEITY_VALUABLE_MIN ]),
  });

  prayers = ([ ]);
  words = _LANG_DEITY_PRAYER_WORDS;
  ids = map_indices(words);
  for (i = 0; i < sizeof(ids); i++)
    prayers[ids[i]] = ([ DEITY_PRAYER_WORDS : words[ids[i]],
                         DEITY_PRAYER_LUCK  : DEITY_PRAY_LUCK ]);

  ::create();
}

// --- Gender ------------------------------------------------------------------

/** A deity is a god unless its setup says it is a goddess. */
int query_gender()
{
  int gender;

  gender = ::query_gender();
  return gender ? gender : GENDER_MALE;
}

/** "god" or "goddess", for the texts that name what the deity is. */
string query_god_noun()
{
  return query_gender() == GENDER_FEMALE ? _LANG_DEITY_NOUN_FEMALE
                                         : _LANG_DEITY_NOUN_MALE;
}

/**
 * Fills a deity text: $god$ is its name, $noun$ god or goddess.
 */
string fill_text(string text)
{
  text = replace_string(text, "$god$", query_short());
  return replace_string(text, "$noun$", query_god_noun());
}

// --- Who may worship it ------------------------------------------------------

/**
 * The deity's own path, as social objects are named: the full path with ".c".
 */
string query_deity_path() { return file_name(this_object()) + ".c"; }

void set_races(string * list) { races = list ? list : ({ }); }
void set_forbidden_races(string * list) { forbidden_races = list ? list : ({ }); }
void set_lineages(string * list) { lineages = list ? list : ({ }); }
void set_forbidden_lineages(string * list) { forbidden_lineages = list ? list : ({ }); }
string * query_races() { return races; }
string * query_forbidden_races() { return forbidden_races; }
string * query_lineages() { return lineages; }
string * query_forbidden_lineages() { return forbidden_lineages; }

/** Guild paths (with ".c"), any of which admits a worshipper. Empty admits everybody. */
void set_guilds(string * list) { guilds = list ? list : ({ }); }
void set_forbidden_guilds(string * list) { forbidden_guilds = list ? list : ({ }); }
string * query_guilds() { return guilds; }
string * query_forbidden_guilds() { return forbidden_guilds; }

/** The real alignment a worshipper must have, both ends included. */
void set_alignment_range(int min, int max)
{
  min_align = min;
  max_align = max;
}

/** The same with a band of <deities.h>: DEITY_ALIGN_EVIL, ... */
void set_alignment_band(int * band)
{
  if (sizeof(band) == 2)
    set_alignment_range(band[0], band[1]);
}

int * query_alignment_range() { return ({ min_align, max_align }); }

/**
 * Whether a race id may worship it, by the race lists alone.
 */
int query_legal_race(string race)
{
  if (member_array(race, forbidden_races) != -1)
    return 0;
  if (sizeof(races) && member_array(race, races) == -1)
    return 0;
  return 1;
}

// A race named admits every lineage of it, so both the living's race id and
// the one of its base race are asked.
private string race_refusal(object player)
{
  string race, base;

  race = player->query_race_id();
  base = player->query_base_race_id();

  if (member_array(race, forbidden_races) != -1 ||
      (strlen(base) && member_array(base, forbidden_races) != -1))
    return _LANG_DEITY_REFUSE_RACE;

  // through query_legal_race, so a deity that masks it keeps its own rule
  if (!query_legal_race(race) && (!strlen(base) || !query_legal_race(base)))
    return _LANG_DEITY_REFUSE_RACE;

  return nil;
}

// A living whose race has a base race is of a lineage, its race id the
// lineage's.
private string lineage_refusal(object player)
{
  string lineage;

  lineage = strlen(player->query_base_race_id()) ? player->query_race_id() : "";

  if (strlen(lineage) && member_array(lineage, forbidden_lineages) != -1)
    return _LANG_DEITY_REFUSE_LINEAGE;

  if (sizeof(lineages) && member_array(lineage, lineages) == -1)
    return _LANG_DEITY_REFUSE_LINEAGE;

  return nil;
}

private string alignment_refusal(object player)
{
  int align;

  align = player->query_real_align();
  if (align < min_align)
    return _LANG_DEITY_REFUSE_TOO_EVIL;
  if (align > max_align)
    return _LANG_DEITY_REFUSE_TOO_GOOD;
  return nil;
}

private string guild_refusal(object player)
{
  mixed guild;

  guild = player->query_guild_ob();

  if (stringp(guild) && member_array(guild, forbidden_guilds) != -1)
    return _LANG_DEITY_REFUSE_GUILD_FORBIDDEN;

  if (!sizeof(guilds))
    return nil;

  if (stringp(guild) && member_array(guild, guilds) != -1)
    return nil;

  return _LANG_DEITY_REFUSE_GUILD_REQUIRED;
}

/**
 * Whether a citizenship of the diplomacy graph lets this deity be worshipped.
 *
 * @param citizenship the citizenship name
 * @param from an object of the game, to reach its diplomacy handler
 */
int query_accepted_by(string citizenship, object from)
{
  object diplomacy;

  if (!citizenship || !strlen(citizenship))
    return 1;

  diplomacy = handler("diplomacy", from);
  if (!diplomacy)
    return 1;

  return diplomacy->accepts_deity(citizenship, query_deity_path());
}

private string citizenship_refusal(object player)
{
  mixed city, name;
  object citizenship;

  city = player->query_city_ob();
  if (!stringp(city) || !strlen(city))
    return nil;

  citizenship = load_object(city);
  name = citizenship ? citizenship->query_name() : nil;
  if (!stringp(name) || query_accepted_by(name, player))
    return nil;

  return _LANG_DEITY_REFUSE_CITIZENSHIP;
}

/**
 * Whether a living may have this deity at all: what it was born as. NPC
 * templates and set_deity_ob ask this; worshipping asks the whole of
 * query_worship_refusal.
 */
int query_legal_player(object player)
{
  if (!player)
    return 0;
  return !race_refusal(player) && !lineage_refusal(player);
}

/**
 * Why a player may not worship this deity, nil when they may. A deity masks it
 * for a rule of its own, calling this one for the rest.
 *
 * @return the refusal, filled, or nil
 */
string query_worship_refusal(object player)
{
  string refusal;

  if (!player)
    return "";

  refusal = race_refusal(player);
  if (!refusal)
    refusal = lineage_refusal(player);
  if (!refusal)
    refusal = alignment_refusal(player);
  if (!refusal)
    refusal = guild_refusal(player);
  if (!refusal)
    refusal = citizenship_refusal(player);

  return refusal ? fill_text(refusal) : nil;
}

// --- Offerings ---------------------------------------------------------------

/** Adds an offering rule, after the ones there are. */
void add_offering(mapping rule)
{
  if (mappingp(rule))
    offerings += ({ rule });
}

/** Drops every offering rule, the defaults included. */
void clear_offerings() { offerings = ({ }); }

mapping * query_offerings() { return offerings; }

// Whether a material id is the one named or below it ("metal:steel" is metal).
// Weapons, armours and shields keep their material as a number: the materials
// table turns it into its id.
private int material_matches(mixed material, string wanted)
{
  material = table("materials")->query_material_id(material);
  if (!stringp(material))
    return 0;
  return material == wanted ||
         (strlen(material) > strlen(wanted) &&
          material[0 .. strlen(wanted)] == wanted + ":");
}

/**
 * The value of an item, 0 when it has none. A corpse has no value of its own
 * and says what it is worth as an offering.
 */
int value_of(object item)
{
  mixed value;

  if (!item)
    return 0;

  value = item->query_corpse() ? item->query_offering_value() : item->query_value();
  return intp(value) ? value : 0;
}

// Whether a rule's filters on the dead (race, alignment, level, players) let a
// corpse or a body part through. A body part answers query_source_*.
private int body_matches(mapping rule, object item)
{
  mixed races, parts;
  string race, base;
  int align, level, player;

  if (item->query_body_part())
  {
    race = item->query_source_race_id();
    base = item->query_source_base_race_id();
    align = item->query_source_align();
    level = item->query_source_level();
    player = item->query_source_was_player();

    parts = rule[DEITY_OFFER_PART];
    if (stringp(parts))
      parts = ({ parts });
    if (arrayp(parts) && member_array(item->query_part_id(), parts) == -1)
      return 0;
  }
  else
  {
    race = item->query_race_id();
    base = item->query_base_race_id();
    align = item->query_dead_align();
    level = item->query_dead_level();
    player = item->query_was_player();
  }

  races = rule[DEITY_OFFER_RACES];
  if (arrayp(races) && member_array(race, races) == -1 &&
      (!base || !strlen(base) || member_array(base, races) == -1))
    return 0;

  if (intp(rule[DEITY_OFFER_ALIGN_MIN]) && align < rule[DEITY_OFFER_ALIGN_MIN])
    return 0;
  if (intp(rule[DEITY_OFFER_ALIGN_MAX]) && align > rule[DEITY_OFFER_ALIGN_MAX])
    return 0;
  if (intp(rule[DEITY_OFFER_LEVEL_MIN]) && level < rule[DEITY_OFFER_LEVEL_MIN])
    return 0;
  if (player && !undefinedp(rule[DEITY_OFFER_PLAYERS]) && !rule[DEITY_OFFER_PLAYERS])
    return 0;

  return 1;
}

/**
 * The first rule that takes coins, nil when the deity takes none.
 */
mapping query_coin_rule()
{
  int i;

  for (i = 0; i < sizeof(offerings); i++)
    if (offerings[i][DEITY_OFFER_KIND] == DEITY_OFFER_COINS)
      return offerings[i];

  return nil;
}

/**
 * The first rule that takes an item, nil when none does.
 */
mapping query_offering_rule(object item)
{
  mapping rule;
  int i;

  if (!item)
    return nil;

  for (i = 0; i < sizeof(offerings); i++)
  {
    rule = offerings[i];

    switch (rule[DEITY_OFFER_KIND])
    {
      // bodies and their parts are taken only by the rules made for them
      case DEITY_OFFER_ITEM:
        if (item->query_corpse() || item->query_body_part())
          continue;
        break;
      case DEITY_OFFER_CORPSE:
        if (!item->query_corpse() || !body_matches(rule, item))
          continue;
        break;
      case DEITY_OFFER_BODY_PART:
        if (!item->query_body_part() || !body_matches(rule, item))
          continue;
        break;
      case DEITY_OFFER_WEAPON:
        if (!item->query_weapon())
          continue;
        break;
      case DEITY_OFFER_ARMOUR:
        if (!item->query_armour())
          continue;
        break;
      case DEITY_OFFER_SHIELD:
        if (!item->query_shield())
          continue;
        break;
      default:
        continue;
    }

    if (stringp(rule[DEITY_OFFER_BASE]) &&
        rule[DEITY_OFFER_BASE] != base_name(item) + ".c")
      continue;

    if (stringp(rule[DEITY_OFFER_MATERIAL]) &&
        !material_matches(item->query_material(), rule[DEITY_OFFER_MATERIAL]))
      continue;

    if (intp(rule[DEITY_OFFER_MIN]) && value_of(item) < rule[DEITY_OFFER_MIN])
      continue;

    return rule;
  }

  return nil;
}

/**
 * What an offering of that value counts for under a rule.
 */
int query_offering_worth(mapping rule, int value)
{
  int worth;

  worth = (rule && intp(rule[DEITY_OFFER_WORTH]) && rule[DEITY_OFFER_WORTH])
        ? rule[DEITY_OFFER_WORTH] : 100;
  return value * worth / 100;
}

/** What the deity answers to an offering it takes. */
string query_offering_reply(mapping rule)
{
  if (rule && stringp(rule[DEITY_OFFER_REPLY]) && strlen(rule[DEITY_OFFER_REPLY]))
    return fill_text(rule[DEITY_OFFER_REPLY]);
  return fill_text(_LANG_DEITY_OFFERING_ACCEPTED);
}

// --- Prayers -----------------------------------------------------------------

/**
 * Adds or replaces a prayer. A new id needs a grant_prayer that knows it.
 *
 * @param id the prayer id
 * @param words what the player types to ask for it
 * @param luck the luck of the roll, the default when 0
 */
void add_prayer(string id, string * words, varargs int luck)
{
  prayers[id] = ([ DEITY_PRAYER_WORDS : words ? words : ({ }),
                   DEITY_PRAYER_LUCK  : luck ? luck : DEITY_PRAY_LUCK ]);
}

/** The deity will not hear that prayer. */
void remove_prayer(string id) { prayers = m_delete(prayers, id); }

void set_prayer_luck(string id, int luck)
{
  if (prayers[id])
    prayers[id][DEITY_PRAYER_LUCK] = luck;
}

mapping query_prayers() { return prayers; }

/** The prayer some words ask for, nil if none the deity hears. */
string query_prayer_for(string str)
{
  string * ids;
  int i;

  if (!str)
    return nil;

  str = lower_case(trim(str));
  ids = map_indices(prayers);
  for (i = 0; i < sizeof(ids); i++)
    if (member_array(str, prayers[ids[i]][DEITY_PRAYER_WORDS]) != -1)
      return ids[i];

  return nil;
}

/**
 * Whether some words ask for one of the default prayers this deity has
 * removed: it will not hear that one.
 */
int query_refused_prayer(string str)
{
  mapping words;
  string * ids;
  int i;

  if (!str)
    return 0;

  str = lower_case(trim(str));
  words = _LANG_DEITY_PRAYER_WORDS;
  ids = map_indices(words);
  for (i = 0; i < sizeof(ids); i++)
    if (!prayers[ids[i]] && member_array(str, words[ids[i]]) != -1)
      return 1;

  return 0;
}

/** The first word of every prayer, for the syntax of the pray skill. */
string * query_prayer_names()
{
  string * ids, * names;
  int i;

  names = ({ });
  ids = map_indices(prayers);
  for (i = 0; i < sizeof(ids); i++)
    if (sizeof(prayers[ids[i]][DEITY_PRAYER_WORDS]))
      names += ({ prayers[ids[i]][DEITY_PRAYER_WORDS][0] });

  return names;
}

int query_prayer_luck(string id)
{
  return prayers[id] ? prayers[id][DEITY_PRAYER_LUCK] : 0;
}

int query_pray_base() { return DEITY_PRAY_BASE; }
int query_pray_annoyed() { return DEITY_PRAY_ANNOYED; }
int query_pray_cursed() { return DEITY_PRAY_CURSED; }

/**
 * Gives the favour a heard prayer asked for. A deity with a prayer of its own
 * masks this and calls it for the rest.
 */
void grant_prayer(object caster, string id)
{
  int amount;

  switch (id)
  {
    case DEITY_PRAYER_HEAL:
      amount = caster->query_max_hp() / (10 + roll(1, 5));
      tell_object(caster, _LANG_DEITY_PRAYER_HEALED);
      caster->adjust_hp(amount, caster);
      break;

    case DEITY_PRAYER_POWER:
      amount = caster->query_max_gp() / (10 + roll(1, 5));
      tell_object(caster, _LANG_DEITY_PRAYER_POWERED);
      caster->adjust_gp(amount);
      break;

    case DEITY_PRAYER_BLESSING:
      amount = random(5) + 2;
      tell_object(caster, _LANG_DEITY_PRAYER_BLESSED);
      caster->adjust_tmp_con(amount);
      break;

    case DEITY_PRAYER_STRENGTH:
      amount = random(3) + 1;
      tell_object(caster, _LANG_DEITY_PRAYER_STRONG);
      caster->adjust_tmp_str(amount);
      break;

    case DEITY_PRAYER_WISDOM:
      amount = random(2) + 1;
      tell_object(caster, _LANG_DEITY_PRAYER_WISE);
      caster->adjust_tmp_wis(amount);
      caster->adjust_tmp_int(amount);
      break;
  }
}

/**
 * Answers a prayer: heard on a roll against a number that grows with every
 * recent prayer; past the annoyed mark the deity scolds, past the cursed one
 * it curses (hit points to one, a point less in every stat, two in charisma)
 * and the caster cannot pray for a while. Recorded in the deities handler.
 *
 * @return one of the DEITY_PRAYER_* answers of <deities.h>
 */
int answer_prayer(object caster, string id)
{
  object deities;
  int number, result;

  deities = handler("deities", caster);
  number = query_pray_base() + caster->query_time_remaining(DEITY_PRAY_PROP);

  if (number >= query_pray_annoyed() && number <= query_pray_cursed())
  {
    tell_object(caster, fill_text(_LANG_DEITY_PRAYER_SCOLDED));
    caster->add_timed_property(DEITY_PRAY_PROP, number, number);
    result = DEITY_PRAYER_SCOLDED;
  }
  else if (number > query_pray_cursed())
  {
    tell_object(caster, fill_text(_LANG_DEITY_PRAYER_CURSE));
    caster->set_hp(1);
    caster->adjust_tmp_con(-1);
    caster->adjust_tmp_str(-1);
    caster->adjust_tmp_wis(-1);
    caster->adjust_tmp_int(-1);
    caster->adjust_tmp_dex(-1);
    caster->adjust_tmp_cha(-2);
    caster->add_timed_property(DEITY_PRAY_CURSE_PROP, number, number);
    result = DEITY_PRAYER_CURSED;
  }
  else if (random(number) <= query_prayer_luck(id))
  {
    tell_object(caster, fill_text(_LANG_DEITY_PRAYER_HEARD));
    caster->add_timed_property(DEITY_PRAY_PROP, number + query_pray_base(),
                               number + query_pray_base());
    grant_prayer(caster, id);
    result = DEITY_PRAYER_HEARD;
  }
  else
  {
    tell_object(caster, fill_text(_LANG_DEITY_PRAYER_UNANSWERED));
    caster->add_timed_property(DEITY_PRAY_PROP, number, number);
    result = DEITY_PRAYER_UNANSWERED;
  }

  if (deities)
  {
    deities->record_prayer(caster, query_deity_path(),
                           result == DEITY_PRAYER_HEARD);
    if (result == DEITY_PRAYER_CURSED)
      deities->record_curse(caster, query_deity_path());
  }

  return result;
}

// --- The worshipper ----------------------------------------------------------

// Mask this and put your shadows in here, scope to this though!
// Every worshipper knows how to pray; given here, so a follower from before the
// skill existed gets it on the next login.
void start_player(object pl)
{
/*
   int i;
   if (!pointerp(job_commands))
      job_commands = ({ });
   for (i=0;i<sizeof(job_commands);i+=2)
      if (job_commands[i+1][0] || !interactive(pl))
         pl->add_known_command(job_commands[i]);
*/
  if (pl && userp(pl) && !pl->query_known_skill(SKILL_PRAY))
    pl->add_known_skill(SKILL_PRAY);
}

void on_death(object player, varargs object killer)
{
}

void on_kill(object player, object victim)
{
}

int player_quit(object player)
{
    return 1;
}
