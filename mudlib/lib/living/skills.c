//
// New skill system for CcMud, neverbot 11/10/08
//
// - The old commands (std/commands) become skills (obj/skills), to tell
//   them apart from the /cmds commands
// - Core functionality extracted from /global/living/groups_obs.c
// - Fully reprogrammed 18/10/08

#include <mud/secure.h>
#include <living/skills.h>
#include <common/properties.h>
#include <language.h>

string * known_skills;         // name of each skill (must match the names
                              // in the skills table)
mapping skill_list;            // skill_list[name] = ({ row })
                              //    keyed by the names in known_skills
                              // A subset of the skills table, holding only
                              // the skills the player actually knows
                              //    row == ({ skill filename, value, times_used, passive [, kind [, levels]] })
                              //    the value means the skill's kind: a percentage, a
                              //    level or 1 for known (see <living/skills.h>)
                              //    a passive skill has no associated action to run

void create()
{
  skill_list = ([ ]);
  known_skills = ({ });
}

mapping query_skill_list() { return skill_list; }
string * query_known_skills() { return known_skills; }

void set_skill_list(mapping skills) 
{ 
  int i;
  string * indices;

  // allow the login process (and migrations) to change this
  if (!SECURE->valid_progname("/lib/core/login"))
    return;  

  skill_list = ([ ]);
  known_skills = ({ });
  indices = keys(skills);
  
  for (i = 0; i < sizeof(indices); i++)
  {
    skill_list[indices[i]] = ({ }) + skills[indices[i]];
    known_skills += ({ indices[i] });
  }
}

// Does it know a given skill?
int query_known_skill(string name)
{
  return (member_array(name, known_skills) != -1);
}

int query_skill_ability(string str);
int query_skill_kind(string str);
int adjust_skill_ability(string str, int value, varargs int silence);
int update_skill_used_times(string str);
int query_skill_used_times(string str);
int query_skill_level(string str);

// HACK
/*
  string * query_known_commands() { return ({ }); }
  int add_known_command(string str){ return 1; }
  string help_command(string str) { return ""; }
*/

// The verbs that are always there, whoever this is: registered from create,
// through social_commands, like every other half of a living registers its own.
void skills_commands()
{
  string * verbs;
  int i;

  verbs = _LANG_SKILL_LIST_VERBS;

  for (i = 0; i < sizeof(verbs); i++)
    add_private_action("list_skills", verbs[i]);
}

// Whether a known skill is active or passive, as the catalogue says now: a
// character's own row keeps what the skill was when it was learnt, and a skill
// may have gained a command since.
private int skill_type(string id)
{
  mixed * data;

  data = table("skills")->query_skill_data(id);

  if (arrayp(data) && sizeof(data) > 3)
    return data[3];

  return skill_list[id][3];
}

/**
 * The row of a skill as the catalogue has it now, or the one stored when it
 * was learnt if the catalogue no longer lists it.
 * @param id the skill id
 * @return the row, or ({ }) when the skill is not known and not listed
 */
private mixed * skill_row(string id)
{
  mixed * data;

  data = table("skills")->query_skill_data(id);

  if (arrayp(data) && sizeof(data))
    return data;

  if (mappingp(skill_list) && arrayp(skill_list[id]))
    return skill_list[id];

  return ({ });
}

/**
 * Number of levels of a levelled skill, as the catalogue says now.
 * @param id the skill id
 * @return the level count, at least 1; 0 if the skill is not levelled
 */
private int skill_levels(string id)
{
  mixed * row;

  if (query_skill_kind(id) != SKILL_KIND_LEVELLED)
    return 0;

  row = skill_row(id);

  if (sizeof(row) > SKILL_DATA_LEVELS && intp(row[SKILL_DATA_LEVELS]) &&
      row[SKILL_DATA_LEVELS] > 1)
    return row[SKILL_DATA_LEVELS];

  return 1;
}

/**
 * The stored value of a known skill, clamped to what its kind allows now
 * (a skill may have changed kind or levels since it was saved). The stored
 * value is corrected when it was out of range.
 * @param id the skill id
 * @return the value, 0 if the skill is not known
 */
