
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
