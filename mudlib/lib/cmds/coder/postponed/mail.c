#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;

void setup()
{
position = 1;
}


protected int cmd(string str, object me, string verb) {
    object ob;

    if(str != 0) {
      write("De: "+(string)this_player()->query_cap_name()+"\n");
      write("Para: "+
        (string)this_player()->expand_nickname(str)+"\n");
    }
    ob = new("/obj/handlers/post");
    ob->move(me);
    ob->start_mail(str);
    return 1;
}
 
