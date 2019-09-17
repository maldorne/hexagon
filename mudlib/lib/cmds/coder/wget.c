#include <standard.h>
#include <cmd.h>

#include "/secure/heimdall/include/heimdall.h"

inherit CMD_BASE;

object conn;

void setup()
{
  position = 1;
}

protected int cmd(string str, object me, string verb) 
{
  object conns;
  string * pieces;
  string * url_and_port;

  string url;
  string port;
  string path;

  if ((conns = load_object(CONNECTIONS_MANAGER)) == 0)
  {
    tell_object(me, " -- cannot reach connections manager\n");
    return 1;
  }

  if (!str || (str == ""))
  {
    notify_fail("Sintaxis: wget <url>\n");
    return 0;
  }    

  // remove the protocol
  if (str[0..6] == "http://")
    str = str[7..];
  else if (str[0..7] == "https://")
    str = str[8..];

  pieces = explode(str, "/");

  url_and_port = explode(pieces[0], ":");
  if (sizeof(url_and_port) > 1)
  {
    url = url_and_port[0];
    port = url_and_port[1];
  }
  else
  {
    url = pieces[0];
    port = "80"; // default http port
  }

  if (sizeof(pieces) > 1)
    path = "/" + implode(pieces[1..], "/");
  else
    path = "/";

  // params = explode(str, " ");

  // if (sizeof(params) < 3)
  // {
  //   notify_fail("Sintaxis: wget <ip> <port> <file>\n"
  //               "          (file starting with '/')\n");
  //   return 0;
  // }

  // conns->new_connection("file", me, params[0], params[1], params[2]);

  conns->new_connection("file", me, url, port, path);

  return 1;
} 
 