/* *******************************************************
 *
 *  Effect system
 *
 *  Generic, applies to commands, spells, etc.
 *  neverbot@CcMud 27/11/2008
 *
 * ******************************************************* */

#include <living/effects.h>
#include <common/properties.h>
#include <living/skills.h>
#include <living/combat.h>
#include <translations/combat.h>
#include <translations/common.h>
#include <language.h>

inherit "/lib/effects/combat_fixes.c";
inherit "/lib/effects/guild_fixes.c";

string effect_name;       // effect name (spell, command, etc)
string * aliases;         // extra words that also invoke this effect in the
                          // current language (e.g. accent-less forms); empty
                          // when the language needs none
mapping categories;       // categories (spheres, schools, etc)
                          // ([ category_name : level_in_the_category ])
int is_passive;           // the effect is passive (runs nothing)
int effect_type;          // effect type (skill, spell, etc)
int combat_role_needed;   // required combat role (offensive, defensive, etc)
int gp_cost;              // gp cost (not applied to spells)
int lockout_time;         // minimum time between two runs of the same effect

string help_desc;         // the help text
string help_extras;       // extra help fields (shown before the description)
string * help_see_also;   // other skills or spells the help points at
string target_type;       // target type the effect can have
                          //   possible: TARGET_TYPE_NONE, TARGET_TYPE_ONE, TARGET_TYPE_MANY,
                          //             TARGET_TYPE_ALL, TARGET_TYPE_ITEM,
                          //             TARGET_TYPE_PASS, TARGET_TYPE_SELF, TARGET_TYPE_TOUCH
string effect_intent;     // whether the effect helps or harms its target
                          //   possible: EFFECT_INTENT_NONE, EFFECT_INTENT_HELP,
                          //             EFFECT_INTENT_HARM
int max_targets;          // how many targets a TARGET_TYPE_MANY effect takes
string* property_checks;  // properties that prevent the effect from running
// string* shadow_checks;    // functions checked to know if it has shadows
int effect_range;         // how many exits away a target that moves is still
                          // followed once the effect has started (0: none)
string effect_reach;      // reach id set by hand, see query_effect_reach
static int casting_at_level;  // set while cast_effect_at starts the effect
int silence;              // whether messages are shown on screen (useful for
                          // silent spells and similar)
int fast_casting;         // instant effects

string * start_phrases;   // messages shown at the start of the effect (first for the caster, second for the room)

// Effect permissions
int allow_on_self;        // can be cast on oneself
int allow_on_mounted;     // on mounted people
int allow_while_mounted;  // can we cast it while mounted
int allow_on_dead;        // on spirits
int allow_while_dead;     // if we are a spirit
int allow_on_passed_out;  // on objects with the passed_out property
int allow_on_pacified;    // on pacified people
int allow_while_pacified; // can we cast it while pacified
int allow_on_hidden;      // on hidden people
int allow_while_hidden;   // if we are hidden
int allow_on_moving;      // on a moving target
int allow_while_moving;   // if we are moving

// Permissions for the place the effect is cast from
int allow_on_water;
int allow_on_underground;
int allow_on_inside;
int allow_on_outside;

// Permissions depending on the current situation
int allow_in_combat;
int allow_only_in_combat;

/* The following can be either function names or plain strings.
 * If they are function names, those functions are called during the
 * corresponding casting round. If they are strings they are shown during
 * those rounds.
 * (Yes in both cases they are strings you idiot, but we check if they are
 * functions with function_exists).
 *
 * The function format must be:
 *    int function(object caster, mixed target, mixed out, int time, int quiet)
 *      - caster is the caster
 *      - target is 0, a single target object or an array of target objects.
 *        (all of them are within the spell range during this round)
 *      - out can be 0, a single target or an array of targets that are now
 *        outside the caster's range.
 *      - time is the current round number.
 *      - quiet is whether the effect is really being cast or not.
 *        ('no' means it is, e.g. a spell from a wand or similar).
 */
mixed * rounds;           // what happens during the different rounds


void create()
{
  // Default permissions
  allow_on_self = 1;
  allow_on_mounted = 1;
  allow_while_mounted = 0;
  allow_on_dead = 0;
  allow_while_dead = 0;
  allow_on_passed_out = 1;
  allow_on_pacified = 0;
  allow_while_pacified = 0;
  allow_on_hidden = 1;
  allow_while_hidden = 1;
  allow_on_moving = 1;
  allow_while_moving = 0;
  
  // Where the effect can be applied
  allow_on_water = 0;
  allow_on_underground = 1;
  allow_on_inside = 1;
  allow_on_outside = 1;
  
  allow_in_combat = 1;
  allow_only_in_combat = 0;
  
  is_passive = 0;
  
  effect_name = "test effect";
  aliases = ({ });
  categories = ([ ]);
  effect_type = EFFECT_IS_SKILL;
  gp_cost = 1;
  combat_role_needed = NEUTRAL_ROLE;
  
  help_desc = "Help";
  help_extras = "";
  help_see_also = ({ });
  target_type = TARGET_TYPE_NONE;
  effect_intent = EFFECT_INTENT_NONE;
  max_targets = EFFECT_DEFAULT_MAX_TARGETS;
  property_checks = 
    ({ 
      PROPERTY_IS_CASTING, 
      "LOCK " + base_name(this_object()),
     });
  // shadow_checks = ({ });
  lockout_time = 5;
  
  effect_range = 0;
  effect_reach = nil;
  silence = 0;
  fast_casting = 0;
  start_phrases = ({ });
 
  rounds = ({ });

  // euid is handled by the object base in hexagon (see lib/core/object.c);
  // the old "/secure/master"->creator_file() seteuid is not needed here.
  this_object()->setup();
}

void set_effect_name(string name) { effect_name = name; }
string query_effect_name() { return effect_name; }

int query_effect_type() { return effect_type; }
void set_effect_type(int t) { effect_type = t; }

void set_aliases(string * a) { aliases = a ? a : ({ }); }
string * query_aliases() { return aliases; }

string * query_categories() { return keys(categories); }

// the categories with the level the effect has in each
mapping query_category_levels() { return ([ ]) + categories; }

/**
 * The lowest level the effect has in any of its categories (1 when it has
 * none): the level it is priced by when no caster is at hand.
 */
int query_lowest_level()
{
  mixed * levels;
  int i, lowest;

  levels = map_values(categories);
  lowest = 0;
  for (i = 0; i < sizeof(levels); i++)
    if (!lowest || (int)levels[i] < lowest)
      lowest = (int)levels[i];

  return lowest < 1 ? 1 : lowest;
}

void set_categories(mapping str) { categories = map_copy(str); }
void add_category(string str, int level) { categories[str] = level; }

// Translate a category id to its display name: a skill category (English,
// see <living/skills.h>) from the translations, a sphere or school of spells
// from the game's spells table (/lib/tables/spells.c).
private string category_display(string id)
{
  string disp;
  object spells;

  disp = _LANG_SKILL_CATEGORIES[id];
  if (disp)
    return disp;

  spells = table("spells", this_object());
  return spells ? spells->query_spell_category_name(id) : id;
}

int query_combat_role_needed() { return combat_role_needed; }
void set_combat_role_needed(int value) { combat_role_needed = value; }
string query_combat_role_needed_string() { return COMBAT_ROLE_STRINGS[combat_role_needed]; }

void set_help_desc(string str) { help_desc = str; }
string query_help_desc() { return help_desc; }

