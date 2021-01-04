
#include <basic/communicate.h>
#include <language.h>

void create()
{

}

string read_message(string str, string lang, varargs int size, string frame_style)
{
  mixed bing;
  object handler, frames;

  handler = handler("languages");
  frames = handler("frames");

  if (member_array(lang, this_object()->query_languages()) == -1)
  {
    if (handler->language_exists(lang))
    {
      str = handler->garble(str, lang);
      if (strlen(frame_style))
        str = frames->frame(str, "", 0, 0, frame_style);

      return _LANG_READ_UNABLE_WITH_GARBLE;
    }
    return _LANG_READ_UNABLE_WITHOUT_GARBLE;

    // if ((bing = (mixed)handler("languages")->query_garble_object(lang)))
    // {
    //   if ((bing = (mixed)bing->garble_say(str, previous_object())))
    //     return _LANG_READ_UNABLE_WITH_GARBLE;
    //   else
    //     return _LANG_READ_WHAT; /* Invisible text... */
    // }
    // else
    //   return _LANG_READ_UNABLE_WITHOUT_GARBLE;
  }
  
  if (handler("languages")->query_language_magic(lang))
    if ((bing = (mixed)handler("languages")->query_garble_object(lang)))
      return (string)bing->magical_text(str, previous_object());
    else 
    {
      return _LANG_READ_IMPOSSIBLE;
    }

  if (!lang)
    return _LANG_READ;
  
  return _LANG_READ_IN_LANGUAGE;
}

mixed *stats() 
{
  return ({
          });
}