private int clamped_value(string id)
{
  int value, max;

  if (member_array(id, known_skills) == -1)
    return 0;

  value = skill_list[id][SKILL_DATA_VALUE];

  switch (query_skill_kind(id))
  {
    case SKILL_KIND_FIXED:
      max = 1;
      break;
    case SKILL_KIND_LEVELLED:
      max = skill_levels(id);
      break;
    default:
      max = 100;
      break;
  }

  if (value < 1)
    value = 1;
  if (value > max)
    value = max;

  skill_list[id][SKILL_DATA_VALUE] = value;

  return value;
}

// One action per skill this living actually knows. Kept apart from the verbs
// above because it can only run once the skills are known: a player's are
// restored from its savefile long after create, and until then there is
// nothing here to register.
private void known_skill_commands()
{
  object skill_ob;
  string * words;
  int i, w;

  for (i = 0; i < sizeof(known_skills); i++)
  {
    // Passive skills have no associated action
    if (skill_type(known_skills[i]) == PASSIVE_SKILL)
      continue;

    catch(skill_ob = load_object(skill_list[known_skills[i]][SKILL_DATA_PATH]));

    // If the skill object fails to load, do not add the action
    if (!objectp(skill_ob))
      continue;

    // Register the action on the skill's translated name and on every
    // alias. add_private_action only covers the first word; do_skill
    // checks the following words when the command actually runs.
    words = ({ skill_ob->query_effect_name() }) + skill_ob->query_aliases();

    for (w = 0; w < sizeof(words); w++)
      add_private_action("do_skill", explode(words[w], " ")[0]);
  }
}

/**
 * The graphic of a skill in the listing, 22 columns wide like the bar of a
 * percentage skill: the bar itself, one slot per level with the known ones
 * filled ([#][#][ ]), or a single filled slot for a fixed skill.
 * @param id the skill id of a known skill
 * @return the cell, with colour codes
 */
private string skill_value_cell(string id)
{
  string cell;
  int i, level, levels, width;

  switch (query_skill_kind(id))
  {
    case SKILL_KIND_LEVELLED:
      level = query_skill_level(id);
      levels = skill_levels(id);
      cell = "";
      for (i = 1; i <= levels; i++)
      {
        if (i <= level)
          cell += "[%^BOLD%^GREEN%^#%^RESET%^]";
        else
          cell += "[ ]";
      }
      width = 3 * levels;
      break;

    case SKILL_KIND_FIXED:
      cell = "[%^BOLD%^GREEN%^#%^RESET%^]";
      width = 3;
      break;

    default:
      return "[" + percentage_bar(query_skill_ability(id)) + "]";
  }

  // pad to the width of a percentage bar, so the text that follows lines up
  for (; width < 22; width++)
    cell += " ";

  return cell;
}

/**
 * The text after the graphic of a skill in the listing: 45%, 2/3, or the
 * word for a fixed skill.
 * @param id the skill id of a known skill
 * @return the text
 */
private string skill_value_text(string id)
{
  switch (query_skill_kind(id))
  {
    case SKILL_KIND_LEVELLED:
      return "" + query_skill_level(id) + "/" + skill_levels(id);
    case SKILL_KIND_FIXED:
      return _LANG_SKILL_FIXED_TEXT;
  }

  return "" + query_skill_ability(id) + "%";
}

