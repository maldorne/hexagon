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
inherit info   "/lib/user/roles/coder_info.c";
inherit comm   "/lib/user/roles/coder_communicate.c";

inherit        "/lib/user/roles/coder_utils.c";

void create()
{
  base::create();
  path::create();
  editor::create();
  obj::create();
  info::create();
  comm::create();
}

static void start_role(object user)
{
  base::start_role(user);
  path::start_role(user);
}

static void role_commands()
{
  // player commands
  base::role_commands();

  path::role_commands();
  editor::role_commands();
  obj::role_commands();
  info::role_commands();
  comm::role_commands();
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
                 obj::stats() +
                 info::stats() +
                 comm::stats();
}