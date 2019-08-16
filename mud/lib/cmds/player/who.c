/*
 * Who.c, once upon in /global/finger.c
 * made splitted by Ducky, some changes by Baldrick and Aragorn.
 * Externalized by someone@RD.
 * Moved back to FR and Fr'ized by Baldrick. oct '95
 *
 * Color added by Radix ... splish splash
 * Damned Hide command, fixed for players and immortals, Radix 1996
 * For an overall ~12% increase, you get who sorted - Radix 1996
 *  - ie with 25 users, it's about 8k cpu cycles sorted, normally 7k
 *  Nevertheless, will improve greatly if sorted the FINAL string
 *  array, instead of doing the obs first
 *
 * Added race option, and cleaned up the 'logon the unknown's - Radix
 * Some translations made by Osucaru@Rl. Nov '97
 * Modified for selective who Spp@Rl Jan'98
 *
 * neverbot 21/01/03, ported to ccmud:
 *  - less filter_array done to the object list
 *  - Added who <my name>, same behaviour as whoami.
 *  - new guild_joined, 18/7/03
 *
 */

#include <mud/cmd.h>
#include <living/races.h>

inherit CMD_BASE;

// TODO: this shouldn`t be hard coded here
#define RACES ({ \
  "humano", \
  "tresio", \
  "velan", \
  "aenur", \
})

void setup() {
  position = 0;
}

string query_usage() {
  return "command";
}

// Sorted and race functions - Radix 1996
int compare_obs(object ob1, object ob2) {
  string s1, s2;
  s1 = ob1->query_name();
  s2 = ob2->query_name();
  if (s1 == s2)
    return 0;
  if (!s1 || !s2)
    return 1;
  if (s1 > s2)
    return 1;
  return -1;
}

int query_valid_race(string race) {
  return member_array(lower_case(race), RACES) != -1;
}

int query_inmortal(object me) {
  return me->query_coder();
}

int no_coders(object me) {
  return !(me->query_coder());
}

// base_race added, for the subrace/culture system, neverbot 6/03
int compare_race(object ob, string str) {
  return ((lower_case(ob->query_race_name()) == str) ||
    (lower_case(ob->query_base_race_name()) == str));
}

int remove_logon(object ob) {
  return (!(ob->query_short() == "logon"));
}

string who_am_i(object player) {
  mapping guilds, jobs;
  string ret;
  string *g_names, *j_names;
  int i;

  guilds = player->query_guild_joined();
  jobs = player->query_job_joined();
  g_names = keys(guilds);
  j_names = keys(jobs);

  ret = player->query_cap_name() + " " +
    player->query_article() + " " +
    player->query_race_name() + ":\n";

  if (player->query_class_ob()) {
    ret += "   Clase:\n     " + (player->query_class_ob())->query_short() +
      " de nivel " + player->query_level() + ".\n";
  }

  for (i = 0; i < sizeof(g_names); i++) {
    if (i == 0)
      ret += "   Gremios:\n";
    if (!load_object(g_names[i]))
      continue;
    ret += "     " + capitalize(g_names[i]->query_short());
    ret += " de nivel " + guilds[g_names[i]][0] + ".\n";
  }

  for (i = 0; i < sizeof(j_names); i++) {
    if (i == 0)
      ret += "   Oficios:\n";
    if (!load_object(j_names[i]))
      continue;
    ret += "     " + capitalize(j_names[i]->query_short());
    ret += " de nivel " + jobs[j_names[i]][0] + ".\n";
  }

  // ret += ".\n";
  return ret;
}

