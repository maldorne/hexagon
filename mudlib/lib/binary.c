
// Front of every binary connection.
//
// The driver gives each connection on a binary port to this object, and a
// connection cannot be handed to another object afterwards.  So this
// object buffers what the client sends until it can tell which protocol it
// speaks, creates the server (a session object) for that protocol and from
// then on only forwards bytes between the driver and the session.
// A client that sends something no protocol recognises is told so and
// dropped, as is one that sends nothing decisive in time.

#include <user/login.h>
#include <mud/secure.h>
#include <net/binary.h>
#include <net/http.h>
#include <kernel.h>

inherit obj "/lib/core/object.c";

#define _UNKNOWN_PROTOCOL "unknown protocol\r\n"

static int     timestamp;     // last input time
int            start_time;    // time of first connection
static mapping _protocols;    // name -> ({ server program, first bytes it may start with })
static string  _buffer;       // bytes received before the protocol was known
static object  _session;      // the protocol server, nil until detected
static string  _protocol;     // name of the detected protocol
static int     _closing;      // the connection is being dropped once sent

void create()
{
  obj::create();

  seteuid(NETWORK_EUID);

  timestamp  = 0;
  start_time = time();
  _buffer    = "";
  _closing   = 0;

  // one more protocol is one more entry here
  _protocols = ([
    "http" : ({ HTTP_OB, ({ "GET ", "HEAD ", "POST ", "PUT ", "DELETE ",
                            "OPTIONS ", "PATCH ", "TRACE ", "CONNECT " }) }),
  ]);
}

// do not allow movement of the binary object
int move(mixed dest, varargs mixed messin, mixed messout)
{
  return obj::move(LOGIN_LOUNGE, messin, messout);
}

nomask int query_link() { return 0; }
nomask int query_player() { return 0; }
nomask int query_user() { return 0; }
nomask object user() { return this_object(); }
// total time connected
nomask int query_timestamp() { return timestamp; }

void dest_me()
{
  // remove the user from the user handler
  if (clonep(this_object()))
    find_object(BINARY_HANDLER)->remove_connection(this_object());

  if (_session)
    destruct_object(_session);

  obj::dest_me();
}

// ---------------------------------------------------------------------
// protocol detection
// ---------------------------------------------------------------------

/**
 * Looks at the first bytes of a connection to tell the protocol.
 * @param data bytes received so far
 * @return the name of the protocol, "" when more bytes are needed to
 *         decide, nil when no known protocol can start with them
 */
private string _detect_protocol(string data)
{
  string * names;
  string * starts;
  string   start;
  int      i, j, len, wait;

  if (::strlen(data) == 0)
    return "";

  wait = 0;
  names = map_indices(_protocols);

  for (i = 0; i < sizeof(names); i++)
  {
    starts = _protocols[names[i]][1];

    for (j = 0; j < sizeof(starts); j++)
    {
      start = starts[j];
      len = ::strlen(data);

      // the data may be shorter than the start: compare what is there
      if (len > ::strlen(start))
        len = ::strlen(start);

      if (data[0..len - 1] != start[0..len - 1])
        continue;

      if (::strlen(data) >= ::strlen(start))
        return names[i];

      wait = 1;
    }
  }

  return (wait ? "" : nil);
}

/**
 * Creates the server of a protocol and hands it the connection with the
 * bytes buffered so far.
 */
private void _start_session(string protocol)
{
  string program;
  object server;
  string data;

  program = _protocols[protocol][0];
  server = find_object(program);
  if (!server)
    server = compile_object(program);

  _protocol = protocol;
  _session  = clone_object(server);
  data      = _buffer;
  _buffer   = "";

  _session->start(this_object(), data);
}

/** Answers a client whose protocol is not known and drops it once sent. */
private void _refuse_connection()
{
  _closing = 1;
  ::send_message(_UNKNOWN_PROTOCOL);
}

// ---------------------------------------------------------------------
// entry points for the protocol server
// ---------------------------------------------------------------------

/**
 * Writes bytes to the client on behalf of the session.
 * @return how many bytes the driver accepted
 */
int send(string str)
{
  if (previous_object() != _session || _session == nil || str == nil)
    return 0;

  return ::send_message(str);
}

/** Closes the connection on behalf of the session. */
void end_connection()
{
  if (previous_object() != _session || _session == nil)
    return;

  dest_me();
}

/**
 * Drops a connection that did not show its protocol in time.  It is public
 * because call_out reaches it through call_other, so it re-checks the age.
 */
void detect_timeout()
{
  if (_session == nil && time() - start_time >= BINARY_DETECT_TIMEOUT)
    dest_me();
}

// ---------------------------------------------------------------------
// driver callbacks
// ---------------------------------------------------------------------

// called from the driver
static void open()
{
  timestamp = time();

  // a client that stays silent, or sends half a method, is dropped
  call_out("detect_timeout", BINARY_DETECT_TIMEOUT);
}

// called from the driver
static void close(int flag)
{
  // the session has no connection of its own to outlive this one
  if (_session)
    destruct_object(_session);

  // the client left: nothing else keeps this object alive
  if (!flag)
    dest_me();
}

// called from the driver once the output buffer has emptied
static void message_done()
{
  if (_closing)
  {
    dest_me();
    return;
  }

  if (_session)
    _session->message_done();
}

// called from the driver with whatever the client sent
static void receive_message(string str)
{
  string protocol;

  timestamp = time();

  if (_session)
  {
    _session->receive(str);
    return;
  }

  // already told to leave: ignore anything else it sends
  if (_closing)
    return;

  _buffer += str;
  protocol = _detect_protocol(_buffer);

  if (protocol == "")
    return;  // wait for more bytes

  if (protocol == nil)
  {
    _refuse_connection();
    return;
  }

  _start_session(protocol);
}

nomask void save_me() {}

nomask int restore_me(varargs string whatever)
{
  return 0;
}

int query_start_time() { return start_time; }

// query_idle - determine how many seconds an interactive player has been idle
// int query_idle( object ob );
// Query how many seconds a player object (ob) has been idling.

nomask int query_idle()
{
  return time() - timestamp;
}

mixed * stats()
{
  return ({
    ({ "Timestamp (nosave)", timestamp, }),
    ({ "Protocol (nosave)", _protocol != nil ? _protocol : "<undetected>", }),
    ({ "Buffered (nosave)", ::strlen(_buffer), }),
          }) + obj::stats();
}