int list_skills(varargs string str)
{
  string ret, skill_name;
  int passive_skills_found;
  string * categories;
  int i, j;

  ret = "";
  skill_name = "";
  passive_skills_found = 0;
  categories = ({ });
  i = 0;
  j = 0;

  if (this_object()->query_dead())
  {
    notify_fail(_LANG_SKILL_DEAD_LIST);
    return 0;
  }
      
  if (!sizeof(known_skills))
  {
    notify_fail(_LANG_SKILL_NONE_YET);
    return 0;
  }
    
  // Build a list of all the skill categories
  for (i = 0; i < sizeof(known_skills); i++)
  {
    object f;
    string cat;

    f = load_object(skill_list[known_skills[i]][0]);

    if (!f)
      continue;
    
    cat = implode(f->query_categories(), " - ");
    
    if (member_array(cat, categories) == -1)
      categories += ({ cat });
  }


  ret = sprintf("%p%|*s\n\n", '-', this_user()->query_cols()+18,
      _LANG_SKILL_LIST_HEADER);
  
  for (j = 0; j < sizeof(categories); j++)
  {
    string cat_disp;

    // Categories are stored as English ids; translate for display.
    cat_disp = _LANG_SKILL_CATEGORIES[categories[j]];
    if (!cat_disp)
      cat_disp = categories[j];

    ret += "    %^GREEN%^" + capitalize(cat_disp) + "%^RESET%^:\n";

    for (i = 0; i < sizeof(known_skills); i++)
    {
      object f;
      string cat;

      f = load_object(skill_list[known_skills[i]][0]);

      if (!f)
        continue;

      cat = implode(f->query_categories(), " - ");

      if (categories[j] != cat)
        continue;

      // The skill is keyed by an English id; show its translated name.
      if (skill_type(known_skills[i]) == ACTIVE_SKILL)
        skill_name = "%^BOLD%^"+capitalize(f->query_effect_name()) +"%^RESET%^";
      else
      {
        skill_name = "%^BOLD%^"+capitalize(f->query_effect_name()) +"%^RESET%^ (*)";
        passive_skills_found = 1;
      }
      
      ret += sprintf("\t%-35s %s (%s)\n", skill_name,
                skill_value_cell(known_skills[i]),
                skill_value_text(known_skills[i]));
    }
  }
    
  if (passive_skills_found)
    ret += _LANG_SKILL_PASSIVE_LEGEND;
    
  ret += sprintf("\n%p%*s\n", '-', this_user()->query_cols(), "");
  tell_object(this_object(), ret);
  
  return 1;
}

int add_known_skill(string str, varargs int silence)
{
  mixed * skill;
  object skill_ob;

  if (!silence)
    silence = 0;

  // Already known
  if (member_array(str, known_skills) != -1)
    return 0;

  skill = table("skills")->query_skill_data(str);

  // The table returned no data
  if (!arrayp(skill) || !sizeof(skill))
    return 0;

  catch(skill_ob = load_object(skill[SKILL_DATA_PATH]));

  // The skill/command failed to load
  if (!objectp(skill_ob))
    return 0;

  // Add the skill. The initial ability is whatever the table sets in
  // index [1] of the skill row (e.g. orientation starts at 10%).
  known_skills += ({ str });
  skill_list[str] = ({ }) + skill;

  // Tell the player, using the skill object's translated display name
  // (str is the English id, which would show untranslated).
  if (!silence)
    tell_player(this_object(),
      _LANG_SKILL_GAINED_PRE + capitalize(skill_ob->query_effect_name()) +
      _LANG_SKILL_GAINED_POST);

  // the new skill needs its action attached; the standing verbs are already up
  known_skill_commands();
  
  return 1;
    
} /* add_known_skill() */

// Grant the mudlib-wide default skills (DEFAULT_SKILLS) to this player.
// Called on every login from living::start_player. Idempotent, since
// add_known_skill returns 0 for skills already known, and silent (second
// argument 1) so a brand-new character is not flooded with gained-skill
// messages and adding a new default reaches everyone on their next login.
void grant_default_skills()
{
  int i;
  string * defaults;

  defaults = DEFAULT_SKILLS;

  for (i = 0; i < sizeof(defaults); i++)
    add_known_skill(defaults[i], 1);
}

// Everything the skills half does for somebody entering the world: hand over
// the defaults, then attach an action to each skill they turn out to know.
void start_skills()
{
  grant_default_skills();
  known_skill_commands();
}

int remove_known_skill(string str)
{
  int i;

  // Not known
  if ((i = member_array(str, known_skills)) == -1)
    return 0;
    
  known_skills = delete(known_skills, i, 1);
  skill_list = m_delete(skill_list, str);
  
  return 1;
    
} /* remove_known_skill() */

