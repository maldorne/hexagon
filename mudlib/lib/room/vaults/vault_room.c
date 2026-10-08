/*
  March 13, 1995
  Vaults - Originally designed to contain items for players
     to save'em through reboots.  Too much work, changed to
     group/guild/clan/house access instead of an object file
     for each individual player.
  For FR III, guilds and any other group willing to purchase
  these for 5000 platinum coins will be allowed to have access.
  Main purpose was for newbie equipment but other vaults can be
  purchased for individual Guild masters and even senior members
  of a group. Keep in mind, the fewer with access, charge
  them more. 
  Vault restrictions NOT to be changed: 
  Nothing but weapons, armours, scrolls, and wands, and definitely
  no unsaveable items.

  How to make your own in the game:
  Simple, inherit this file and describe your room. Make sure to
  copy or design your own add_sign()   *grin*   
  However, before putting it into the game, must be approved by
  a Demi-God beforehand and /d/apriors/doms/VAULTS should be 
  updated.
  Have fun!

                       Radix : Thane of Hoerk
  Added logging with player readable abilities for monitoring
  by Guild leader and immortals.
  Use set_admins( ({"name1", "name2",... }) ); or "all" for everyone.
  Radix - December 4, 1995
 
  Changed to use /global/auto_load as Taniwha showed me how  (:
  New driver allows larger buffer, save object file can be huge,
  won't break BUT, the restriction is set to 30k for a reason.
  Radix - February 10, 1996
 
  Missing item bug regretfully fixed. Calling save_object
  when someone else simultaneously uses the vault as well
  caused it. Radix - Jan 4, 1996

  Translated for CcMud, neverbot 08/07/04 
  Listings by item category
  Translated again for Hexagon, neverbot 01/2021
  The vault behaviour moved to vault-actions.c, shared with locations, 2026
*/ 

// A room that is a vault. What the vault does lives in vault-actions.c; this
// file only makes it a room.

inherit room    "/lib/room.c";
inherit actions "/lib/room/vaults/vault-actions.c";

#include <language.h>

void create()
{
  actions::create();
  room::create();

  // every game's vaults are saved inside that game's save directory
  set_vault_storage(base_name(this_object()));
  create_vault_sign(this_object());
}

void setup()
{
  set_light(80);
  set_short(_LANG_VAULTS_ROOM_SHORT);
  set_long(_LANG_VAULTS_ROOM_LONG);
}

void init()
{
  room::init();
  actions::init();
}

mixed * stats()
{
  return room::stats() + actions::stats();
}
