
// /global/cmd/cmd.c -- Inherit for object-based commands

// Added verb, Baldrick dec '97
// Added query_cmdp to return function pointer, 980626 Skullslayer
// Ported to dgd, neverbot aug '15

#pragma optimize

#include <mud/cmd.h>
#include <common/properties.h>

static int position, dodest, doclone;
static object command_giver;

void create()
{
  position = 0;
  dodest = 0;
  doclone = 0;

  seteuid(getuid());
  this_object()->setup();
}

void dest_me()
{
  destruct(this_object());
}

void clean_up()
{
  dest_me();
}

void setup() { return; }

/* Added by Baldrick.
 * using this_player ain't good.
int notify_fail(string fa)
  {
  this_player()->set_notified(1);
  fail_msg = fa;
  return 0;
}
*/

static int cmd(string tail, object thisob, string verb) { return 0; }

string query_usage() { return ""; }
string query_short_help() { return ""; }

int _cmd(string tail, object thisob, string verb)
{
  string euid;
  int ret;

  if (previous_object(1) != find_object(CMD_HANDLER))
    return 0;

  command_giver = thisob;
  euid = geteuid(command_giver);

  // Set the euid to that of either the creator, or to PLAYER.
  seteuid(euid);

  // Will check who is using command and if allowed here sort of :)
  switch(position)
  {
    case ADMIN_CMD:
      if (!thisob->query_admin() && !thisob->query_highlord(euid))
        return 0;
      break;

    case CODER_CMD:
      if (!thisob->query_coder())
        return 0;
      break;

    case PLAYER_CMD:
      if (!thisob->query_player())
        return 0;
      break;

    case USER_CMD:
      // always allowed
      break;
  }

  // neverbot, 21/02/04
  // cmds should not be executed if we have a passed out
  if ((position == PLAYER_CMD) && thisob->query_timed_property_exists(PASSED_OUT_PROP) )
  {
    notify_fail(thisob->query_timed_property(PASSED_OUT_PROP));
    return 0;
  }

  if (thisob->query_timed_property_exists(LOADING_PROP))
  {
    notify_fail("Tu equipo aún se está cargando, espera un segundo.\n");
    return 0;
  }

  ret = cmd(tail, thisob, verb);

  seteuid(getuid(this_object()));

  return ret;
}

int _query_position() { return position; }
int _query_dodest() { return dodest; }
int _query_doclone() { return doclone; }