void set_help_extras(string str) { help_extras = str; }
string query_help_extras() { return help_extras; }

void set_help_see_also(string * words) { help_see_also = words ? words : ({ }); }
string * query_help_see_also() { return help_see_also; }

void set_target_type(string str) { target_type = str; }
string query_target_type() { return target_type; }

/**
 * Declares whom the effect is meant for.
 * @param  intent  EFFECT_INTENT_HELP when it benefits its target, EFFECT_INTENT_HARM
 *                 when it hurts it, EFFECT_INTENT_NONE (the default) when
 *                 there is no target to judge
 */
void set_effect_intent(string intent) { effect_intent = intent; }

/**
 * Whether the effect helps or harms its target: whoever picks targets for it
 * (an NPC) chooses allies for a help and enemies for a harm.
 * @return one of the EFFECT_INTENT_* ids
 */
string query_effect_intent() { return effect_intent; }

/**
 * The most targets a TARGET_TYPE_MANY effect takes in one cast; the others the
 * caster names are dropped.
 * @param  n  at least 1
 */
void set_max_targets(int n) { max_targets = n < 1 ? 1 : n; }
int query_max_targets() { return max_targets; }

/*
void add_shadow_to_check(string str) { shadow_checks += ({ str }); }
string* query_shadow_checks() { return shadow_checks;}
*/

void add_property_to_check(string str) { property_checks += ({ str }); }
string* query_property_checks() { return property_checks;}

int query_range() { return effect_range; }
void set_range(int i) { effect_range = i; }

/**
 * Overrides the reach the effect would get from its target type and range:
 * a touch spell that uses TARGET_TYPE_ONE, an area one that names its targets,
 * a spell that hits several places.
 * @param  reach  one of the EFFECT_REACH_* ids
 */
void set_effect_reach(string reach) { effect_reach = reach; }

/**
 * Where the target must be when the effect starts: the id set by hand, or
 * else the one its target type says. A target is always looked for in the
 * caster's place, so only an effect that finds it elsewhere declares ranged.
 * @return one of the EFFECT_REACH_* ids
 */
string query_effect_reach()
{
  if (effect_reach)
    return effect_reach;

  switch (target_type)
  {
  case TARGET_TYPE_TOUCH:
    return EFFECT_REACH_TOUCH;
  case TARGET_TYPE_ONE:
  case TARGET_TYPE_MANY:
    return EFFECT_REACH_SINGLE;
  case TARGET_TYPE_ALL:
    return EFFECT_REACH_AREA;
  }

  return EFFECT_REACH_SELF;
}

/**
 * The reach as the player reads it.
 * @return the translated reach name
 */
string query_effect_reach_name()
{
  string reach, name;

  reach = query_effect_reach();
  name = _LANG_EFFECT_REACH_NAMES[reach];
  return name ? name : reach;
}

/**
 * How far a target that moves is followed, as the player reads it.
 * @return the translated distance, or the text for none
 */
string query_effect_follow_name()
{
  if (effect_range <= 0)
    return _LANG_EFFECT_FOLLOW_NONE;
  if (effect_range == 1)
    return _LANG_EFFECT_FOLLOW_ONE;
  return _LANG_EFFECT_FOLLOW_PRE + effect_range + _LANG_EFFECT_FOLLOW_POST;
}

int query_silence() { return silence; }
void set_silence(int i) { silence = i; }

int query_fast_casting() { return fast_casting; }
void set_fast_casting(int i) { fast_casting = i; }

int query_gp_cost() { return gp_cost; }
void set_gp_cost(int i) { gp_cost = i; }

int query_lockout_time() { return lockout_time; }
void set_lockout_time(int i) { lockout_time = i; }

mixed * query_rounds() { return rounds; }
void set_rounds(mixed * p)
{
    // If the number of strings passed differs from the number of rounds, it's a bug:
    /*
    if ( sizeof(p) != casting_time )
        tell_object(this_player(),"This effect has a bug, report it "
          "to a programmer.\n");
    */
    rounds = array_copy(p);
}

void set_start_phrases(string a, string b)
{
    start_phrases = ({a, b, });
}

/* Which of this effect's categories is the one in use.
 * By default we use the category the player has the highest rating in.
 *
 * Note for whoever has to change this some day:
 * caster->query_effect_category("name") returns the level the player has in
 *     that category (int) 0: none, 1: minor, 2: neutral, 3: major.
 * caster->query_effect_category_level("name") returns the level as a string
 *     ("minor", "major", ...).
 * When in doubt: lib/living/effects.c
 * neverbot 7/01
 */

string query_effect_category(varargs object caster)
{
    string cat;
    string * cat_names;
    int cat_lvl, cast_lvl, i;

    cat_names = ({ });
    cat_lvl = 0;
    cast_lvl = 10;

    if(!caster)
        caster = this_player();

    cat_names = keys(categories);

    // A skill has a single fixed category; the player-rating logic below
    // (which asks caster->query_effect_category(name)) is a spell concept
    // for spheres. Skip it for skills and return the category directly.
    if (this_object()->query_effect_type() == EFFECT_IS_SKILL)
        return sizeof(cat_names) ? cat_names[0] : _LANG_EFFECT_CATEGORY_NONE;

    for(i = 0; i < sizeof(cat_names);i++)
    {
        if(caster->query_effect_category(cat_names[i])>cat_lvl)
        {
            cat = cat_names[i];
            cat_lvl = caster->query_effect_category(cat_names[i]);
            cast_lvl = categories[cat];
        }
        
        if(categories[cat_names[i]]<cast_lvl && 
           caster->query_effect_category(cat_names[i])>cat_lvl)
        {
            cat = cat_names[i];
            cat_lvl = caster->query_effect_category(cat_names[i]);
            cast_lvl = categories[cat];
        }
        
        // Added for the physical-combat skills (old commands)
        if ((cat_names[i] == SKILL_TYPE_UNARMED) || (cat_names[i] == SKILL_TYPE_ARMED))
            return cat_names[i];
    }
    
    if(!cat) 
         return _LANG_EFFECT_CATEGORY_NONE;

    return cat;
}

/* Returns the effect's real level based on which category we use.
   We use the one query_effect_category returns, i.e. the one the player is highest in. */

int query_effect_level(varargs object caster)
{
    int clvl;
    mixed level;
    if(!caster) 
        caster = this_player();
    // a category the caster does not know is not in the mapping: level 1
    level = categories[this_object()->query_effect_category(caster)];
    clvl = intp(level) && level ? level : 1;
    return clvl;
}

/**
 * How strong the caster is in this effect: what the guild gives for this
 * spell alone (add_guild_spell) when it sets it, else how well the caster
 * knows the best of the effect's categories.
 * @return 0 none, 1 minor, 2 neutral, 3 major
 */
int query_effect_power(varargs object caster)
{
  string path;
  mixed given;

  if (!caster)
    caster = this_player();

  // a cast with a given power (a scroll) uses it instead of the caster's
  given = caster->query_timed_property(PROPERTY_CAST_POWER);
  if (intp(given) && given)
    return given;

  path = object_name(this_object());
  if (strlen(path) < 2 || path[strlen(path) - 2..] != ".c")
    path += ".c";

  given = caster->query_guild_spell_power(path);
  if (intp(given) && given)
    return given;

  return caster->query_effect_category(query_effect_category(caster));
}

/**
 * The gp a spell costs at a spell level and with a power (how well the caster
 * knows its category): the curve of the level, doubled for a minor power and
 * halved for a major one.
 */
