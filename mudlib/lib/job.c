/* 
 * Base job file...
 * neverbot -  March 18, 2002
 * 
 * auto-remove job quests when abandoning the job
 * neverbot 06/2014
 */

inherit "/lib/core/object.c";
// the skills and spells it teaches or relates, shared with the guilds
inherit "/lib/social/abilities.c";

#define BASIC_JOB_XP_COST 2000

// mixed *job_commands = ({ });

// race and class ids the job admits, see <living/races.h> and
// <living/classes.h>; an empty list admits every one
private string * legal_races, * legal_classes;
// the markdown text of the job's rules, see set_rules_book
private string rules_book;
// whether players may take the job; a job only NPCs hold sets it to 0
private int playable;

void create()
{
  legal_races = ({ });
  legal_classes = ({ });
  rules_book = "";
  playable = 1;
  create_abilities();
  ::create();
}

string * query_legal_races() { return legal_races + ({ }); }
void set_legal_races(string * list) { legal_races = list ? list + ({ }) : ({ }); }
string * query_legal_classes() { return legal_classes + ({ }); }
void set_legal_classes(string * list) { legal_classes = list ? list + ({ }) : ({ }); }

int query_legal_race(string race)
{
  return !sizeof(legal_races) || member_array(race, legal_races) != -1;
}
int query_legal_base_race(string race) { return(1); }
int query_legal_class(string my_class)
{
  return !sizeof(legal_classes) || member_array(my_class, legal_classes) != -1;
}

// A race named in the list admits all of its lineages, a lineage named admits
// only that lineage.
int query_legal_player(object player){
   return ((query_legal_race(player->query_race_id()) ||
            query_legal_race(player->query_base_race_id())) &&
           query_legal_base_race(player->query_base_race_id()) &&
           query_legal_class(player->query_class_id()) );
}

/**
 * What else query_legal_player checks, in words, for the normas of the job
 * (/lib/handlers/rules.c). A job that checks more than its lists masks this
 * beside the check; nil says nothing more.
 */
string query_requirements_text() { return nil; }

/**
 * Names the book of the job's rules: the full path of a markdown text, as a
 * library shelves it (see /lib/handlers/books.c). normas shows it after what
 * the job object says of itself, so the book and normas never disagree.
 */
void set_rules_book(string path) { rules_book = path ? path : ""; }
string query_rules_book() { return rules_book; }

/** Whether players may take the job (1 unless the job says otherwise). */
int query_playable() { return playable; }
void set_playable(int value) { playable = value ? 1 : 0; }

// neverbot 07/12/06
int query_next_level_xp(object player)
{
  int res;

  res = player->query_job_level() * BASIC_JOB_XP_COST;
  if (res == 0)
    return BASIC_JOB_XP_COST;
  return res;
}

// Change to another number to set a maximum limit
// for this job
int query_max_level() { return 20; }

/**
 * Teaches a skill to those who hold the job, from a job level on.
 *
 * Call it from the job's setup. They get it when they join or reach the level
 * (see grant_job_skills) and keep it when they leave, but they cannot use it
 * outside a guild or job that allows it (teaches it or relates it, see
 * add_job_related_skill), unless it is a common effect: the effect refuses
 * the use (see query_usable_by in /lib/effects/base_effect.c). Inside the job
 * it runs at the holder's job level, unless their guild allows it too.
 *
 * @param  id       the skill id, see <living/skills.h>
 * @param  level    job level needed to know it
 * @param  ability  least value they have in it when taught (an ability for a
 *                  percentage skill, a level for a levelled one)
 */
void add_job_skill(string id, int level, varargs int ability)
{
  add_ability_skill(id, level, ability);
}

/**
 * The skills the job teaches: id -> ({ job level needed, least ability }).
 */
mapping query_job_skills() { return query_ability_skills(); }

/**
 * Teaches a spell to those who hold the job, from a job level on. They learn
 * it with aprender in the job room once they reach the level; the rest is as
 * for add_job_skill.
 *
 * @param  path   the spell object, inheriting BASE_EFFECT as a spell
 * @param  level  job level needed to learn it
 * @param  power  optional EFFECT_CAT_MINOR, EFFECT_CAT_NEUTRAL or
 *                EFFECT_CAT_MAJOR: the power they get in this spell, instead
 *                of the one its categories give; it also lets them cast it
 *                without knowing any of its categories
 */
void add_job_spell(string path, int level, varargs string power)
{
  add_ability_spell(path, level, power);
}

/**
 * The spells the job teaches: path -> job level needed.
 */
mapping query_job_spells() { return query_ability_spells(); }

/**
 * The power those who hold the job get in one spell when the job sets it apart
 * from the spell's categories (see add_job_spell).
 * @param  path  the spell object
 * @return 0 when the job sets none, else 1 minor, 2 neutral, 3 major
 */
