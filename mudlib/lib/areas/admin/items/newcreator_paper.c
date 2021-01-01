// Paper that is cloned in the coder's workrooms

inherit "/lib/item.c";

#include <living/language.h>

void setup()
{
  set_name("paper");
  set_short("Paper");
  set_long("It's a small piece of paper.\n");
  set_read_mess("Welcome to the coders team of " + mud_name() + ".\n" +
    "You can and should customize your own workroom to suit your needs. " +
    "In the code of these workrooms you can find examples and a lot of " +
    "comments about how to create and modify basic rooms, and you can " +
    "find even more info inside the \"/docs\" directory or using " +
    "the \"man\" and \"help\" commands.\n\n" +
    "\t\t Have fun!!\n", STD_LANG);

  set_value(0);
  set_weight(0);
}

