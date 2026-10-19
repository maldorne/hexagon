/* 
 * Reviewed and touched up for CcMud
 * Removed skills and weapon specializations from this system
 * neverbot 13/7/03
 *
 * Added system for guild level advancement through quests,
 * neverbot 29/07/2010
 */

#include <living/effects.h>

inherit "/lib/core/object.c";
// the skills and spells it teaches or relates, shared with the jobs
inherit "/lib/social/abilities.c";
// Quest system, not in hexagon yet: neither /global/basic/quests.c nor
// <quests.h> exist here, so everything below that needs a quest is kept
// commented until they do.
// inherit "/global/basic/quests.c";

// #include <quests.h>

#define DEFAULT_COST 500
#define DEFAULT_XP_COST 4500

// string *guild_commands;
// race and class ids, see <living/races.h> and <living/classes.h>
string *legal_races, *legal_classes;
string requirements_func;
int needed_ext_align;
string *specs_available;
// the categories of the spells its members cast, -> EFFECT_CAT_* (how well
// they know each, which makes its spells cheaper or dearer): spheres of divine
// magic (clerics, paladins) and schools of arcane magic (mages, sorcerers)
mapping spheres, schools;
// the markdown text of the guild's rules, see set_rules_book
string rules_book;

nomask int query_is_guild() { return 1; }

void create() 
{
  // guild_commands = ({ });
  legal_races = ({ });
  legal_classes = ({ });
  specs_available = ({ });
  create_abilities();
  spheres = ([ ]);
  schools = ([ ]);
  needed_ext_align = 0;
  requirements_func = "";
  rules_book = "";
  // quests::create(); // before object, or setup will give errors due to uninitialized mappings!!
  ::create();
}

string query_requirements_func() { return requirements_func; }
void set_requirements_func(string func) { requirements_func = func; }

/**
 * What the requirements function checks, in words, for the normas of the
 * guild (/lib/handlers/rules.c).
 *
 * A guild that names a requirements function masks this beside it, so the
 * check and its description are read together; nil says nothing more.
 */
string query_requirements_text() { return nil; }

/**
 * Names the book of the guild's rules: the full path of a markdown text, as a
 * library shelves it (see /lib/handlers/books.c). normas shows it after what
 * the guild object says of itself, so the book and normas never disagree.
 */
void set_rules_book(string path) { rules_book = path ? path : ""; }
string query_rules_book() { return rules_book; }

// To check alignments
// 0 does not mean neutral, it means the alignment does not matter!!!!
// Values:
// 0: does not matter
// 1: neutral
// 100: good
// -100: evil
int query_needed_ext_align() { return needed_ext_align; }
void set_needed_ext_align(int bing) { needed_ext_align = bing; }

int query_valid_align(int align)
{
  switch(needed_ext_align){
    case 1:
       // the neutral band of the alignment table: 0..100
       return (align >= 0 && align <= 100);
       break;
    case 100:
       return (align > 100);
       break;
    case -100:
       return (align < 0);
       break;
    case 0:
    default:
       return 1;
       break;
  }
}

// To check races
string * query_legal_races() { return legal_races; }
void set_legal_races(string * list) { legal_races = list; }

// A guild that names no races admits every one of them. Naming some is how a
// guild closes itself to the rest -- the drow orders do, the open ones do not
// -- so the empty list has to mean "anyone", or a guild would have to
// enumerate every people in the world just to stay open to it.
int query_legal_race(string race)
{
  if (!sizeof(legal_races))
    return 1;
  if (member_array(race, legal_races) == -1)
    return 0;
  return 1;
}

// To check classes
string * query_legal_classes() { return legal_classes; }
void set_legal_classes(string * list) { legal_classes = list; }

// Same rule as the races above: no list is no restriction.
int query_legal_class(string name)
{
  if (!sizeof(legal_classes))
    return 1;
  if (member_array(name, legal_classes) == -1)
    return 0;
  return 1;
}

int query_legal_weapon(string weapon_name) { return 1; }
int query_legal_armour(string weapon_name) { return 1; }

/**
 * Whether a member may cast spells while wearing this piece (asked of every
 * worn piece when casting). The default judges only the kind of armour, as
 * query_legal_armour does; a guild that cares what the piece is made of, or
 * which piece it is, masks this.
 * @param  armour  the worn piece
 */