int query_gp_cost_at(int level, int power)
{
  // Base cost per spell level (index level-1); levels beyond the curve fall
  // back to a linear cost. Skills never reach this -- they cost gp_cost.
  int * curve;
  int base;

  curve = ({ 2, 4, 6, 9, 13, 18, 23, 29, 35 });

  if (level >= 1 && level <= sizeof(curve))
    base = curve[level - 1];
  else
    base = 5 * level;

  // Adjust by the caster's knowledge of the effect's category.
  if (sizeof(query_categories()))
    switch (power)
    {
    case 1: return 2 * base;   // knows it poorly -> costs more
    case 3: return base / 2;   // knows it well  -> costs less
    }

  return base;
}

// Effective gp cost of this effect for a caster. Skills cost a fixed amount;
// spells scale with the effect level and are then surcharged or discounted
// by how well the caster knows the effect's category. Absorbed from the old
// effects_table, which was a single stateless formula rather than data.
int query_effect_gp_cost(varargs object caster)
{
  if (effect_type != EFFECT_IS_SPELL)
    return gp_cost;

  if (!caster)
    caster = this_player();

  return query_gp_cost_at(query_effect_level(caster), query_effect_power(caster));
}

/**
 * What the effect does to hit points, for the balance dump (the balance
 * command): a pure description of one cast, from the same numbers the cast
 * uses. No rolls, no messages, no change of state. An effect that neither
 * damages nor heals answers nil (this default); one that does overrides it and
 * builds the answer with make_balance. See lib/obj/skills/skills.md.
 *
 * @param  caster  who casts it; its level and power (PROPERTY_CAST_LEVEL and
 *                 PROPERTY_CAST_POWER) and stats are the ones the numbers follow
 * @return nil, or a mapping with the keys of make_balance
 */
mapping query_balance(object caster)
{
  return nil;
}

/**
 * Whether the effect asks the god of the caster before running. A base that
 * has the rule overrides it.
 */
int query_needs_faith() { return 0; }

/**
 * The names of the targeting data of the balance dump, in the order of its
 * columns. Each is a property the effect declares (see query_targeting).
 */
string * query_targeting_keys()
{
  return ({ "target_type", "intent", "max_targets", "reach", "start_range", "range", "lockout_time",
    "needs_faith", "needs_sight", "touch_protectable", "allow_on_self", "allow_on_mounted", "allow_on_dead",
    "allow_on_passed_out", "allow_on_pacified", "allow_on_hidden",
    "allow_on_moving", "allow_while_mounted", "allow_while_dead",
    "allow_while_pacified", "allow_while_hidden", "allow_while_moving",
    "allow_on_water", "allow_on_underground", "allow_on_inside",
    "allow_on_outside", "allow_in_combat", "allow_only_in_combat" });
}

/**
 * How many targets one cast takes, for the targeting data.
 * @return the maximum of a many-target effect, 1 for a single or touch target,
 *         0 when it takes the whole place or no target
 */
private int query_targets_per_cast()
{
  switch (target_type)
  {
  case TARGET_TYPE_MANY:
    return max_targets;
  case TARGET_TYPE_ONE:
  case TARGET_TYPE_TOUCH:
    return 1;
  }

  return 0;
}

/**
 * Everything the effect declares about whom it can reach and from where, read
 * from the effect itself (keys of query_targeting_keys):
 *   target_type  the TARGET_TYPE_* the effect declares
 *   intent       the EFFECT_INTENT_* id: whether it helps or harms its target
 *   max_targets  how many targets one cast takes: the maximum of a many-target
 *                effect, 1 for a single or touch target, 0 for the rest (the
 *                whole place, or no target)
 *   reach        the EFFECT_REACH_* id, see query_effect_reach
 *   start_range  exits away a target may be when the effect starts: 0 unless
 *                the reach is ranged, as targets are looked for in the
 *                caster's place
 *   range        exits away a target that moves is still followed once it has
 *                started, see query_range (0: it must stay in the place)
 *   lockout_time minimum time between two runs
 *   needs_faith  whether the god of the caster is asked
 *   needs_sight  whether a blinded caster cannot run it on another (the
 *                single and touch targets)
 *   touch_protectable  whether a protector of the target may take the touch
 *   allow_*      the permissions on the target, on the caster, on the place
 *                and on the combat
 *
 * @return key -> value
 */
mapping query_targeting()
{
  string reach;

  reach = query_effect_reach();

  return ([
    "target_type":          target_type,
    "intent":               effect_intent,
    "max_targets":          query_targets_per_cast(),
    "reach":                reach,
    "start_range":          reach == EFFECT_REACH_RANGED ? effect_range : 0,
    "range":                effect_range,
    "lockout_time":         lockout_time,
    "needs_faith":          this_object()->query_needs_faith(),
    "needs_sight":          target_type == TARGET_TYPE_ONE ||
                            target_type == TARGET_TYPE_TOUCH ||
                            target_type == TARGET_TYPE_MANY,
    "touch_protectable":    target_type == TARGET_TYPE_TOUCH,
    "allow_on_self":        allow_on_self,
    "allow_on_mounted":     allow_on_mounted,
    "allow_on_dead":        allow_on_dead,
    "allow_on_passed_out":  allow_on_passed_out,
    "allow_on_pacified":    allow_on_pacified,
    "allow_on_hidden":      allow_on_hidden,
    "allow_on_moving":      allow_on_moving,
    "allow_while_mounted":  allow_while_mounted,
    "allow_while_dead":     allow_while_dead,
    "allow_while_pacified": allow_while_pacified,
    "allow_while_hidden":   allow_while_hidden,
    "allow_while_moving":   allow_while_moving,
    "allow_on_water":       allow_on_water,
    "allow_on_underground": allow_on_underground,
    "allow_on_inside":      allow_on_inside,
    "allow_on_outside":     allow_on_outside,
    "allow_in_combat":      allow_in_combat,
    "allow_only_in_combat": allow_only_in_combat,
  ]);
}

/**
 * Builds the answer of query_balance. Every cast is assumed to land: each hit
 * rolls true and no target resists, unless the note says otherwise. The
 * targeting data (query_targeting) are added by this function, read from the
 * effect.
 *
 * @param  caster   who casts it
 * @param  kind     "damage" or "heal"
 * @param  cast_rounds  combat rounds from the start of the cast until the
 *                  last of the amount is delivered
 * @param  range    ({ minimum, average, maximum }) of what one target gets in
 *                  the whole cast, all rounds: ints, a float, an int
 * @param  type     the damage type, "" when none
 * @param  note     short free text ("per hit", "vs undead only")
 * @param  extra    optional: further numbers only this kind of effect has
 *                  (name -> int, float or string), each a column of the dump
 */
mapping make_balance(object caster, string kind, int cast_rounds,
                     mixed * range, string type, string note,
                     varargs mapping extra)
{
  int cost;

  // a spell is priced at its lowest category level, the one a caster who
  // knows that category best pays; a skill costs what it costs
  if (effect_type == EFFECT_IS_SPELL)
    cost = query_gp_cost_at(query_lowest_level(), query_effect_power(caster));
  else
    cost = gp_cost;

  // the rounding of each roll can push the scaled average off the bounds
  if ((float)range[1] < (float)(int)range[0])
    range[1] = (float)(int)range[0];
  if ((float)range[1] > (float)(int)range[2])
    range[1] = (float)(int)range[2];

  return query_targeting() + ([
    "kind":        kind,
    "rounds":      cast_rounds,
    "minimum":     (int)range[0],
    "average":     (float)range[1],
    "maximum":     (int)range[2],
    "damage_type": type,
    "cost":        cost,
    "note":        note,
    "extra":       extra ? extra : ([ ]),
  ]);
}

