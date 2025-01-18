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

object create_sign(varargs object where)
{
  object menu;

  if (!where)
    where = this_object();

  menu = where->add_sign(_LANG_PUB_SIGN_DESC, 
                         this_object()->query_menu_text(),
                         _LANG_PUB_SIGN_NAME);

  menu->add_aliases(_LANG_PUB_SIGN_ALIASES);
  return menu;
}