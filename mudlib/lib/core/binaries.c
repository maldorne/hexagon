
// Binary connections handler

#include <net/binary.h>

static mapping _connections;

#define _CONN_OB_POS 0
#define _CONN_PORT_POS 1

void create()
{
  ::create();
  _connections = ([ ]);

  if (!find_object(BINARY_OB))
    compile_object(BINARY_OB);
}

object new_connection(int port)
{
  object conn;
  string id;

  conn = clone_object(find_object(BINARY_OB));
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