/**
 * The minimum, average and maximum of some dice and a fixed bonus.
 *
 * @return ({ minimum, average, maximum }), see make_balance
 */
mixed * dice_range(int dice, int faces, int bonus)
{
  // dice without faces roll nothing
  if (faces < 1 || dice < 1)
    return ({ bonus, (float)bonus, bonus });

  return ({ dice + bonus,
            (float)dice * (float)(faces + 1) / 2.0 + (float)bonus,
            dice * faces + bonus });
}

/**
 * A range repeated some times (the same amount delivered again and again).
 *
 * @return ({ minimum, average, maximum }), see make_balance
 */
mixed * repeat_range(mixed * range, int times)
{
  return ({ (int)range[0] * times, (float)range[1] * (float)times,
            (int)range[2] * times });
}

/**
 * A range adjusted the way the cast adjusts its amount (fix_amount of the
 * spell base, the caster's wisdom or intelligence): the bounds go through it as
 * they are, the average by the factor it gives, so the whole-number rounding
 * of each roll is not counted.
 *
 * @return ({ minimum, average, maximum }), see make_balance
 */
mixed * scale_range(object caster, mixed * range)
{
  int low, high;
  float factor;

  low = (int)this_object()->fix_amount(caster, (int)range[0]);
  high = (int)this_object()->fix_amount(caster, (int)range[2]);
  factor = (float)(int)this_object()->fix_amount(caster, 1000) / 1000.0;

  // a fixed amount has no average to scale
  if ((int)range[0] == (int)range[2])
    return ({ low, (float)low, high });

  return ({ low, (float)range[1] * factor, high });
}

// The effect's help text.
string help()
{
    string ret;

    ret = "\n";
    ret += _LANG_EFFECT_HELP_NAME + "%^BOLD%^" + capitalize(effect_name) + "%^RESET%^";

    if (!is_passive)
    {
      ret += _LANG_EFFECT_HELP_LEVEL + this_object()->query_effect_level();

      // Effect category: a skill has no player categories, a spell does.
      if (this_object()->query_effect_type() == EFFECT_IS_SKILL)
          ret += _LANG_EFFECT_HELP_CATEGORY + capitalize(category_display(this_object()->query_categories()[0]));
      else
        ret += _LANG_EFFECT_HELP_CATEGORY + capitalize(category_display(this_object()->query_effect_category()));

      ret += _LANG_EFFECT_HELP_REACH + query_effect_reach_name();
      if (target_type == TARGET_TYPE_MANY)
        ret += _LANG_EFFECT_HELP_MAX_TARGETS + max_targets;
      // only a single target can run away from the effect
      if (member_array(query_effect_reach(), ({ EFFECT_REACH_SINGLE, EFFECT_REACH_RANGED })) != -1)
        ret += _LANG_EFFECT_HELP_FOLLOW + query_effect_follow_name();

      ret += _LANG_EFFECT_HELP_COST + query_effect_gp_cost(this_player());

      if (combat_role_needed != NEUTRAL_ROLE)
        ret += _LANG_EFFECT_HELP_ROLE_NEEDED + COMBAT_ROLE_STRINGS[combat_role_needed];
    }
    else
    {
      // Effect category: a skill has no player categories, a spell does.
      if (this_object()->query_effect_type() == EFFECT_IS_SKILL)
          ret += _LANG_EFFECT_HELP_CATEGORY + capitalize(category_display(this_object()->query_categories()[0]));
      else
          ret += _LANG_EFFECT_HELP_CATEGORY + capitalize(category_display(this_object()->query_effect_category()));
    }

    ret += _LANG_EFFECT_HELP_DESC;
    if ( help_extras )
        ret += help_extras + "\n";
    ret += sprintf("  %-=*s\n", (this_user() ? this_user()->query_cols() - 2 : 77), query_help_desc());

    if (sizeof(help_see_also))
        ret += "\n" + _LANG_EFFECT_HELP_SEE_ALSO;

    ret += "\n";
    return ret;
}

int cast_effect(string str, object who, int quiet);
private int run_cast(string str, object * given, object who, int quiet);

/**
 * The level a cast "at a given level" carries (see cast_effect_at): 0 when
 * the caster is not casting that way. A spell reads it first, in the helpers
 * that feed its formulas the level of the caster, so the amounts and
 * durations follow the level it was given instead of the caster's own.
 */
int query_given_level(varargs object caster)
{
  mixed given;

  if (!caster)
    caster = this_player();

  given = caster->query_timed_property(PROPERTY_CAST_LEVEL);
  return intp(given) ? given : 0;
}

/**
 * What guilds teach this effect by: the spell's path (with .c) or the skill's
 * id (its file name without .c), as add_guild_spell and add_guild_skill take it.
 */
string query_effect_key()
{
  string path;
  string * parts;

  path = object_name(this_object());
  if (strlen(path) < 2 || path[strlen(path) - 2..] != ".c")
    path += ".c";

  if (query_effect_type() == EFFECT_IS_SPELL)
    return path;

  parts = explode(path, "/");
  path = parts[sizeof(parts) - 1];
  return path[0..strlen(path) - 3];
}

/**
 * Whether the guild the caster belongs to teaches this effect.
 */
private int is_taught_by_guild_of(object caster)
{
  string path;
  object guild;

  path = caster->query_guild_ob();
  if (!path)
    return 0;

  guild = nil;
  catch(guild = load_object(path));
  return guild && guild->query_teaches(query_effect_key());
}

/**
 * Whether some guild of the game teaches this effect. Those that none does
 * (hiding, climbing, the skills of a class) are generic, open to everyone.
 */
int query_guild_effect(object caster)
{
  object guilds;

  guilds = handler("guilds", caster);
  return guilds && guilds->query_taught(query_effect_key(), caster);
}

/**
 * Whether the caster may use the effect on its own: it is generic, or the
 * guild the caster belongs to now teaches it. A cast at a given level (a
 * scroll) does not ask: see cast_effect_at.
 */
int query_usable_by(object caster)
{
  if (is_taught_by_guild_of(caster))
    return 1;

  return !query_guild_effect(caster);
}

/**
 * The one level every guild spell and skill is fed: a given level first (a
 * scroll, an item), else the caster's level in the guild that teaches the
 * effect, which is the guild the caster belongs to now. A generic effect
 * (no guild teaches it) uses the guild level of the caster, or its level for
 * a monster with no guild. At least 1: an effect the caster may not use is
 * refused when cast (see query_usable_by) and never gets here, but one that
 * outlives the cast may still ask.
 * The level of a target (resist rolls) is not this one: it stays the class
 * level.
 * @param  caster  who uses the effect
 * @return 1 to 20
 */
int query_casting_level(object caster)
{
  int level;

  level = query_given_level(caster);
  if (level < 1)
    level = caster->query_guild_level();
  if (level < 1)
    level = caster->query_level();

  return level < 1 ? 1 : level;
}

/**
 * Starts the effect for a caster at a given level, as a scroll or a wand does:
 * it spends no gp, shows no begin messages (the item tells its own) and the
 * level is held on the caster, as a timed property, for as long as the rounds
 * of the effect last. A cast that does not start leaves nothing behind.
 *
 * @param  str    what the caster named (the target)
 * @param  who    the caster
 * @param  level  the guild level (1 to 20) the effect runs at
 * @param  power  the power it runs with (1 minor, 2 neutral, 3 major); 0 or
 *                none leaves the caster's own
 * @return 1 when the effect started, 0 when it was refused (notify_fail says why)
 */
