
// people.c -- what can be said about whoever is connected right now.
//
// The old mudlib kept a "peopler" handler that did everything at once: it
// gathered the facts, formatted them into columns, sorted the lines and paged
// the result, and each creator stored a private column layout as a property on
// themselves. This keeps the gathering and drops the rest -- the report belongs
// to the command that asks for it (/lib/cmds/admin/people.c), and the layouts
// are views that command names.
//
// Everything here reads; nothing is written, and no user is touched.

#include <user/roles.h>

// The body behind a connection. `users()` answers with the user objects -- the
// connection and its terminal -- and everything about the person (race, guild,
// level, where they stand) hangs off the player it drives. Somebody still at
// the login prompt drives nothing yet.
object query_body(object user)
{
  object body;

  body = user ? (object)user->player() : nil;

  // Before a character is chosen the connection drives a link object, which
  // answers to none of this. It is not a body.
  return (body && body->query_player()) ? body : nil;
}

// What a connected user is, in one word. A user that has not finished
// arriving has no character yet, which is worth saying rather than leaving the
// rest of the row empty.
string query_kind(object user)
{
  string type;

  if (!user)
    return "gone";

  type = (string)user->query_object_type();

  if (type == O_ADMINISTRATOR)
    return "admin";
  if (type == O_CODER)
    return "coder";
  if (type == O_LOGIN)
    return "logging in";
  if (type == O_LOGGED)
    return "no character";

  return "player";
}

// A span of seconds as the shortest readable thing: 3d, 4h, 7m, 12s.
string query_span(int seconds)
{
  if (seconds < 0)
    seconds = -seconds;

  if (seconds >= 86400)
    return (seconds / 86400) + "d";
  if (seconds >= 3600)
    return (seconds / 3600) + "h";
  if (seconds >= 60)
    return (seconds / 60) + "m";

  return seconds + "s";
}

// Everybody connected, by name, optionally only those whose name begins with
// `prefix`. A user still at the login prompt has no name yet and answers to no
// prefix, so it is only listed when nothing was asked for.
object * query_users(varargs string prefix)
{
  object * all, * out;
  int i;

  all = users();
  if (!prefix || !strlen(prefix))
    return all;

  out = ({ });
  prefix = lower_case(prefix);
  for (i = 0; i < sizeof(all); i++)
  {
    mixed name;

    name = query_body(all[i]) ? query_body(all[i])->query_name() : nil;
    if (stringp(name) && strlen(name) >= strlen(prefix) &&
        lower_case(name[0 .. strlen(prefix) - 1]) == prefix)
      out += ({ all[i] });
  }

  return out;
}

// The guild somebody belongs to, named rather than pathed. A guild whose file
// has gone answers with its path, which is the useful thing to see.
private string guild_of(object user)
{
  mixed path;

  path = user->query_guild_ob();
  if (!stringp(path) || !strlen(path))
    return "-";

  if (file_size(path) >= 0 || file_size(path + ".c") >= 0)
    return (string)load_object(path)->query_short();

  return path;
}

// Where a coder is working. The path belongs to the role the connection
// wears, not to the body: a player role has none.
private string path_of(object user)
{
  object role;
  mixed dir;

  role = user ? (object)user->query_role() : nil;
  if (!role)
    return "-";

  dir = role->query_current_path();
  return (stringp(dir) && strlen(dir)) ? dir : "-";
}

// A number somebody may not be able to answer. The int cast is a conversion
// kfun and errors on nil, and a user still at the login prompt has no level,
// no alignment and no window size.
private string number(mixed value)
{
  return intp(value) ? "" + value : "-";
}

// A span somebody may not be able to answer, in the shortest readable form.
private string span(mixed seconds)
{
  return intp(seconds) ? query_span(seconds) : "-";
}

// Everything worth knowing about one connected user, as strings ready to be
// printed. A fact nobody can answer reads "-" rather than being left out, so
// every row of a report has the same shape.
mapping query_facts(object user)
{
  mapping facts;
  object body;
  mixed race, where;

  if (!user)
    return ([ ]);

  // the connection answers for the terminal and the address; the body it
  // drives answers for the person
  body = query_body(user);
  facts = ([ ]);

  facts["name"] = (body && stringp(body->query_name()))
                    ? body->query_name() : "-";
  facts["cap_name"] = (body && stringp(body->query_cap_name()))
                        ? body->query_cap_name() : "(nobody yet)";
  // somebody in the editor is not reading what is said to them
  facts["editing"] = (body && body->query_in_editor()) ? "*" : " ";
  facts["kind"] = query_kind(user);
  facts["gender"] = (body && stringp(body->query_gender_string()))
                      ? body->query_gender_string() : "-";

  race = body ? body->query_race_name() : nil;
  facts["race"] = (stringp(race) && strlen(race)) ? race : "-";
  facts["guild"] = body ? guild_of(body) : "-";
  facts["level"] = body ? number(body->query_level()) : "-";
  facts["align"] = body ? number(body->query_real_align()) : "-";
  facts["ext_align"] = body ? number(body->query_ext_align()) : "-";

  // query_time_on counts backwards from the moment of connecting
  facts["on"] = body ? span(body->query_time_on()) : "-";
  facts["idle"] = span(user->query_idle());

  facts["terminal"] = stringp(user->query_term_name())
                        ? user->query_term_name() : "-";
  facts["rows"] = number(user->query_rows());
  facts["cols"] = number(user->query_cols());

  facts["ip"] = query_ip_number(user) ? query_ip_number(user) : "-";
  facts["host"] = query_ip_name(user) ? query_ip_name(user) : "-";

  // A location answers with its own save file; a legacy room and anything else
  // only with the object's name, which is what the driver knows it by.
  where = environment(body ? body : user);
  if (where)
  {
    mixed file;

    file = where->query_file_name();
    facts["where"] = (stringp(file) && strlen(file)) ? file
                                                     : object_name(where);
  }
  else
    facts["where"] = "-";

  facts["euid"] = where ? geteuid(where) : geteuid(user);
  facts["dir"] = path_of(user);

  return facts;
}
