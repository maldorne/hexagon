// Text editor: one clone per session, driven by input_to. Based on the
// line editor of CcMud (global/player/line_ed.c), rewritten for Hexagon,
// neverbot 09/2026.
//
// Three modes, chosen by each user:
// - menu: lines are typed one after another; a line with '**' opens a menu
//   to insert, delete, list or change lines, and to save or quit;
// - command: the same, but '**' saves straight away, and '~<letter>' runs a
//   menu command without leaving the text;
// - ed: the driver editor, for coders only, since it reads and writes files.
//
// When the text is done, end_func is called on end_object with the text, or
// with nil when nothing is kept.

#include <mud/secure.h>
#include <user/editor.h>
#include <language.h>

inherit "/lib/core/object";

#define ED_HELP \
  "  (.)     a        append lines after .     (.,.)  p        print lines\n" + \
  "  (.)     i        insert lines before .    (.,.)  #        numbered lines\n" + \
  "  (.,.)   c        change lines             (.,.)  d        delete lines\n" + \
  "  (.,.)   s/a/b/   substitute a by b        (.,.)  m line   move lines\n" + \
  "  (.,.)   t line   copy lines               (.+1)  z        page of lines\n" + \
  "  (1,$)   g/re/cmd run cmd where re matches (1,$)  I        indent LPC code\n" + \
  "          u        undo last change                w [file] write\n" + \
  "          x        write and quit                  q, q!    quit, quit discarding\n" + \
  "  Lines: . current, $ last, 5+7, /re/ forward, ?re? backward.\n" + \
  "  Insert mode ends with a line holding just a dot. More in 'help line-ed'.\n"

static object owner;
static string mode;
static string * lines;
static int insertion_point;
static object end_object;
static string end_func;
// a file edited with ed, and whether it is a temporary copy of the text
static string ed_file;
static int ed_is_copy;
// the range and text of a modification in progress
static int range_from, range_to;
static string modify_from;

private void menu_prompt();
private void finish(string text);
private void start_ed(string file, int is_copy);
private void write_copy(string text);
void main_menu(string str);

void create()
{
  ::create();
  lines = ({ });
}

int clean_up(varargs int flag)
{
  if (owner)
    return 1;

  return ::clean_up(flag);
}

private int valid_session()
{
  if (!owner || this_player() != owner)
  {
    dest_me();
    return 0;
  }

  return 1;
}

private int can_use_ed()
{
  return owner && owner->query_coder();
}

private void insert_prompt()
{
  write(sprintf("%-2d] ", insertion_point + 1));
  input_to("editor_loop");
}

// Edit a text for the player, and hand it to end_ob->end_f when done.
void start(object player, string text, string end_f, object end_ob, string editor_mode)
{
  owner = player;
  end_func = end_f;
  end_object = end_ob;
  mode = editor_mode;

  if (mode == EDITOR_MODE_ED && !can_use_ed())
    mode = EDITOR_MODE_MENU;

  lines = strlen(text) ? explode(text, "\n") : ({ });
  insertion_point = sizeof(lines);

  if (mode == EDITOR_MODE_ED)
  {
    write_copy(text ? text : "");
    return;
  }

  if (mode == EDITOR_MODE_COMMAND)
    write(_LANG_EDITOR_START_COMMAND);
  else
    write(_LANG_EDITOR_START_MENU);

  insert_prompt();
}

// Open a file with ed, for the coder command.
void start_file(object player, string file)
{
  owner = player;
  mode = EDITOR_MODE_ED;

  if (!can_use_ed())
  {
    dest_me();
    return;
  }

  start_ed(file, 0);
}

void editor_loop(string str)
{
  if (!valid_session())
    return;

  if (!str)
    str = "";

  if (mode == EDITOR_MODE_COMMAND && strlen(str) > 1 && str[0] == '~')
  {
    main_menu(str[1..]);
    return;
  }

  if (str == EDITOR_END_OF_TEXT)
  {
    if (mode == EDITOR_MODE_MENU)
      main_menu("");
    else
      finish(sizeof(lines) ? implode(lines, "\n") : nil);
    return;
  }

  lines = lines[0..insertion_point - 1] + ({ str }) + lines[insertion_point..];
  insertion_point++;
  insert_prompt();
}

private void menu_prompt()
{
  if (mode == EDITOR_MODE_MENU)
  {
    write(_LANG_EDITOR_MENU_PROMPT);
    input_to("main_menu");
  }
  else
    insert_prompt();
}

void main_menu(string str)
{
  string text;
  int i;

  if (!valid_session())
    return;

  str = trim(str ? str : "");

  if (!strlen(str))
  {
    menu_prompt();
    return;
  }

  switch (lower_case(str[0..0]))
  {
    case "i":
      write(_LANG_EDITOR_ASK_INSERT);
      input_to("editor_insert");
      return;
    case "d":
      write(_LANG_EDITOR_ASK_DELETE);
      input_to("editor_delete");
      return;
    case "l":
      text = "";
      for (i = 0; i < sizeof(lines); i++)
        text += sprintf("%3d: %s\n", i + 1, lines[i]);
      write(strlen(text) ? text : _LANG_EDITOR_NO_LINES);
      menu_prompt();
      return;
    case "m":
      write(_LANG_EDITOR_ASK_MODIFY);
      input_to("editor_modify");
      return;
    case "c":
      insertion_point = sizeof(lines);
      write(_LANG_EDITOR_CONTINUE);
      insert_prompt();
      return;
    case "e":
      if (!can_use_ed())
        break;
      write_copy(implode(lines, "\n"));
      return;
    case "s":
      write(_LANG_EDITOR_SAVING);
      finish(implode(lines, "\n"));
      return;
    case "q":
      write(_LANG_EDITOR_ASK_QUIT);
      input_to("editor_check_quit");
      return;
    case "h":
      write(_LANG_EDITOR_HELP);
      menu_prompt();
      return;
  }

  write(_LANG_EDITOR_UNKNOWN_COMMAND);
  menu_prompt();
}

