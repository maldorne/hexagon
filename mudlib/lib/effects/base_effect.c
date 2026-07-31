/* *******************************************************
 *
 *  Effect system
 *
 *  Generic, applies to commands, spells, etc.
 *  Folken@CcMud 27/11/2008
 *
 * ******************************************************* */

#include <living/effects.h>
#include <common/properties.h>
#include <living/skills.h>
#include <living/combat.h>
#include <language.h>

inherit "/lib/effects/combat_fixes.c";
inherit "/lib/effects/guild_fixes.c";

string effect_name;       // effect name (spell, command, etc)
mapping categories;       // categories (spheres, schools, etc)
                          // ([ category_name : level_in_the_category ])
int is_passive;           // the effect is passive (runs nothing)
int effect_type;          // effect type (skill, spell, etc)
int combat_role_needed;   // required combat role (offensive, defensive, etc)
int gp_cost;              // gp cost (not applied to spells)
int lockout_time;         // minimum time between two runs of the same effect

string help_desc;         // the help text
string help_extras;       // extra help fields (shown before the description)
string target_type;       // target type the effect can have
                          //   possible: TARGET_TYPE_NONE, TARGET_TYPE_ONE, TARGET_TYPE_MANY,
                          //             TARGET_TYPE_ALL, TARGET_TYPE_ITEM,
                          //             TARGET_TYPE_PASS, TARGET_TYPE_SELF, TARGET_TYPE_TOUCH
string* property_checks;  // properties that prevent the effect from running
// string* shadow_checks;    // functions checked to know if it has shadows
int effect_range;         // effect range
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
  categories = ([ ]);
  effect_type = EFFECT_IS_SKILL;
  gp_cost = 1;
  combat_role_needed = NEUTRAL_ROLE;
  
  help_desc = "Help";
  help_extras = "";
  target_type = TARGET_TYPE_NONE;
  property_checks = 
    ({ 
      PROPERTY_IS_CASTING, 
      "LOCK " + base_name(this_object()),
     });
  // shadow_checks = ({ });
  lockout_time = 5;
  
  effect_range = 0;
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

string * query_categories() { return keys(categories); }
void set_categories(mapping str) { categories = map_copy(str); }
void add_category(string str, int level) { categories[str] = level; }

// Translate a category id (English, see <living/skills.h>) to its display
// name. Ids with no mapping (e.g. spell spheres) are shown as-is.
private string category_display(string id)
{
  string disp;

  disp = _LANG_SKILL_CATEGORIES[id];
  return disp ? disp : id;
}

int query_combat_role_needed() { return combat_role_needed; }
void set_combat_role_needed(int value) { combat_role_needed = value; }
string query_combat_role_needed_string() { return _LANG_COMBAT_ROLE_STRINGS[combat_role_needed]; }

void set_help_desc(string str) { help_desc = str; }
string query_help_desc() { return help_desc; }

void set_help_extras(string str) { help_extras = str; }
string query_help_extras() { return help_extras; }

void set_target_type(string str) { target_type = str; }
string query_target_type() { return target_type; }

/*
void add_shadow_to_check(string str) { shadow_checks += ({ str }); }
string* query_shadow_checks() { return shadow_checks;}
*/

void add_property_to_check(string str) { property_checks += ({ str }); }
string* query_property_checks() { return property_checks;}

int query_range() { return effect_range; }
void set_range(int i) { effect_range = i; }

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
 * Folken 7/01
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
    if(!caster) 
        caster = this_player();
    clvl = categories[this_object()->query_effect_category(caster)];
    if(!clvl) 
        clvl = 1;
    return clvl;
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

      if (effect_range <= 1)
          ret += _LANG_EFFECT_HELP_RANGE_NONE;
      else
          ret += _LANG_EFFECT_HELP_RANGE + effect_range;

      ret += _LANG_EFFECT_HELP_COST + EFFECTS_TABLE->query_gp_cost(this_player(), this_object());

      if (combat_role_needed != NEUTRAL_ROLE)
        ret += _LANG_EFFECT_HELP_ROLE_NEEDED + _LANG_COMBAT_ROLE_STRINGS[combat_role_needed];
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
    ret += sprintf("  %-=*s\n", (this_user() ? this_user()->query_cols() - 2 : 79), query_help_desc());

    if (this_player()->query_coder())
        ret += "DEBUG: effect file: "+file_name(this_object()) + "\n";

    ret += "\n";
    return ret;
}

