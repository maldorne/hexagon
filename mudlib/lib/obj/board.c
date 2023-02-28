// Traducción revisada para CcMud, neverbot 7/03
// Añadido numero de notas sin leer, neverbot 2/06

#include <mud/mail.h>
#include <item/board.h>
#include <basic/communicate.h>

inherit "/lib/item.c";

/* Taniwha 1995, a list of immort write only boards, easy to extend */
// static string *readonly = ({"playerinfo","quests","announcements"});
static string *readonly;
string board_name;
mapping being_written;

void create()
{
  ::create();

  readonly = ({ "informacion" });
  board_name = "informacion";
  being_written = ([ ]);
  reset_get();
}

void setup()
{
  set_name("tablón de notas");
  add_alias("tablon");
  add_alias("tablón");

  set_short("Tablón de Notas");
  set_main_plural("Tablones");
  add_plural("tablones");
}

void init()
{
  add_action("read", "leer");
  add_action("post", "escribir");
  add_action("eat", "borrar");
  add_action("followup", "responder");
  add_action("reply", "mudmail");
  add_action("subjects", "temas");
  ::init();
}

string query_plural()
{
  mixed *stuff;

  stuff = (mixed *)BOARD_HAND->get_subjects(board_name);
  switch (sizeof(stuff)) {
    case 0:  return pluralize(::short(0))+" [ Vacío ]";
    case 1:  return pluralize(::short(0))+" [ 1 nota ]";
  }
   return pluralize(::short(0))+" [ "+sizeof(stuff)+" notas ]";
}

string short(varargs int dark)
{
  mixed *stuff;
  int pending;

  pending = 0;
  stuff = (mixed *)BOARD_HAND->get_subjects(board_name);
  pending = this_object()->query_new_messages();

  switch (sizeof(stuff)) {
    case 0:  return ::short(dark)+" [ Vacío ]";
    case 1:  return ::short(dark)+" [ 1 nota ]";
    default:
    if (pending)
      return ::short(dark)+ " [ "+sizeof(stuff)+" notas (" +
                            ((pending == 1)?("una"):query_num(pending, 20)) +
                            " sin leer) ]";
    else
      return ::short(dark)+" [ "+sizeof(stuff)+" notas ]";
  }
}

string the_date(int i)
{
  return ctime(i)[4..9];
}

int subjects(string str, int dark)
{
  int i;
  mixed *stuff;
  string ret;
  mapping news_rc;

  stuff = (mixed *)BOARD_HAND->get_subjects(board_name);
  if (!sizeof(stuff)) {
    notify_fail("El tablón está completamente vacío.\n");
    return 0;
  }
  news_rc = (mapping)this_player()->query_property(NEWS_RC);
  if (!news_rc)
    news_rc = ([ ]);
  ret = "";

  if (this_player()->query_coder())
    ret += "El tablón de notas de '%^BOLD%^"+board_name+"%^RESET%^'.\n\n";

  for (i=0;i<sizeof(stuff);i++)
  {
    if (news_rc[board_name] < stuff[i][B_TIME])
      ret += sprintf("N %2d: %-=*s\n", i+1, (int)this_user()->query_cols()-6,
             stuff[i][B_SUBJECT]+" ("+
             ((!this_player()->query_coder() &&
                sizeof(get_files("/home/"+stuff[i][B_NAME])))?"Administrador":capitalize(stuff[i][B_NAME])) +
             " "+the_date(stuff[i][B_TIME])+")");
    else
      ret += sprintf("  %2d: %-=*s\n", i+1, (int)this_user()->query_cols()-6,
             stuff[i][B_SUBJECT]+" ("+
             ((!this_player()->query_coder() &&
                sizeof(get_files("/home/"+stuff[i][B_NAME])))?"Administrador":capitalize(stuff[i][B_NAME])) +
             " "+the_date(stuff[i][B_TIME])+")");
  }
  this_player()->set_finish_func(0);
  this_player()->more_string(ret, "Temas");
  return 1;
} /* subjects() */

string long(string str, int dark)
{
  int i,newones;
  mixed *stuff;
  string ret;
  mapping news_rc;

  ret = "";

  stuff = (mixed *)BOARD_HAND->get_subjects(board_name);

  if (this_player()->query_coder())
    ret += "El tablón de notas de '%^BOLD%^"+board_name+"%^RESET%^'.\n";

  ret += "Comandos:\n";
  ret += sprintf("%#-*s\n\n", this_user()->query_cols(),
                            "leer [número de nota]\nescribir <tema>\n"+
                            "temas\nborrar <número de nota>\n"+
                            "responder <número de nota>\nmudmail <número de nota>\n");
  if (!sizeof(stuff))
    return ret+"El tablón está completamente vacío.\n";

  news_rc = (mapping)this_player()->query_property(NEWS_RC);

  if (!news_rc)
    news_rc = ([ ]);
  newones = 0;

  for (i=0;i<sizeof(stuff);i++)
  {
    if (news_rc[board_name] < stuff[i][B_TIME])
    {
      ret += sprintf("N %2d: %-=*s\n", i+1, (int)this_user()->query_cols()-6,
             stuff[i][B_SUBJECT]+" ("+
             ((!this_player()->query_coder() &&
                sizeof(get_files("/home/"+stuff[i][B_NAME])))?"Administrador":capitalize(stuff[i][B_NAME])) +
             " "+the_date(stuff[i][B_TIME])+")");
      newones +=1;
    }
    else if (news_rc[board_name] < stuff[i][B_TIME]+(2*24*60*60))
      ret += sprintf("  %2d: %-=*s\n", i+1, (int)this_user()->query_cols()-6,
             stuff[i][B_SUBJECT]+" ("+
             ((!this_player()->query_coder() &&
                sizeof(get_files("/home/"+stuff[i][B_NAME])))?"Administrador":capitalize(stuff[i][B_NAME])) +
             " "+the_date(stuff[i][B_TIME])+")");
  }

  if (!newones)
    ret += "\nNo hay nuevas notas.\n";

  return ret;
} /* long() */