/**
 * The kind of a skill, read from the catalogue now and, if the catalogue no
 * longer lists it, from the row stored when it was learnt. A row without a
 * kind entry is a percentage skill.
 * @param str the skill id
 * @return SKILL_KIND_PERCENTAGE, SKILL_KIND_LEVELLED or SKILL_KIND_FIXED
 */
int query_skill_kind(string str)
{
  mixed * row;

  row = skill_row(str);

  if (sizeof(row) > SKILL_DATA_KIND && intp(row[SKILL_DATA_KIND]))
    return row[SKILL_DATA_KIND];

  return SKILL_KIND_PERCENTAGE;
}

/**
 * Percentage mastery of a percentage skill (1-100).
 * @param str the skill id
 * @return the ability; 0 if the skill is not known or is of another kind
 *         (use query_skill_level or query_known_skill for those)
 */
int query_skill_ability(string str)
{
  if (query_skill_kind(str) != SKILL_KIND_PERCENTAGE)
    return 0;

  return clamped_value(str);
}

/**
 * Level of a levelled skill.
 * @param str the skill id
 * @return the level (1..max); 0 if the skill is not known or not levelled
 */
int query_skill_level(string str)
{
  if (query_skill_kind(str) != SKILL_KIND_LEVELLED)
    return 0;

  return clamped_value(str);
}

/**
 * Number of levels of a levelled skill, whether or not it is known.
 * @param str the skill id
 * @return the maximum level; 0 if the skill is not levelled
 */
int query_skill_max_level(string str)
{
  return skill_levels(str);
}

/**
 * The stored value of a known skill, whatever its kind: the percentage, the
 * level, or 1 if fixed.
 * @param str the skill id
 * @return the value; 0 if the skill is not known
 */
int query_skill_value(string str)
{
  return clamped_value(str);
}

/**
 * Raises or lowers the ability of a percentage skill, within 1-100.
 * @param str the skill id
 * @param value what to add (negative to lower)
 * @param silence 1 to skip the message when it improves
 * @return the new ability; 0 if the skill is not known or not a percentage one
 */
int adjust_skill_ability(string str, int value, varargs int silence)
{
  int current;
  object f;

  if (!silence)
    silence = 0;

  // Not known, or not a percentage skill
  if ((member_array(str, known_skills) == -1) ||
      (query_skill_kind(str) != SKILL_KIND_PERCENTAGE))
    return 0;

  current = clamped_value(str) + value;

  if (current < 1)
    current = 1;
  if (current > 100)
    current = 100;

  skill_list[str][SKILL_DATA_VALUE] = current;

  // Reset the times-used counter
  skill_list[str][SKILL_DATA_USES] = 0;

  // Message uses the skill's translated display name, not the English id.
  f = load_object(skill_list[str][SKILL_DATA_PATH]);

  if ((value > 0) && !silence)
    tell_player(this_object(),
      _LANG_SKILL_IMPROVED_PRE + (f ? f->query_effect_name() : str) +
      _LANG_SKILL_IMPROVED_POST);

  return current;
}

/**
 * Sets the level of a known levelled skill, within 1..max. No message.
 * @param str the skill id
 * @param level the new level
 * @return the level now; 0 if the skill is not known or not levelled
 */
int set_skill_level(string str, int level)
{
  int max;

  if ((member_array(str, known_skills) == -1) ||
      (query_skill_kind(str) != SKILL_KIND_LEVELLED))
    return 0;

  max = skill_levels(str);

  if (level < 1)
    level = 1;
  if (level > max)
    level = max;

  skill_list[str][SKILL_DATA_VALUE] = level;
  skill_list[str][SKILL_DATA_USES] = 0;

  return level;
}

/**
 * Raises or lowers the level of a known levelled skill, within 1..max. The
 * player is told when it rises.
 * @param str the skill id
 * @param delta levels to add (negative to lower)
 * @param silence 1 to skip the message
 * @return the level now; 0 if the skill is not known or not levelled
 */
