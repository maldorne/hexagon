
/*
 *  Object auto-inherited by every object in the mud,
 *  equivalent to the master.c in MudOS
 */

#include <kernel.h>
#include <status.h>
#include <type.h>
#include <trace.h>

#include <mud/version.h>

#include "/lib/core/efuns.c"

// default high-level create function
void create() { }

// Name of this function defined in dgd config file
nomask void _auto_create()
{
  string object_name;
  string class_name;
  int clone, number;
  string creator;

  _shadows = ({ });
  _is_shadowing = 0;

  rlimits (-1; -1)
  {
    object_name = object_name(this_object());

    creator = "/lib/core/secure/files"->creator_file(object_name);
    setuid(creator);
    seteuid(creator);
    // owner = creator;

    clone = (sscanf(object_name, "%s#%d", class_name, number) > 1);

    // stderr(" - create: "+object_name+" "+creator+":"+owner+"\n");
    stderr(" - create: "+object_name+" : "+getuid()+"\n");

    // Register object in object handler
    // if (clone) or if (number >= 0)
    // {
    // }
    // else
    // {
    // }

  }

  create();
}