int cast_effect_at(string str, object who, int level, varargs int power)
{
  string err;
  int ok;

  if (level < 1)
    level = 1;

  who->add_timed_property(PROPERTY_CAST_LEVEL, level, sizeof(rounds) + 3);
  if (power > 0)
    who->add_timed_property(PROPERTY_CAST_POWER, power > 3 ? 3 : power,
      sizeof(rounds) + 3);

  casting_at_level = 1;
  err = catch(ok = cast_effect(str, who, 1));
  casting_at_level = 0;

  if (err || !ok)
  {
    who->remove_timed_property(PROPERTY_CAST_LEVEL);
    who->remove_timed_property(PROPERTY_CAST_POWER);
  }

  if (err)
    error(err);

  return ok;
}

/**
 * Why a living cannot be the target of this effect, from the permissions the
 * effect declares (allow_on_*). A single and a many-target effect share it.
 *
 * @param  ob      who would be the target
 * @param  caster  who casts
 * @return the message that says why, or nil when it can
 */
private string target_refusal(object ob, object caster)
{
    if (!allow_on_self && caster == ob)
        return MSG_NO_SELF;

    if (!allow_on_mounted && ob->query_riding())
        return MSG_NO_ON_MOUNT;

    if (!allow_on_pacified && ob->query_pacify_effect())
        return _LANG_EFFECT_PROTECTED_TARGET_PRE + ob->query_cap_name() +
            _LANG_EFFECT_PROTECTED_TARGET_POST;

    if (!allow_on_passed_out && ob->query_property(PASSED_OUT_PROP))
        return _LANG_EFFECT_CANT_ON_PRE + ob->query_cap_name() +
            _LANG_EFFECT_CANT_ON_POST;

    if (!allow_on_hidden && ob->query_hidden())
        return MSG_NO_TARGET;

    return nil;
}

/**
 * Whom a many-target effect works on: the objects the caller hands over (an
 * NPC that already chose them) or else the names the caster typed, separated
 * by commas or by the word for "and" (the same reading as find_match, so "all"
 * names everybody). The targets must be living, distinct and valid for the
 * effect like those of a single-target one; the ones that are not, or that
 * cannot be found, are dropped with a message to the caster, and no more than
 * max_targets are kept (the first ones).
 *
 * @param  str     what the caster typed
 * @param  given   the targets chosen by the caller, empty to read str
 * @param  caster  who casts
 * @return the targets, empty when none is left
 */
private object * find_many_targets(string str, object * given, object caster)
{
    object * wanted, * found, * targets;
    string * names;
    string name, err;
    mixed exstr;
    int i, j;

    wanted = ({ });

    if (sizeof(given))
        wanted = given;
    else
    {
        names = explode(implode(explode(str, " " + _LANG_AND + " "), ","), ",");

        for (i = 0; i < sizeof(names); i++)
        {
            name = trim(names[i]);
            if (!strlen(name))
                continue;

            found = find_unique_match(name, environment(caster), !allow_on_dead);

            // items and exits may answer to a name too
            for (j = sizeof(found) - 1; j >= 0; j--)
                if (!living(found[j]))
                    found = delete(found, j, 1);

            if (!sizeof(found))
                tell_object(caster, _LANG_EFFECT_MANY_MISSING_PRE + name +
                    _LANG_EFFECT_MANY_MISSING_POST);
            else
                wanted += found;
        }
    }

    targets = ({ });
    for (i = 0; i < sizeof(wanted); i++)
    {
        // a repeated name or a nil from the caller
        if (!wanted[i] || member_array(wanted[i], targets) != -1)
            continue;

        err = target_refusal(wanted[i], caster);

        if (!err && function_exists("post_extra_checks", this_object()) &&
            stringp(exstr = this_object()->post_extra_checks(wanted[i], caster)))
            err = exstr;

        if (err)
            tell_object(caster, err);
        else
            targets += ({ wanted[i] });
    }

    if (sizeof(targets) > max_targets)
    {
        tell_object(caster, _LANG_EFFECT_MANY_TOO_MANY_PRE + max_targets +
            _LANG_EFFECT_MANY_TOO_MANY_POST);
        targets = targets[0..max_targets - 1];
    }

    return targets;
}

/**
 * Starts the effect on the targets the caller already chose, the entry point
 * for an NPC (a player names them in the text instead, see cast_effect).
 * Everything else is checked as in cast_effect.
 *
 * @param  targets  the living targets: a many-target effect takes up to
 *                  query_max_targets of them (the others are dropped), a single
 *                  or touch one the first; the rest of the target types ignore
 *                  them and look for theirs as ever
 * @param  who      the caster
 * @param  quiet    1 to show no begin messages
 * @return 1 when the effect started, 0 when it was refused (notify_fail says why)
 */
int cast_effect_on(object * targets, object who, varargs int quiet)
{
    return run_cast("", targets ? targets : ({ }), who, quiet);
}

/**
 * Starts the effect: every check, the search of the target and the rounds.
 * Whoever types a command goes through cast_effect, an NPC with its targets
 * already chosen through cast_effect_on.
 *
 * @param  str    what the caster named: the target, or the targets separated
 *                by commas or "and" for a many-target effect
 * @param  who    the caster
 * @param  quiet  1 to show no begin messages
 * @return 1 when the effect started, 0 when it was refused (notify_fail says why)
 */
int cast_effect(string str, object who, int quiet)
{
    return run_cast(str, ({ }), who, quiet);
}

