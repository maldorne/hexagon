/* 
 * Base job file...
 * neverbot -  March 18, 2002
 * 
 * auto-remove job quests when abandoning the job
 * neverbot 06/2014
 */

inherit "/lib/core/object.c";

#define BASIC_JOB_XP_COST 2000

// mixed *job_commands = ({ });

int query_legal_race(string race) { return(1); }
int query_legal_base_race(string race) { return(1); }
int query_legal_class(string my_class) { return(1); }

int query_legal_player(object player){
   return (query_legal_race(player->query_race_name()) &&
           query_legal_base_race(player->query_base_race_name()) &&
           query_legal_class(player->query_class_name()) );
}

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
int query_max_level() { return 100; }

int query_gp_dice() { return 5; }

int query_channel() { return 0; }

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

int player_quit(object player)
{
    return 1;
}

void new_levels(int lvl, object ob) 
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

// Will be called when a player tries to leave the job
// If we return 1, we allow them to do it
// If we return 0, the player cannot stop having this job
int job_abandon(object player)
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
