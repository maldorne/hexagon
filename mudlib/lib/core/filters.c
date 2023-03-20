
// common filter functions to be used with filter_array from several places
// moved here so we have less functions in the auto object

// use them like this:
// list = filter_array(list, "not_hidden", load_object("/lib/handlers/filters"));

int not_hidden(object ob, varargs mixed extra...)
{
  return (objectp(ob) &&
         !ob->query_hide_shadow() &&
         !ob->query_invis() &&
         !ob->query_hidden_object() &&
         !(ob->is_money() && ob->query_number_coins() == 0));
}

// filter items which responds to dynamic ids, like the room items
// function not_dynamic_ids;
int not_dynamic_ids(object ob, varargs mixed extra...)
{
  return (!ob->is_room_item());
}

// filter only coder users
int coders(object ob, varargs mixed extra...)
{
  return (ob->query_coder());
}

