
#include "../path.h"
#include <language.h>

inherit "/lib/room/vaults/vault_room.c";

void setup()
{
 set_short(_LANG_VAULT_SHORT);
 set_long(_LANG_VAULT_LONG);
 set_light(50);

 add_exit(DIR_WEST, ROOMS + "guild.c", "door");
}
