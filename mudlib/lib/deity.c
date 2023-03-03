// Base deity file...
// Eressea -  Marzo 18, 2002
// No creo que sean necesarios ni canales para el grupo deidad, ni comandos 
// propios por tener una deidad en concreto, pero dejo el codigo comentado por si acaso
//    Eressea 3/2002

inherit "/std/object.c";

// mixed *job_commands = ({ });

int query_legal_race(string race) { return(1); }
int query_legal_player(object player) { return 1; }

// int query_channel() { return 0; }
/*
int add_job_command(string name, int add_if_player) 
{
   if (member_array(name, job_commands) != -1)
      return 0;
   job_commands += ({ name, ({ add_if_player }) });
   return 1;
}
*/
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

