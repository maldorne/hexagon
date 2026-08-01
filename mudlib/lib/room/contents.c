
inherit contents "/lib/core/basic/contents";

object * hidden_objects;

void create()
{
  hidden_objects = ({ });  
}

void init()
{
  int i;
  
  hidden_objects -= ({ nil });

  for (i = 0; i < sizeof(hidden_objects); i++)
    hidden_objects[i]->init();  
}

object * query_hidden_objects() { return hidden_objects; }
void set_hidden_objects(object * obs) { hidden_objects = obs; }

object * add_hidden_object(object ob)
{
  hidden_objects += ({ob});
  return hidden_objects;
}

string query_contents(varargs string str, object *ob)
{
  if (!hidden_objects)
    hidden_objects = ({ });
  if (ob)
    ob -= hidden_objects;
  else
    ob = all_inventory(this_object()) - hidden_objects;
  return contents::query_contents(str, ob);
}

