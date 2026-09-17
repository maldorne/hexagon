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
// the players whose messages this account does not want, by name; it is saved
// with the account, so it holds for every character played from it
string * blocks;

void create()
{
  term_name = "ansi";
  colour_map = ([ ]);
  blocks = ({ });

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

// Change the terminal type without a word to the player; 0 if it is unknown.
int set_terminal(string str)
{
  if (member_array(str, (string *)TERM_HANDLER->query_term_types()) == -1)
    return 0;

  colour_map = (mapping)TERM_HANDLER->set_term_type(str);
  term_name = str;
  return 1;
}
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
    if (set_terminal(str))
    {
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

// Whether this account blocks a player, by name.
int query_blocking(string name)
{
  if (!pointerp(blocks) || !stringp(name))
    return 0;

  return member_array(name, blocks) != -1;
}

string * query_blocks()
{
  return pointerp(blocks) ? blocks + ({ }) : ({ });
}

// The name a player typed, through their nicknames, as it is stored.
private string block_name(string str)
{
  if (this_object()->player())
    str = this_object()->player()->expand_nickname(str);

  return lower_case(trim(str));
}

int do_block(string str)
{
  string name, * names;
  int i;

  if (!pointerp(blocks))
    blocks = ({ });

  if (!str || !strlen(trim(str)))
  {
    if (!sizeof(blocks))
    {
      write(_LANG_BLOCK_NOBODY);
      return 1;
    }

    names = ({ });
    for (i = 0; i < sizeof(blocks); i++)
      names += ({ capitalize(blocks[i]) });

    write(_LANG_BLOCK_LIST);
    return 1;
  }

  name = block_name(str);

  if (this_object()->player() &&
      name == this_object()->player()->query_name())
  {
    notify_fail(_LANG_BLOCK_NOT_YOURSELF);
    return 0;
  }

  if (!player_exists(name))
  {
    notify_fail(_LANG_BLOCK_NO_SUCH_PLAYER);
    return 0;
  }

  if (member_array(name, blocks) != -1)
  {
    notify_fail(_LANG_BLOCK_ALREADY);
    return 0;
  }

  blocks += ({ name });
  this_object()->save_me();

  write(_LANG_BLOCK_DONE);
  return 1;
}

int do_unblock(string str)
{
  string name;

  if (!str || !strlen(trim(str)))
  {
    notify_fail(_LANG_BLOCK_SYNTAX);
    return 0;
  }

  name = block_name(str);

  if (!pointerp(blocks) || member_array(name, blocks) == -1)
  {
    notify_fail(_LANG_BLOCK_NOT_BLOCKED);
    return 0;
  }

  blocks -= ({ name });
  this_object()->save_me();

  write(_LANG_BLOCK_UNDONE);
  return 1;
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
void set_earmuffs(int on) { earmuffs = on ? 1 : 0; }

// The earmuffs verb: on or off. With them on, shouts are not heard, and the
// one wearing them cannot shout either.
int earmuffs(string str)
{
  if (!str || !strlen(trim(str)))
  {
    write(earmuffs ? _LANG_EARMUFFS_ON : _LANG_EARMUFFS_OFF);
    return 1;
  }

  str = lower_case(trim(str));

  if (affirmative(str))
  {
    earmuffs = 1;
    write(_LANG_EARMUFFS_PUT_ON);
    return 1;
  }

  if (negative(str))
  {
    earmuffs = 0;
    write(_LANG_EARMUFFS_TAKE_OFF);
    return 1;
  }

  notify_fail(_LANG_EARMUFFS_SYNTAX);
  return 0;
} /* earmuffs() */

mixed * stats()
{
  return ({
    ({ "Terminal Name", term_name }),
    ({ "Columns", cols, }),
    ({ "Rows", rows }),
  });
}