// Main function, this is where we start running the effect.
private int run_cast(string str, object * given, object who, int quiet)
{
    mixed ob;        // effect target (find_match)
    mixed exstr;     // output of extra_checks (if it exists)
    object caster;   // the object casting the effect
    int i;
    int my_gp_cost;
    int at_level;    // started by cast_effect_at: no gp, at the given level
    string verb;     // what was typed, to ask what it was meant for
    string refusal;  // why the named target cannot be the target
    
    if (who) 
        caster = who;
    else 
        caster = this_player();

    at_level = casting_at_level;
    casting_at_level = 0;

    if (!environment(caster))
        return 0;

    // For whoever uses the inaudible effect
    if (caster->query_static_property(PROP_EFECTO_INAUDIBLE))
        quiet = 1;

    // If the effect is called from an object (wand, scroll) we call
    // cast_effect(...., 1) directly.
    if (silence)
        quiet = 1;

    if ( interactive(caster) )
    {
        string s;

        s = base_name(this_object());

        // An effect written in somebody's home directory is still being
        // worked on: only a coder or a tester (the "TEST" property) may fire
        // it. Everything that has landed in the mudlib is fair game.
        if ( strlen(s) > 6 && s[0 .. 5] == "/home/" &&
          !caster->query_coder() &&
          !caster->query_property(PROP_EFFECT_TESTER) )
        {
            notify_fail(_LANG_EFFECT_NOT_APPROVED);
            return 0;
        }
        str = (string)caster->expand_nickname(str);
    }

    if(caster->query_property(NOSKILL_PROP) ||
      environment(caster)->query_property(NOSKILL_PROP) ||
      caster->query_property(PASSED_OUT_PROP))
    {
        notify_fail(MSG_CANNOT_NOW);
        return 0;
    }

    // Uncomment the following if we allow multiple casting
    /*
    if ( (int)caster->query_active_effect(sphere) )
    {
        tell_object(caster,"You are already casting a spell of "+
          (sphere ? sphere : "") + ".\n");
        return 1;
    }
    */
        
    if ( (!allow_on_water && environment(caster)->query_in_water(caster)) ||
         (!allow_on_underground && environment(caster)->query_underground()) ||
         (!allow_on_inside && !environment(caster)->query_outside()) ||
         (!allow_on_outside && environment(caster)->query_outside()) )
    {
        notify_fail(MSG_WRONG_ENV);
        return 0;
    }
         
    // This must be commented out if we allow multiple casting
    if ( caster->query_active_effect("effect") )
    {
        notify_fail(MSG_BUSY);
        return 0;
    }

    if (!allow_in_combat && (sizeof(caster->query_attacker_list()) > 0) )
    {
        notify_fail(MSG_NO_COMBAT);
        return 0;
    }

    if (allow_only_in_combat && (sizeof(caster->query_attacker_list()) <= 0) )
    {
        notify_fail(MSG_ONLY_COMBAT);
        return 0;
    }

    // Check the required properties
    for ( i=0; i<sizeof(property_checks); i++)
        if ( caster->query_property(property_checks[i]) )
        {
            notify_fail(MSG_CANNOT_NOW);
            return 0;
        }

    if (!allow_while_mounted && caster->query_riding())
    {
      notify_fail(MSG_NO_RIDING);
      return 0;
    }
    
    if (!allow_while_pacified && caster->query_pacify_effect())
    {
      notify_fail(_LANG_EFFECT_OWN_AURA_BLOCKS);
      return 0;
    }

    if (!allow_while_hidden && caster->query_hidden())
    {
      notify_fail(_LANG_EFFECT_CANT_WHILE_HIDDEN);
      return 0;
    }

    // nothing named for an effect that needs something to act on
    if ((!str || !strlen(trim(str))) && !sizeof(given) &&
        (target_type == TARGET_TYPE_ITEM || target_type == TARGET_TYPE_ONE ||
         target_type == TARGET_TYPE_TOUCH || target_type == TARGET_TYPE_MANY))
    {
        verb = query_verb();
        if (!verb || !strlen(verb))
            verb = effect_name;

        notify_fail(target_type == TARGET_TYPE_ITEM ? MSG_WHICH_ITEM : MSG_WHICH_TARGET);
        return 0;
    }

    // Taniwha, prevent "spam" casting
    caster->add_timed_property(PROPERTY_IS_CASTING, 1, 1);

    // Custom extra checks: if the effect defines an "extra_checks" function,
    // call it and show its result on screen.

    if ( function_exists("extra_checks", this_object() ) &&
      ( stringp( exstr = this_object()->extra_checks(str, caster) ) ) )
    {
        notify_fail(exstr);
        return 0;
    }

    // a guild effect is only usable inside a guild that teaches it; a scroll
    // or an item casts at its own level, whoever reads it
    if (!at_level && !query_usable_by(caster))
    {
        notify_fail(MSG_NOT_TAUGHT);
        return 0;
    }

    // Look for targets
    ob = 0;

    if (( target_type == TARGET_TYPE_ONE ) || ( target_type == TARGET_TYPE_TOUCH ))
    {
        // a caller that already chose its target hands it over
        if (sizeof(given))
          ob = given[0];
        else
          ob = find_one_match( str, environment(caster), !allow_on_dead);
        
        if (ob)
        {
          refusal = target_refusal(ob, caster);
          if (refusal)
          {
            notify_fail(refusal);
            return 0;
          }
        }

        // if the effect type is touch and the victim is being protected,
        // the target may change
        if ( target_type == TARGET_TYPE_TOUCH && objectp(ob) )
            ob = check_melee_protector(ob, caster);

        if (!ob)
        {
            notify_fail(MSG_NO_TARGET);
            return 0;
        }
    }
    else if ( target_type == TARGET_TYPE_MANY )
        ob = find_many_targets(str, given, caster);

    else if ( target_type == TARGET_TYPE_ITEM )
    {      
        /*
        string str1, str2;  // name of who to search in, and what
        object inwho;       // who to search the item in

        // check if we typed run xxx <item> in <name>
        if(strsrch(str," en ") >= 0) 
        {
            sscanf(str,"%s en %s",str1,str2);
            if(stringp(str2))
            {
                inwho = present(str2,environment(caster));
                if(inwho)
                {
                   ob = present(str1,inwho);
                }
            }
        }
        if(!ob)
        {
            ob = find_one_match( str, caster );
        }
        if(!ob)
            ob = find_one_match( str, environment(caster), !on_ghost);
        */
        
        // effects cannot be run on items in other characters' inventories,
        // only on your own inventory or the caster's environment
        
        if (sizeof(given))
            ob = given[0];
        else
            ob = find_one_match( str, caster );

        if (!ob)
            ob = find_one_match( str, environment(caster), !allow_on_dead);

        if (!ob)
        {
            notify_fail(MSG_NO_ITEM);
            return 0;
        }

        if (ob->query_npc() || ob->query_player())    
        {
            notify_fail(MSG_ON_ITEMS);
            return 0;
        }    
    }

    else if ( target_type == TARGET_TYPE_PASS )
        ob = str;
    else if ( target_type == TARGET_TYPE_SELF )
        ob = caster;

    // If ob is empty, we could not find the effect's target
    if ( ( (target_type == TARGET_TYPE_ONE || target_type == TARGET_TYPE_ITEM ||
          target_type == TARGET_TYPE_TOUCH) && !ob ) || 
      (target_type == TARGET_TYPE_MANY && !sizeof(ob) ) )
    {
        notify_fail(MSG_NO_TARGET);
        return 0;
    }

    // If we are blinded, we can only run a one/touch effect on ourselves.
    if(caster->query_timed_property(PROPERTY_BLINDED) &&
      (target_type == TARGET_TYPE_ONE || target_type == TARGET_TYPE_TOUCH)&& 
      // (effect_range > 0) &&
      (ob != caster) )
    {
        notify_fail(MSG_NO_BLIND);
        return 0;
    }

    // the same for a many-target one: nobody but ourselves
    if (caster->query_timed_property(PROPERTY_BLINDED) &&
      target_type == TARGET_TYPE_MANY && sizeof(ob - ({ caster })))
    {
        notify_fail(MSG_NO_BLIND);
        return 0;
    }

    // Custom extra checks: if the effect defines a "post_extra_checks"
    // function, call it and show its result on screen.

    if ( function_exists("post_extra_checks", this_object() ) &&
      ((target_type == TARGET_TYPE_ONE) || (target_type == TARGET_TYPE_SELF)) && 
      ( stringp( exstr = this_object()->post_extra_checks(ob, caster) ) ) )
    {
        notify_fail(exstr);
        return 0;
    }

    /* Added by neverbot 3/2002, to try to stop the shadow effects from */
    /*  crashing all day */
    /* Check the required properties */

    // This is horrendous, removing it again, neverbot 11/2008
    /*
    if ((target_type == TARGET_TYPE_ONE) || (target_type == TARGET_TYPE_SELF))
        for (i = 0; i < sizeof(shadow_checks); i++)
            if ( call_other(ob, "query_aura_"+shadow_checks[i] ) )
            {
                notify_fail("You cannot invoke that effect right now, the " +
                  "effect of some other effect prevents it.\n");
                return 0;
            }

    */
    my_gp_cost = at_level ? 0 : query_effect_gp_cost(caster);

    if (caster->query_gp() < my_gp_cost)
    {
        notify_fail(MSG_NO_GPS);
        return 0;
    }

    // Combat roles, neverbot 01/2013
    if (combat_role_needed != NEUTRAL_ROLE)
    {
      int current_caster_role, last_time;

      current_caster_role = caster->query_combat_role();
      last_time = caster->query_static_property(COMBAT_ROLE_CHANGED_PROP);

      // hasn't changed role since connecting (on connect all players
      // default to DPS_ROLE)
      if (!last_time)
      {
        caster->add_static_property(COMBAT_ROLE_CHANGED_PROP, time());
        if (current_caster_role != combat_role_needed)
          caster->change_combat_role(combat_role_needed);
      }
      // If we have to change the combat role
      else if (current_caster_role != combat_role_needed)
      {
        // We don't allow changing role in less than five minutes
        if (time() - last_time < 300)
        {
          notify_fail(_LANG_EFFECT_ROLE_BLOCKS);
          return 0;
        }
        // We can change it because enough time has passed
        else
        {
          caster->add_static_property(COMBAT_ROLE_CHANGED_PROP, time());
          caster->change_combat_role(combat_role_needed);        
        }
      }
    }

    /* Uncomment this if we allow multiple casting. */
    /*
     * if ( sphere )
     *   caster->add_effect_effect(casting_time, sphere, effect_name,
     *                            this_object(), "hb_effect",
     *                            ({ ob, quiet, environment(caster), my_gp_cost }));
     * else
     *  tell_object(caster, "This effect has a bug, report it to a God.\n");
     */

    // a normal cast does not inherit the level a previous cast was given
    if (!at_level)
    {
        caster->remove_timed_property(PROPERTY_CAST_LEVEL);
        caster->remove_timed_property(PROPERTY_CAST_POWER);
    }

    // This prevents casting other effects at the same time, and stores in the
    // player the effect to run when it calls its do_active_effects() function.
    caster->add_active_effect(sizeof(rounds), "effect", effect_name,
      this_object(), "hb_effect", 
      ({ ob, quiet, environment(caster), my_gp_cost })   );

    // We pass the gp to spend to add_active_effect; we spend it on the
    //   last round of the effect if all went well.

    // If quiet != 0, show the "you begin to run..." message
    if( !quiet && (effect_type == EFFECT_IS_SPELL))
    {
        tell_object(caster, _LANG_EFFECT_START_SPELL_PRE + effect_name +
          _LANG_EFFECT_START_SPELL_POST);
        tell_room(environment(caster), SPE_INFO +
          caster->query_cap_name() + _LANG_EFFECT_START_SPELL_ROOM,
          caster);
    }
    else if ( !quiet && (effect_type == EFFECT_IS_SKILL))
    {
        if (sizeof(start_phrases) == 2)
        {
			if (start_phrases[0] != "")
            	tell_object(caster, SPE_INFO + start_phrases[0] + "\n");
			
			if (start_phrases[1] != "")
              tell_room(environment(caster), SPE_INFO +
				caster->query_cap_name() + " " + start_phrases[1] + "\n", caster);            
        }
        else
            tell_object(caster, SPE_INFO + _LANG_EFFECT_START_SKILL_PRE +
              effect_name + _LANG_EFFECT_START_SKILL_POST);
    }
    
    // else tell_object(caster,"You begin to chant, but the words seem to "
    //                         "vanish as they leave your lips.\n");

    // We stored the effect in the player as "effect" with the
    // "hb_effect" function.

    // Fast casting (if the effect is 'fast_casting' we always do it, though
    // there is a random chance it applies to any effect)
    if( fast_casting || 
      ((caster->query_level() + 10) > random(100)) )
        caster->do_active_effects(nil);

    return 1;
}

