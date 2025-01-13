
#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "testcolors" }));
  set_usage("testcolors");
  set_help("Prints a test using every color to see how your current " +
           "terminal and client manages them.");
}

static int cmd(string str, object me, string verb)
{
  write("You are currently using the " + me->user()->query_term_name() + " terminal.\n\n");
  write("Test color begins here:\n\n");

  write("\tRESET: %^RESET%^This is a test%^RESET%^\n");
  write("\tBOLD: %^BOLD%^This is a test%^RESET%^\n");
  write("\tFLASH: %^FLASH%^This is a test%^RESET%^\n");
  write("\tUNDERLINE: %^UNDERLINE%^This is a test%^RESET%^\n");
  write("\tREVERSE: %^REVERSE%^This is a test%^RESET%^\n");
  write("\tBLACK: %^BLACK%^This is a test%^RESET%^\n");
  write("\tBOLD BLACK: %^BOLD%^BLACK%^This is a test%^RESET%^\n");
  write("\tRED: %^RED%^This is a test%^RESET%^\n");
  write("\tBOLD RED: %^BOLD%^RED%^This is a test%^RESET%^\n");
  write("\tBLUE: %^BLUE%^This is a test%^RESET%^\n");
  write("\tBOLD BLUE: %^BOLD%^BLUE%^This is a test%^RESET%^\n");
  write("\tCYAN: %^CYAN%^This is a test%^RESET%^\n");
  write("\tBOLD CYAN: %^BOLD%^CYAN%^This is a test%^RESET%^\n");
  write("\tMAGENTA: %^MAGENTA%^This is a test%^RESET%^\n");
  write("\tBOLD MAGENTA: %^BOLD%^MAGENTA%^This is a test%^RESET%^\n");
  write("\tORANGE: %^ORANGE%^This is a test%^RESET%^\n");
  write("\tBOLD ORANGE: %^BOLD%^ORANGE%^This is a test%^RESET%^\n");
  write("\tYELLOW: %^YELLOW%^This is a test%^RESET%^\n");
  write("\tBOLD YELLOW: %^BOLD%^YELLOW%^This is a test%^RESET%^\n");
  write("\tGREEN: %^GREEN%^This is a test%^RESET%^\n");
  write("\tBOLD GREEN: %^BOLD%^GREEN%^This is a test%^RESET%^\n");
  write("\tWHITE: %^WHITE%^This is a test%^RESET%^\n");
  write("\tBOLD WHITE: %^BOLD%^WHITE%^This is a test%^RESET%^\n");
  write("\tB_RED: %^B_RED%^This is a test%^RESET%^\n");
  write("\tB_ORANGE: %^B_ORANGE%^This is a test%^RESET%^\n");
  write("\tB_YELLOW: %^B_YELLOW%^This is a test%^RESET%^\n");
  write("\tB_BLACK: %^B_BLACK%^This is a test%^RESET%^\n");
  write("\tB_CYAN: %^B_CYAN%^This is a test%^RESET%^\n");
  write("\tB_WHITE: %^B_WHITE%^This is a test%^RESET%^\n");
  write("\tB_GREEN: %^B_GREEN%^This is a test%^RESET%^\n");
  write("\tB_MAGENTA: %^B_MAGENTA%^This is a test%^RESET%^\n");
  write("\tB_BLUE: %^B_BLUE%^This is a test%^RESET%^\n");
  write("\tSTATUS: %^STATUS%^This is a test%^RESET%^\n");
  write("\tWINDOW: %^WINDOW%^This is a test%^RESET%^\n");
  write("\tINITTERM: %^INITTERM%^This is a test%^RESET%^\n");
  write("\tENDTERM: %^ENDTERM%^This is a test%^RESET%^\n\n");

  return 1;
}
