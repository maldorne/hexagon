
#include <user/terminal.h>

string fix_string(string str)
{
  object term;

  term = find_object(TERM_HANDLER);

  if (term)
    return term->fix_string(str, this_player());

  return str;
} 

