
// geteuid - return the effective user id of an object
// string geteuid(object ob);
//
// Self-contained, object-argument-only form of geteuid, suitable for
// translation units that need it without bringing in the full
// /lib/core/efuns/euid.c (which also provides getuid/setuid/seteuid
// and a private _euid fallback for the no-arg form).
//
// Most code should use the version in euid.c; this extract exists for
// the driver object, which does not inherit the auto object and only
// ever queries the EUID of a known caller object.

nomask string geteuid(object ob)
{
  if (!ob)
    return nil;
  if (ob->query_player())
    ob = ob->user();
  if (!ob)
    return nil;
  return ob->geteuid();
}
