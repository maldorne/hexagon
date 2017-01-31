// Base citizenship file...
// Folken 4/2003
// No creo que sean necesarios ni canales para el grupo ciudadania, ni
// comandos propios, pero dejo el codigo comentado por si acaso
//      Folken 4/2003

inherit "/std/object.c";

// mixed *job_commands = ({ });
string init_room = "";

int query_legal_race(string race) { return(1); }
int query_legal_player(object player) { return 1; }

void set_init_room(string str) { init_room = str; }
string query_init_room() { return init_room; }

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