int hb_effect(object caster, mixed *params, int time)
{
    /* params: [0] target, [1] quiet, [2] environment(caster), */

    int which_round;    /* current round */
    mixed target;
    mixed out_of_range; /* where we store target when it goes out of range */
    object * kept;      /* the targets of a many-target effect still in range */
    mixed mess;         /* message or function of the current round */
    int quiet;
    int ret;
    int i;

    // Checks whether we should stop running the effect:
    if(caster->query_property(NOSKILL_PROP) ||
      environment(caster)->query_property(NOSKILL_PROP) ||
      caster->query_dead() ||
      caster->query_property(PASSED_OUT_PROP))
    {
        tell_object(caster, MSG_NO_CONTINUE);
        call_out("end_effect_now", 0, caster);
        return 0;
    }
    
    if (!allow_in_combat && (sizeof(caster->query_attacker_list()) > 0) )
    {
        tell_object(caster, MSG_NO_COMBAT);
        return 0;
    }

    if (allow_only_in_combat && (sizeof(caster->query_attacker_list()) <= 0) )
    {
        tell_object(caster, MSG_ONLY_COMBAT);
        return 0;
    }
    
    // If we move, end the effect
    if ( !allow_while_moving &&
         //(target_type != TARGET_TYPE_NONE) && 
         (environment(caster) != params[2]) )
    {
        caster->adjust_gp(-1);
        tell_object(caster, MSG_NO_MOVE);
        call_out("end_effect_now", 0, caster);
        return 0;
    }

    which_round = sizeof(rounds) - time;
    target = params[0];
    quiet = params[1];
    mess = rounds[which_round];

    // First we do range checking.  What fun.
    switch( target_type )
    {
    case TARGET_TYPE_PASS:
    case TARGET_TYPE_NONE:
        out_of_range = 0;
        break;
        
    case TARGET_TYPE_ITEM:
        out_of_range = 0;
        
        // Check the object hasn't moved
        if ( ( target &&
              (environment(target) != caster) && 
              (environment(target) != environment(caster)) ) ||
             (!target) )
        {
            target = 0;
            tell_object(caster, MSG_DISSAPPEAR);
            call_out("end_effect_now", 0, caster);
            return 0;
        }
        break;

    case TARGET_TYPE_TOUCH:
        effect_range = 0;
        
    case TARGET_TYPE_ONE:
        out_of_range = 0;
        
        if (!target)
        {
            tell_object(caster, MSG_DISSAPPEAR);
            call_out("end_effect_now", 0, caster);
            return 0;
        }
        else
        {
            // If it is not a living target, we lose it
            if ( !living(target) )
            {
                out_of_range = 0;
                target = 0;
                tell_object(caster, MSG_NO_ITEMS);
                tell_room(environment(caster), MSG_CANNOT_NOW_ROOM, caster);
                call_out("end_effect_now", 0, caster);
                return 0;
            }
            // Check it is within range: without one, it must still be here
            else if ((effect_range <= 0 && environment(target) != environment(caster)) ||
                     (effect_range > 0 && !RANGE_HANDLER->check_in_range( caster, target, effect_range )))
            {
                out_of_range = target;
                target = 0;
                tell_object(caster, MSG_NO_RANGE);
                tell_room(environment(caster), MSG_CANNOT_NOW_ROOM, caster);
                call_out("end_effect_now", 0, caster);
                return 0;
            }
        }
        break;

    case TARGET_TYPE_MANY:
        /*** IMPORTANT CHANGE (neverbot 7/2001)
             We don't allow many-target effects; either we have single-target
             effects or we affect every character in the
             habitacion. ***/
        // Restored 10/2026: an effect names up to max_targets distinct
        // targets, each one valid and in range like a single target; the
        // area effects (TARGET_TYPE_ALL) stay everybody in the place.
        out_of_range = ({ });
        kept = ({ });

        for (i = 0; i < sizeof(target); i++)
        {
            // Remove vanished and non-living targets
            if (!target[i] || !living(target[i]))
                continue;

            // Check each one is within range: without one, it must still be here
            if ((effect_range <= 0 && environment(target[i]) != environment(caster)) ||
                (effect_range > 0 && !RANGE_HANDLER->check_in_range(caster, target[i], effect_range)))
            {
                out_of_range += ({ target[i] });
                tell_object(caster, _LANG_EFFECT_MANY_GONE_PRE +
                    target[i]->query_cap_name() + _LANG_EFFECT_MANY_GONE_POST);
            }
            else
                kept += ({ target[i] });
        }

        // the ones lost stay lost in the rounds that follow
        target = kept;
        params[0] = kept;

        // If we ran out of targets, end:
        if (!sizeof(target))
        {
            tell_object(caster, MSG_DISSAPPEAR);
            tell_room(environment(caster), MSG_CANNOT_NOW_ROOM, caster);
            call_out("end_effect_now", 0, caster);
            return 0;
        }
        break;

    case TARGET_TYPE_ALL:
        out_of_range = ({ });
        target = all_inventory(environment(caster));
        
        if ( !target )
            target = ({ });

        for ( i = sizeof(target)-1; i >= 0; i-- )
        {
            // Remove non-living targets
            if ( !living(target[i]) )
            {
                target = delete(target, i, 1);
            }
            else if (!RANGE_HANDLER->check_in_range(caster, target[i], effect_range))
            {
                // Out-of-range targets go into out_of_range
                out_of_range += ({ target[i] });
                target = delete(target, i, 1);
            }
        }

        if(!sizeof(target))
        {
            tell_object(caster, MSG_DISSAPPEAR);
            tell_room(environment(caster), MSG_CANNOT_NOW_ROOM, caster);
            call_out("end_effect_now", 0, caster);
            return 0;
        }
        break;

    case TARGET_TYPE_SELF:
        target = caster;
        if(!target)
            target = ({ });
        break;


    default:
        tell_object(this_player(), "[effect type] " + MSG_ERROR);
        call_out("end_effect_now", 0, caster);                    
        return 0;
    }

    // Added the "invulnerable_efectos" checks from area 23, neverbot 2001
    // Check the target is not invulnerable to the effect.
    /*
    if (pointerp(target))
    {
        for (i = sizeof(target) - 1; i >= 0; i--)
            if (target[i]->query_property(PROPERTY_INVUL) >= query_effect_level(caster))
            {
                tell_object(caster, target[i]->query_cap_name()+" is invulnerable to your effect.\n");
                tell_object(target[i],caster->query_cap_name()+"'s effect has no " +
                  "effect on you!\n");
                tell_room(environment(caster),caster->query_cap_name()+"'s effect has no " +
                  "effect on "+target[i]->query_cap_name()+"!\n",({caster,target[i]}));
                target = delete(target, i, 1);

                if (caster->query_coder())
                    tell_object(caster, "DEBUG: "+target[i]->query_cap_name() +
                      " 'invulnerable_efectos': ("+
                      target[i]->query_property(PROPERTY_INVUL)+
                      ") Effect level: ("+query_effect_level(caster)+")\n");
            }
        if (!sizeof(target))
        {
            tell_object(caster,"Your effect seems to have run out of targets.\n");
            tell_room(environment(caster),caster->query_cap_name()+" stops running an effect.\n", caster);
            call_out("end_effect_now",0,caster);
            return 0;
        }
    }
    else if (objectp(target))
    {
        if (target->query_property(PROPERTY_INVUL) >= query_effect_level(caster))
        {
            if (target == caster)
                tell_object(caster,"You are invulnerable to your own effect.\n");
            else
            {
                tell_object(caster,target->query_cap_name()+" is invulnerable to your effect.\n");
                tell_object(target,caster->query_cap_name()+"'s effect has no " +
                  "effect on you!\n");
                tell_room(environment(caster),caster->query_cap_name()+"'s effect has no " +
                  "effect on "+target->query_cap_name()+"!\n",({caster,target}));
            }
            target = 0;

            if (caster->query_coder())
                tell_object(caster, "DEBUG: "+caster->query_cap_name() +
                  " 'invulnerable_efectos': ("+
                  caster->query_property(PROPERTY_INVUL)+
                  ") Effect level: ("+query_effect_level(caster)+")\n");
        }
        if (!target)
        {
            tell_object(caster,"Your effect seems to have run out of targets.\n");
            tell_room(environment(caster),caster->query_cap_name()+" stops running an effect.\n",caster);
            call_out("end_effect_now",0,caster);
            return 0;
        }
    }
    */

    // Now we call the functions or whatever we have.
    if ( stringp(mess) )
    {
        if ( function_exists( mess, this_object() ) )
        {
            // if the string is a function name, we call it
            ret = call_other(this_object(), mess, caster, target,
              out_of_range, which_round, quiet);
        }
        else
        {
            // the string is a message for the caster object.
            if ( !quiet )
                tell_object(caster, mess);
            ret = 1;
        }
    }
    else if ( pointerp( mess ) )
    {
        // if the current message is a pair ({ "message 1", "message 2" }), we
        //   have one message for the caster and another for the target
        ret = 1;
        if ( quiet ) 
            return ret;
            
        if ( sizeof(mess) >= 2 )
        {
            // one to caster, one to room
            tell_object(caster, mess[0]);
            tell_room(environment(caster), mess[1], caster);
        }
        else
        {
            tell_room(environment(caster), "[message count] " + MSG_ERROR);
        }
    }
    else 
    {
      tell_object(this_player(), "[round check] " + MSG_ERROR);
    }
    
    return ret;
}

