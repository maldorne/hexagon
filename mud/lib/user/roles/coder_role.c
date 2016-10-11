// Radix - Added wis_present call to 'cd' allowing 'cd here'
// or any other object, same as 'ed'
// December 15, 1995

//inherit "/std/basic/ls";
//inherit "/global/le";

// #define MASTER "/secure/master"

inherit base   "/lib/user/roles/player_role.c";

inherit path   "/lib/user/roles/coder_path.c";
inherit editor "/lib/user/roles/coder_editor.c";
inherit obj    "/lib/user/roles/coder_object.c";

// inherit "/global/coder/coder_object_comm.c";
// inherit "/global/coder/coder_info_comm.c";
// inherit "/global/coder/coder_communicate.c";

inherit        "/lib/user/roles/coder_utils.c";


void create()
{
  base::create();
  path::create();
  editor::create();
  obj::create();
}

static void initialize_roles(object player)
{
  path::initialize_roles(player);
}

static void role_commands()
{
  // add_action("indent", "indent");

  // neverbot 07/05
  // add_action("review", "review");

  // This may not work: (baldrick)
  // coder_object_comm::wiz_commands();
  // coder_info_comm::wiz_commands();
  // coder_communicate::wiz_commands();

  // coder_object_comm::app_commands();
  // coder_info_comm::app_commands();
  // coder_inter_comm::wiz_commands();

  // add_action("list_files","ls");
  // add_action("le", "le");

  // coder_object_comm::all_commands();
  // coder_info_comm::all_commands();

  path::role_commands();
  editor::role_commands();
  obj::role_commands();
} 

// int review() 
// {
//   player::review();
//   coder_info_comm::review();
//   return 1;
// } 


mixed stats() 
{
  return ({ }) + base::stats() + 
                 path::stats() + 
                 editor::stats() + 
                 obj::stats();
}