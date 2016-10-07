/*
 * Common coder room
 */

#define ADMIN "/lib/areas/admin/"

inherit "/lib/room";

// object board;

void init()
{
  add_action("do_ttt", "test");
  ::init();
}

int do_ttt(string str)
{
  tell_object(this_player(), "YAH! We've done a '"+query_verb()+"' with '"+str+"'\n");

  notify_fail("But i want a fail message.\n");
  return 0;
}

void reset() 
{
  // int i,n;
  // object *all;
  
  // if(!board) 
  // {
  //   board = clone_object("/obj/misc/board");
  //   board->set_datafile("programadores_comun");
  //   board->move(this_object());
  // }
  
  /* Try to clean some memory */
  // n = reclaim_objects();
  
  // for(i = 0; i < sizeof(all = children("/global/god.c")); i++)
  // {
  //   if(all[i])
  //     tell_object(all[i],"[ Objetos liberados (reclaim_object): "+ n +" ]\n");
  // }

  //::reset();
}

void setup() 
{
  set_light(60);
  set_short("Habitación común de los programadores");
  set_long("Esta habitación es en la que los programadores de "+mud_name()+
        " se reúnen para discutir temas de cualquier tipo concernientes al juego. "+
        "Utiliza el tablón para poner cualquier nota y dejar tus opiniones sobre "+
        "cualquier tema de discusión actual, también es tu mud... \n");

  // add_exit("cc", "/d/ciudad_capital/common", "standard");
  add_exit("sur", ADMIN + "admin1", "standard");
  add_exit("norte", "/room/admin/meeting.c", "standard");
}

void dest_me() 
{
  // if (board)
  //   board->dest_me();
  ::dest_me();
}
