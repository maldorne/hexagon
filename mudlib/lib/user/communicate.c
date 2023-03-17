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
#include <language.h>

#define HUD "hud_table.c"

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

  if (!str)
  {
    notify_fail(_LANG_COMM_COLS_SYNTAX);
    return 0;
  }

  if (!sscanf(str, "%d", val) || val <= 35 || val >200)
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
//   // log_file("BLOCK",TO->query_cap_name()+" blocked "+capitalize(block)+
//   //         " : "+ctime(time())+"\n");
//   return 1;
// }

int query_earmuffs() { return earmuffs; }
int earmuffs(string frog){
  string *bits, *types, *on;
  int i;

  types = ({ "shout" });
  if (this_object()->query_coder())
  types += ({ "creator-tell", "multiple-soul", "remote-soul",
  "inter-creator-tell" });
  if (!frog)
  {
    if (earmuffs)
    tell_object(this_player(), "Tus tapones estan activados.\n");
    else
    tell_object(this_player(), "Tus tapones estan desactivados.\n");
    on = (string *)this_object()->query_property("earmuffs");
    if (!on) on = ({ });
    if (!sizeof(on))
    {
      tell_object(this_player(), "No tienes ningun canal o evento activo que evitar oir.\n");
    }
    else
    {
      for (i=0;i<sizeof(on);i++)
      tell_object(this_player(), "Tienes "+on[i]+" taponado.\n");
      on = types - on;
      for (i=0;i<sizeof(on);i++)
      tell_object(this_player(), on[i]+" no estás evitando oir los eventos en este momento.\n");
    }
    return 1;
  }
  bits = explode(lower_case(frog), " ");
  bits = bits - ({ "" });
  on = (string *)this_object()->query_property("earmuffs");
  if (!on) on = ({ });
  if (sizeof(bits) == 1)
  switch (bits[0])
  {
    case "on" :
    earmuffs = 1;
    tell_object(this_player(), "Te colocas los tapones (on).\n");
    return 1;

    case "off" :
    earmuffs = 0;
    on=({ });
    tell_object(this_player(), "Retiras los tapones de tus oidos (off).\n");
    return 1;

    case "todo" :
    case "all" :
    bits = types;
    break;
    case "none" :
    on = ({ });
    tell_object(this_player(), "Tus tapones no evitarían nada (no estas escuchando nada)\n");
    return 1;
  }
  for (i=0;i<sizeof(bits);i++)
  if (member_array(bits[i], types) == -1)
  {
    tell_object(this_player(), "No puedo dejar de escuchar los eventos "+bits[i]+".\n");
  }
  else
  {
    if (sizeof(bits) > i+1)
    {
      switch (bits[i+1])
      {
        case "on" :
        if (member_array(bits[i], on) == -1)
        on += ({ bits[i] });
        tell_object(this_player(), "Tus eventos de "+bits[i]+" serán taponados.\n");
        i++;
        break;

        case "off" :
        on = on - ({ bits[i] });
        tell_object(this_player(), "Tus eventos de "+bits[i]+" no serán taponados.\n");
        i++;
        break;

        default :
        if (member_array(bits[i], on) == -1)
        {
          tell_object(this_player(), "Tus eventos de "+bits[i]+" serán taponados.\n");
          on += ({ bits[i] });
        }
        else
        {
          tell_object(this_player(), "Tus eventos de "+bits[i]+" no serán taponados.\n");
          on = on - ({ bits[i] });
        }
        break;
      } // switch
    }
    else
    {
      if (member_array(bits[i], on) == -1)
      {
        tell_object(this_player(), "Tus eventos de "+bits[i]+" serán taponados.\n");
        on += ({ bits[i] });
      }
      else
      {
        tell_object(this_player(), "Tus eventos de "+bits[i]+" no serán taponados.\n");
        on = on - ({ bits[i] });
      }
    }
  }
  this_player()->add_property("earmuffs", on);
  return 1;
} /* earmuffs() */

int check_earmuffs(string type) {
  string *on;

  on = (string *)this_object()->query_property("earmuffs");
  if (!on) return 0;
  if (member_array(type, on) == -1)
  return 0;
  return 1;
} /* check_earmuffs() */

mixed stats()
{
  return ({
    ({ "Terminal Name", term_name }),
    ({ "Columns", cols, }),
    ({ "Rows", rows }),
  });
}