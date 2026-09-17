/*
 * /lib/user/communicate.c, old /global/events.c, neverbot 05/19
 *
 *  Changed a bit, added Begosh new channel system.
 *  And some more basic stuff..
 *  Baldrick, may '94.
 *
 *  added block command - Radix 1996
 *
 *  some changes and improvements for ccmud, neverbot 10/04
 */

#include <user/terminal.h>
#include <common/properties.h>
#include <basic/communicate.h>
#include <language.h>

int earmuffs, cols, rows;
string term_name;

// special characters for the terminal output
static mapping colour_map;
static string * blocks;

void create()
{
  term_name = "ansi";
  colour_map = ([ ]);

  // default text-mode size
  cols = 80;
  rows = 25;
}

void event_commands()
{
  add_action("set_our_rows", _LANG_COMM_ROWS);
  add_action("set_our_cols", _LANG_COMM_COLS);
  add_action("set_term_type", _LANG_COMM_TERMINAL);

  add_action("earmuffs", _LANG_COMM_EARMUFFS);

  add_action("do_block", _LANG_COMM_BLOCK);
  add_action("do_unblock", _LANG_COMM_UNBLOCK);
}

void set_rows(int i) { rows = i; }
int query_rows() { return rows; }
int set_our_rows(string str)
{
  int val;

  if (!str)
  {
    notify_fail(_LANG_COMM_ROWS_SYNTAX);
    return 0;
  }

  if (!sscanf(str, "%d", val) || val <= 10)
  {
    notify_fail(_LANG_COMM_ROWS_WRONG);
    return 0;
  }

  write(_LANG_COMM_ROWS_SET);
  rows = val;
  return 1;
} /* set_our_rows() */

int query_cols() { return cols; }
void set_cols(int i) { cols = i; }
int set_our_cols(string str)
{
  int val;

  // The bare verb arrives as an empty string, not as nil, so asking for it
  // without a number has to be caught here or it falls through to the parse
  // below and answers with an error instead of the current setting.
  if (!str || !strlen(trim(str)))
  {
    notify_fail(_LANG_COMM_COLS_SYNTAX);
    return 0;
  }

  // the bounds the message quotes are inclusive
  if (!sscanf(str, "%d", val) || val < 35 || val > 200)
  {
    notify_fail(_LANG_COMM_COLS_WRONG);
    return 0;
  }

  write(_LANG_COMM_COLS_SET);
  cols = val;
  return 1;
} /* set_our_cols() */

string query_term_name() { return term_name; }
int set_term_type(string str)
{
  if (!str)
  {
    notify_fail(sprintf("%-=*s", cols,
      "Sintaxis: " + query_verb() + " <tipo_terminal>\n" +
      "Donde tipo_terminal puede ser alguna de las siguientes:\n - " +
      implode((string *)TERM_HANDLER->query_term_types(), "\n - ") +
      "\nTipo de terminal actual: '" + term_name + "'.\n"));
    return 0;
  }
  if (str != term_name)
  {
    if (member_array(str, (string *)TERM_HANDLER->query_term_types()) != -1)
    {
      colour_map = (mapping)TERM_HANDLER->set_term_type(str);
      term_name = str;
      tell_object(this_player(), "Ok, tipo de terminal seleccionado: "+str+".\n");
      return 1;
    }
    else
    {
      notify_fail("Tipo de terminal " + str  + " no soportado.\n");
      return 0;
    }
  }
  else
  {
    notify_fail("Tipo de terminal cambiado a " + str + ".\n");
    return 0;
  }
}

int query_blocking(string name)
{
  return (member_array(name, blocks) != -1);
}

// int do_unblock(string name){
//   if (!name || name == "") {
//     notify_fail("Sintaxis: desbloquear <jugador>\n");
//     return 0;
//   }
//   if (!block || block == "") {
//     notify_fail("No estas bloqueando a nadie, ¿¡qué quieres desbloquear!?\n");
//     return 0;
//   }
//   if (!user_exists(lower_case(name)) && !find_player(name)) {
//     notify_fail("El usuario parece no estar conectado en este momento.\n");
//     return 0;
//   }
//   if (name == block) {
//     block = "";
//     tell_object(this_player(), "El usuario ha sido desbloqueado.\n");
//     return 1;
//     } else {
//     notify_fail("No estás bloqueando a ese usuario.\n");
//     return 0;
//   }
// }