int query_legal_casting_armour(object armour)
{
  return query_legal_armour((string)armour->query_armour_type_name());
}
/*
int query_valid_melee_weapon(object ob) { return 1;} 
int query_number_worn(string str) { return 1; }
string query_spell_directory() { return 0; }
mixed query_legal_spheres() { return ({ }); }
*/

int query_legal_player(object player)
{  
  if (!objectp(player) || !player)
    return 0;
  // We check race, class and external alignment. A race named in the list
  // admits all of its lineages, a lineage named admits only that lineage.
  if (!query_legal_race(player->query_race_id()) &&
      !query_legal_race(player->query_base_race_id()))
    return 0;
  if (!query_legal_class(player->query_class_id()))
    return 0;
  if (!query_valid_align(player->query_ext_align()))
    return 0;
    
  // an extra requirement the guild may name for itself; with none, race,
  // class and alignment were the whole test
  if (strlen(requirements_func) &&
      function_exists(requirements_func, this_object()))
    return call_other(this_object(), requirements_func, player);

  return 1;
}

// string help() { return read_file("/doc/helpdir/guilds"); }
int query_channel() { return 1; }

int query_next_level_xp(object player)
{  
  int res;

  res = player->query_guild_level() * DEFAULT_XP_COST;
  if (res == 0)
     return DEFAULT_XP_COST;
  return res;
}

/* Simply how much money the guild will charge to train you  */
int query_advance_cost() { return DEFAULT_COST; }
/* Seems we won't use this one, but check *bits of skills instead */
// int query_xp_cost() { return DEFAULT_XP_COST; }
int query_max_level() { return 20; }
int query_extra_gp_bonus() { return 0; }

/*
// Guild commands:
string *query_guild_commands() { return guild_commands; }

int add_guild_command(string name, int add_if_player) {
  if (member_array(name, guild_commands) != -1)
    return 0;
  guild_commands += ({ name, ({ add_if_player }) });
  return 1;
}

int remove_guild_command(string name) {
  int i;
  if ((i = member_array(name, guild_commands)) == -1)
    return 0;
  guild_commands = delete(guild_commands, i, 1);
  return 1;
}
*/

/**
 * Teaches a skill to the members of the guild, from a guild level on.
 *
 * Call it from the guild's setup. Members get it when they join or reach the
 * level (see grant_guild_skills) and keep it when they leave, but they cannot
 * use it outside a guild or job that allows it (teaches it or relates it, see
 * add_guild_related_skill), unless it is a common effect: the effect refuses
 * the use (see query_usable_by in /lib/effects/base_effect.c). Inside one it
 * runs at the member's guild level.
 *
 * @param  id       the skill id, see <living/skills.h>
 * @param  level    guild level needed to know it
 * @param  ability  least value the member has in it when taught (an ability
 *                  for a percentage skill, a level for a levelled one)
 */
void add_guild_skill(string id, int level, varargs int ability)
{
  add_ability_skill(id, level, ability);
}

/**
 * The skills the guild teaches: id -> ({ guild level needed, least ability }).
 */
mapping query_guild_skills() { return query_ability_skills(); }

/**
 * Ties a skill to the guild without teaching it: members may use it, but it
 * is not offered by aprender, not granted on joining or levelling and not
 * listed by query_guild_skills (it is learned through quests, items...).
 * @param  id  the skill id, compared exactly
 */
void add_guild_related_skill(string id) { add_related_ability_skill(id); }

/**
 * Ties a spell to the guild without teaching it, as add_guild_related_skill
 * does for a skill.
 * @param  path  the spell object, full path with .c, compared exactly
 */
void add_guild_related_spell(string path) { add_related_ability_spell(path); }

/**
 * The ids of the skills tied to the guild but not taught by it.
 */
string * query_guild_related_skills() { return query_related_ability_skills(); }

/**
 * The paths of the spells tied to the guild but not taught by it.
 */
string * query_guild_related_spells() { return query_related_ability_spells(); }

/**
 * Gives a member every skill of the guild their guild level allows.
 *
 * Idempotent: a skill already known is left as it is, only raised to the
 * least value the guild promises.
 */
void grant_guild_skills(object player)
{
  grant_ability_skills(player, player->query_guild_level());
}

