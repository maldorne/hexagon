/*
 * Changes for Hexagon, neverbot 01/2025
 * 
 * - pub functions refactored in several files, to be inherited both
 *   from the pub room (/lib/ventures/pub.c) and the pub component
 *   for locations (/lib/location/components/pub.c). More info about
 *   previous changes in the header comments of the original pub file.
 *
 */

#include <language.h>
#include <room/ventures.h>

// The menu sign this pub last created. Tracked so create_sign is idempotent:
// initialize() runs it on every load, and reconversion can run it again on a
// still-live pub, which would otherwise stack a second "Menú" item in the
// room. Destruct the previous one before making a new one.
static object menu_sign;

// The chat globe this pub put up, tracked for the same reason.
static object globe;

object create_sign(varargs object where)
{
  object menu;

  if (!where)
    where = this_object();

  if (menu_sign)
    menu_sign->dest_me();

  menu = where->add_sign(_LANG_PUB_SIGN_DESC,
                         this_object()->query_menu_text(),
                         _LANG_PUB_SIGN_NAME);

  menu->add_alias(_LANG_PUB_SIGN_ALIASES);
  menu_sign = menu;
  return menu;
}

// Every pub has a chat globe, linked to the globes of every other pub.
object place_globe(varargs object where)
{
  if (!where)
    where = this_object();

  if (globe && environment(globe) == where)
    return globe;

  if (globe)
    globe->dest_me();

  globe = clone_object(GLOBE_OB);
  globe->move(where);
  return globe;
}
