
#include <kernel.h>

#include <mud/secure.h>
#include <mud/mudos.h>

private string _euid;
private string _uid;

// getuid - return the user id (uid) of an object
// string getuid( object ob );
// Returns the user id of an object.  The uid of an object is determined at
// object creation by the creator_file() function.

nomask string getuid( varargs object ob )
{
  if (ob && ob->query_player())
    ob = ob->user();

  if (ob)
    return ob->getuid();

  return _uid;
}

static nomask int setuid(string id)
{
  if (!stringp(id) || !strlen(id))
    return 0;

  if (!is_auto_object())
    return 0;

  _uid = id;
}

// geteuid - return the effective user id of an object or function
// string geteuid( object|function );
// If given an object argument, geteuid returns the effective user id (euid)
// of the object.  If given an argument of type 'function', it returns the
// euid of the object that created that 'function' variable.  If the object,
// at the time of the function variable's construction, had no euid, the
// object's uid is stored instead.

nomask string geteuid( varargs object ob )
{
  if (ob && ob->query_player())
    ob = ob->user();

  if (ob)
    return ob->geteuid();

  return _euid;
}

// seteuid - set the effective user id (euid) of an object
// int seteuid( string str );
// Set effective uid to 'str'.  valid_seteuid() in master.c controls which
// values the euid of an object may be set to.
// When this value is 0, then the current object's uid can be changed by
// export_uid(), and only then.
// But, when the value is 0, no objects can be loaded or cloned by this object.

static nomask int seteuid( string id )
{
  // allow seteuid("") similar to the old seteuid(0)
  // if (!stringp(id) || !strlen(id))
  //   return 0;

  string file;

  // efuns always allowed to set their own euid
  if (is_auto_object())
  {
    _euid = id;
    return 1;
  }

  file = object_name(this_object());

  if (((id == DRIVER_EIUD) && (file == DRIVER || file == MUDOS_PATH)) ||
      (SECURE->valid_seteuid(this_object(), id)))
  {
    if (_euid != id)
      stderr(" *** seteuid changed in " + object_name(this_object()) +
             " from " + (strlen(_euid) ? _euid : "<empty>") + " to " + id + "\n");

    _euid = id;
    return 1;
  }

  stderr(" *** seteuid invalid " + object_name(this_object()) + " " + id + "\n");
  // stderr(to_string(call_trace()) + "\n");
  return 0;
}