/**
 * Teaches a spell to the members of the guild, from a guild level on.
 *
 * Call it from the guild's setup. Members learn it with aprender in the guild
 * room once they reach the level, and keep it when they leave, but they cannot
 * cast it outside a guild or job that allows it (teaches it or relates it, see
 * add_guild_related_spell), unless it is a common effect: the effect refuses
 * the cast (see query_usable_by in /lib/effects/base_effect.c). Inside one it
 * runs at the member's guild level.
 *
 * @param  path   the spell object, inheriting BASE_EFFECT as a spell
 * @param  level  guild level needed to learn it
 * @param  power  optional EFFECT_CAT_MINOR, EFFECT_CAT_NEUTRAL or
 *                EFFECT_CAT_MAJOR: the power the members get in this spell,
 *                instead of the one their spheres or schools give; it also
 *                lets them cast it without knowing any of its categories
 */
void add_guild_spell(string path, int level, varargs string power)
{
  add_ability_spell(path, level, power);
}

/**
 * The spells the guild teaches: path -> guild level needed.
 */
mapping query_guild_spells() { return query_ability_spells(); }

/**
 * The power the members get in one spell when the guild sets it apart from
 * its categories (see add_guild_spell).
 * @param  path  the spell object
 * @return 0 when the guild sets none, else 1 minor, 2 neutral, 3 major
 */
int query_guild_spell_power(string path)
{
  return query_ability_spell_power(path);
}

/**
 * Gives a member every spell of the guild their guild level allows and they
 * do not know yet. Asking for the level now, not when it was reached, lets a
 * spell moved to an earlier level reach whoever is past it already.
 *
 * @return the paths of the spells learned, in the order of their levels
 */
string * teach_guild_spells(object player)
{
  return teach_ability_spells(player, player->query_guild_level());
}

/**
 * The spheres of divine magic the members cast from, and how well they know
 * each.
 * @param  map  sphere -> EFFECT_CAT_MINOR, EFFECT_CAT_NEUTRAL or EFFECT_CAT_MAJOR
 */
void set_spheres(mapping map) { spheres = map ? ([ ]) + map : ([ ]); }
mapping query_spheres() { return ([ ]) + spheres; }

/**
 * The schools of arcane magic the members cast from, and how well they know
 * each.
 * @param  map  school -> EFFECT_CAT_MINOR, EFFECT_CAT_NEUTRAL or EFFECT_CAT_MAJOR
 */
void set_schools(mapping map) { schools = map ? ([ ]) + map : ([ ]); }
mapping query_schools() { return ([ ]) + schools; }

/**
 * The name a player reads for a category of spells, from the game's spells
 * table (/lib/tables/spells.c).
 * @param  category  a sphere or school id
 * @return the display name
 */
string query_category_name(string category)
{
  object spells;

  spells = table("spells", this_object());
  return spells ? spells->query_spell_category_name(category) :
                  capitalize(category);
}

/**
 * How well the members know a category of spells, a sphere or a school.
 * @return 0 not at all, 1 minor, 2 neutral, 3 major
 */
int query_category_knowledge(string category)
{
  mixed known;

  known = spheres[category] ? spheres[category] : schools[category];
  switch (known)
  {
    case EFFECT_CAT_MINOR:
      return 1;
    case EFFECT_CAT_NEUTRAL:
      return 2;
    case EFFECT_CAT_MAJOR:
      return 3;
  }
  return 0;
}

/**
 * Why the guild withholds its powers from a member right now.
 *
 * A guild whose members lose their gifts under some condition (a paladin who
 * turns evil) masks this; its skills ask for it before they run.
 *
 * @return  the message for the member, or nil when nothing is withheld
 */
string query_powers_refusal(object player)
{
  return nil;
}

/**
 * What a skill of the guild answers to someone who is not a member.
 *
 * A guild with skills only its members may use masks this with a message in
 * character; nil leaves the skill free for anyone who learned it.
 *
 * @return  the message for the non-member, or nil
 */
string query_non_member_refusal(object player)
{
  return nil;
}

