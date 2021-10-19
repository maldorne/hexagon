/* 
 * functions to say if a ventures if open for business, Dimmak@Rl '99
 * reviewed for CcMud, neverbot '03
 * reviewed for Hexagon, neverbot '21
 */

string open_condition;
string attender_name;

void set_open_condition(string str){
  open_condition = str;
}

string query_open_condition() { return open_condition; }

int check_open_condition(){
  if(!open_condition) 
    return 1;
 
  notify_fail("Por diversas razones no pueden atenderte en este momento.\n");
  return call_other(this_object(),open_condition);
}

void set_attender(string str){
  attender_name = str;
  set_open_condition("standard_open_condition");
}

int standard_open_condition(){
  object *list;
  object who;
  int i;
   
  list = find_match(attender_name, this_object());
  if(!sizeof(list)) 
    return 0;

  who = list[0];
  list = who->query_attacker_list();
  for(i = 0; i < sizeof(list); i++)
    if(environment(list[i]) == this_object()) 
      return 0;
  
  return 1;
}
