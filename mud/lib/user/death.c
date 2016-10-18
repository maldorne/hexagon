
#include <living/death.h>

/* second life routine... handles the player dieing. */
int second_life(object corpse, object initiator) 
{
  string str;
  int i, no_dec;
  object tmp;
  object * fightings;

  str = query_cap_name() + " matad"+G_CHAR+" por ";

  if (!sizeof(attacker_list))
    str += initiator->query_cap_name()+" (sobre sí mismo, quizá con un call)";
  else
    for (i = 0; i < sizeof(attacker_list); i++)
      if (attacker_list[i])
      {
        str += attacker_list[i]->query_name()+" <"
                +geteuid(attacker_list[i])+">";
        attacker_list[i]->stop_fight(this_object());
        no_dec += interactive(attacker_list[i]);
      }
  
  log_file("deaths", ctime(time())+": "+str + "\n");
  // log_file("DEATH2",str+" : "+sprintf("%O\n",previous_object(-1)));
  
  event(users(), "inform", str, "deaths");
  attacker_list = ({ });

  // force stop fights
  fightings = query_call_outed();

  for (i = 0; i < sizeof(fightings); i++)
    fightings[i]->stop_fight(this_object());

  remove_call_outed();

  say(query_cap_name()+" ha muerto.\n");
  // El save_me deberia ser despues de ajustar puntos de vida y xp,
  // save_me();
  hp = 0;
  gp = 0;

  // total_xp -= xp;
  // xp = 0;
  
  // Cambiado, solo quitamos un tercio de la xp que lleve encima, neverbot 04/2009
  // changed again, do not lose xp, neverbot 10/2016
  // xp -= xp / 3;

  this_object()->save_me();

  // DEATH_CHAR->person_died(query_name());

  if (!no_dec)
  {
    adjust_tmp_con(-2);
    adjust_tmp_str(-2);
    adjust_tmp_dex(-2);
    adjust_tmp_int(-2);
    adjust_tmp_wis(-2);
    adjust_tmp_cha(-2);
    adjust_tmp_per(-2);
    adjust_tmp_wil(-2);
  }

  tmp = clone_object(DEATH_SHADOW);
  tmp->setup_shadow(this_object());

  // Info para resucitar: tenemos informacion sobre cual es nuestro cuerpo
  tmp->set_my_corpse(corpse);
  return 1;
}
