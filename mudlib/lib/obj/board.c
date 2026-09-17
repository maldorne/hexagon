// Traducción revisada para CcMud, neverbot 7/03
// Añadido numero de notas sin leer, neverbot 2/06
// Reviewed for Hexagon, neverbot 09/2026: translated, notes written with the
// user's editor, and notes answered by mail through the mail reader.

#include <item/board.h>
#include <basic/communicate.h>
#include <files/postal.h>
#include <user/player.h>
#include <language.h>

inherit "/lib/item.c";

/* Taniwha 1995, a list of immort write only boards, easy to extend */
// static string *readonly = ({"playerinfo","quests","announcements"});
static string *readonly;
string board_name;
// character name -> subject of the note being written
static mapping being_written;

int subjects(string str);
int read(string str);
int post(string str);
int eat(string str);
int followup(string str);
int reply(string str);

void create()
{
  ::create();

  readonly = ({ "informacion" });
  board_name = "informacion";
  being_written = ([ ]);
  reset_get();

  set_name(_LANG_BOARD_NAME);
  add_alias(_LANG_BOARD_ALIASES);
  set_short(_LANG_BOARD_SHORT);
  set_main_plural(_LANG_BOARD_PLURAL);
  add_plural(_LANG_BOARD_PLURALS);
}

int query_board() { return 1; }
string query_board_name() { return board_name; }

void init()
{
  add_action("read", _LANG_BOARD_READ_CMDS);
  add_action("post", _LANG_BOARD_POST_CMDS);
  add_action("eat", _LANG_BOARD_DELETE_CMDS);
  add_action("followup", _LANG_BOARD_FOLLOWUP_CMDS);
  add_action("reply", _LANG_BOARD_MUDMAIL_CMDS);
  add_action("subjects", _LANG_BOARD_SUBJECTS_CMDS);
  ::init();
}

string query_plural()
{
  mixed *stuff;

  stuff = (mixed *)BOARD_HAND->get_subjects(board_name);
  switch (sizeof(stuff)) {
    case 0:  return pluralize(::short(0)) + _LANG_BOARD_EMPTY_TAG;
    case 1:  return pluralize(::short(0)) + _LANG_BOARD_ONE_NOTE_TAG;
  }
  return pluralize(::short(0)) + _LANG_BOARD_NOTES_TAG;
}

string short(varargs int dark)
{
  mixed *stuff;
  int pending;

  pending = 0;
  stuff = (mixed *)BOARD_HAND->get_subjects(board_name);
  pending = this_object()->query_new_messages();

  switch (sizeof(stuff)) {
    case 0:  return ::short(dark) + _LANG_BOARD_EMPTY_TAG;
    case 1:  return ::short(dark) + _LANG_BOARD_ONE_NOTE_TAG;
    default:
    if (pending)
      return ::short(dark) + _LANG_BOARD_NOTES_UNREAD_TAG;
    else
      return ::short(dark) + _LANG_BOARD_NOTES_TAG;
  }
}

string the_date(int i)
{
  return ctime(i, 4)[0..7];
}

// Who wrote a note, as a player sees it: the names of coders are hidden.
private string author_of(mixed * note)
{
  if (!this_player()->query_coder() && sizeof(get_files("/home/" + note[B_NAME])))
    return _LANG_BOARD_ADMINISTRATOR;

  return capitalize(note[B_NAME]);
}

private string note_line(mixed * note, int number, int unread)
{
  return sprintf("%s %2d: %-=*s\n", (unread ? _LANG_BOARD_MARK_UNREAD : " "), number,
                 (int)this_user()->query_cols() - 6,
                 note[B_SUBJECT] + " (" + author_of(note) + " " +
                 the_date(note[B_TIME]) + ")");
}

// When the player last read each board: board name -> time of that note.
private mapping query_news_rc()
{
  mixed news_rc;

  news_rc = this_player()->query_property(NEWS_RC);

  if (!mappingp(news_rc))
    news_rc = ([ ]);

  if (!intp(news_rc[board_name]))
    news_rc[board_name] = 0;

  return news_rc;
}