void string_more(string arg, string prompt)
{
  /* changed to our more_string

  if (!(obj = (object)MAIL_SERVER->mail_reader(this_player()))) {
    log_file(LOG_FILE, "board: got NULL MAIL_READER from MAIL_SERVER.\n");
    write("MAIL_SERVER error ...\n");
    return;
  }
  obj->string_more(arg, prompt);
  */
  this_player()->set_finish_func(0);
  this_player()->more_string(arg, prompt);
} /* string_more() */

int read(string str)
{
  int num, i;
  mixed stuff;
  mapping news_rc;
  string mensaje,lang;

  notify_fail("Sintaxis: leer <número de nota>\n");
  stuff = (mixed *)BOARD_HAND->get_subjects(board_name);
  news_rc = (mapping)this_player()->query_property(NEWS_RC);
  if (!news_rc)
    news_rc = ([ ]);
  if (!strlen(str)) {
    i = sizeof(stuff)-1;
    while (i >= 0 &&stuff[i][B_TIME] > news_rc[board_name] )
      i--;
    if (i == sizeof(stuff)-1) {
      notify_fail("No te quedan mensajes por leer.\n");
      return 0;
    }
    num = i+2;
  } else if (sscanf(str, "%d", num) != 1)
    return 0;
  if (num < 1 || num > sizeof(stuff)) {
    notify_fail("No hay ninguna nota con ese número.\n");
    return 0;
  }
  num --;
  if (news_rc[board_name] < stuff[num][B_TIME])
  {
    news_rc[board_name] = stuff[num][B_TIME];
    this_player()->add_property(NEWS_RC, news_rc);
  }
  mensaje = BOARD_HAND->get_message(board_name, num);
  lang = BOARD_HAND->get_language(board_name,num);

  if (!lang)
      lang="comun";

  if (member_array(lang, this_player()->query_languages()) == -1)
  {
       mixed stri;

       if ((stri = (mixed)LANGUAGE_HANDLER->query_garble_object(lang)))
            if ((stri = (mixed)stri->garble_say("",mensaje)))
            {
                 mensaje = stri[1];
            }
  }

  mensaje = sprintf("  %-=*s\n", (int)this_user()->query_cols() - 2, mensaje);

  string_more(sprintf("Nota #%d escrita por %s el %s\nTítulo: '%s'\n\n",
              num+1, "%^GREEN%^" +
              ((!this_player()->query_coder() &&
                 sizeof(get_files("/home/"+stuff[num][B_NAME])))?"Administrador":capitalize(stuff[num][B_NAME])) +
              "%^RESET%^",
              ctime(stuff[num][B_TIME]),
              "%^GREEN%^"+stuff[num][B_SUBJECT][0..(int)this_user()->query_cols()-10]+"%^RESET%^")+
              mensaje,
              "[Nota "+(num+1)+"]");
  return 1;
} /* read() */

int post(string str)
{
  if ( (member_array(board_name,readonly) != -1) && !this_player()->query_coder())
  {
    write("Sólo los programadores pueden escribir en este tablón.\n");
    return 1;
  }
  notify_fail("Sintaxis: escribir <tema>\n");

  if (!strlen(str))
    return 0;
  /* ok shove the editing stuff in here.  Lets make it function string_edit
   * sound froggy?
   */
  /*
  string_edit("");
  body = string_edit_res;
  */
  being_written[this_player()->query_name()] = str;
  this_player()->do_edit(0,"end_of_thing");
  return 1;
} /* post() */

void end_of_thing(string body)
{
  if (body && (body != "") && being_written[this_player()->query_name()])
    if (!BOARD_HAND->add_message(board_name, this_player()->query_name(),
                            being_written[this_player()->query_name()], body,this_player()->query_current_language()))
      write("Error escribiendo mensaje.\n");
    else
    {
      write("Mensaje escrito.\n");
      event(users(), "inform", this_player()->query_cap_name()+" escribe una nota "+
            "en '"+board_name+"'", "tablones");
    }
  else
    write("El texto no se ha guardado.\n");
  being_written = m_delete(being_written, this_player()->query_name());
  return ;
} /* end_of_thing() */

