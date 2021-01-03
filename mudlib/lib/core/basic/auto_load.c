
/* I was hoping I never had to touch this code..
* But it seems like i have to.
* Adding a "loading" property, so that players won't be able to quit before 
* all equipment are loaded.
* Baldrick, nov '94
* Taniwha 1995, 1996 (ad nauseum)
*/

/* First, add a global, shame. */
static int stuff;

void auto_clone(mixed arg);

string *create_auto_load(object *obs) 
{
  mixed s, s1, s3;
  mixed auto_string;
  string *fname2;
  int i;
  
  if (!sizeof(obs)) 
    return ({ });
    
  auto_string = ({ });
  
  /* do this backwards, means it comes out right on logon */
  for (i = sizeof(obs) -1; i >= 0; i--)
  {
    s = s3 = ([ ]);

    if (!function_exists("query_auto_load", obs[i]) ||
       (!catch(s = obs[i]->query_auto_load()) && !s))
    {
      catch(s = obs[i]->query_static_auto_load());
      catch(s3 = obs[i]->query_dynamic_auto_load());

      if (!map_sizeof(s) && !map_sizeof(s3))
        continue;
    /*
    * Fix by Aragorn. Used to be !s below
    * Name of function is a bit mismatching (query_auto_load())
    * because if it returns something it won't...
    */
    }
    else 
    {
      if (mappingp(s) && map_sizeof(s))
        continue;
      else
        s3 = ([ ]);
    }
      
    // Taniwha, there's some evidence that things aren't SAVING maybe something dests in query_auto_load()
    if (!obs[i]) 
      continue;
      
    s1 = base_name(obs[i]);
    auto_string += ({ 1, s1, ({ s, s3 }) });
  }
  
  return auto_string;
} /* create_auto_load() */

/*
void birthday_gifts() 
{
  if (!sizeof(find_match("birthday card", this_object())))
    if (this_object()->query_is_birthday_today())
    if (!catch(call_other("/obj/b_day/card", "??")) &&
  !catch(call_other("/obj/b_day/demon", "??"))) {
    call_out("card_arrives", 5);
  }
} 

int query_special_day(string type) 
{
  string s1, s2, month;
  int day;
  sscanf(ctime(time()), "%s %s %d %s", s1, month, day, s2);
  switch (type) {
    case "cabbage day" :
    if (month == "Feb" && day == 17)
      return 1;
    return 0;
    case "test day" :
    if (month == "Feb" && day == 16)
      return 1;
    return 0;
  }
  return 0;
} 

void card_arrives() 
{
  object card;
  card = (object)"/global/cloner"->clone("/obj/b_day/card");
  card->move(this_object());
  tell_room(environment(), "You hear a rumbling in the distance. " +
    "Then, suddenly, a malformed goblin wizzes past you, " +
    "frantically pedalling a fire-engine red tricycle!\n");
  write("You feel something thrust into your hand by a greener, " +
    "wartier one.\n");
} 
*/

void make_iou(mixed stuff_tmp, object dest)
{
  object iou;
  log_file("loader",  "["+ctime(time(), 4)+"] "+ 
    (string)dest->query_cap_name()+" has ["+(string)stuff_tmp[1]+
    "] which did not load.\n");
  catch(iou = clone_object("/obj/misc/iou"));
  
  if (iou)
  {
    iou->add_auto_string(stuff_tmp);
    catch(iou->move(dest));
  }
}

object *load_auto_load(mixed auto_string, object dest) 
{
  object ob, *obs;
  int i;
  obs = ({ });
  
  if (stringp(auto_string))
    return ({ });
    
  if (!sizeof(auto_string))
    return ({ });
    
  /* This for loop gives errors sometimes..
  * should be made bulletproof..
  * Baldrick.
  */
  for (i = 0; i < sizeof(auto_string); i+=3)
  {
    /* Maybe this line makes it bulletproof?
    * Baldrick May '95
    */
    // If there's a file name here
    if (strlen(auto_string[i+1]))
    {
      // Try to clone one
      catch(ob = clone_object(auto_string[i+1]));
      
      if (ob)
      {
        // This initializes the data in the object,and saved vars
        auto_clone( ({ ob, auto_string[i+2], dest }));
        // Useful this, you can tell what didn;t laod for example.
        obs += ({ ob });
      }
      
      if (!ob)
      {
        // And if it didn't work , give em an IOU
        make_iou( ({auto_string[i], auto_string[i+1], auto_string[i+2]}), dest);
      }
    } /* If auto_string */
  } /* for */
  
  return obs;
} /* load_auto_load() */

void auto_clone(mixed arg) 
{
  if (!arg[0])
  {
    tell_object(this_object(),"¡Ups, algo va mal!\n");
    stuff --;
    return;
  }

  if (sizeof(arg[1]) == 1 || !arg[1][1])
  {
    arg[0]->init_arg(arg[1][0]);
  }
  else
  {
    arg[0]->init_static_arg(arg[1][0]);
    arg[0]->init_dynamic_arg(arg[1][1]);
  }
  
  // Taniwha 1995, if the container hasn't loaded, what shall we do ?
  if (arg[0] && arg[2] && !(arg[0]->move(arg[2]))); // if they exist AND the move succeeds
  else
  {
    log_file("loader", "["+ctime(time(), 4)+"] "+ (string)this_object()->query_cap_name()+
      " dropped ["+(string)arg[0]->query_name()+"] when logging in.\n");
    tell_object(this_object(),"Ups, se te ha caído algo (quizá estés cargando demasiadas cosas y no puedas con todo).\n");
    if (arg[0])
      arg[0]->move(environment());
  }
  
  stuff--;
} /* auto_clone() */

/* The really stupid thingie.. 
* Baldrick.
*/
int query_loading()
{
  if (stuff<1)
    return 0;
  return 1;
}

/* In case of bugs.. */
void reset_stuff()
{
  stuff=0;
  return;
}

/* Hamlet added me -- I'm used for the 'update' command. */
string * create_update_auto_load(object ob)
{
  string s, s1, s3;
  mixed auto_string;
  int j;

  if (!ob) return ({ });

  if (function_exists("query_static_auto_load", ob))
    catch(s = (string)ob->query_static_auto_load());
  if (function_exists("query_dynamic_auto_load", ob))
    catch(s3 = (string)ob->query_dynamic_auto_load());

  if (!s && !s3)
    return ({ });

  if (sscanf(file_name(ob), "%s#%d", s1, j) == 2)
    auto_string = ({ 1, s1, ({ s, s3 }) });
  else
    auto_string = ({ 0, file_name(ob), ({ s, s3 }) });

  return auto_string;
} /* create_update_auto_load() */
