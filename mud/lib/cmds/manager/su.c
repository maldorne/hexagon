#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;

void setup(){

position = 1;
}

protected int cmd(string str, object me, string verb) {
  if (!str || str == "") {
    notify_fail("Usage: su <name>\n");
    return 0;
  }

  notify_fail("");
  clone_object("/secure/login")->do_su(str);
  return 1;
}