int adjust_skill_level(string str, int delta, varargs int silence)
{
  int before, after;
  object f;

  before = query_skill_level(str);

  if (!before)
    return 0;

  after = set_skill_level(str, before + delta);

  if ((after > before) && !silence)
  {
    f = load_object(skill_list[str][SKILL_DATA_PATH]);

    tell_player(this_object(),
      _LANG_SKILL_LEVEL_UP_PRE + (f ? f->query_effect_name() : str) +
      _LANG_SKILL_LEVEL_UP_MID + after + "/" + skill_levels(str) +
      _LANG_SKILL_LEVEL_UP_POST);
  }

  return after;
}

/**
 * Teaches a skill (if not known yet) and sets its value, which means what its
 * kind says: percentage -> ability, levelled -> level, fixed -> known. Out of
 * range values are clamped; a value below 1 leaves the one the skill starts
 * with. Used by templates, classes and quests.
 * @param str the skill id
 * @param value the ability or level
 * @param silence 1 to skip the messages
 * @return the value now; 0 if the skill could not be added
 */
int set_skill_value(string str, int value, varargs int silence)
{
  int added, current;

  if (!silence)
    silence = 0;

  added = add_known_skill(str, silence);

  if (!query_known_skill(str))
    return 0;

  current = clamped_value(str);

  // a skill just learnt does not announce its starting value as a change
  if (added)
    silence = 1;

  if (value < 1 || value == current)
    return current;

  switch (query_skill_kind(str))
  {
    case SKILL_KIND_PERCENTAGE:
      adjust_skill_ability(str, value - current, silence);
      break;
    case SKILL_KIND_LEVELLED:
      adjust_skill_level(str, value - current, silence);
      break;
  }

  return clamped_value(str);
}

/**
 * Counts one use of a percentage skill and, every 50 uses, rolls to raise it
 * by 1%. Levelled and fixed skills never improve with use.
 * @param str the skill id
 * @return the times used; 0 if the skill is not known or not a percentage one
 */
int update_skill_used_times(string str)
{
  // Not known, or not a percentage skill
  if ((member_array(str, known_skills) == -1) ||
      (query_skill_kind(str) != SKILL_KIND_PERCENTAGE))
    return 0;

  // Stop updating once at the maximum
  if (clamped_value(str) < 100)
  {
    skill_list[str][SKILL_DATA_USES] += 1;

    // Every 50 uses, roll to see whether the ability goes up
    if (skill_list[str][SKILL_DATA_USES] % 50 == 0)
    {
        if (random(100) > skill_list[str][SKILL_DATA_VALUE])
          adjust_skill_ability(str, 1);
    }
  }

  return skill_list[str][SKILL_DATA_USES];
}

int query_skill_used_times(string str)
{
  int i;

  // Not known
  if ((i = member_array(str, known_skills)) == -1)
    return 0;

  return skill_list[str][2];
}

