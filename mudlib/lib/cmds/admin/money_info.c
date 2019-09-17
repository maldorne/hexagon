
#include <mud/cmd.h>
#include <mud/secure.h>

#include <basic/money.h>


inherit CMD_BASE;

void setup()
{
  position = 1;
}

static int cmd(string str, object me, string verb) 
{
  string ret;

  ret = "%^BOLD%^GREEN%^Full domain stats%^RESET%^:\n\n";

  if (str && str != "")
    ret += MONEY_TRACKER->full_domain_stats(str);
  else
    ret += MONEY_TRACKER->full_domain_stats();


  ret += "\n%^BOLD%^GREEN%^Select domain stats%^RESET%^:\n\n";

  if (str && str != "")
    ret += MONEY_TRACKER->select_domain_stats(str);
  else
    ret += MONEY_TRACKER->select_domain_stats();

  me->more_string(ret);
  return 1;
}
