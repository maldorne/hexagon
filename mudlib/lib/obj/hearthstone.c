/*
 * Hearthstone for Ciudad Capital
 * Obviously based on World of Warcraft
 *
 * neverbot 17/04/09
 *
 * Now the fantasy dress of the recall item (/lib/obj/recall.c), which holds
 * the mechanics; any fantasy game can hand it to its players.
 */

#include <language.h>

inherit "/lib/obj/recall.c";

void create()
{
  ::create();

  set_name(_LANG_HEARTHSTONE_NAME);
  set_short(_LANG_HEARTHSTONE_SHORT);
  add_alias(_LANG_HEARTHSTONE_ALIAS);
  set_main_plural(_LANG_HEARTHSTONE_PLURAL);
  add_plural(_LANG_HEARTHSTONE_PLURALS);
  set_long(_LANG_HEARTHSTONE_LONG);
  set_gender(2);

  set_travel_verbs(_LANG_HEARTHSTONE_TRANSPORT_VERBS);

  set_message("acting",        _LANG_HEARTHSTONE_ACTING);
  set_message("cooldown",      _LANG_HEARTHSTONE_LOCKED);
  set_message("forgotten",     _LANG_HEARTHSTONE_FORGOTTEN);
  set_message("origin",        _LANG_HEARTHSTONE_ORIGIN);
  set_message("start_me",      _LANG_HEARTHSTONE_MSG_ME);
  set_message("start_room",    _LANG_HEARTHSTONE_MSG_ROOM);
  set_message("moved",         _LANG_HEARTHSTONE_MOVE);
  set_message("died",          _LANG_HEARTHSTONE_DEAD2);
  set_message("arrive_me",     _LANG_HEARTHSTONE_TRANSPORT_ME);
  set_message("arrive_room",   _LANG_HEARTHSTONE_TRANSPORT_ROOM);
  set_message("leave_room",    _LANG_HEARTHSTONE_LEAVE_ROOM);
  set_message("mark_what",     _LANG_HEARTHSTONE_MARK_FAIL);
  set_message("marked",        _LANG_HEARTHSTONE_DESTINATION);
  set_message("marked_info",   _LANG_HEARTHSTONE_MARKED_INFO);
  set_message("unmarked_info", _LANG_HEARTHSTONE_UNMARKED_INFO);
  set_message("help",          _LANG_HEARTHSTONE_HELP);
}
