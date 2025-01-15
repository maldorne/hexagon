/*
 * Changes for Hexagon, neverbot 01/2025
 * 
 * - shop functions refactored in several files, to be inherited both
 *   from the shop room (/lib/ventures/shop.c) and the shop component
 *   for locations (/lib/location/components/shop.c). More info about
 *   previous changes in the header comments of the original shop file.
 *
 */

#include <language.h>

object create_sign(varargs object where)
{
  string text;
  text = _LANG_SHOP_SIGN_INFO;

  if (!this_object()->query_only_sell())
    text += _LANG_SHOP_SIGN_INFO_SELL;
    
  if (!where)
    where = this_object();
  
  return where->add_sign(_LANG_SHOP_SIGN_DESC, text);  
}
