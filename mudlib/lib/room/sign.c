/* 
 * add_sign(string long, string read_mess, 
 *          string lang, string frame_style, string short, string name)
 *   [lang, frame_style, short and name are optional]
 * This function will return a sign that can be used by a room in any way it sees fit.
 * This function was the brainchild of Wyrm - 7 Feb '92
 * Added language, neverbot 01/2021
 * Moved from room.c to reuse also in locations, neverbot 01/2025
 * 
 */

#include <common/frames.h>
#include <language.h>

object add_sign(string long, string mess, 
  varargs string lang, string frame_style, string short, string sname)
{
  object sign;

  sign = clone_object("/lib/item.c");

  if (!sname)
    sname = _LANG_DEFAULT_SIGN_NAME;

  sign->set_name(sname);
  sign->set_main_plural(pluralize(sname));

  if (!short)
    short = capitalize(_LANG_DEFAULT_SIGN_NAME);
  
  if (!frame_style)
    frame_style = DEFAULT_FRAME_STYLE;

  sign->set_short(short);
  sign->set_long(long);
  sign->reset_get();
  //if (short && short != "")
  sign->move(this_object());
  // hack, set messages after moving, so the item is IN a game
  // and knows what language handler to use
  sign->set_read_mess(mess, lang, 0, frame_style);
  //else
  //  hidden_objects += ({ sign });
  
  this_object()->add_destable(sign);
  return sign;
}
