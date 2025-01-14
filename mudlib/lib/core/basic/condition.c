/*
 * condition.c, reviewed for Cc, neverbot 4/03
 * max_cond removed, neverbot 8/08
 *
 */

#include <basic/condition.h>
#include <language.h>

int cond;

void create() 
{
  cond = MAX_COND;
}

// this should be masked by the object
void break_me()
{ 
  string str;

  // add message so the player knows the weapon broke
  if (this_object()->query_weapon())
    str = _LANG_CONDITION_WEAP;
  else if (this_object()->query_armour())
    str = _LANG_CONDITION_ARMO;
  else if (this_object()->query_shield())
    str = _LANG_CONDITION_SHIE;
  else
    str = this_object()->query_name();

  tell_player(this_object(), _LANG_CONDITION_BROKE_MSG_ME);
  tell_room(environment(environment(this_object())), 
              _LANG_CONDITION_BROKE_MSG_ROOM, environment(this_object()));
  
  this_object()->dest_me();
}

string cond_string() 
{
  string aux;
  aux = this_object()->query_vowel();
  /*
  if (this_object()->query_gender() == 2)
       aux = "a";
  else 
       aux = "o";
  */

  switch ((100*cond)/MAX_COND) 
  {
    case 0 :
      return _LANG_CONDITION_STATUS_0;
    case 1..10 :
      return _LANG_CONDITION_STATUS_10;
    case 11..20 :
      return _LANG_CONDITION_STATUS_20;
    case 21..30 :
      return _LANG_CONDITION_STATUS_30;
    case 31..40 :
      return _LANG_CONDITION_STATUS_40;
    case 41..50 :
      return _LANG_CONDITION_STATUS_50;
    case 51..60 :
      return _LANG_CONDITION_STATUS_60;
    case 61..70 :
      return _LANG_CONDITION_STATUS_70;
    case 71..80 :
      return _LANG_CONDITION_STATUS_80;
    case 81..90 :
      return _LANG_CONDITION_STATUS_90;
    default :
      return _LANG_CONDITION_STATUS_DEF;
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
  cond = (MAX_COND * i) / 100;
}

// stats added
mixed * stats()
{
  return ({
           ({ "Cond", cond, }),
         });
}

