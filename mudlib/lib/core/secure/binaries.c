
// Binary connections handler

#include <net/binary.h>
#include <net/http.h>

static mapping _connections;

#define _CONN_OB_POS 0
#define _CONN_PORT_POS 1

void create()
{
  ::create();
  _connections = ([ ]);

  if (!find_object(BINARY_OB))
    compile_object(BINARY_OB);
  if (!find_object(HTTP_OB))
    compile_object(HTTP_OB);
}

object new_connection(int port)
{
  object conn;
  string id;
  string target;

  // route by port: 8080 → HTTP, everything else → raw binary
  target = (port == HTTP_PORT ? HTTP_OB : BINARY_OB);

  conn = clone_object(find_object(target));
  id = file_name(conn);

  _connections += ([ id : allocate(2) ]);

  _connections[id][_CONN_OB_POS]     = conn;
  _connections[id][_CONN_PORT_POS]   = port;

  return conn;
}

void remove_connection(object ob)
{
  string id;

  // if (base_name(previous_object()) != BINARY_OB)
  //   return;

  id = file_name(ob);

  _connections[id] = nil;
  _connections -= ({ nil });
}