// Main function, this is where we start running the effect.
int cast_effect(string str, object who, int quiet)
{
    mixed ob;        // effect target (find_match)
    mixed exstr;     // output of extra_checks (if it exists)
    object caster;   // the object casting the effect
    int i;
    int my_gp_cost;
    
    if (who) 
        caster = who;
    else 
        caster = this_player();

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

        // Test check: if the effect is not somewhere under /obj we only let
        // immortals and testers (the "TEST" property) try it.
        if ( strlen(s) && !caster->query_coder() &&
          !caster->query_property(PROP_EFFECT_TESTER) )
        {
            string* ss;

            ss = explode( s, "/" );
            if ( sizeof(ss) && ss[0] != "obj" )
            {
                notify_fail(_LANG_EFFECT_NOT_APPROVED);
                return 0;
            }
        }
        str = (string)caster->expand_nickname(str);
    }

    if(caster->query_property(NOSKILL_PROP) ||
      environment(caster)->query_property(NOSKILL_PROP) ||
      caster->query_property(PASSED_OUT_PROP))
    {
        notify_fail(MSG_NO_PUEDO);
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
        
    if ( (!allow_on_water && environment(caster)->query_water()) ||
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
        notify_fail(MSG_OCUPADO);
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
            notify_fail(MSG_NO_PUEDO);
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

    // Look for targets
    ob = 0;

    if (( target_type == TARGET_TYPE_ONE ) || ( target_type == TARGET_TYPE_TOUCH ))
    {
        ob = find_one_match( str, environment(caster), !allow_on_dead);
        
        if (ob)
        {
          if ( !allow_on_self && (caster == ob))
          {
            notify_fail(MSG_NO_SELF);
            return 0;
          }
  
          if ( !allow_on_mounted && ob->query_riding() )
          {
            notify_fail(MSG_NO_ON_MOUNT);
            return 0;
          }

          if (!allow_on_pacified && ob->query_pacify_effect())
          {
            notify_fail(_LANG_EFFECT_PROTECTED_TARGET_PRE +
              ob->query_cap_name() + _LANG_EFFECT_PROTECTED_TARGET_POST);
            return 0;
          }

          if (!allow_on_passed_out && ob->query_property(PASSED_OUT_PROP))
          {
            notify_fail(_LANG_EFFECT_CANT_ON_PRE +
              ob->query_cap_name() + _LANG_EFFECT_CANT_ON_POST);
            return 0;
          }

          if (!allow_on_hidden && ob->query_hidden()) 
          {
            ob = 0;
          }
        }

        // if the effect type is touch and the victim is being protected,
        // the target may change
        if ( target_type == TARGET_TYPE_TOUCH )
            ob = check_melee_protector(ob, caster);

        if (!ob)
        {
            notify_fail(MSG_NO_TARGET);
            return 0;
        }
    }
    else if ( target_type == TARGET_TYPE_MANY )
        ob = find_unique_match( str, environment(caster), !allow_on_dead);

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
        
        ob = find_one_match( str, caster );

        if (!ob)
            ob = find_one_match( str, environment(caster), !allow_on_dead);

        if (!ob)
        {
            notify_fail(MSG_NO_TARGET);
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

    // Custom extra checks: if the effect defines a "post_extra_checks"
    // function, call it and show its result on screen.

    if ( function_exists("post_extra_checks", this_object() ) &&
      ((target_type == TARGET_TYPE_ONE) || (target_type == TARGET_TYPE_SELF)) && 
      ( stringp( exstr = this_object()->post_extra_checks(ob, caster) ) ) )
    {
        notify_fail(exstr);
        return 0;
    }

    /* Added by Folken 3/2002, to try to stop the shadow effects from */
    /*  crashing all day */
    /* Check the required properties */

    // This is horrendous, removing it again, Folken 11/2008
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
    if (effect_type == EFFECT_IS_SPELL)
      my_gp_cost = EFFECTS_TABLE->query_gp_cost(caster, this_object());
    else
      my_gp_cost = gp_cost;

    if (caster->query_gp() < my_gp_cost)
    {
        notify_fail(MSG_NO_GPS);
        return 0;
    }

    // Combat roles, Folken 01/2013
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
            	tell_object(caster, start_phrases[0] + "\n");
			
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
        caster->do_active_effects(0);

    return 1;
}

int hb_effect(object caster, mixed *params, int time)
{
    /* params: [0] target, [1] quiet, [2] environment(caster), */

    int which_round;    /* current round */
    mixed target;
    mixed out_of_range; /* where we store target when it goes out of range */
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
                tell_room(environment(caster), MSG_NO_PUEDO_R, caster);
                call_out("end_effect_now", 0, caster);
                return 0;
            }
            // Check it is within range
            else if (!RANGE_HANDLER->check_in_range( caster, target, effect_range ))
            {
                out_of_range = target;
                target = 0;
                tell_object(caster, MSG_NO_RANGE);
                tell_room(environment(caster), MSG_NO_PUEDO_R, caster);
                call_out("end_effect_now", 0, caster);
                return 0;
            }
        }
        break;

    case TARGET_TYPE_MANY:
        /*** IMPORTANT CHANGE (Folken 7/2001)
             We don't allow many-target effects; either we have single-target
             effects or we affect every character in the
             habitacion. ***/
        /* 
         * out_of_range = ({ });
         * for (i = sizeof(target)-1; i >= 0; i-- )
         *    
         *   // Remove non-living targets
         *   if ( !target[i] || !living(target[i]) )
         *        target = delete(target, i, 1);
         *
         *    // Targets out of sight or range go into out_of_range
         *    else if (!RANGE_HANDLER->check_in_range(caster, target[i], effect_range))
         *    {
         *        out_of_range += ({ target[i] });
         *        target = delete(target, i, 1);
         *    }
         *
         * // If we ran out of targets, end:
         * if(!sizeof(target))
         * {
         *    tell_object(caster,"Your effect's target has vanished.\n");
         *    return 0;
         * }
         * break;
         */

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
            tell_room(environment(caster), MSG_NO_PUEDO_R, caster);
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

    // Added the "invulnerable_efectos" checks from area 23, Folken 2001
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
