
#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;
void setup(){
position = 1;
}
protected int cmd(string str, object me, string verb) {
   object player;

  if (this_player(1) != this_player()) return 0;
    if (!str) {
       notify_fail("Usage: register player\n");
       return 0;
    }
    player = find_player(str);
    if (!player) {
        notify_fail("No player " + str + " logged on.\n");
       return 0;
    }
    if(!player->query_desc())
    {
        notify_fail((string)player->query_cap_name()+" does not have a description set.\n"
        "An automatic message has been sent telling them to do it.\n"
        );
        tell_object(player,"Sorry, but you can't be registered until "
        "your description has been filled in.\n"
        "Simply \"describe (what you'd want others to see when they look at you)\"\n"
        "Then tell "+this_player()->query_cap_name()+" or another immortal when "
        "it's done.\n"
        "Do a good job of it, you have to live with it.\n"
        );
        return 0;
    }
    player->set_registrated(1);
    player->add_property("reg_by",this_player()->query_cap_name());
    write((string)player->query_cap_name()+" is registered.\n");
    tell_object(player,"You are now registered. Enjoy your time here.\n");
    return 1;
} /* register player */
 

