
#include <user/terminal.h>

static nomask string fix_string(string str)
{
  object term;

  term = find_object(TERM_HANDLER);

  if (term)
    return term->_fix_string(str, this_player());

  return str;
} 

