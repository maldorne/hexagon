/* 
 * Base job file...
 * Folken@Rl -  March 18, 2002
 * 
 * auto-remove job quests when abandoning the job
 * neverbot 06/2014
 */

#include <quests.h>

inherit "/std/object.c";

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

// Folken 07/12/06
int query_next_level_xp(object player)
{
  int res = player->query_job_level() * BASIC_JOB_XP_COST;
  if (res == 0) 
    return BASIC_JOB_XP_COST;
  return res;
}

// Cambiar por otro numero para establecer un limite maximo
// para este oficio
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

void on_death(object player, object killer)
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
    int e = 0;
    int gps = 0;
    
    ob->reset_all();
    
    // Subimos todos los niveles
    for (e = 0; e < lvl; e++)
    {
        // Solo subimos gps si es un nuevo nivel maximo
        if (ob->query_job_level() >= ob->query_max_job_level())
        {
            // En el primer nivel se obtiene el maximo
            if (ob->query_job_level() == 0)
                gps = query_gp_dice() + 5;
            else 
                gps = roll(1, query_gp_dice()) + 5;
          
            if (gps <= 0) 
                gps = 1;
            
            ob->set_max_gp(ob->query_max_gp() + gps);
    
            // Logeamos en /secure, para que nadie pueda ver estas cosas
            if (interactive(ob))
                secure_log_file("jobs", "[" + ctime(time(),4) + "] " + 
					ob->query_cap_name() + 
                    " sube "+gps+" pgs ("+base_name(this_object())+") "+
					"(a nivel "+(ob->query_job_level()+lvl)+").\n");
        }
    }
}

// Se llamara cuando un jugador intente abandonar el oficio
// Si devolvemos 1, le permitimos hacerlo
// Si devolvemos 0, el jugador no puede dejar de tener este oficio
int job_abandon(object player)
{
  if (player->query_player())
  {
    string * quests = player->query_quests();
    int i;
    object q;
    
    // Buscamos todas las quests hasta encontrar la de este oficio 
    for (i = 0; i < sizeof(quests); i+=2)
    {
      q = load_object(quests[i]);

      if (!q)
        continue;

      if ((q->query_quest_category() == QUEST_JOB) && 
          (q->query_quest_job() == base_name(this_object())))
      {
        player->remove_quest(quests[i]);
      }
    }
  }
  
	return 1;
}

// Se llama cuando el jugador se alista, para añadir dotes, etc
int join_player(object pl)
{
	return 1;
}
