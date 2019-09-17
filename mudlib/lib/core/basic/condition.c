/*
 * Traducido por Ishmar@Rl
 *
 * Condition.c, retocado para Cc, neverbot 4/03
 * Eliminado max_cond, neverbot 8/08
 *
 */

#include <basic/condition.h>

int cond;

void create() 
{
  cond = MAX_COND;
}

/* this should be masked by the object */
void break_me()
{ 
  string str;
  string str_aux;

  // Añadido mensaje para que el player se entere de que el arma se ha roto
  if(this_object()->query_weapon())
  {
    str_aux = "El";
    str = "arma";
  }
  else if(this_object()->query_armour())
  {
    str_aux = "La";
    str = "armadura";
  }
  else if(this_object()->query_shield())
  {
    str_aux = "El";
    str = "escudo";
  }
  else
  {
    str_aux = capitalize(this_object()->query_article());
    str = this_object()->query_name();
  }

  tell_player(this_object(), "Tu "+str+" se acaba rompiendo a causa del uso.\n");
  tell_room(environment(environment(this_object())), 
              "%^BOLD%^" + str_aux + " " +str+" de "+
              environment(this_object())->query_cap_name()+" se rompe a "+
              "causa del uso.%^RESET%^\n", environment(this_object()));
  this_object()->dest_me();
}

string cond_string() 
{
  string aux;
  aux = this_object()->query_vocal();
  /*
  if (this_object()->query_gender() == 2)
       aux = "a";
  else 
       aux = "o";
  */

  switch ((100*cond)/MAX_COND) 
  {
    case 0 :
      return "Está destrozad"+aux+".\n";
    case 1..10 :
      return "Está en muy malas condiciones.\n";
    case 11..20 :
      return "Está en malas condiciones.\n";
    case 21..30 :
      return "Está en un estado más bien pobre.\n";
    case 31..40 :
      return "Está bastante estropead"+aux+".\n";
    case 41..50 :
      return "Está estropead"+aux+".\n";
    case 51..60 :
      return "Está un poco estropead"+aux+".\n";
    case 61..70 :
      return "Está en un estado razonable.\n";
    case 71..80 :
      return "Está en buenas condiciones.\n";
    case 81..90 :
      return "Está en muy buenas condiciones.\n";
    default :
      return "Está en excelente estado.\n";
  }
}
 
// void set_max_cond(int i) { max_cond = i; }

void set_cond(int i) 
{
  cond = i;
  // if ((cond < lowest_cond) || (cond < 0)){
  if (cond < 0)
  {
    this_object()->break_me();
    return;
  }
}

int adjust_cond(int i) 
{
  cond += i;
  // if ((cond < lowest_cond) || (cond < 0)) {
  if (cond < 0)
  {
    this_object()->break_me();
    return 0;
  }

  if (cond > MAX_COND)
    cond = MAX_COND;
  return cond;
}

int query_cond() { return cond; }
int query_max_cond() { return MAX_COND; }
int query_condition() { return cond; }
int query_max_condition() { return MAX_COND; }

int set_percentage(int i) 
{
  if (i > 100) i = 100;
  cond = (MAX_COND*i)/100;
}

// stats añadido
mixed stats()
{
  return ({
           ({ "Cond", cond, }),
         });
}

