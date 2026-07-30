//
// New skill system for CcMud, Folken 11/10/08
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
                              //    row == ({ skill filename, percentage, times_used, passive })
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
int adjust_skill_ability(string str, int value, varargs int silence);
int update_skill_used_times(string str);
int query_skill_used_times(string str);

// HACK
/*
  string * query_known_commands() { return ({ }); }
  int add_known_command(string str){ return 1; }
  string help_command(string str) { return ""; }
*/

void skills_commands()
{
  int i;
  mixed * skill;
  object skill_ob;
  string * pieces;
  string aux;

  {
    string * verbs;
    int v;
    verbs = _LANG_SKILL_LIST_VERBS;
    for (v = 0; v < sizeof(verbs); v++)
      add_private_action("list_skills", verbs[v]);
  }

  for (i = 0; i < sizeof(known_skills); i++) 
  {
    // Passive skills have no associated action
    if (skill_list[known_skills[i]][3] == PASSIVE_SKILL)
      continue;      
    
    skill = SKILLS_TABLE->query_skill_data(known_skills[i]);
    
    if (!sizeof(skill))
      continue;
    
    catch(skill_ob = load_object(skill[SKILL_DATA_PATH]));

    // skill_list[known_skills[i]] = 
    //     (string *)skill_SERVER->query_skill(known_skills[i]);

    // If the skill/command fails to load, do not add the action
    if (!objectp(skill_ob))
      continue;

    // Important: for a multi-word skill, add_private_action only covers
    // the first word; when the command runs we must also check that the
    // following words match
    
    pieces = explode(known_skills[i], " ");
    
    add_private_action("do_skill", pieces[0]);

    // Also register the action without accents or other symbols
    if ((aux = SKILLS_TABLE->skill_translate_to_action(pieces[0])) != "")
      add_private_action("do_skill", aux);
    
    // tell_object(find_living("folken"), "[SKILL] add_private_action(do_skill, "+pieces[0]+")\n");
  }
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


  ret = sprintf("%*'-'|s\n\n", this_user()->query_cols()+18,
      _LANG_SKILL_LIST_HEADER);
  
  for (j = 0; j < sizeof(categories); j++)
  {
    ret += "    %^GREEN%^" + capitalize(categories[j]) + "%^RESET%^:\n";
    
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
        
      if (skill_list[known_skills[i]][3] == ACTIVE_SKILL)
        skill_name = "%^BOLD%^"+capitalize(known_skills[i]) +"%^RESET%^";
      else
      {
        skill_name = "%^BOLD%^"+capitalize(known_skills[i]) +"%^RESET%^ (*)";
        passive_skills_found = 1;
      }
      
      ret += sprintf("\t%35-s %25|s (%s)\n", skill_name, 
                "["+percentage_bar(query_skill_ability(known_skills[i]))+"]",
                ""+query_skill_ability(known_skills[i])+"%");
    }
  }
    
  if (passive_skills_found)
    ret += _LANG_SKILL_PASSIVE_LEGEND;
    
  ret += sprintf("\n%*'-'s\n", this_user()->query_cols(), "");
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

  skill = SKILLS_TABLE->query_skill_data(str);

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

  // Tell the player
  if (!silence)
    tell_player(this_object(),
      _LANG_SKILL_GAINED_PRE + capitalize(str) + _LANG_SKILL_GAINED_POST);

  // Refresh the commands
  skills_commands();
  
  return 1;
    
} /* add_known_skill() */

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

// Percentage mastery of a skill
int query_skill_ability(string str)
{
  int i;

  // Not known
  if ((i = member_array(str, known_skills)) == -1)
    return 0;

  return skill_list[str][1];
}

int adjust_skill_ability(string str, int value, varargs int silence)
{
  int i;

  if (!silence)
    silence = 0;

  // Not known
  if ((i = member_array(str, known_skills)) == -1)
    return 0;

  skill_list[str][1] += value;
    
  if (skill_list[str][1] <= 1)
      skill_list[str][1] = 1;

  // Reset the times-used counter
  skill_list[str][2] = 0;

  if ((value > 0) && !silence)
    tell_player(this_object(),
      _LANG_SKILL_IMPROVED_PRE + str + _LANG_SKILL_IMPROVED_POST);

  return skill_list[str][1];
}

int update_skill_used_times(string str)
{
  int i;

  // Not known
  if ((i = member_array(str, known_skills)) == -1)
    return 0;

  // Stop updating once at the maximum
  if (skill_list[str][1] < 100)
  {
    skill_list[str][2] += 1;

    // Every 50 uses, roll to see whether the ability goes up
    if (skill_list[str][2] % 50 == 0)
    {
        if (random(100) > skill_list[str][1])
          adjust_skill_ability(str, 1);
    }
  }

  return skill_list[str][2];
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
    skill_pieces = explode(known_skills[i], " ");
    if ((skill_pieces[0] == query_verb()) ||
    // The same name without accents or symbols also matches
    (skill_pieces[0] == SKILLS_TABLE->skill_translate(query_verb())) )
    {
      found = i;
      
      for (j = 1; (j < sizeof(skill_pieces)) && (found == -1); j++)
      {
        // No match
        if ((j - 1 < sizeof(str_pieces)) &&
            (skill_pieces[j] != str_pieces[j - 1]))
          found = -1;
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
      
  // Debug info for immortals (Folken 7/01)
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
                                                                                
string help_skill(string str) 
{
  object table;
  string aux;

  table = load_object(SKILLS_TABLE);

  if (!table)
    return "";
  
  if (!pointerp(skill_list[str]))
  {
    aux = table->skill_translate(str);
    if (str != aux)
      str = aux;
    else
      return "";
  }

  return (string)skill_list[str][0]->help(str);
} 

mixed * stats() 
{
  return ({ 
    ({"Known skills", known_skills, }),
    ({"Skill list", skill_list, }),             
  });
}
