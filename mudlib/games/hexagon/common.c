/*
 *  Inherit base room for domain common rooms
 *  Orginal from who knows where...
 *  Fixed and cleaned up code by Radix@FR : Dec 17, 1996
 *  Domain security added by Radix@FR : Dec 18, 1996
 *  Version 1.0
 *
 *  Updated to common game room for Hexagon, neverbot 12/2020
*/

inherit "/lib/room.c";

#include <translations/exits.h>
#include <areas/common.h>

string game_name;
object door, board;

int do_list(string arg);
int add_creator(string arg);
int remove_creator(string arg);
int add_project(string arg);

void init() 
{
  ::init();
  add_action("do_list", "list");
  add_action("add_creator", "add");
  add_action("remove_creator", "remove");
  add_action("set_project", "project");
}

void create()
{
  // default values
  game_name = "hexagon";

  ::create();

  // if we do not have a short desc after create/setup, add a default one
  if (!strlen(query_short())) {
    set_short("Hexagon common room");
  }

  door = add_exit(DIR_COMMON, CODER_COMMON_ROOM, "door");  
  if (door){
    door->set_init_status(0);
    door->reset_message();
    door->set_dir_other_side(game_name);
  }

  // back exit
  door = "/home/common"->add_exit(game_name, "/games/"+game_name+"/common.c", "door");
  if (door){
    door->set_init_status(0);
    door->reset_message();
    door->set_dir_other_side(DIR_COMMON);
  }
  "/home/common"->renew_exits();

  board = clone_object("/lib/obj/board.c");
  board->set_datafile(game_name + "-common");
  board->move(this_object());
}

void dest_me()
{
  if (door)
    door->dest_me();
  if (board)
    board->dest_me();
  ::dest_me();
}

string query_game_name() { return game_name; }
void set_game_name(string str) { game_name = str; }

int do_list(string arg) 
{
  string *members, *domains, mem_list;
  mixed mast;
  int i, j;

  if (arg == "all")
    domains = get_dir("/games/");
  else 
    if (!arg)
      domains = ({ game_name });
    else 
    {
      notify_fail("Syntax: list\n     list all\n");
      return 0;
    }
  if (sizeof(domains) > 1) 
    write("Todos los Dominios:\n");
  else 
    write("Este Dominio:\n");
  for(i=0; i<sizeof(domains); i++) 
  {
    mast = "/games/" + domains[i] + "/master";
    members = (string *)mast->query_members(arg);
    printf("%8s tiene %2d miembros y es gobernado por %s.\n", 
          domains[i], sizeof((string *)mast->query_members()),
          capitalize((string)mast->query_dom_lord()));
    mem_list = "";
    for(j=0; j<sizeof(members); j++)
      mem_list+=capitalize(members[j])+
             (j==sizeof(members)-1?".":",");
    write(mem_list + "\n\n");
  }
  return 1;
}

int add_creator(string arg) 
{
  mixed mast;
  
  if (!arg) 
  {
    notify_fail("Syntax: add <coder>\n");
    return 0;
  }
  arg = (string)this_player()->expand_nickname(arg);
  mast = "/games/" + game_name + "/master";
  if ((string)mast->query__dom_lord() != geteuid(this_player())) 
  {
    notify_fail("No eres el Lord de este Dominio.\n");
    return 0;
  }
  if (!mast->add_member(arg)) 
  {
    notify_fail("Fallo anyadiendo a "+capitalize(arg)+" en "
             +game_name + ".\n");
    return 0;
  }
  write("Creador "+capitalize(arg)+" anyadido en "+game_name+".\n");
  return 1;
}

int remove_creator(string arg) 
{
  mixed mast;
  
  if (!arg) 
  {
    notify_fail("Syntax: remove <coder>\n");
    return 0;
  }
  arg = (string)this_player()->expand_nickname(arg);
  mast = "/games/"+game_name+"/master";
  if ((string)mast->query_dom_lord() != geteuid(this_player())) 
  {
    notify_fail("No eres el Lord de este Dominio.\n");
    return 0;
  }
  if (!mast->remove_member(arg)) 
  {
    notify_fail("Fallo al quitar a "+capitalize(arg)+" de "+
             game_name+".\n");
    return 0;
  }
  write("Miembro "+capitalize(arg)+" borrado de "+game_name+".\n");
  return 1;
}

int set_project(string arg) 
{
  mixed mast;
  string proj;
  if (!arg || sscanf(arg,"%s %s", arg, proj) != 2)
  {
    notify_fail("Uso: proyecto <creador> <proyecto>\n");
    return 0;
  }
  arg = (string)this_player()->expand_nickname(arg);
  mast = "/games/"+game_name+"/master";
  if ((string)mast->query_dom_lord() != geteuid(this_player())) 
  {
    notify_fail("No eres el Lord de este Dominio.\n");
    return 0;
  }
  if (!mast->set_project(arg, proj)) 
  {
    notify_fail("Fallo al asignar el proyecto \""+capitalize(arg)
            +"\" en "+game_name+".\n");
    return 0;
  }
  write("Proyecto "+capitalize(arg)+" asignado en "+game_name+".\n");
  return 1;
}