// Runs the skill (if it has an associated command)
int do_skill(string str)
{
  int i, j;
  int found;
  int result;
  string * skill_pieces;
  string * str_pieces;

  i, j = 0;
  found = -1;
  result = 0;
  skill_pieces = ({ });

  if (!strlen(str))
  {
    str = "";
    str_pieces = ({ str });
  }
  else
    str_pieces = explode(str, " "); 

  if (!known_skills) 
  {
    known_skills = ({ });
    return 0;
  }

  if (!mappingp(skill_list))
    skill_list = ([ ]);

  // tell_object(find_living("folken"), "[SKILL] verb='"+query_verb()+"' str='"+str+"'\n");
  
  // Look up the command

  // Obviously not the best way to look up a command that may contain
  // whitespace, but doing it only here (skills run rarely) rather than in
  // the generic add_action system (which runs constantly) should be more
  // efficient
  
  for (i = 0; (i < sizeof(known_skills)) && (found == -1); i++)
  {
    object f;
    string * cands;
    int c;

    f = load_object(skill_list[known_skills[i]][SKILL_DATA_PATH]);
    if (!f)
      continue;

    // Match the typed verb against the skill's translated name or any of
    // its aliases; for a multi-word entry the following words must match.
    cands = ({ f->query_effect_name() }) + f->query_aliases();

    for (c = 0; (c < sizeof(cands)) && (found == -1); c++)
    {
      skill_pieces = explode(cands[c], " ");

      if (skill_pieces[0] != query_verb())
        continue;

      found = i;
      for (j = 1; j < sizeof(skill_pieces); j++)
      {
        if (!((j - 1 < sizeof(str_pieces)) &&
              (skill_pieces[j] == str_pieces[j - 1])))
        {
          found = -1;
          break;
        }
      }
    }
  }

  if (found == -1)
    return 0;
  /*
  {
      tell_object(find_living("folken"), "[SKILL] found="+found+", skill=<not found>\n");
      return 0;
  }
  else
  {
      tell_object(find_living("folken"), "[SKILL] found="+found+", skill="+known_skills[found]+"\n");
  }
  */

  // First look for an exact match
  // if((i = member_array(query_verb(), my_skills)) != -1)

  if (this_object()->query_timed_property_exists(PASSED_OUT_PROP))
  {
    notify_fail(this_object()->query_timed_property(PASSED_OUT_PROP));
    return 0;
  }

  if (this_object()->query_dead())
  {
    notify_fail(_LANG_SKILL_DEAD_USE);
    return 0;
  }    

  // Call the skill's cast_effect, passing the string (targets to search),
  // the effect initiator, and quiet as false
  result = (int)call_other( skill_list[known_skills[found]][0],
                          "cast_effect", 
                          str, 
                          this_object(), 
                          0);

  // REMOVED!!!
  // Some skills (object-crafting ones, of type "pass") can run without
  // crafting anything, which would let them improve without really being
  // used. Learning now happens in /global/living/effects.c, at the end of
  // do_active_effects on the skill's last round only, and only if it ran
  // successfully (same as the gp cost).

  // If the effect actually fired (all its basic requirements were met),
  // update the times-used counter. Even so, if the effect spans several
  // turns it may have failed on a later one (but it did fire).
  // ... You learn something even when things go wrong ...
  // if (result == 1)
  //     update_skill_used_times(known_skills[found]);
  
  return result;
}
  
int skill_damage(int damage, string type, object attacker)
{
  int prot;
  
  prot = this_object()->query_resistance(type);
      
  // Debug info for immortals (neverbot 7/01)
  if ((prot != 0) && this_object()->query_coder())
    tell_object(this_object(), "DEBUG (skill_damage): applied resistance " +
                                "against '"+type+"' ("+prot+"%).\n");

  // Apply the damage modifier from resistances:
  damage = (damage * (100 - prot)) / 100;
                        
  if(!attacker)
  {
    return (this_object()->query_hp() - this_object()->adjust_hp(-damage) );
    // return 1;
  }
  else 
    return (this_object()->query_hp() - this_object()->adjust_hp(-damage, attacker) );
    // return 1;
}
                                                                                
// Help for a skill the player names. The typed word may be the English id
// or the skill's translated display name; we resolve it against every skill
// in the table by loading each object and comparing its effect_name, then
// return that object's own help() (rendered in the current language).
string help_skill(string str)
{
  object skills_table, f;
  mapping skills;
  string * ids;
  int i;

  if (!str || !strlen(str))
    return "";

  skills_table = table("skills");
  if (!skills_table)
    return "";

  skills = skills_table->query_skills();
  ids = keys(skills);

  for (i = 0; i < sizeof(ids); i++)
  {
    f = load_object(skills[ids[i]][SKILL_DATA_PATH]);
    if (!f)
      continue;

    if ((ids[i] == str) ||
        (lower_case(f->query_effect_name()) == lower_case(str)) ||
        (member_array(lower_case(str), f->query_aliases()) != -1))
      return (string) f->help();
  }

  return "";
}

mixed * stats() 
{
  return ({ 
    ({"Known skills", known_skills, }),
    ({"Skill list", skill_list, }),             
  });
}
