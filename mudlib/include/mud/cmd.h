#ifndef _CMD_H
#define _CMD_H

#define SOUL_OBJECT "/lib/handlers/souls"
#define SOUL_DATA   "/lib/tables/souls"

#define CMD_HANDLER "/lib/cmds/handler"
#define CMD_BASE    "/lib/cmds/cmd"

// command positions
#define USER_CMD 1
#define PLAYER_CMD 2
#define CODER_CMD 3
#define ADMIN_CMD 4

#define CMD_CATEGORIES ({ USER_CMD, PLAYER_CMD, CODER_CMD, ADMIN_CMD })

#endif
