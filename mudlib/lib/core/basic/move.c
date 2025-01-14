
#include <basic/move.h>

private int move_flag;

// added Mar 3 '93 by Dank to support goback command
private object previous;

void create()
{
  previous = nil;
}

object query_prev() { return previous; }

// default to being gettable and dropable...
// modify as you will...
void reset_get() { move_flag |= GET; }   // can't be gotten
void set_get() { move_flag &= ~GET; }    // can be gotten
void reset_drop() { move_flag |= DROP; } // can't be dropped
void set_drop() { move_flag &= ~DROP; }  // can be dropped

// these should have been called "query...", but they're not
int drop() { return move_flag & DROP; }
int get() { return move_flag & GET; }

int gettable() { return !get(); }
int droppable() { return !drop(); }

void set_move_flag(int i) { move_flag = i; }

int move(mixed dest, varargs mixed messin, mixed messout)
{
  previous = environment();

  if (stringp(dest))
    dest = load_object(dest);

  if (!objectp(dest))
    return MOVE_EMPTY_DEST;  

  // previous environment can let go
  if (previous && !previous->test_remove(this_object(), move_flag & DROP))
    return MOVE_NO_DROP;

  // destination can accept the object
  if (!dest->test_add(this_object(), move_flag & GET))
    return MOVE_NO_GET;

  // event_exit
  if (previous)
    event(previous, "exit", messout, dest, this_object());

  // efun that simulates the mudos inventory and
  // environment handling
  ::move(dest);

  // event_enter
  event(dest, "enter", messin, previous, this_object());

  return MOVE_OK;
}

void dest_me()
{
  int i;
  object* obs;
  object ob;

  if (environment())
    event(environment(), "dest_me");

  // Destruct shadows of this object, Wonderflug 96
  obs = ({ });
  ob = shadow(this_object(), 0);

  while (ob)
  {
    obs += ({ ob });
    ob = shadow(ob, 0);
  }

  for (i = 0; i < sizeof(obs); i++)
    if (obs[i])
      destruct(obs[i]);

  // if this object is a user, do not destruct it right now,
  // just flag it and it will be destroyed after processing everything
  if (this_object()->query_user() && 
      query_ip_number(this_object()))
  {
    call_out("dwep", 0);
  }
  else
    destruct(this_object());
}

/* Do not I repeat do not mask this function.
 * is used when all else fails to dest the object...
 * If we find you masking this function... We will cut you up
 * into little bits... slowly
 */
nomask mixed dwep()
{
  destruct(this_object());
  // ::destruct(this_object());
  return "Destruct With Extreme Prejuce";
}

mixed * stats()
{
  return ({
    ({"Move Flag (nosave)", move_flag, }),
    ({"Previous (nosave)", previous, }),
     });
}
