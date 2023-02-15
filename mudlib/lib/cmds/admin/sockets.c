#include <standard.h>
#include <cmd.h>

inherit CMD_BASE;

void setup()
{
	position = 1;
}

protected int cmd(string str, object me, string verb) 
{
#ifdef __PACKAGE_SOCKETS__

  // write(socket_status());

  int i;
  string ret = "";
  mixed * sockets = socket_status();

  ret += " fd       state      mode  l_addr  r_addr  owner\n";
  ret += "------------------------------------------------\n";

  for (i = 0; i < sizeof(sockets); i++)
  {
    ret += sprintf( "%3d  %10s  %8s  %s  %s  (%s)\n", sockets[i][0], 
              sockets[i][1], 
              sockets[i][2],
              sockets[i][3],
              sockets[i][4],
              ((sockets[i][5] == 0) ? "-" : file_name(sockets[i][5])));
  }

  write(ret);
	return 1;

#else

	notify_fail("PACKAGE_SOCKETS no está compilado.\n");
	return 0;

#endif
}
 
