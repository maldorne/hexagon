// Vault component: the location is a vault, with the same behaviour as the
// vault room (/lib/room/vaults/vault-room.c), both from vault-actions.c.
//
// The sign is put up on every load: from the vault help document, so changing
// that document changes every vault, unless the vault has its own sign text or
// document. A vault may also be restricted (guilds that may use it, the
// property value it accepts, a maximum of items); these are saved as the
// attributes vault_guilds, vault_accept_prop, vault_accept_value,
// vault_accept_name, vault_max_items, vault_sign_text and vault_sign_doc.
// The contents are kept under the room the location was converted from, so a vault keeps
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

object query_vault_place() { return query_entity(); }

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
  mapping attrs;

  attrs = component::query_auto_load_attributes();

  if (sizeof(vault_admins))
    attrs["vault_admins"] = vault_admins;
  if (sizeof(vault_guilds))
    attrs["vault_guilds"] = vault_guilds;
  if (strlen(vault_accept_prop))
  {
    attrs["vault_accept_prop"] = vault_accept_prop;
    attrs["vault_accept_value"] = vault_accept_value;
  }
  if (strlen(vault_accept_name))
    attrs["vault_accept_name"] = vault_accept_name;
  if (vault_max_items)
    attrs["vault_max_items"] = vault_max_items;
  if (strlen(vault_sign_text))
    attrs["vault_sign_text"] = vault_sign_text;
  if (strlen(vault_sign_doc))
    attrs["vault_sign_doc"] = vault_sign_doc;

  return attrs;
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);

  if (!undefinedp(args["vault_admins"]))
    set_admins(args["vault_admins"]);
  if (!undefinedp(args["vault_guilds"]))
    set_vault_guilds(args["vault_guilds"]);
  if (!undefinedp(args["vault_accept_prop"]))
    set_vault_accept(args["vault_accept_prop"], args["vault_accept_value"],
                     args["vault_accept_name"]);
  else if (!undefinedp(args["vault_accept_name"]))
    set_vault_accept("", "", args["vault_accept_name"]);
  if (!undefinedp(args["vault_max_items"]))
    set_vault_max_items(args["vault_max_items"]);
  if (!undefinedp(args["vault_sign_text"]))
    set_vault_sign_text(args["vault_sign_text"]);
  if (!undefinedp(args["vault_sign_doc"]))
    set_vault_sign_doc(args["vault_sign_doc"]);
}

mixed * stats()
{
  return component::stats() + actions::stats();
}