void end_effect_now(object ob)
{
    if (!ob)
        ob = this_player();
    ob->remove_active_effect(effect_name);
}

/**
 * The id of this effect's lasting effect on a living: its file, the same in
 * every language.
 */
string query_lasting_id() { return base_name(this_object()); }

/**
 * Starts this effect's lasting effect on the caster: lasting_tick runs every
 * heart_beat for ticks heart_beats, then lasting_end. Starting it again
 * replaces the one in force.
 *
 * @param  data  whatever the ticks need (a target, a bonus); shared by
 *               reference with every tick
 */
int start_lasting(object caster, int ticks, mixed data)
{
  return caster->add_lasting_effect(query_lasting_id(), this_object(),
    LASTING_TICK_FUNC, ticks, data);
}

/**
 * Ends this effect's lasting effect on the caster (LASTING_END_REMOVED).
 */
int stop_lasting(object caster)
{
  return caster->remove_lasting_effect(query_lasting_id());
}

/**
 * Whether this effect's lasting effect is in force on the caster.
 */
int query_lasting(object caster)
{
  return caster->query_lasting_effect(query_lasting_id()) != nil;
}

/**
 * One tick of the lasting effect; an effect masks it. Returning 0 ends it.
 *
 * The default does nothing until the ticks run out, enough for an effect that
 * only lasts and undoes itself in lasting_end.
 */
int lasting_tick(object caster, mixed data, int ticks_left) { return 1; }

/**
 * Called when the lasting effect ends, reason being one of LASTING_END_*; an
 * effect masks it to give its own message and undo what it did.
 */
void lasting_end(object caster, string id, mixed data, string reason) { }
