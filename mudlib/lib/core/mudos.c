
/*
 *  Unique object storing global values about the current
 *  operation being performed, providing functions to the
 *  mudlib to emulate MudOS internals.
 */

#include <mud/mudos.h>
#include <mud/config.h>

inherit hb "/lib/core/heart_beats";

private static object initiator_user, initiator_player, initiator_object;
private static string current_command;
private static string current_verb;
private static string notify_fail_msg;

private static mixed ** call_outs;

void create()
{
  initiator_user = nil;
  initiator_player = nil;
  initiator_object = nil;

  current_verb = "";
  current_command = "";
  notify_fail_msg = "";
  call_outs = ({ });
  hb::create();
}

// TODO security in set_current_verb
string query_current_verb() { return current_verb; }
void set_current_verb(string v) { current_verb = v; }

// TODO security in set_current_command
string query_current_command() { return current_command; }
void set_current_command(string v) { current_command = v; }

// TODO security in set_notify_fail_msg
string query_notify_fail_msg() { return notify_fail_msg; }
void set_notify_fail_msg(string msg) { notify_fail_msg = msg; }

// useful for logging
private _change_initiator_object(object ob)
{
  if (CONFIG_LOG_INITIATOR_OBJECTS)
    stderr(" @@@@@@@@@ new initiator_object: " + to_string(ob));
  initiator_object = ob;
}

private _change_initiator_player(object ob)
{
  if (CONFIG_LOG_INITIATOR_OBJECTS)
    stderr(" @@@@@@@@@ new initiator_player: " + to_string(ob));
  initiator_player = ob;
}

private _change_initiator_user(object ob)
{
  if (CONFIG_LOG_INITIATOR_OBJECTS)
    stderr(" @@@@@@@@@ new initiator_user: " + to_string(ob));
  initiator_user = ob;
}

int set_initiator_object(object ob)
{
  if (!mudlib_privileges())
  {
    if (objectp(ob))
      stderr(" *** illegal set_initiator_object with <" + object_name(ob) + ">\n");
    return 0;
  }

  _change_initiator_object(ob);

  return 1;
}

int set_initiator_user(object user)
{
  if (!mudlib_privileges())
  {
    if (objectp(user))
      stderr(" *** illegal set_initiator_user with <" + object_name(user) + ">\n");
    return 0;
  }

  _change_initiator_user(user);

  if (!objectp(user) || !user->player())
    _change_initiator_player(nil);
  else
    _change_initiator_player(user->player());

  return 1;
}

int set_initiator_player(object player)
{
  if (!mudlib_privileges())
  {
    if (objectp(player))
      stderr(" *** illegal set_initiator_player with <" + object_name(player) + ">\n");
    return 0;
  }

  _change_initiator_player(player);

  // if (!objectp(player) || !player->user())
  //   _change_initiator_user(nil);

  return 1;
}

object this_player(varargs int i)
{
  if (i && (i == 1))
  {
    // if (initiator_player)
    //   return initiator_player;
    // return initiator_object;
    return initiator_user;
  }

  return initiator_object;
}

object this_user()
{
  if (initiator_user)
    return initiator_user;

  return nil;
}

// call init in ob2 with ob1 as this_player()
void do_init(object ob1, object ob2)
{
  object old_this_user, old_this_player;

  if (!mudlib_privileges())
  {
    stderr(" *** illegal do_init with <" + object_name(ob1) + "> and <" + object_name(ob2) + ">\n");
    return;
  }

  old_this_player = initiator_player;
  old_this_user = initiator_user;

  stderr(" ~~~ mudos::do_init()\n");
  set_initiator_object(ob1);
  set_initiator_player(ob1);

  call_other(ob2, "__call_other", "init");
}

static void _purge_call_out_stack()
{
  int i, time, size;

  size = sizeof(call_outs);

  if (!size)
    return;

  time = time();

  for (i = 0; i < size; i++)
  {
    if (call_outs[i][CALL_OUT_STACK_TIME] <= time)
    {
      if (i+1 > size)
        call_outs = ({ });
      else
        call_outs = call_outs[i+1..];

      i--;
      size--;
    }
    else
      break;
  }
}

int query_call_out_stack_size()
{
  _purge_call_out_stack();
  return sizeof(call_outs);
}
mixed ** query_call_out_stack()
{
  _purge_call_out_stack();
  return call_outs;
}

