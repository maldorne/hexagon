
#include <mud/secure.h>

static string euid;

// seteuid - set the effective user id (euid) of an object
// int seteuid( string str );
// Set effective uid to 'str'.  valid_seteuid() in master.c controls which
// values the euid of an object may be set to.
// When this value is 0, then the current object's uid can be changed by
// export_uid(), and only then.
// But, when the value is 0, no objects can be loaded or cloned by this object.

// TODO seteuid
// call valid_seteuid first

static nomask int seteuid( string str )
{
  euid = str;
}


// geteuid - return the effective user id of an object or function
// string geteuid( object|function );
// If given an object argument, geteuid returns the effective user id (euid)
// of the object.  If given an argument of type 'function', it returns the
// euid of the object that created that 'function' variable.  If the object,
// at the time of the function variable's construction, had no euid, the
// object's uid is stored instead.

// TODO geteuid

static nomask string geteuid( varargs object ob )
{
  // if (ob && objectp(ob))
  //   return ob->geteuid();
  
  // TODO

  if (objectp(ob))
    return ob->query_name();

  return euid;
}


// getuid - return the user id (uid) of an object
// string getuid( object ob );
// Returns the user id of an object.  The uid of an object is determined at
// object creation by the creator_file() function.

// TODO getuid

static nomask string getuid( varargs object ob )
{
  return euid;
}


static nomask string get_root_uid() { return ROOT; }
static nomask string get_bb_uid() { return BACKBONE; }

// Radix, July 1996
// Called from create() in /std/object.c
// if creator made me, tag their name to it

string get_create_me(string tmp)
{
  int i;
  object obj;

  obj = previous_object(-1);

  // for (i = sizeof(obj)-1; i > -1; --i)
  //    if (obj[i]->query_coder())
  //       return geteuid(obj[i]);

  if (obj->query_coder())
    return geteuid(obj[i]);

  return tmp;
}