// int do_block(string name){
//   if (!name || name == "")
//   {
//     if (block)
//     notify_fail("Ya estabas bloqueando los mensajes de: "+
//     capitalize(block)+".\n");
//     else
//     notify_fail("Sintaxis: bloquear <jugador>\n");
//     return 0;
//   }
//   if (!user_exists(lower_case(name)) && !find_player(name))
//   {
//     notify_fail("Ese jugador no está conectado en " + mud_name() + ".\n");
//     return 0;
//   }
//   block = name;
//   tell_object(this_player(), "Ok, bloqueando los mensajes de "+capitalize(block)+".\n");
//   // log_file("block",TO->query_cap_name()+" blocked "+capitalize(block)+
//   //         " : "+ctime(time())+"\n");
//   return 1;
// }

int query_earmuffs() { return earmuffs; }

// The kinds of event this user has asked not to hear. The list is kept as a
// property on the user, the same object the flag above lives on.
string * query_muffled()
{
  mixed on;

  on = this_object()->query_property(EARMUFFS_PROP);
  return pointerp(on) ? on : ({ });
}

// The kinds of event that can be muffled at all.
private string * muffle_types()
{
  string * types;

  types = ({ "shout" });

  if (this_object()->query_coder())
    types += ({ "creator-tell", "multiple-soul", "remote-soul",
                "inter-creator-tell" });

  return types;
}

// The earmuffs verb: the flag, and the list of events it applies to. With no
// argument it says what is on and what is muffled; the words for the flag, for
// all and for none are translated, and a kind of event may be followed by the
// word for on or off, or be left to toggle.
int earmuffs(string str)
{
  string * types, * on, * bits;
  string type;
  int i;

  types = muffle_types();
  on = query_muffled();

  if (!str || !strlen(str))
  {
    write(earmuffs ? _LANG_EARMUFFS_ON : _LANG_EARMUFFS_OFF);

    if (!sizeof(on))
      write(_LANG_EARMUFFS_NOTHING_MUFFLED);
    else
    {
      type = query_multiple_short(on);
      write(_LANG_EARMUFFS_MUFFLING);
    }

    write(_LANG_EARMUFFS_TYPES);
    return 1;
  }

  bits = explode(lower_case(str), " ") - ({ "" });

  if (sizeof(bits) == 1)
  {
    if (member_array(bits[0], _LANG_EARMUFFS_ON_WORDS) != -1)
    {
      earmuffs = 1;
      write(_LANG_EARMUFFS_PUT_ON);
      return 1;
    }

    if (member_array(bits[0], _LANG_EARMUFFS_OFF_WORDS) != -1)
    {
      earmuffs = 0;
      write(_LANG_EARMUFFS_TAKE_OFF);
      return 1;
    }

    if (member_array(bits[0], _LANG_EARMUFFS_NONE_WORDS) != -1)
    {
      this_object()->add_property(EARMUFFS_PROP, ({ }));
      write(_LANG_EARMUFFS_CLEARED);
      return 1;
    }

    if (member_array(bits[0], _LANG_EARMUFFS_ALL_WORDS) != -1)
      bits = types;
  }

  for (i = 0; i < sizeof(bits); i++)
  {
    type = bits[i];

    if (member_array(type, types) == -1)
    {
      write(_LANG_EARMUFFS_UNKNOWN_TYPE);
      continue;
    }

    // a type may be followed by on or off; without one it is toggled
    if (sizeof(bits) > i + 1 &&
        member_array(bits[i + 1], _LANG_EARMUFFS_ON_WORDS) != -1)
      i++;
    else if (sizeof(bits) > i + 1 &&
             member_array(bits[i + 1], _LANG_EARMUFFS_OFF_WORDS) != -1)
    {
      on -= ({ type });
      write(_LANG_EARMUFFS_WILL_NOT_MUFFLE);
      i++;
      continue;
    }
    else if (member_array(type, on) != -1)
    {
      on -= ({ type });
      write(_LANG_EARMUFFS_WILL_NOT_MUFFLE);
      continue;
    }

    if (member_array(type, on) == -1)
      on += ({ type });

    write(_LANG_EARMUFFS_WILL_MUFFLE);
  }

  this_object()->add_property(EARMUFFS_PROP, on);
  return 1;
} /* earmuffs() */

int check_earmuffs(string type)
{
  return member_array(type, query_muffled()) != -1;
} /* check_earmuffs() */

mixed * stats()
{
  return ({
    ({ "Terminal Name", term_name }),
    ({ "Columns", cols, }),
    ({ "Rows", rows }),
  });
}