int eat(string str) {
  int num, i, eaten;

  notify_fail("Sintaxis: borrar [hasta] <número de nota>\n");
  if (!str || (str == ""))
    return 0;
  if (sscanf(str, "hasta %d", num)) {
    for (i=0;i<num;i++)
      eaten += (int)BOARD_HAND->delete_message(board_name, i);
    if (!eaten) {
      notify_fail("Fallo al borrar.\n");
      return 0;
    }
    write("Borras "+eaten+" notas.\n");
    say(this_player()->query_cap_name()+" arranca y destruye la nota "+
        query_num(eaten, 0)+".\n");
    if (eaten == 1)
      event(users(), "inform", this_player()->query_cap_name()+" borra una nota "+
            "de '"+board_name+"'", "tablones");
    else
      event(users(), "inform", this_player()->query_cap_name()+" borra "+
            query_num(eaten, 0)+" notas de '"+board_name+"'", "tablones");
    return 1;
  } else if (sscanf(str, "%d", num) != 1)
    return 0;
  if (!BOARD_HAND->delete_message(board_name, num-1))
    return 0;
  write("Arrancas y destruyes la nota número "+num+".\n");
  say(this_player()->query_cap_name()+" arranca y destruye la nota número "+
         num+".\n");
  event(users(), "inform", this_player()->query_cap_name()+" borra una nota "+
                           "de "+board_name, "tablones");
  return 1;
} /* eat() */

int followup(string str)
{
  int num, i;
  mixed stuff;
  string s;

  if ( (member_array(board_name,readonly) != -1) && !this_player()->query_coder())
  {
    write("Sólo los programadores pueden escribir en este tablón.\n");
    return 1;
  }
  notify_fail("Sintaxis: responder <número de nota>\n");
  if (!strlen(str))
    return 0;
  if (sscanf(str, "%d", num) != 1)
    return 0;
  stuff = (mixed *)BOARD_HAND->get_subjects(board_name);
  if (num < 1 || num > sizeof(stuff)) {
    notify_fail("No hay ninguna nota con ese número.\n");
    return 0;
  }
  if (sscanf(stuff[num-1][B_SUBJECT], "Re:#%d %s", i, s) != 2)
    being_written[this_player()->query_name()] = "Re:#1 "+stuff[num-1][B_SUBJECT];
  else
    being_written[this_player()->query_name()] = "Re:#"+(i+1)+" "+s;
  this_player()->do_edit(0,"end_of_thing");
  return 1;
} /* followup() */

int reply(string str)
{
  int num;
  mixed stuff;

  // PENDIENTE: Revisar esto el dia que revise el sistema de correo!!!!
  // neverbot 7/03 (el MAIL_TRACK intenta clonar un objeto que no existe
  //  para enviar la carta... habra que revisar el sistema con calma!!!

  if (this_player()->query_player())
  {
    notify_fail("El servicio de respuesta de notas por correo está " +
        "desactivado temporalmente. Utiliza 'responder <nota>' " +
        "para responder dentro del propio tablón.\n");
    return 0;
  }

  // if (board_name=="announcements" && !this_player()->query_coder() )
  if ( (member_array(board_name, readonly) != -1) && !this_player()->query_coder())
  {
    write("Sólo los programadores pueden escribir en este tablón.\n");
    return 1;
  }
  notify_fail("Sintaxis: mudmail <número de nota>\n");
  if (!strlen(str))
    return 0;
  if (sscanf(str, "%d", num) != 1)
    return 0;
  stuff = (mixed *)BOARD_HAND->get_subjects(board_name);
  if (num < 1 || num > sizeof(stuff)) {
    notify_fail("No hay ninguna nota con ese número.\n");
    return 0;
  }
  MAIL_TRACK->mail(stuff[num-1][B_NAME], stuff[num-1][B_SUBJECT]);
  return 1;
} /* reply() */

void set_board_name(string str)
{
  str = lower_case(str);
  board_name = str;
  BOARD_HAND->create_board(board_name, 0);
}

void set_datafile(string str)
{
  set_board_name(str);
  /*board_name = str;*/
}

int query_new_messages()
{
  mixed *notes;
  mixed news_rc;
  int i;
  int number;

  number = 0;

  news_rc = this_player()->query_property(NEWS_RC);

  if (!news_rc || !mappingp(news_rc))
    news_rc = ([ ]);

  notes = (mixed *)BOARD_HAND->get_subjects(board_name);

  if (!sizeof(notes))
    return 0;

  if (undefinedp(news_rc[board_name]))
  {
      news_rc[board_name] = this_player()->query_start_time();
      this_player()->add_property(NEWS_RC, news_rc);
  }

  // Cambiado por neverbot, 02/2006
  // Ahora devolvemos el numero de notas que quedan por leer
  // return (notes[sizeof(notes)-1][B_TIME] > news_rc[board_name]);

  if (notes[sizeof(notes)-1][B_TIME] > news_rc[board_name])
  {
    for (i = 0; i < sizeof(notes); i++)
    {
      if (notes[i][B_TIME] > news_rc[board_name])
       number++;
    }
    return number;
  }
  else
    return 0;
} /* query_new_messages() */
