
#include <basic/communicate.h>

void create()
{

}

string read_message(string str, string type, string lang, varargs int size) 
{
  mixed bing;

  if (member_array(lang, this_object()->query_languages()) == -1)
    if ((bing = (mixed)LANGUAGE_HANDLER->query_garble_object(lang)))
      if ((bing = (mixed)bing->garble_say(str, previous_object())))
        return "No puedes leer este texto "+bing+
          (lang?" escrito en "+lang:"")+"\n";
      else
        return "Hubieras jurado que habia algo escrito aquí...\n"; /* Invisible text... */
    else
      return "No puedes leer este texto "+(lang?" escrito en "+lang:"")+"\n";
  
  if (LANGUAGE_HANDLER->query_language_magic(lang))
    if ((bing = (mixed)LANGUAGE_HANDLER->query_garble_object(lang)))
      return (string)bing->magical_text(str, previous_object());
    else 
    {
      return "El MUD tiembla al encontrar una escritura mágica que puedes leer, "+
        "pero no encontrar intérprete para ella.\n";
    }

  if (!lang)
    return "Lees:\n" + str +"\n";
  
  return "Lees un texto escrito en " + lang + ":\n" + str + "\n";
}

mixed *stats() 
{
  return ({
          });
}
