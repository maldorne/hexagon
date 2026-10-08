// Vault component: the location is a vault, with the same behaviour as the
// vault room (/lib/room/vaults/vault-room.c), both from vault-actions.c.
//
// Nothing of the sign is stored: it is put up on every load from the vault
// help document, so changing that document changes every vault. The contents
// are kept under the room the location was converted from, so a vault keeps
// what it held as a room.

#include <room/location.h>

inherit component "/lib/location/component.c";
inherit actions   "/lib/room/vaults/vault-actions.c";

void create()
{
  actions::create();
  component::create();

  set_type(LOCATION_COMPONENT_VAULT);
}

object query_vault_place() { return query_my_location(); }

void init()
{
  actions::init();
}

void initialize(object loc)
{
  string source;

  component::initialize(loc);

  if (!loc)
    return;

  source = loc->query_original_room_file_name();
  if (!stringp(source) || !strlen(source))
    source = loc->query_file_name();

  set_vault_storage(source);
  create_vault_sign(loc);
}

// Take the sign out of the location. The location destructs a component it
// drops without calling it, so whoever removes this one calls this first.
void take_down()
{
  take_down_vault_sign();
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         (sizeof(vault_admins) ? ([ "vault_admins" : vault_admins ]) : ([ ]));
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);

  if (!undefinedp(args["vault_admins"]))
    set_admins(args["vault_admins"]);
}

mixed * stats()
{
  return component::stats() + actions::stats();
}