// Line numbers written as "3" or "1..4".
private int * parse_range(string str)
{
  int from, to, swap;

  str = trim(str ? str : "");

  if (sscanf(str, "%d..%d", from, to) != 2)
  {
    if (sscanf(str, "%d", from) != 1)
      return nil;
    to = from;
  }

  if (from > to)
  {
    swap = from;
    from = to;
    to = swap;
  }

  if (from < 1 || to > sizeof(lines))
    return nil;

  return ({ from, to });
}

void editor_insert(string str)
{
  int number;

  if (!valid_session())
    return;

  if (sscanf(str ? str : "", "%d", number) != 1 || number < 1 ||
      number > sizeof(lines) + 1)
  {
    write(_LANG_EDITOR_OUT_OF_RANGE);
    menu_prompt();
    return;
  }

  insertion_point = number - 1;
  write(_LANG_EDITOR_INSERTING);
  insert_prompt();
}

void editor_delete(string str)
{
  int * range;

  if (!valid_session())
    return;

  if (!(range = parse_range(str)))
  {
    write(_LANG_EDITOR_OUT_OF_RANGE);
    menu_prompt();
    return;
  }

  lines = lines[0..range[0] - 2] + lines[range[1]..];

  if (insertion_point > sizeof(lines))
    insertion_point = sizeof(lines);

  write(_LANG_EDITOR_DELETED);
  menu_prompt();
}

void editor_modify(string str)
{
  int * range;

  if (!valid_session())
    return;

  if (!(range = parse_range(str)))
  {
    write(_LANG_EDITOR_OUT_OF_RANGE);
    menu_prompt();
    return;
  }

  range_from = range[0];
  range_to = range[1];
  write(_LANG_EDITOR_ASK_MODIFY_FROM);
  input_to("editor_modify_from");
}

void editor_modify_from(string str)
{
  if (!valid_session())
    return;

  if (!strlen(str))
  {
    write(_LANG_EDITOR_ABORTED);
    menu_prompt();
    return;
  }

  modify_from = str;
  write(_LANG_EDITOR_ASK_MODIFY_TO);
  input_to("editor_modify_to");
}

void editor_modify_to(string str)
{
  int i;

  if (!valid_session())
    return;

  if (!str)
    str = "";

  for (i = range_from - 1; i < range_to; i++)
    if (strsrch(lines[i], modify_from) != -1)
    {
      lines[i] = replace_string(lines[i], modify_from, str);
      write(sprintf("%3d: %s\n", i + 1, lines[i]));
    }

  write(_LANG_EDITOR_DONE);
  menu_prompt();
}

void editor_check_quit(string str)
{
  if (!valid_session())
    return;

  if (!strlen(str) || member_array(lower_case(str[0..0]), _LANG_EDITOR_YES) == -1)
  {
    write(_LANG_EDITOR_BACK);
    menu_prompt();
    return;
  }

  write(_LANG_EDITOR_QUITTING);
  finish(nil);
}

// ed works on a file, so the text goes to a temporary copy first.
private void write_copy(string text)
{
  string file;

  file = EDITOR_TMP_DIR + owner->query_name();

  if (file_exists(file))
    rm(file);

  write_file(file, strlen(text) ? text + "\n" : "");
  start_ed(file, 1);
}

// The driver editor. Its instance belongs to this object, and every line the
// coder types goes to it until the editor closes.
private void start_ed(string file, int is_copy)
{
  string output;

  ed_file = file;
  ed_is_copy = is_copy;

  write(_LANG_EDITOR_ED_START);

  output = editor("e " + file);
  if (strlen(output))
    write(output);

  input_to("ed_input");
}

void ed_input(string str)
{
  string output, text;

  if (!valid_session())
    return;

  // the driver editor has no help of its own
  if (query_editor(this_object()) == "command" && trim(str ? str : "") == "h")
  {
    write(ED_HELP + ":");
    input_to("ed_input");
    return;
  }

  output = editor(str ? str : "");
  if (strlen(output))
    write(output);

  if (query_editor(this_object()))
  {
    if (query_editor(this_object()) == "command")
      write(":");
    input_to("ed_input");
    return;
  }

  // ed closed: a file opened by the coder is done; a copy of the text goes
  // back to the menu editor, or straight to whoever asked for the text
  if (!ed_is_copy)
  {
    owner->user()->query_role()->fini_editor();
    dest_me();
    return;
  }

  text = read_file(ed_file);
  rm(ed_file);

  if (mode == EDITOR_MODE_ED)
  {
    finish(strlen(text) ? text : nil);
    return;
  }

  lines = strlen(text) ? explode(text, "\n") : ({ });
  insertion_point = sizeof(lines);
  menu_prompt();
}

// Called by the driver for every file the editor reads or writes: resolve
// the path the coder typed and allow it only if they may use that file.
string query_editor_path(string path, int writing)
{
  if (previous_program() != DRIVER || !can_use_ed())
    return nil;

  // the temporary copy of a letter or a note belongs to this session
  if (ed_is_copy && path == ed_file)
    return path;

  path = get_path(path);

  if (writing)
    return SECURE->valid_write(path, geteuid(owner), "editor") ? path : nil;

  return SECURE->valid_read(path, geteuid(owner), "editor") ? path : nil;
}

private void finish(string text)
{
  object ob;
  string func;

  ob = end_object;
  func = end_func;
  lines = ({ });

  if (ob)
    call_other(ob, func, text);

  dest_me();
}