string who_string(int width, int cre, string str) {

  object * arr, * arr_aux, user;
  int i, num_people, num_disconnected_people;
  int creators, what;
  string s, tmp, nam, imm, play, prt, race;
  //  mixed ee;
  string ttl;
  ttl = fix_string("======] %^GREEN%^" + mud_name() + "%^RESET%^ [======");
  // Radix...

  if (!strlen(str))
    what = 0;
  else if (str == "programadores"[0..strlen(str) - 1])
    what = 1;
  else if (str == "jugadores"[0..strlen(str) - 1])
    what = 2;
  else if (str == this_player()->query_name() ||
    this_player()->expand_nickname(str) == this_player()->query_name())
    what = 4;
  else if (query_valid_race(str))
    what = 3;
  else
    return "Sintaxis: who\n" +
      "          who programadores\n" +
      "          who jugadores\n" +
      "          who <raza|cultura>\n" +
      "          who <mi nombre>\n";

  if (what == 4) {
    return who_am_i(this_player());
  }

  arr = players();
  arr = filter_array(arr, "remove_logon", this_object());

  if (what == 1) {
    arr = filter_array(arr, "query_inmortal", this_object());
  } else if (what == 3) {
    // if we want to filter by an specific race, we can use
    // the query_race_name function
    // filter coders, neverbot 21/01/03
    arr = filter_array(arr, "no_coders", this_object());
    arr = filter_array(arr, "compare_race", this_object(), lower_case(str));
    race = str;
    str = "jugadores";
  } else {
    arr = sort_array(arr, "compare_obs", this_object());
    race = "jugadores";
  }

  creators = num_people = 0;
  s = imm = play = "";

  prt = "\n";
  prt += sprintf("%p%|*s\n", '-', width + (strlen(ttl) - visible_strlen(ttl)), ttl);
  prt += sprintf("%|*s\n", width, "Hoy es " + ctime(time()) + " en el mundo real", width);
  prt += sprintf("%|*s\n", width, ctime(time(), 3), width);

  // traverse the player list
  // if what == 1 only coders
  // if what == 2 only players
  // if what == 3 only players of an specific race
  // if !what coders + players
  for (i = 0; i < sizeof(arr); i++) {
    // get the user object
    user = arr[i]->user();

    if (!(tmp = (string) arr[i]->query_short()))
      continue;

    if (arr[i]->query_coder()) {
      if (!what || what == 1) {
        // Here it denies to show "super invis"
        if (user->query_invis() > 1) {
          if (!this_player()->query_administrator())
            continue;
        }

        if (cre && user->query_invis() == 1)
          tmp = "*" + (string) arr[i]->query_short() + "*";
        if (cre && user->query_invis() == 2)
          tmp = "**" + (string) arr[i]->query_short() + "**";
        if (!cre && user->query_invis() == 1)
          continue;
        s = "";
        nam = tmp;

        if (cre && (tmp = arr[i]->query_in_editor())) {
          // only admins see this
          if (stringp(tmp))
            if (this_player()->query_admin())
              s += " %^GREEN%^(Editando: " + (string) tmp + ")%^RESET%^";

        }

        if (tmp = (string) arr[i]->query_gtitle())
          s += ", " + tmp;
        // if (tmp = (string) arr[i]->query_title())
        //   s += ", " + tmp;
        if (tmp = arr[i]->query_extitle())
          s += " (" + tmp + ")";
        if (cre && arr[i]->query_property("ausente"))
          s += " (%^WHITE%^%^BOLD%^Ausente%^RESET%^)";
        if (user->query_idle() > 120)
          s += " (%^CYAN%^%^BOLD%^Inactivo: " + (user->query_idle() / 60) + "%^RESET%^)";

        // imm += sprintf(" %s%*-=s", nam, width, s) + "%^RESET%^\n";
        imm += sprintf(" %s%-*s", nam, width, s) + "%^RESET%^\n";

        if (!user->query_invis() || cre) {
          creators++;
        }
      }
    } // if (query_coder)
    else // is not a coder
      if (what != 1) // have to show players
    {
      tmp = (string) arr[i]->query_short() + " " +
        (arr[i]->query_race_ob() ?
          arr[i]->query_race_ob()->query_race_gender_string(arr[i]) :
          STD_RACE->query_race_gender_string(arr[i]));
      // GENDER_TABLE->query_race_gender_string(arr[i]);

      if (!tmp)
        continue;
      s = "";
      nam = tmp;

      if (arr[i]->query_property("guest"))
        s += " invitad" + ((arr[i]->query_gender() == 2) ? "a" : "o") + " en " + mud_name();
      if (tmp = (string) arr[i]->query_extitle())
        s += " (" + tmp + ")";
      if (user->query_idle() > 120)
        s += " (%^GREEN%^Inactivo: " + (user->query_idle() / 60) + "%^RESET%^)";

      play += sprintf("          %*-=s", width - 10, nam + s) + "\n";
      num_people++;
    }
  } // for

  if (!what || (what == 1)) {
    // coders
    ttl = fix_string("] %^BOLD%^WHITE%^Programadores%^RESET%^ [");

    // show only if we have found coders
    if (creators) {
      prt += sprintf("%p%|*s\n", '-', width + (strlen(ttl) - visible_strlen(ttl)), ttl);
      prt += imm;
    }
  }

  // players
  if (what != 1) {
    if (what == 3) {
      ttl = fix_string("] %^BOLD%^WHITE%^" + capitalize(race) + "%^RESET%^ [");
    } else {
      ttl = fix_string("] %^BOLD%^WHITE%^Jugadores%^RESET%^ [");
    }
    if (num_people) {
      prt += sprintf("%p%*|s\n", '-', width + (strlen(ttl) - visible_strlen(ttl)), ttl);
      prt += play;
    }
  }

  // only coders
  if (what == 1) {
    if (!creators)
      tmp = "> %^GREEN%^No hay programadores conectados%^RESET%^ <";
    else
      tmp = "> %^GREEN%^Hay " + query_num(creators, 100) + " programador" +
        (creators < 2 ? "" : "es") + " en " + mud_name() + "%^RESET%^ <";
  }
  // only players
  else if (what == 2 || what == 3) {
    if (!num_people)
      tmp = "> %^GREEN%^No hay jugadores conectados%^RESET%^ <";
    else if (cre && num_people == 1)
      tmp = "> %^GREEN%^Es el único que está conectado en " + mud_name() + "%^RESET%^ <";
    else if (num_people == 1)
      tmp = "> %^GREEN%^Eres el único que está conectado en " + mud_name() + "%^RESET%^ <";
    else
      tmp = "> %^GREEN%^Hay " + query_num(num_people, 100) + " jugador" + (num_people < 2 ? "" : "es") +
      " en " + mud_name() + "%^RESET%^ <";
  }
  // what == 0, coders + players
  else {
    // uncanny case... except we are in invis 2 and we are the only one coder connected
    if (!num_people && !creators)
      tmp = "> %^GREEN%^No hay jugadores conectados%^RESET%^ <";

    else if (num_people + creators == 1)
      tmp = "> %^GREEN%^Eres el único que está conectado en " + mud_name() + "%^RESET%^ <";

    else if (!creators && num_people)
      tmp = "> %^GREEN%^Hay " + query_num(num_people, 100) + " jugador" + (num_people < 2 ? "" : "es") +
      " en " + mud_name() + "%^RESET%^ <";

    else if (!num_people && creators)
      tmp = "> %^GREEN%^Hay " + query_num(creators, 100) + " programador" +
      (creators < 2 ? "" : "es") + " en " + mud_name() + "%^RESET%^ <";

    else
      tmp = "> %^GREEN%^Hay " + query_num(creators, 100) + " programador" +
      (creators < 2 ? "" : "es") + " y " + query_num(num_people, 100) + " jugador" +
      (num_people < 2 ? "" : "es") + " en " + mud_name() + "%^RESET%^ <";
  }

  tmp = fix_string(tmp);
  prt += sprintf("%p%|*s\n", '-', width + (strlen(tmp) - visible_strlen(tmp)), tmp);

  //     tmp ="> Total gente conectada: "+number+" <";
  //  prt += sprintf("%*'-'|s\n", width+0, tmp);

  // A�adido por Folken, para comprobar gente con la conexion caida
  // (no salen en users())
  if (this_player()->query_coder()) {
    // Todos los objectos con enable_commands() llamado
    arr_aux = livings();
    // Quitamos los del who
    arr_aux -= users();
    num_disconnected_people = 0;
    for (i = 0; i < sizeof(arr_aux); i++) {
      if (arr_aux[i]->query_player() && (arr_aux[i]->query_invis() != 2)) {
        // prt += sprintf("          %*-=s", width, capitalize(arr_aux[i]->query_name())) + "\n";
        prt += sprintf("          %-*s", width, capitalize(arr_aux[i]->query_name())) + "\n";
        num_disconnected_people++;
      }
    }
    if (num_disconnected_people > 0) {
      tmp = (num_disconnected_people == 1) ?
          ("> %^GREEN%^Un personaje con la conexión caída%^RESET%^ <") :
          ("> %^GREEN%^" + capitalize(number_as_string(num_disconnected_people)) + " personajes con la conexión caída%^RESET%^ <");
      tmp = fix_string(tmp);
      prt += sprintf("%p%|*s\n", '-', width + (strlen(tmp) - visible_strlen(tmp)), tmp);
    }
  }

  return prt;
} /* who_string() */

int do_who(string str) {
  tell_object(this_player(),
    who_string((int) this_user()->query_cols(),
      this_player()->query_coder(), str));
  return 1;
}

int cmd(string str, object me, string verb) {
  write(who_string((int) me->user()->query_cols(),
    (int) me->query_coder(), str));
  // me->set_trivial_action();

  return 1;
}