void start_player(object pl) 
{
  grant_guild_skills(pl);

  /*
  int i;

  // level = (int)pl->query_level();
  if (!pointerp(guild_commands))
    guild_commands = ({ });
  for (i=0;i<sizeof(guild_commands);i+=2)
    if (guild_commands[i+1][0] || !interactive(pl))
      pl->add_known_command(guild_commands[i]);
  */
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

void new_levels(int lvl, object ob) 
{
  int e, gp_bonus, gps;
  object class_ob;

  // the class is kept as a path, like every social object
  class_ob = ob->query_class_ob() ? load_object(ob->query_class_ob()) : nil;

  ob->reset_all();

  gp_bonus = ob->query_stat_bonus(ob->query_gp_main_stat()) + (query_extra_gp_bonus());

  // We go up all the levels
  for (e = 0; e < lvl; e++)
  {
      // We only raise gps if it is a new maximum level
      if (ob->query_guild_level() >= ob->query_max_guild_level())
      {
          if(class_ob)
          {
              // On the first level the maximum is obtained
              if (ob->query_guild_level() == 0)
                  gps = class_ob->query_gp_dice();
              else 
                  gps = roll(1, class_ob->query_gp_dice());
          }
          else // Newbies and/or NPCs without a guild.
          {
              if(interactive(ob))
                  gps = roll(1, 6);
              else
                  gps = roll(1, 8);
          }

          gps += gp_bonus;
          
          if (gps <= 0) 
              gps = 1;

          ob->set_max_gp(ob->query_max_gp() + gps);
    
          if (interactive(ob))
              log_file("guilds", "[" + ctime(time(),4) + "] " + 
                  ob->query_cap_name() + 
                  " sube "+gps+" pgs ("+base_name(this_object())+") "+
                   "(a nivel "+(ob->query_guild_level()+lvl)+").\n");
        }  
    }
}

/* Not something players should be able to do until we add dual class humans.
 * Up until then we leave the function open so that lords can call it
 * instead of set_guild_ob() which they often seem to bungle.
 * We will stop removing spells when dual class is implemented
 */
 /*
void leave_guild(){
  if (this_object()->query_spell_directory() != 0) // neverbot
    "/global/spells"->reset_spells();
}
*/
/* What happens when you advance in level */
/*
int do_command(string name, mixed param) {
  mixed *tmp;

  if (member_array(name, guild_commands))
    return 0;
  tmp = (mixed *)COMMAND_SERVER->query_command(name);
  return (int)call_other(tmp[0], tmp[1], param);
}
*/


// Will be called when a player tries to leave the guild
// If we return 1, we allow them to do it
// If we return 0, the player cannot stop having this guild
int can_abandon(object player)
{
  return 1;
}

// Called when the guild is actually abandoned
int guild_abandon(object player)
{
  // if (player->query_player())
  // {
  //   string * quests = player->query_quests();
  //   int i;
  //   object q;
  //
  //   // We search all the quests until we find the one for this guild
  //   // (at most one guild quest can be held, but we search
  //   //  through all of them in case there has been some error)
  //   for (i = 0; i < sizeof(quests); i+=2)
  //   {
  //     q = load_object(quests[i]);
  //
  //     if (!q)
  //       continue;
  //
  //     if ((q->query_quest_category() == QUEST_GUILD) &&
  //         (q->query_provider_path() == base_name(this_object())))
  //     {
  //       player->remove_quest(quests[i]);
  //     }
  //   }
  // }
  
  return 1;
}

// int asign_guild_quest(object pl, int level)
// {
//   string * quests = this_object()->query_quests();
//   int i, result = 0;
//   object q;
//
//   // We walk through all the quests looking for the one matching
//   // the next level
//   for (i = 0; i < sizeof(quests); i++)
//   {
//     q = load_object(quests[i]);
//
//     if (!q)
//       continue;
//
//     if ((q->query_quest_category() == QUEST_GUILD) && 
//         (q->query_provider_path() == base_name(this_object())) &&
//         (q->query_quest_advance_level() == level))
//     {
//       result = pl->add_quest(quests[i]);
//       if (result)
//         tell_player(pl, "Tienes una nueva misión de gremio.\n");
//       break;
//     }
//   }
//
//   return result;
// }

// Called when the living joins, to add feats, etc
int join_player(object pl)
{
  // We assign the first quest (the one to advance to level 2)
  // Changed to level + 1, they may have previously left the guild and come back
  // if (pl->query_player())
  //   asign_guild_quest(pl, pl->query_guild_level() + 1);
  
  return 1;
}

// Guild specializations system
string * query_guild_specs() { return specs_available; }
void set_guild_specs(string * arr) { specs_available = ({ }) + arr; }
void add_guild_specs(string name) { specs_available += ({ name }); }

int query_legal_spec(string str) { return member_array(str, specs_available) != -1; }