private int can_write()
{
  if ((member_array(board_name, readonly) != -1) && !this_player()->query_coder())
  {
    write(_LANG_BOARD_READONLY);
    return 0;
  }

  if (this_player()->query_property(GUEST_PROP))
  {
    write(_LANG_BOARD_NO_GUESTS);
    return 0;
  }

  return 1;
}

int subjects(string str)
{
  int i;
  mixed *stuff;
  string ret;
  mapping news_rc;

  stuff = (mixed *)BOARD_HAND->get_subjects(board_name);
  if (!sizeof(stuff)) {
    notify_fail(_LANG_BOARD_IS_EMPTY);
    return 0;
  }
  news_rc = query_news_rc();
  ret = "";

  if (this_player()->query_coder())
    ret += "Board '%^BOLD%^" + board_name + "%^RESET%^'.\n\n";

  for (i = 0; i < sizeof(stuff); i++)
    ret += note_line(stuff[i], i + 1, news_rc[board_name] < stuff[i][B_TIME]);

  this_user()->set_finish_func(0);
  this_user()->more_string(ret, _LANG_BOARD_SUBJECTS_PROMPT);
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
    ret += "Board '%^BOLD%^" + board_name + "%^RESET%^'.\n";

  ret += _LANG_BOARD_COMMANDS;

  if (!sizeof(stuff))
    return ret + _LANG_BOARD_IS_EMPTY;

  news_rc = query_news_rc();
  newones = 0;

  // unread notes, and the ones read in the last two days
  for (i = 0; i < sizeof(stuff); i++)
  {
    if (news_rc[board_name] < stuff[i][B_TIME])
    {
      ret += note_line(stuff[i], i + 1, 1);
      newones += 1;
    }
    else if (news_rc[board_name] < stuff[i][B_TIME] + (2*24*60*60))
      ret += note_line(stuff[i], i + 1, 0);
  }

  if (!newones)
    ret += _LANG_BOARD_NO_NEW_NOTES;

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
  this_user()->set_finish_func(0);
  this_user()->more_string(arg, prompt);
} /* string_more() */

int read(string str)
{
  int num, i;
  mixed stuff;
  mapping news_rc;
  string message;

  notify_fail(_LANG_BOARD_READ_SYNTAX);
  stuff = (mixed *)BOARD_HAND->get_subjects(board_name);
  news_rc = query_news_rc();

  // with no number, the first note not read yet
  if (!strlen(str)) {
    i = sizeof(stuff)-1;
    while (i >= 0 &&stuff[i][B_TIME] > news_rc[board_name] )
      i--;
    if (i == sizeof(stuff)-1) {
      notify_fail(_LANG_BOARD_NOTHING_TO_READ);
      return 0;
    }
    num = i+2;
  } else if (sscanf(str, "%d", num) != 1)
    return 0;

  if (num < 1 || num > sizeof(stuff)) {
    notify_fail(_LANG_BOARD_NO_SUCH_NOTE);
    return 0;
  }
  num --;
  if (news_rc[board_name] < stuff[num][B_TIME])
  {
    news_rc[board_name] = stuff[num][B_TIME];
    this_player()->add_property(NEWS_RC, news_rc);
  }
  message = BOARD_HAND->get_message(board_name, num);

  // lang = BOARD_HAND->get_language(board_name,num);
  //
  // if (!lang)
  //     lang="comun";
  //
  // if (member_array(lang, this_player()->query_languages()) == -1)
  // {
  //      mixed stri;
  //
  //      if ((stri = (mixed)LANGUAGE_HANDLER->query_garble_object(lang)))
  //           if ((stri = (mixed)stri->garble_say("",mensaje)))
  //           {
  //                mensaje = stri[1];
  //           }
  // }

  message = sprintf("  %-=*s\n", (int)this_user()->query_cols() - 2, message);

  string_more(_LANG_BOARD_NOTE_HEADER + message, _LANG_BOARD_NOTE_PROMPT);
  return 1;
} /* read() */

// Writing a note: the subject comes with the command, the text is written
// in the user's editor.
private void start_note(string subject)
{
  being_written[this_player()->query_name()] = subject;
  this_user()->do_edit("", "end_of_thing", this_object());
}

