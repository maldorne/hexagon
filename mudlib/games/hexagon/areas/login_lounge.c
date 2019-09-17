
// the connection objects (/lib/link.c) are moved here
// until the user chooses a character or disconnects

inherit "/games/hexagon/base/forbidden_room";

void setup()
{
  set_log_filename("login_lounge");
  set_short("Login lounge");
  set_long("This is a the login lounge, here the connection objects rest until " +
    "the user chooses a character. Don't touch anything. Your presence here has " +
    "been logged.\n");
}
