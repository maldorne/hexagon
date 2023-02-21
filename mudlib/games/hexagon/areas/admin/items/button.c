
inherit "/lib/item.c";

void setup() 
{
  reset_get();
  reset_drop();
  set_weight(10000000);
  
  set_name("red button");

  set_short("%^BOLD%^%^RED%^Red Button%^RESET%^");
  add_alias(({ "button", "red button", "red" }));

  set_main_plural("%^BOLD%^%^RED%^Red Buttons%^RESET%^");
  add_plural(({ "buttons", "red buttons", "reds" }));
  
  set_long("This button destroys every object in the room but players.\n" +
    "Use 'push button' to use it, but BE CAREFUL.\n");
}

void init() 
{
  ::init();
  add_action("do_push", "push");
}

int do_push(string str) 
{
  object ob, temp;
  int changed;
  changed = 1;

  if (!strlen(str) || ((str != "button") && (str != "red button")))
  {
    notify_fail("Push what? Maybe you want to 'push button'.\n");
    return 0;
  }

  tell_object(this_player(), "You push the red button.\n");
  tell_room(environment(this_object()), this_player()->query_cap_name() +
    " pushes the red button.\n", this_player());

  while (changed) 
  {
    changed = 0;
    ob = first_inventory(environment(this_object()));

    while (ob) 
    {
      // everything except players and doors
      if (ob != this_object() && !interactive(ob) && !ob->query_door()) 
      {
        changed = 1;
        temp = ob;
      }
    
      ob = next_inventory(ob);
    
      if (temp) 
      {
        if (temp->query_short())
          tell_room(environment(this_object()), temp->query_short() + " is destroyed.\n");
        
        temp->dest_me();
      }
    }
  }

  return 1;
}

