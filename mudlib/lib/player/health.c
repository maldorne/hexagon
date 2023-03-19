
#include <translations/stats.h>
#include <translations/inform.h>

void create()
{
}

// change this function to show health the same way as with the
// score cmd, neverbot 12/05
void display_monitor(varargs int use_colors)
{
  string COLOR, RESET, color2, color3;

  COLOR = "%^GREEN%^";

  if (use_colors)
  {
    RESET = "%^RESET%^GREEN%^";
    color2 = "";
    color3 = "";

    if (!this_object()->user()->query_inform(INFORM_EVENT_HEALTH))
      return;

    if (this_object()->query_hp() >= (this_object()->query_max_hp()/2))
      color2 += "%^GREEN%^";
    else if (this_object()->query_hp() >= (this_object()->query_max_hp()/4))
      color2 += "%^BOLD%^MAGENTA%^";
    else
      color2 += "%^BOLD%^RED%^";

    if (this_object()->query_hp() >= (this_object()->query_max_gp()/2))
      color3 += "%^GREEN%^";
    else if (this_object()->query_hp() >= (this_object()->query_max_gp()/4))
      color3 += "%^BOLD%^MAGENTA%^";
    else
      color3 += "%^BOLD%^RED%^";

    tell_object(this_object(), "[ " + COLOR +
      _LANG_HEALTH_POINTS_SHORT + ": " + color2 + this_object()->query_hp() + RESET +
      " (" + this_object()->query_max_hp() + RESET + ") " +
      _LANG_GUILD_POINTS_SHORT+": " + color3 + this_object()->query_hp() + RESET +
      " (" + this_object()->query_max_gp() + RESET + ")%^RESET%^ ]\n");
  }
  else
  {
    tell_object(this_object(), "[ " +
                COLOR + sprintf(_LANG_HEALTH_POINTS_SHORT + ": %d (%d) " + 
                                _LANG_GUILD_POINTS_SHORT + ": %d (%d)\n",
                                this_object()->query_hp(),
                                this_object()->query_max_hp(),
                                this_object()->query_hp(),
                                this_object()->query_max_gp()) + "%^RESET%^ ]\n");
  }
}

mixed * stats()
{
  return ({ });
}