int post(string str)
{
  if (!can_write())
    return 1;

  notify_fail(_LANG_BOARD_POST_SYNTAX);

  if (!strlen(str))
    return 0;

  start_note(str);
  return 1;
} /* post() */

void end_of_thing(string body)
{
  string name;

  name = this_player()->query_name();

  if (strlen(trim(body ? body : "")) && being_written[name])
  {
    if (!BOARD_HAND->add_message(board_name, name, being_written[name], body + "\n"))
      write(_LANG_BOARD_WRITE_ERROR);
    else
      write(_LANG_BOARD_WRITTEN);
  }
  else
    write(_LANG_BOARD_NOT_SAVED);

  map_delete(being_written, name);
} /* end_of_thing() */


int eat(string str) {
  int num, i, eaten;

  notify_fail(_LANG_BOARD_DELETE_SYNTAX);
  if (!str || (str == ""))
    return 0;
  if (sscanf(str, _LANG_BOARD_DELETE_UNTIL + " %d", num)) {
    for (i = 0; i < num; i++)
      eaten += (int)BOARD_HAND->delete_message(board_name, 0);
    if (!eaten) {
      notify_fail(_LANG_BOARD_DELETE_FAILED);
      return 0;
    }
    write(_LANG_BOARD_DELETED_MANY_ME);
    say(_LANG_BOARD_DELETED_MANY_ROOM);
    event(users(), "inform", this_player()->query_cap_name() + " deletes " +
          eaten + " note(s) from '" + board_name + "'", "tablones");
    return 1;
  } else if (sscanf(str, "%d", num) != 1)
    return 0;
  if (!BOARD_HAND->delete_message(board_name, num-1))
  {
    notify_fail(_LANG_BOARD_DELETE_FAILED);
    return 0;
  }
  write(_LANG_BOARD_DELETED_ONE_ME);
  say(_LANG_BOARD_DELETED_ONE_ROOM);
  event(users(), "inform", this_player()->query_cap_name() + " deletes a note " +
                           "from '" + board_name + "'", "tablones");
  return 1;
} /* eat() */

int followup(string str)
{
  int num, i;
  mixed stuff;
  string s;

  if (!can_write())
    return 1;

  notify_fail(_LANG_BOARD_FOLLOWUP_SYNTAX);
  if (!strlen(str))
    return 0;
  if (sscanf(str, "%d", num) != 1)
    return 0;
  stuff = (mixed *)BOARD_HAND->get_subjects(board_name);
  if (num < 1 || num > sizeof(stuff)) {
    notify_fail(_LANG_BOARD_NO_SUCH_NOTE);
    return 0;
  }
  if (sscanf(stuff[num-1][B_SUBJECT], "Re:#%d %s", i, s) != 2)
    start_note("Re:#1 " + stuff[num-1][B_SUBJECT]);
  else
    start_note("Re:#" + (i+1) + " " + s);
  return 1;
} /* followup() */

// Answer the author of a note with a letter instead of another note.
int reply(string str)
{
  int num;
  mixed stuff;
  object mailer;

  if (this_player()->query_property(GUEST_PROP))
  {
    notify_fail(_LANG_BOARD_NO_GUESTS);
    return 0;
  }

  notify_fail(_LANG_BOARD_MUDMAIL_SYNTAX);
  if (!strlen(str))
    return 0;
  if (sscanf(str, "%d", num) != 1)
    return 0;
  stuff = (mixed *)BOARD_HAND->get_subjects(board_name);
  if (num < 1 || num > sizeof(stuff)) {
    notify_fail(_LANG_BOARD_NO_SUCH_NOTE);
    return 0;
  }

  mailer = clone_object(MAILER_OB);
  mailer->start(this_player(), stuff[num-1][B_NAME], "Re: " + stuff[num-1][B_SUBJECT]);
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

  news_rc = query_news_rc();

  notes = (mixed *)BOARD_HAND->get_subjects(board_name);

  if (!sizeof(notes))
    return 0;

  if (!news_rc[board_name])
  {
      news_rc[board_name] = this_player()->query_start_time();
      if (!intp(news_rc[board_name]))
        news_rc[board_name] = 0;
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