int query_job_spell_power(string path)
{
  return query_ability_spell_power(path);
}

/**
 * Ties a skill to the job without teaching it: holders may use it, but it is
 * not granted on joining or levelling and not listed by query_job_skills (it
 * is learned through quests, items...).
 * @param  id  the skill id, compared exactly
 */
void add_job_related_skill(string id) { add_related_ability_skill(id); }

/**
 * Ties a spell to the job without teaching it, as add_job_related_skill does
 * for a skill.
 * @param  path  the spell object, full path with .c, compared exactly
 */
void add_job_related_spell(string path) { add_related_ability_spell(path); }

/**
 * The ids of the skills tied to the job but not taught by it.
 */
string * query_job_related_skills() { return query_related_ability_skills(); }

/**
 * The paths of the spells tied to the job but not taught by it.
 */
string * query_job_related_spells() { return query_related_ability_spells(); }

/**
 * Gives a holder every skill of the job their job level allows.
 *
 * Idempotent: a skill already known is left as it is, only raised to the
 * least value the job promises.
 */
void grant_job_skills(object player)
{
  grant_ability_skills(player, player->query_job_level());
}

/**
 * Gives a holder every spell of the job their job level allows and they do
 * not know yet (see teach_ability_spells).
 *
 * @return the paths of the spells learned, in the order of their levels
 */
string * teach_job_spells(object player)
{
  return teach_ability_spells(player, player->query_job_level());
}

int query_gp_dice() { return 5; }

int query_channel() { return 0; }

nomask int query_is_job() { return 1; }

/*
int add_job_command(string name, int add_if_player) 
{
   if (member_array(name, job_commands) != -1)
      return 0;
   job_commands += ({ name, ({ add_if_player }) });
   return 1;
}*/

// Mask this and put your shadows in here, scope to this though!
void start_player(object pl) 
{
  // the skills the job teaches, at the job level they have now
  grant_job_skills(pl);

		/*
   int i;
   if (!pointerp(job_commands))
      job_commands = ({ });
   for (i=0;i<sizeof(job_commands);i+=2)
      if (job_commands[i+1][0] || !interactive(pl))
         pl->add_known_command(job_commands[i]);
		*/
}

void on_death(object player, varargs object killer)
{
}

void on_kill(object player, object victim)
{
}

/**
 * Called when somebody who has this job takes a part from a corpse with one
 * of the extraction skills (/lib/obj/skills/base-extraction.c).
 *
 * @param player who took it
 * @param part the body part taken
 * @param corpse the corpse it came from
 */
void on_body_part_taken(object player, object part, object corpse)
{
}

int player_quit(object player)
{
    return 1;
}

/**
 * Called when the job level of a living changes, before the level is adjusted.
 * @param  ob    the living
 * @param  lvl   the change, negative when it goes down
 */
void on_level_change(object ob, int lvl) 
{
    int e, gps;
    
    ob->reset_all();
    
    // We go up all the levels
    for (e = 0; e < lvl; e++)
    {
        // We only raise gps if it is a new maximum level
        if (ob->query_job_level() >= ob->query_max_job_level())
        {
            // On the first level the maximum is obtained
            if (ob->query_job_level() == 0)
                gps = query_gp_dice() + 5;
            else 
                gps = roll(1, query_gp_dice()) + 5;
          
            if (gps <= 0) 
                gps = 1;
            
            ob->set_max_gp(ob->query_max_gp() + gps);
    
            if (interactive(ob))
                log_file("jobs", "[" + ctime(time(),4) + "] " + 
					ob->query_cap_name() + 
                    " sube "+gps+" pgs ("+base_name(this_object())+") "+
					"(a nivel "+(ob->query_job_level()+lvl)+").\n");
        }
    }
}

// Whether a player may give up this job now; asked before they are asked to
// confirm it. on_leave is called once they have.
int can_abandon(object player)
{
  return 1;
}

/**
 * Called when a player leaves the job, once they confirmed it.
 * @param  player  who leaves
 * @return 1 lets them leave, 0 keeps them in the job
 */
int on_leave(object player)
{
  // Quest removal, waiting on a quest system: hexagon has neither
  // <quests.h> nor the query_quests / remove_quest half of the player.
  //
  // if (player->query_player())
  // {
  //   string * quests = player->query_quests();
  //   int i;
  //   object q;
  //
  //   // We search all the quests until we find the one for this job
  //   for (i = 0; i < sizeof(quests); i+=2)
  //   {
  //     q = load_object(quests[i]);
  //
  //     if (!q)
  //       continue;
  //
  //     if ((q->query_quest_category() == QUEST_JOB) &&
  //         (q->query_quest_job() == base_name(this_object())))
  //     {
  //       player->remove_quest(quests[i]);
  //     }
  //   }
  // }

  return 1;
}

// Called when the player joins, to add feats, etc
int join_player(object pl)
{
	return 1;
}