int _store_call_out(object ob, int handle, string func, int delay, varargs mixed args...)
{
  int i, time;

  if (delay < 0)
    delay = 0;

  // every time we modify or check the list, we purge the pending call_out
  _purge_call_out_stack();

  time = time() + delay;

  // sorted so the purge_call_out_stack call is faster
  for (i = sizeof(call_outs) - 1; i >= 0; i--)
  {
    if (time >= call_outs[i][CALL_OUT_STACK_TIME])
      break;
  }

  call_outs = insert(call_outs, ({ ob, handle, func, time, args, this_player(), this_user() }), i+1);

  if (CONFIG_LOG_CALL_OUTS)
  {
    stderr(" *** stored call_out func <" + func + "> on object <" + object_name(ob) + ">\n");
    // stderr("     info stored: " + to_string(({ ob, handle, func, time, args, this_player(), this_user() })) + "\n");
    // stderr(" *** all stored call_outs: " + to_string(call_outs) + "\n");
  }

  return handle;
}

// this function will be called when the call_out is done
int _call_out(object ob, object player, object user, string func, varargs mixed args...)
{
  object old_this_object, old_this_player, old_this_user;

  if (CONFIG_LOG_CALL_OUTS)
  {
    stderr(" ~~~ mudos::call_out() func <" + func + "> on object <" + object_name(ob) + ">\n");
    // stderr(to_string(initial_object()));
    // stderr("object " + to_string(ob));
    // stderr("player " + to_string(player));
    // stderr("user " + to_string(user));
    // stderr("args " + to_string(args));
  }

  // every time we modify or check the list, we purge the pending call_out
  _purge_call_out_stack();

  // preserve current context
  old_this_object = initiator_object;
  old_this_player = initiator_player;
  old_this_user = initiator_user;

  // restore the context when the call_out was programmed
  _change_initiator_object(ob);
  _change_initiator_player(player);
  _change_initiator_user(user);

  catch(call_other(ob, func, args...));

  if (CONFIG_LOG_CALL_OUTS)
    stderr(" ~~~ end mudos::call_out()\n");

  // restore current context
  _change_initiator_object(old_this_object);
  _change_initiator_player(old_this_player);
  _change_initiator_user(old_this_user);
}

int _remove_call_out(object ob, int handle)
{
  int i, ret;

  // every time we modify or check the list, we purge the pending call_out
  _purge_call_out_stack();

  for (i = sizeof(call_outs) - 1; i >= 0; i--)
  {
    if ((call_outs[i][CALL_OUT_STACK_OB] == ob) &&
        (call_outs[i][CALL_OUT_STACK_HANDLE] == handle))
    {
      // stderr("REMOVE_CALL_OUT: " + to_string(call_outs) + " " +
      //   object_name(ob) + " " +
      //   handle + "\n");

      // time left to the call_out
      ret = call_outs[i][CALL_OUT_STACK_TIME] - time();

      // remove the pending call_out from the list
      call_outs = delete(call_outs, i, 1);

      return ret;
    }
  }

  return -1;
}

int _find_call_out(object ob, string func)
{
  int i;

  // every time we modify or check the list, we purge the pending call_out
  _purge_call_out_stack();

  for (i = sizeof(call_outs) - 1; i >= 0; i--)
  {
    if ((call_outs[i][CALL_OUT_STACK_OB] == ob) &&
        (call_outs[i][CALL_OUT_STACK_FUNC] == func))
    {
      return call_outs[i][CALL_OUT_STACK_TIME] - time();
    }
  }

  return -1;
}

mixed * _call_out_info()
{
  mixed * ret;
  int i, time;

  ret = ({ });
  time = time();

  // every time we modify or check the list, we purge the pending call_out
  _purge_call_out_stack();

  // An array is returned,
  // where every item in the array consists 4 elements: the object,
  // the function, the delay to go, and the optional argument.
  for (i = 0; i < sizeof(call_outs); i++)
  {
    ret += ({
              ({
                call_outs[i][CALL_OUT_STACK_OB],
                call_outs[i][CALL_OUT_STACK_FUNC],
                call_outs[i][CALL_OUT_STACK_TIME] - time,
              })
              +
              (sizeof(call_outs[i][CALL_OUT_STACK_ARG]) ?
                ({ call_outs[i][CALL_OUT_STACK_ARG] }) :
                ({ }))
            });
  }

  return ret;
}
