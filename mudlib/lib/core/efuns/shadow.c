
#include <mud/mudos.h>

private object * _shadows;
private int _is_shadowing;

// shadow - shadow one or more functions in some object
// object shadow( object ob, int flag );

// If 'flag' is 1 or missing, then current object will shadow 'ob'. If
// 'flag' is 0, then either 0 will be returned, or the object that is
// already shadowing 'ob'.

// The master object defines the function "valid_shadow()".  If it returns 1
// the target object can't be shadowed, and the "shadow()" function will
// return 0 instead of 'ob'.

// If an object 'a' shadows an object 'b', then all "call_other(func)" to 'b'
// will be redirected to 'a'. If object 'a' has not defined the function,
// then the call will be forwarded to 'b' (as if there were no shadow).
// There is only one object that can call functions in 'b' with
// call_other(), and that is 'a'. Not even object 'b' can "call_other()"
// itself. All normal (internal) function calls inside 'b' will however remain
// internal to 'b'.

// There are two ways to remove the shadow. Either destruct it, or the object
// that was shadowed. In the latter case, the shadow will also be destructed
// automatically.  Also, there is a efun remove_shadow() in the contrib package.

// The result is that it is possible to hide an object behind another one,
// but everything can be totally transparent.  The shadow() efunction makes
// it possible to change the behavior of an object without changing the
// code for the object in question.  One possible use for shadow() is to
// add special capabilities to various classes of players (thief, fighter,
// mage, etc).  This usage would make it possible to keep the player object
// much simpler than it could be if the code for the various classes had
// to be in the player object itself.

static nomask object shadow(object ob, varargs int flag)
{
  int i;
  object * dest_shadows;

  dest_shadows = ob->_query_shadows();

  // flag == 0, asking for who is shadowing me
  if (flag == 0)
  {
    if (sizeof(dest_shadows))
      return dest_shadows[0];

    return nil;
  }

  // this object is already shadowing something
  if (_is_shadowing)
  {
    stderr(" *** shadow: object " + object_name(this_object()) + 
           " is already shadowing other object.\n");
    return nil;
  }

  if ( !clonep(ob) || !clonep(this_object()) )
    return nil;

  // avoid same shadow again
  for (i = 0; i < sizeof(dest_shadows); i++)
    if (base_name(this_object()) == base_name(dest_shadows[i]))
    {
      stderr(" *** shadow: object " + object_name(ob) + 
             " is already being shadowed by a clone of " +
             base_name(this_object()) + ".\n");

      return nil;
    }

  ob->_add_shadow(this_object());

  _is_shadowing = 1;

  return ob;
}

nomask object * _query_shadows()
{
  // if (!is_auto_object())
  //   return ({ });
 
  _shadows -= ({ nil });

  return _shadows;
}

nomask void _add_shadow(object ob)
{
  if (!is_auto_object())
  {
    stderr(" *** illegal _add_shadow\n");
    return;
  }

  stderr(" *** shadow: object " + object_name(ob) + 
         " starts shadowing object " + object_name(this_object()) + ".\n");

  // not needed, done masking call_other in the auto object
  // call_touch(this_object());

  _shadows += ({ ob });
}

// query_shadowing - determine whether or not a given object it shadowing another
// object query_shadowing( object ob );

// Returns the object that `ob' is shadowing, or zero (0) if it is not
// shadowing any object.

static nomask int query_shadowing(varargs object ob)
{
  if (!ob)
    ob = this_object();

  return _is_shadowing;
}
